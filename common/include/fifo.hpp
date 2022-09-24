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

#include <common.hpp>
#include <diag.hpp>
#include <pool.hpp>

template <typename ObjectType, uint32_t aligment, typename Lock,
          uint32_t NElements>
class Fifo {
public:
    Fifo(Buffer<ObjectType, aligment> &buffer) : buffer_(buffer) {
        buffer_.init();
        uint32_t n = buffer_.getNumberOfAlignedElements();
        if (n < (NElements + 1)) {
            RAW_DIAG("memomy buffer too small");
        }
        RAW_DIAG("memomy buffer with %lu element(s)", n);
    }

    ~Fifo() = default;

    PREVENT_COPY_AND_MOVE(Fifo)

    inline bool push(const ObjectType &object) {
        uint32_t current_tail{0};
        bool isOverflow = false;
        {
            LockGuard<Lock> lockGuard(lock_);
            if (!isFull()) {
                current_tail = this->tail_reserved_;
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

        buffer_.insert(object, current_tail);

        {
            LockGuard<Lock> lockGuard(lock_,
                                      LockGuardSignalizeOption::Signalize);
            uint8_t bit_pos = static_cast<uint8_t>(1u << (current_tail % 8u));
            tail_ready[(current_tail / 8u)] |= bit_pos;
        }

        return true;
    }

    inline bool pop(ObjectType &object) {
        uint8_t tail_ready_bit_pos{0};
        uint32_t tail_ready_index{0};
        uint32_t current_head;
        bool isUnderflow = false;
        {
            LockGuard<Lock> lockGuard(lock_);
            current_head = this->head_;
            tail_ready_bit_pos =
                static_cast<uint8_t>(1u << (current_head % 8u));
            tail_ready_index = current_head / 8u;
            if ((tail_ready[tail_ready_index] & tail_ready_bit_pos) == 0) {
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
            LockGuard<Lock> lockGuard(lock_);
            this->head_ = increment(this->head_);
            tail_ready[tail_ready_index] &= ~tail_ready_bit_pos;
        }

        return true;
    }

    inline bool isEmpty() {
        uint32_t current_head = this->head_;
        uint8_t bit_pos = static_cast<uint8_t>(1u << (current_head % 8u));
        bool res = (tail_ready[(current_head / 8u)] & bit_pos == 0);
        return res;
    }

    inline bool isFull() {
        uint32_t tail = increment(this->tail_reserved_);
        bool res = (head_ == tail);
        return res;
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

    CallbackOverflow callbackOverflow{nullptr};
    CallbackUnderflow callbackUnderflow{nullptr};

    Buffer<ObjectType, aligment> &buffer_;
    uint32_t tail_reserved_{0};
    uint32_t head_{0};
    static constexpr auto bit_field_size = (NElements + 1) / 8u + 1u;
    uint8_t tail_ready[bit_field_size]{0};
    Lock lock_;
};