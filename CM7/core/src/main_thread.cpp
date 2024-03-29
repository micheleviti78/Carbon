/**
 ******************************************************************************
 * @file           main_thread.cpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Nov. 2022
 * @brief          CM7 main thread source
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
#include <carbon/main_thread.hpp>
#include <carbon/mp_thread.h>
#include <carbon/pin.hpp>
#include <carbon/sd_thread.hpp>
#include <carbon/trace_thread.hpp>

#include <cmsis_os.h>

#include <task.h>

extern "C" {

void netif_config(void);

void mainThread(const void *argument) {
    start_diag_thread();

    netif_config();
#ifdef FREERTOS_USE_TRACE
    start_trace_thread();
#endif

    start_sd_thread();

    osDelay(200);

    start_micropython();

    while (1) {
        BSP_LED_Toggle(LED_GREEN);
        osDelay(1000);
    }
}
}
