/**
 ******************************************************************************
 * @file           trace_thread.cpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Jan. 2024
 * @brief          trace thread
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
#include <carbon/diag.hpp>
#include <carbon/trace.hpp>
#include <carbon/trace_thread.hpp>

#include <cmsis_os.h>

TraceThread::TraceThread()
    : Thread("trace_thread", osPriorityNormal, configMINIMAL_STACK_SIZE * 5) {}

void TraceThread::run() {
    if (!CARBON::Trace::instance().init()) {
        DIAG(TRACE_DIAG "trace initialization error");
        while (1) {
            osDelay(10000);
        }
    }
    CARBON::Trace::instance().runConnection();
    while (1) {
        DIAG(TRACE_DIAG "trace connection error");
        osDelay(1000);
    }
}