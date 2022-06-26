/**
 ******************************************************************************
 * @file           fifo.hpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Mar. 2022
 * @brief          DISCO-STM32H747 fifo template
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
    Fifo() {
        head = 0;
        tail = 0;
    }

    ~Fifo();

    inline bool add(const ObjectType object) {
        {
            Lock lock;
            if (!isFull()) {
                size_t current_tail = this->tail;
                this->tail = increment(this->tail);
            } else {
                errorOverflow(object);
                return false;
            }
        }
        data[current_tail] = object;
    }

    inline bool remove(ObjectType &object) {
        {
            Lock lock;
            if (!isEmpty()) {
                size_t current_head = this->head;
                this->head = increment(this->head);
            } else {
                return errorUnderflow(object);
            }
        }
        object = data[current_head];
    }

    inline bool isEmpty() {
        bool res = (head == tail);
        return res;
    }
    inline bool isFull() {
        size_t tail = increment(this->tail);:
        bool res = (head == tail);
        return res;
    }

private:
    inline size_t increment(size_t index) {
        if (index < Size) {
            return (index + 1);
        } else {
            return 0;
        }
    }

    inline void errorOverflow(const ObjectType /*object*/) { return false; };
    inline void errorUnderflow(ObjectType & /*object*/) { return false; };

    ObjectType data[Size + 1];
    size_t head{0};
    size_t tail{0};
};