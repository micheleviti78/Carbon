/**
 ******************************************************************************
 * @file           trace_thread.cpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Jan. 2024
 * @brief          starting trace thread
 ******************************************************************************
 * @attention
 * Copyright (c) 2024 Michele Viti.
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
#include <carbon/trace.hpp>

#include <cmsis_os.h>

extern "C" {

void start_trace_thread_imp(void);

void start_trace_thread(void) {
    if (CARBON::Trace::instance().init()) {
        start_trace_thread_imp();
    } else {
        DIAG(TRACE_DIAG "trace initialization error");
    }
}

void trace_thread(const void *argument) {
    while (1) {
        CARBON::Trace::instance().sendTrace();
        DIAG(TRACE_DIAG "can de dio");
        osDelay(1000);
    }
}
}

#endif
