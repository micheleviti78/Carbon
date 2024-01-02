/**
 ******************************************************************************
 * @file           trace_thread.c
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Jan. 2024
 * @brief          starting trace thread, c code
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

#include <cmsis_os.h>

#ifdef FREERTOS_USE_TRACE

static osThreadId trace_handle;

void trace_thread(const void *argument);

void start_trace_thread_imp(void) {
    osThreadDef(Trace, trace_thread, osPriorityNormal, 0,
                configMINIMAL_STACK_SIZE * 5);
    trace_handle = osThreadCreate(osThread(Trace), NULL);
}

#endif
