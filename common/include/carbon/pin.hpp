/**
 ******************************************************************************
 * @file           pin.hpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Feb. 2022
 * @brief          DISCO-STM32H747 GPIO Interface, header file
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

#pragma once

#include <stm32h7xx_hal_gpio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    LED1 = 0U,
    LED_GREEN = LED1,
    LED2 = 1U,
    LED_ORANGE = LED2,
    LED3 = 2U,
    LED_RED = LED3,
    LED4 = 3U,
    LED_BLUE = LED4,
    LEDn
} Led_TypeDef;

int32_t BSP_LED_Init(Led_TypeDef Led);
int32_t BSP_LED_DeInit(Led_TypeDef Led);
int32_t BSP_LED_On(Led_TypeDef Led);
int32_t BSP_LED_Off(Led_TypeDef Led);
int32_t BSP_LED_Toggle(Led_TypeDef Led);
int32_t BSP_LED_GetState(Led_TypeDef Led);

#ifdef __cplusplus
}
#endif
