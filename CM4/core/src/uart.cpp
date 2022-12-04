/**
 ******************************************************************************
 * @file           uart.cpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Dec. 2022
 * @brief          DISCO-STM32H747 UART driver
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

#include <error.hpp>
#include <pin.hpp>

#include <stm32h7xx_hal.h>

#ifdef __cplusplus
extern "C" {
#endif

UART_HandleTypeDef huart1 __attribute__((section(".uart_struct")));

#ifdef __cplusplus
}
#endif
