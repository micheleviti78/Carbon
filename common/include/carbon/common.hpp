/**
 ******************************************************************************
 * @file           common.hpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Sep. 2022
 * @brief          DISCO-STM32H747 common macros
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
#ifndef __cplusplus
#include <stddef.h>
#include <stdint.h>
#else
#include <cstddef>
#include <cstdint>
#endif

#include <carbon/diag.hpp>

#define PREVENT_COPY(class_name)                                               \
    class_name(const class_name &) = delete;                                   \
    class_name &operator=(const class_name &) = delete;

#define PREVENT_COPY_AND_MOVE(class_name)                                      \
    class_name(const class_name &) = delete;                                   \
    class_name &operator=(const class_name &) = delete;                        \
    class_name(class_name &&) = delete;                                        \
    class_name &operator=(class_name &&) = delete;

#define DEFAULT_COPY(class_name)                                               \
    class_name(const class_name &) = default;                                  \
    class_name &operator=(const class_name &) = default;

#define DEFAULT_COPY_AND_MOVE(class_name)                                      \
    class_name(const class_name &) = default;                                  \
    class_name &operator=(const class_name &) = default;                       \
    class_name(class_name &&) = default;                                       \
    class_name &operator=(class_name &&) = default;

#define ASSERT(cond)                                                           \
    do {                                                                       \
        if (!(cond)) {                                                         \
            RAW_DIAG("Assertion failed: %s, file %s, line %d", #cond,          \
                     __FILE__, __LINE__);                                      \
            __asm volatile("BKPT #01");                                        \
        }                                                                      \
    } while (0);
//end of the file