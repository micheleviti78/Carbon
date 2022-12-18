/**
 ******************************************************************************
 * @file           low_level_init.c
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Dec. 2022
 * @brief          DISCO-STM32H747 low level initialization for CM4
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
#include <carbon/hsem.hpp>
#include <carbon/pin.hpp>
#include <carbon/systime.hpp>

#include <stm32h7xx_hal.h>

using namespace CARBON;

extern "C" {

void low_level_init() {
    /*HW semaphore Clock enable*/
    hsemInit();

    /* Activate HSEM notification for Cortex-M4*/
    hSemInitSync.enableNotification();

    /*
    Domain D2 goes to STOP mode (Cortex-M4 in deep-sleep) waiting for Cortex-M7
    to perform system initialization (system clock config, external memory
    configuration.. )
    */

    HAL_PWREx_ClearPendingEvent();
    HAL_PWREx_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_STOPENTRY_WFE,
                            PWR_D2_DOMAIN);

    /* MCU
     * Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the
     * Systick.
     */
    HAL_Init();

    /*init timer*/
    low_level_system_time();

    /* Initialize Pin needed by the Error function */
    BSP_LED_Init(LED_BLUE);
    BSP_LED_Init(LED_RED);

    HAL_Delay(100);
}
}
