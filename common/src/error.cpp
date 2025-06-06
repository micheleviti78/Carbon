/**
 ******************************************************************************
 * @file           error.cpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Mar. 2022
 * @brief          DISCO-STM32H747 error hanndler and type
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

#include <carbon/error.hpp>
#include <carbon/pin.hpp>
#include <carbon/systime.hpp>

#include <stm32h7xx_hal.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
    /* User can add his own implementation to report the HAL error return state
     */
    __disable_irq();
    while (1) {
        HAL_Delay(1000);
#ifdef CORE_CM4
        BSP_LED_Toggle(LED_RED);
#else
        BSP_LED_Toggle(LED_ORANGE);
#endif
    }
}

#ifdef __cplusplus
}
#endif