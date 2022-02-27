/**
 ******************************************************************************
 * @file           pin.cpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Feb. 2022
 * @brief          DISCO-STM32H747 GPIO Interface, source file
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

#include <pin.hpp>
#include <stm32h7xx_hal.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LED1_GPIO_PORT GPIOI
#define LED1_PIN GPIO_PIN_12

#define LED2_GPIO_PORT GPIOI
#define LED2_PIN GPIO_PIN_13

#define LED3_GPIO_PORT GPIOI
#define LED3_PIN GPIO_PIN_14

#define LED4_GPIO_PORT GPIOI
#define LED4_PIN GPIO_PIN_15

#define LEDx_GPIO_CLK_ENABLE() __HAL_RCC_GPIOI_CLK_ENABLE()
#define LEDx_GPIO_CLK_DISABLE() __HAL_RCC_GPIOI_CLK_DISABLE()

static GPIO_TypeDef *LED_PORT[LEDn] = {LED1_GPIO_PORT, LED2_GPIO_PORT,
                                       LED3_GPIO_PORT, LED4_GPIO_PORT};

static const uint32_t LED_PIN[LEDn] = {LED1_PIN, LED2_PIN, LED3_PIN, LED4_PIN};

/**
 * @brief  Configures LED on GPIO.
 * @param  Led LED to be configured.
 *          This parameter can be one of the following values:
 *            @arg  LED1
 *            @arg  LED2
 *            @arg  LED3
 *            @arg  LED4
 * @retval BSP status
 */
int32_t BSP_LED_Init(Led_TypeDef Led) {
  int32_t ret = 0;
  GPIO_InitTypeDef GPIO_InitStruct{0, 0, 0, 0, 0};

  /* Enable the GPIO_LED clock */
  LEDx_GPIO_CLK_ENABLE();

  /* Configure the GPIO_LED pin */
  GPIO_InitStruct.Pin = LED_PIN[Led];
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

  HAL_GPIO_Init(LED_PORT[Led], &GPIO_InitStruct);

  /* By default, turn off LED */
  HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_SET);
  return ret;
}

/**
 * @brief  DeInit LEDs.
 * @param  Led LED to be configured.
 *          This parameter can be one of the following values:
 *            @arg  LED1
 *            @arg  LED2
 *            @arg  LED3
 *            @arg  LED4
 * @retval BSP status
 */
int32_t BSP_LED_DeInit(Led_TypeDef Led) {
  int32_t ret = 0;
  GPIO_InitTypeDef gpio_init_structure{0, 0, 0, 0, 0};

  /* DeInit the GPIO_LED pin */
  gpio_init_structure.Pin = LED_PIN[Led];
  /* Turn off LED */
  HAL_GPIO_WritePin(LED_PORT[Led], (uint16_t)LED_PIN[Led], GPIO_PIN_SET);
  HAL_GPIO_DeInit(LED_PORT[Led], gpio_init_structure.Pin);
  return ret;
}

/**
 * @brief  Turns selected LED On.
 * @param  Led LED to be set on
 *          This parameter can be one of the following values:
 *            @arg  LED1
 *            @arg  LED2
 *            @arg  LED3
 *            @arg  LED4
 * @retval BSP status
 */
int32_t BSP_LED_On(Led_TypeDef Led) {
  int32_t ret = 0;

  HAL_GPIO_WritePin(LED_PORT[Led], (uint16_t)LED_PIN[Led], GPIO_PIN_RESET);
  return ret;
}

/**
 * @brief  Turns selected LED Off.
 * @param  Led LED to be set off
 *          This parameter can be one of the following values:
 *            @arg  LED1
 *            @arg  LED2
 *            @arg  LED3
 *            @arg  LED4
 * @retval BSP status
 */
int32_t BSP_LED_Off(Led_TypeDef Led) {
  int32_t ret = 0;
  HAL_GPIO_WritePin(LED_PORT[Led], (uint16_t)LED_PIN[Led], GPIO_PIN_SET);
  return ret;
}

/**
 * @brief  Toggles the selected LED.
 * @param  Led LED to be toggled
 *          This parameter can be one of the following values:
 *            @arg  LED1
 *            @arg  LED2
 *            @arg  LED3
 *            @arg  LED4
 * @retval BSP status
 */
int32_t BSP_LED_Toggle(Led_TypeDef Led) {
  int32_t ret = 0;
  HAL_GPIO_TogglePin(LED_PORT[Led], (uint16_t)LED_PIN[Led]);
  return ret;
}
/**
 * @brief  Get the selected LED state.
 * @param  Led LED to be get its state
 *          This parameter can be one of the following values:
 *            @arg  LED1
 *            @arg  LED2
 *            @arg  LED3
 *            @arg  LED4
 * @retval LED status
 */
int32_t BSP_LED_GetState(Led_TypeDef Led) {
  int32_t ret = 0;
  ret = (int32_t)HAL_GPIO_ReadPin(LED_PORT[Led], (uint16_t)LED_PIN[Led]);
  return ret;
}

#ifdef __cplusplus
}
#endif