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

namespace CARBON {

template <typename ObjectType, uint32_t aligment, typename Lock,
          uint32_t NElements>
class Fifo {
public:
    Fifo() {}

    ~Fifo() = default;

    PREVENT_COPY_AND_MOVE(Fifo)

    friend class FifoTest;

    bool init(uint32_t startAddress, uint32_t size) {
        startAddress_ = startAddress;
        size_ = size;
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

        buffer_.insert(&object, fifo_pos);

        {
            LockGuard<Lock> lockGuard(lock);
            uint8_t bit_pos = static_cast<uint8_t>(1u << (fifo_pos % 8u));
            tail_ready_[(fifo_pos / 8u)] |= bit_pos;
        }

        return true;
    }

    class ContextPush {
    public:
        ContextPush(Fifo<ObjectType, aligment, Lock, NElements> &fifo,
                    uint32_t nObjects, Lock &lock)
            : fifo_(fifo), nObjects_(nObjects), lock_(lock) {
            LockGuard<Lock> lockGuard(lock_);
            if (!fifo_.isFull(nObjects_)) {
                fifo_pos_start_ = fifo_.tail_reserved_;
                fifo_pos_end_ = fifo_pos_start_;
                fifo_.increment(fifo_pos_end_, nObjects_ - 1);
                fifo_.tail_reserved_ = fifo_.increment(fifo_pos_end_);
                index_ = fifo_pos_start_;
                if (fifo_pos_start_ + nObjects_ <= BUFFER_SIZE) {
                    data_length1_ = nObjects_;
                    data_length2_ = 0;
                } else {
                    data_length1_ = BUFFER_SIZE - fifo_pos_start_;
                    data_length2_ = nObjects_ - data_length1_;
                }
            } else {
                isOverflow_ = true;
            }
        }

        ~ContextPush() {
            LockGuard<Lock> lockGuard(lock_);
            uint32_t index1 = fifo_pos_start_ / 8u;
            uint32_t index2 = fifo_pos_end_ / 8u;
            if (index1 == index2) {
                uint8_t bit_pos1 =
                    static_cast<uint8_t>(0xFF << (fifo_pos_start_ % 8u));
                uint8_t bit_pos2 =
                    static_cast<uint8_t>(0xFF >> (7 - (fifo_pos_end_ % 8u)));
                bit_pos1 = bit_pos1 & bit_pos2;
                fifo_.tail_ready_[index1] |= bit_pos1;
            } else {
                uint8_t bit_pos1 =
                    static_cast<uint8_t>(0xFF << (fifo_pos_start_ % 8u));
                fifo_.tail_ready_[index1] |= bit_pos1;

                uint8_t bit_pos2 =
                    static_cast<uint8_t>(0xFF >> (7 - (fifo_pos_end_ % 8u)));
                fifo_.tail_ready_[index2] |= bit_pos2;

                if (index2 > index1) {
                    for (uint32_t i = (index1 + 1); i < index2; i++) {
                        fifo_.tail_ready_[i] = 0xFF;
                    }
                } else if (index1 > index2) {
                    uint32_t last_index = (NElements) / 8u;
                    for (uint32_t i = (index1 + 1); i < (last_index); i++) {
                        fifo_.tail_ready_[i] = 0xFF;
                    }

                    fifo_.tail_ready_[last_index] =
                        (0xFF >> (7 - (NElements % 8u)));

                    for (uint32_t i = 0; i < index2; i++) {
                        fifo_.tail_ready_[i] = 0xFF;
                    }
                }
            }
        }

        DEFAULT_COPY_AND_MOVE(ContextPush)

        inline bool push(const ObjectType &object) {
            if (isOverflow_) {
                return false;
            }
            if (index_ != fifo_.tail_reserved_) {
                if (!(fifo_.buffer_.insert(&object, index_))) {
                    RAW_DIAG("error pushing in buffer");
                    return false;
                }
                index_ = fifo_.increment(index_);
            } else {
                RAW_DIAG("cannot push in the buffer");
                return false;
            }
            return true;
        }

        template <class ObjectT = ObjectType,
                  std::enable_if_t<sizeof(ObjectT) !=
                                       (((sizeof(ObjectT) + aligment - 1)) &
                                        (~(aligment - 1))),
                                   bool> = true>
        inline bool push_array(const ObjectT *object, uint32_t &length) {
            if (isOverflow_) {
                return false;
            }
            uint32_t i = 0;
            while (index_ != fifo_.tail_reserved_ || i < length) {
                if (!(fifo_.buffer_.insert(*(&object + i), index_))) {
                    RAW_DIAG("error pushing in buffer");
                    return false;
                }
                index_ = fifo_.increment(index_);
                i++;
            }
            length = i;
            return true;
        }

        template <class ObjectT = ObjectType,
                  std::enable_if_t<sizeof(ObjectT) ==
                                       (((sizeof(ObjectT) + aligment - 1)) &
                                        (~(aligment - 1))),
                                   bool> = true>
        inline bool push_array(const ObjectT *object, uint32_t &length) {
            if (isOverflow_) {
                return false;
            }

            if (length <= data_length1_) {
                if (!(fifo_.buffer_.insert(object, index_, length))) {
                    RAW_DIAG("error pushing in buffer length");
                    return false;
                }
                if (!fifo_.increment(index_, data_length1_)) {
                    RAW_DIAG(
                        "error while incrementing, length <= data lenght 1");
                    return false;
                }
                object += length;
            } else {
                if (!(fifo_.buffer_.insert(object, index_, data_length1_))) {
                    RAW_DIAG("error pushing in buffer data_length1_");
                    return false;
                }
                if (!fifo_.increment(index_, data_length1_)) {
                    RAW_DIAG(
                        "error while incrementing, length > data lenght 1");
                    return false;
                }
                if (index_ != 0) {
                    RAW_DIAG("wrong index %lu", index_);
                    return false;
                }
                object += data_length1_;
                if (!(fifo_.buffer_.insert(object, index_,
                                           length - data_length1_))) {
                    RAW_DIAG("error pushing in buffer length-data_length1_");
                    return false;
                }
            }

            return true;
        }

        bool isOverflow() const { return isOverflow_; }

    private:
        Fifo<ObjectType, aligment, Lock, NElements> &fifo_;
        uint32_t nObjects_;
        Lock &lock_;
        uint32_t index_{0};
        uint32_t fifo_pos_start_{0};
        uint32_t fifo_pos_end_{0};
        uint32_t data_length1_{0};
        uint32_t data_length2_{0};
        bool isOverflow_ = false;
    };

    class ContextPull {
    public:
        ContextPull(Fifo<ObjectType, aligment, Lock, NElements> &fifo,
                    Lock &lock)
            : fifo_(fifo), lock_(lock) {
            LockGuard<Lock> lockGuard(lock_);
            currentHead_ = fifo_.head_;
            fifo_.getElementReady(dataLength1_, dataLength2_);
            dataLengthByte1_ = dataLength1_ * sizeof(ObjectType);
            dataLengthByte2_ = dataLength2_ * sizeof(ObjectType);
            dataPtr1_ = fifo_.startAddress_ + currentHead_;
            dataPtr2_ = fifo_.startAddress_;
        }

        ~ContextPull() {
            LockGuard<Lock> lockGuard(lock_);
            uint8_t tail_ready_bit_pos{0};
            uint32_t tail_ready_index{0};
            for (unsigned i = 0; i < dataLength1_ + dataLength2_; i++) {
                tail_ready_bit_pos =
                    static_cast<uint8_t>(1u << (fifo_.head_ % 8u));
                tail_ready_index = fifo_.head_ / 8u;
                fifo_.tail_ready_[tail_ready_index] &= ~tail_ready_bit_pos;
                fifo_.head_ = fifo_.increment(fifo_.head_);
            }
        }

        DEFAULT_COPY_AND_MOVE(ContextPull)

        inline void getDataLength(uint32_t &dataLength1,
                                  uint32_t &dataLength2) {
            dataLength1 = dataLength1_;
            dataLength2 = dataLength2_;
        }

        inline void getDataLengthByte(uint32_t &dataLengthByte1,
                                      uint32_t &dataLengthByte2) {
            dataLengthByte1 = dataLengthByte1_;
            dataLengthByte2 = dataLengthByte2_;
        }

        inline void getDataPtr(uint32_t &dataPtr1, uint32_t &dataPtr2) {
            dataPtr1 = dataPtr1_;
            dataPtr2 = dataPtr2_;
        }

    private:
        Fifo<ObjectType, aligment, Lock, NElements> &fifo_;
        Lock &lock_;
        uint32_t currentHead_{0};
        uint32_t dataLength1_{0};
        uint32_t dataLengthByte1_{0};
        uint32_t dataLength2_{0};
        uint32_t dataLengthByte2_{0};
        uint32_t dataPtr1_{0};
        uint32_t dataPtr2_{0};
    };

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

        buffer_.remove(&object, current_head);

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
        bool res = ((tail_ready_[(current_head / 8u)] & bit_pos) == 0);
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

    inline void getElementReady(uint32_t &dataLength1, uint32_t &dataLength2) {
        dataLength1 = 0;
        dataLength2 = 0;
        uint32_t current_head = this->head_;
        bool empty;
        do {
            uint8_t bit_pos = static_cast<uint8_t>(1u << (current_head % 8u));
            empty = ((tail_ready_[(current_head / 8u)] & bit_pos) == 0);
            // RAW_DIAG("tail_ready_[(%lu / 8u)] %u ", current_head,
            // tail_ready_[(current_head / 8u)]);
            if (!empty)
                dataLength1++;
            current_head = increment(current_head);
            if (current_head < this->head_)
                break;
            if (empty || (current_head == this->head_)) {
                if (dataLength1 + dataLength2 > NElements) {
                    RAW_DIAG("dataLength1 %lu + dataLength2 %lu > NElements",
                             dataLength1, dataLength2);
                }
                return;
            }
        } while (1);
        do {
            uint8_t bit_pos = static_cast<uint8_t>(1u << (current_head % 8u));
            empty = ((tail_ready_[(current_head / 8u)] & bit_pos) == 0);
            if (!empty)
                dataLength2++;
            current_head = increment(current_head);
            if ((current_head >= this->head_) || empty)
                break;
        } while (1);
        if (dataLength1 + dataLength2 > NElements) {
            RAW_DIAG("dataLength1 %lu + dataLength2 %lu > NElements",
                     dataLength1, dataLength2);
        }
        if (dataLength1 == 0 && dataLength2 > 0) {
            RAW_DIAG("dataLength1 == 0 && dataLength2 > 0");
        }
    }

    CallbackOverflow callbackOverflow{nullptr};
    CallbackUnderflow callbackUnderflow{nullptr};

    Buffer<ObjectType, aligment> buffer_;
    uint32_t startAddress_{0};
    uint32_t size_{0};
    uint32_t tail_reserved_{0};
    uint32_t head_{0};
    static constexpr auto BUFFER_SIZE = (NElements + 1u);
    static constexpr auto bit_field_size_ = BUFFER_SIZE / 8u + 1u;
    uint8_t tail_ready_[bit_field_size_]{0};
};
} // namespace CARBON
