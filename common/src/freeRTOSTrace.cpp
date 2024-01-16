/**
 ******************************************************************************
 * @file           freeRTOSTrace.cpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Dec. 2023
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
#ifdef FREERTOS_USE_TRACE

#include <carbon/diag.hpp>
#include <carbon/hsem.hpp>
#include <carbon/shared_memory.hpp>
#include <carbon/systime.hpp>
#include <carbon/trace_format.hpp>

#include <cstddef>
#include <cstdint>

using namespace CARBON;

extern "C" {

void carbon_freertos_trace_malloc(void *address, size_t size) {}
void carbon_freertos_trace_free(void *address, size_t size) {}
void carbon_freertos_trace_switched_in(uint32_t number) {
    TraceTaskSwitchedInEvent trc; // NOLINT
    trc.header.timestamp = systimeUs();
    trc.number = number;
    traceFifoClass::ContextPush context(
        traceFifo, sizeof(TraceTaskSwitchedInEvent), hsemTrace);
    uint32_t len = sizeof(TraceTaskSwitchedInEvent);
    context.push_array(reinterpret_cast<uint8_t *>(&trc), len);
    return;
}
void carbon_freertos_trace_switched_out(uint32_t number) {
    TraceTaskSwitchedOutEvent trc; // NOLINT
    trc.header.timestamp = systimeUs();
    trc.number = number;
    traceFifoClass::ContextPush context(
        traceFifo, sizeof(TraceTaskSwitchedOutEvent), hsemTrace);
    uint32_t len = sizeof(TraceTaskSwitchedOutEvent);
    context.push_array(reinterpret_cast<uint8_t *>(&trc), len);
    return;
}
}
#endif
