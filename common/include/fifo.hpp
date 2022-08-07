/**
 ******************************************************************************
 * @file           fifo.hpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Mar. 2022
 * @brief          DISCO-STM32H747 fifo template. it works for opology like
 *                 one(many) producers --> one consumer
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

template <typename ObjectType, typename Lock, std::size_t Size> class Fifo {
public:
    Fifo() = default;

    ~Fifo() = default;

    inline bool add(const ObjectType &object) {
        size_t current_tail{0};
        {
            Lock lock;
            if (!isFull()) {
                current_tail = this->tail_reserved_;
                this->tail_reserved_ = increment(this->tail_reserved_);
            } else {
                if (errorOverflow)
                    errorOverflow(object);
                return false;
            }
        }

        data[current_tail] = object;

        {
            Lock lock(Lock::signalize);
            uint8_t bit_pos = static_cast<uint8_t>(1u << (current_tail % 8u));
            tail_ready[(current_tail / 8u)] |= bit_pos;
        }

        return true;
    }

    inline bool remove(ObjectType &object) {
        uint8_t tail_ready_bit_pos{0};
        size_t tail_ready_index{0};
        {
            Lock lock;
            size_t current_head = this->head_;
            tail_ready_bit_pos =
                static_cast<uint8_t>(1u << (current_head % 8u));
            tail_ready_index = current_head / 8u;
            if (tail_ready[tail_ready_index] == 0) {
                if (errorUnderflow)
                    errorUnderflow();
                return false;
            }
        }

        object = data[current_head];

        {
            Lock lock(Lock::signalize);
            this->head_ = increment(this->head_);
            tail_ready[tail_ready_index] == 0;
        }
    }

    inline bool isEmpty() {
        size_t current_head = this->head_;
        uint8_t bit_pos = static_cast<uint8_t>(1u << (current_head % 8u));
        bool res = (tail_ready[(current_head / 8u)] & bit_pos == 0);
        return res;
    }

    inline bool isFull() {
        size_t tail = increment(this->tail_reserved_);:
        bool res = (head_ == tail);
        return res;
    }

    typedef void (*CallbackOverFlow)(const ObjectType &object);
    typedef void (*CallbackUnderFlow)(ObjectType object);

    void setCallbackOverFlow(CallbackOverFlow callback) {
        callbackOverFlow = callback;
    }
    void setCallbackUnderFlow(CallbackUnderFlow callback) {
        callbackUnderFlow = callback;
    }

private:
    inline size_t increment(size_t index) {
        if (index < Size) {
            return (index + 1);
        } else {
            return 0;
        }
    }

    CallbackOverFlow callbackOverFlow{nullptr};
    CallbackUnderFlow callbackUnderFlow{nullptr};

    ObjectType data_[Size + 1];
    size_t tail_reserved_{0};
    size_t head_{0};
    constexpr auto bit_field_size = (Size + 1) / 8u + 1u;
    uint8_t tail_ready[bit_field_size]{0};
};