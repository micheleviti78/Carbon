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

#include <carbon/diag.hpp>
#include <carbon/main_thread.hpp>
#include <carbon/pin.hpp>

#include <cmsis_os.h>

#include <task.h>

extern "C" {

void netif_config(void);

/**
 * @brief  Initializes the lwIP stack
 * @param  None
 * @retval None
 */

void mainThread(const void *argument) {
    RAW_DIAG("FreeRTOS version %d.%d.%d", tskKERNEL_VERSION_MAJOR,
             tskKERNEL_VERSION_MINOR, tskKERNEL_VERSION_BUILD);
    netif_config();
    while (1) {
        BSP_LED_Toggle(LED_GREEN);
        osDelay(1000);
    }
}
}
