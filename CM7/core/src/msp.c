/**
 ******************************************************************************
 * @file         msp.c
 * @brief        MCU Support Package Init and Deinit
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <stm32h7xx_hal.h>

/**
 * Initializes the Global MSP.
 */
void HAL_MspInit(void) { __HAL_RCC_SYSCFG_CLK_ENABLE(); }
