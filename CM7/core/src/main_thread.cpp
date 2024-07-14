/**
 ******************************************************************************
 * @file           main_thread.cpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Nov. 2022
 * @brief          CM7 main thread class
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

#include <carbon/diag_thread.hpp>
#include <carbon/display_matrix_spi.hpp>
#include <carbon/main_thread.hpp>
#include <carbon/mp_thread.h>
#include <carbon/pin.hpp>
#include <carbon/sd_thread.hpp>
#include <carbon/trace_thread.hpp>

#include <cmsis_os.h>

#include <task.h>

static DiagThread diagThread;
#ifdef FREERTOS_USE_TRACE
static TraceThread traceThread;
#endif
static SDThread sdThread;

extern "C" {
void netif_config(void);
void start_micropython();
}

MainThread::MainThread()
    : Thread("main_thread", osPriorityNormal, configMINIMAL_STACK_SIZE * 10) {}

void MainThread::run() {
    diagThread.start();

    netif_config();
#ifdef FREERTOS_USE_TRACE
    traceThread.start();
#endif

    sdThread.start();

    osDelay(200);

    start_micropython();

    while (1) {
        BSP_LED_Toggle(LED_GREEN);
        osDelay(1000);
    }
}
