/**
 ******************************************************************************
 * @file           freeRTOSTrace.cpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Dec. 2022
 * @brief          FreeRTOS Trace hooks implementation
 ******************************************************************************
 * @attention
 * Copyright (c) 2023 Michele Viti.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include <carbon/diag.hpp>
#include <cstddef>
#include <cstdint>

extern "C" {

void carbon_freertos_trace_malloc(void *address, size_t size) {}
void carbon_freertos_trace_free(void *address, size_t size) {}
void carbon_freertos_trace_switched_in(uint32_t number) {
    DIAG(TRACE_DIAG "thread %lu switched in", number);
}
void carbon_freertos_trace_switched_out(uint32_t number) {
    DIAG(TRACE_DIAG "thread %lu switched out", number);
}
}
