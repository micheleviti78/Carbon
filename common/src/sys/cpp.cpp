/**
 ******************************************************************************
 * @file           cpp.cpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Oct. 2022
 * @brief          overload global new and delete operator
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

#include <carbon/diag.hpp>

#include <FreeRTOS.h>

#include <cinttypes>
#include <cstdlib>

void *operator new(std::size_t size) {
    void *ptr = pvPortMalloc(size);
    RAW_DIAG("c++ new size=%u ptr=%p", size, ptr);
    return ptr;
}

void *operator new(std::size_t size, std::align_val_t align) {
    void *ptr = pvPortMalloc(size);
    RAW_DIAG("c++ new size=%u (align=%u) ptr=%p", size,
             static_cast<unsigned>(align), ptr);
    return ptr;
}

void operator delete(void *ptr) noexcept {
    RAW_DIAG("c++ delete ptr=%p", ptr);
    vPortFree(ptr);
}

void operator delete(void *ptr, std::size_t /*unused*/) noexcept {
    RAW_DIAG("c++ delete ptr=%p", ptr);
    vPortFree(ptr);
}

void operator delete(void *ptr, std::align_val_t /*unused*/) noexcept {
    RAW_DIAG("c++ delete ptr=%p", ptr);
    vPortFree(ptr);
}
