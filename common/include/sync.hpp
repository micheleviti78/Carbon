/**
 ******************************************************************************
 * @file           sync.hpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Mar. 2022
 * @brief          DISCO-STM32H747 sync utilities
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

template <typename Mutex> class Lock {
public:
    inline Lock() { Mutex::get(); }
    inline ~Lock() { Mutex::release(); }
}
