/**
 ******************************************************************************
 * @file           fifo.hpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Mar. 2022
 * @brief          DISCO-STM32H747 fifo template. it works for topology like
 *                 one(many) producer(s) --> one consumer
 ******************************************************************************
 * @attention
 * Copyright (c) 2022 Michele Viti.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#pragma once

#include <carbon/common.hpp>
#include <carbon/diag.hpp>
#include <carbon/pool.hpp>

template <typename ObjectType, uint32_t aligment, typename Lock,
          uint32_t NElements>
class Fifo {
public:
    Fifo() {}

    ~Fifo() = default;

    PREVENT_COPY_AND_MOVE(Fifo)

    friend class FifoTest;

    bool init(uint32_t startAddress, uint32_t size) {
        buffer_.init(startAddress, size);
        uint32_t n = buffer_.getNumberOfAlignedElements();
        if (n < (BUFFER_SIZE)) {
            RAW_DIAG("memory buffer too small");
            return false;
        }
#ifdef TEST_FIFO
        RAW_DIAG("memory buffer with %lu element(s)", n);
#endif
        return true;
    }

    inline bool push(const ObjectType &object, Lock &lock) {
        uint32_t fifo_pos{0};
        bool isOverflow = false;
        {
            LockGuard<Lock> lockGuard(lock);
            if (!isFull()) {
                fifo_pos = this->tail_reserved_;
                this->tail_reserved_ = increment(this->tail_reserved_);
            } else {
                isOverflow = true;
            }
        }

        if (isOverflow) {
            if (callbackOverflow)
                callbackOverflow(object);
            return false;
        }

        buffer_.insert(object, fifo_pos);

        {
            LockGuard<Lock> lockGuard(lock);
            uint8_t bit_pos = static_cast<uint8_t>(1u << (fifo_pos % 8u));
            tail_ready_[(fifo_pos / 8u)] |= bit_pos;
        }

        return true;
    }

    inline bool push(const ObjectType *object, uint32_t nObjects, Lock &lock) {
        uint32_t fifo_pos_start{0};
        uint32_t fifo_pos_end{0};
        bool isOverflow = false;
        {
            LockGuard<Lock> lockGuard(lock);
            if (!isFull(nObjects)) {
                fifo_pos_start = this->tail_reserved_;
                fifo_pos_end = fifo_pos_start;
                increment(fifo_pos_end, nObjects - 1);
                this->tail_reserved_ = increment(fifo_pos_end);
            } else {
                isOverflow = true;
            }
        }

        if (isOverflow) {
            if (callbackOverflow)
                callbackOverflow(*object);
            return false;
        }

        uint8_t index = fifo_pos_start;

        for (unsigned i = 0; i < nObjects; i++) {
            buffer_.insert(*(object + i), index);
            index = increment(index);
        }

        {
            LockGuard<Lock> lockGuard(lock);
            uint32_t index1 = fifo_pos_start / 8u;
            uint32_t index2 = fifo_pos_end / 8u;
            if (index1 == index2) {
                uint8_t bit_pos1 =
                    static_cast<uint8_t>(0xFF << (fifo_pos_start % 8u));
                uint8_t bit_pos2 =
                    static_cast<uint8_t>(0xFF >> (7 - (fifo_pos_end % 8u)));
                bit_pos1 = bit_pos1 & bit_pos2;
                tail_ready_[index1] |= bit_pos1;
            } else {
                uint8_t bit_pos1 =
                    static_cast<uint8_t>(0xFF << (fifo_pos_start % 8u));
                tail_ready_[index1] |= bit_pos1;

                uint8_t bit_pos2 =
                    static_cast<uint8_t>(0xFF >> (7 - (fifo_pos_end % 8u)));
                tail_ready_[index2] |= bit_pos2;

                if (index2 > index1) {
                    for (uint32_t i = (index1 + 1); i < index2; i++) {
                        tail_ready_[i] = 0xFF;
                    }
                } else if (index1 > index2) {
                    uint32_t last_index = (NElements) / 8u;
                    for (uint32_t i = (index1 + 1); i < (last_index); i++) {
                        tail_ready_[i] = 0xFF;
                    }

                    tail_ready_[last_index] = (0xFF >> (7 - (NElements % 8u)));

                    for (uint32_t i = 0; i < index2; i++) {
                        tail_ready_[i] = 0xFF;
                    }
                }
            }
        }

        return true;
    }

    inline bool pop(ObjectType &object, Lock &lock) {
        uint8_t tail_ready_bit_pos{0};
        uint32_t tail_ready_index{0};
        uint32_t current_head;
        bool isUnderflow = false;
        {
            LockGuard<Lock> lockGuard(lock);
            current_head = this->head_;
            tail_ready_bit_pos =
                static_cast<uint8_t>(1u << (current_head % 8u));
            tail_ready_index = current_head / 8u;
            if ((tail_ready_[tail_ready_index] & tail_ready_bit_pos) == 0) {
                isUnderflow = true;
            }
        }

        if (isUnderflow) {
            if (callbackUnderflow)
                callbackUnderflow(object);
            return false;
        }

        buffer_.remove(object, current_head);

        {
            LockGuard<Lock> lockGuard(lock);
            this->head_ = increment(this->head_);
            tail_ready_[tail_ready_index] &= ~tail_ready_bit_pos;
        }

        return true;
    }

    inline bool isEmpty() {
        uint32_t current_head = this->head_;
        uint8_t bit_pos = static_cast<uint8_t>(1u << (current_head % 8u));
        bool res = (tail_ready_[(current_head / 8u)] & bit_pos == 0);
        return res;
    }

    inline bool isFull() {
        uint32_t tail = increment(this->tail_reserved_);
        bool res = (head_ == tail);
        return res;
    }

    inline bool isFull(uint32_t nIncrement) {
        if (nIncrement > NElements)
            return false;

        if (tail_reserved_ < head_) {
            if (nIncrement >= (head_ - tail_reserved_))
                return true;
            else
                return false;
        } else if (tail_reserved_ > head_) {
            if (nIncrement >= (BUFFER_SIZE - tail_reserved_ + head_))
                return true;
            else
                return false;
        } else
            return false;
    }

    void reset(Lock &lock) {
        LockGuard<Lock> lockGuard(lock);
        tail_reserved_ = 0;
        head_ = 0;
        for (unsigned i = 0; i < bit_field_size_; i++) {
            tail_ready_[i] = 0u;
        }
    }

    typedef void (*CallbackOverflow)(const ObjectType &object);
    typedef void (*CallbackUnderflow)(ObjectType object);

    void setCallbackOverflow(CallbackOverflow callback) {
        callbackOverflow = callback;
    }
    void setCallbackUnderflow(CallbackUnderflow callback) {
        callbackUnderflow = callback;
    }

private:
    inline uint32_t increment(uint32_t index) {
        if (index < NElements) {
            return (index + 1);
        } else {
            return 0;
        }
    }

    inline bool increment(uint32_t &index, uint32_t nIncrement) {
        if (nIncrement > NElements) {
            RAW_DIAG("increment bigger than fifo size");
            return false;
        } else {
            if ((index + nIncrement) < BUFFER_SIZE) {
                index += nIncrement;
                return true;
            } else {
                index = (index + nIncrement) - BUFFER_SIZE;
                return true;
            }
        }
        return true;
    }

    CallbackOverflow callbackOverflow{nullptr};
    CallbackUnderflow callbackUnderflow{nullptr};

    Buffer<ObjectType, aligment> buffer_;
    uint32_t tail_reserved_{0};
    uint32_t head_{0};
    static constexpr auto BUFFER_SIZE = (NElements + 1u);
    static constexpr auto bit_field_size_ = BUFFER_SIZE / 8u + 1u;
    uint8_t tail_ready_[bit_field_size_]{0};
};
