/**
 ******************************************************************************
 * @file           diag_thread.c
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Dec. 2022
 * @brief          starting diag thread
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
#include <carbon/pin.hpp>

#include <cmsis_os.h>

static osThreadId diag_handle;

static void diag_thread(const void *argument);

void start_diag_thread(void) {
    osThreadDef(Diag, diag_thread, osPriorityNormal, 0,
                configMINIMAL_STACK_SIZE * 10);
    diag_handle = osThreadCreate(osThread(Diag), NULL);
}

void diag_thread(const void *argument) {
    while (1) {
        carbon_diag_pull();
        osDelay(1);
    }
}
