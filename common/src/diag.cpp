/**
 ******************************************************************************
 * @file           diag.cpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Feb. 2022
 * @brief          DISCO-STM32H747 Diagnostic Console, source file
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

#include <uart.hpp>

#include <printf.h>
#include <stm32h7xx_hal.h>

#ifdef __cplusplus
extern "C" {
#endif

extern UART_HandleTypeDef huart1;

void _putchar(char ch) {
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
}

void carbon_raw_diag_print(const char *format, ...) {
    va_list vl;
    va_start(vl, format);
    vprintf(format, vl);
    va_end(vl);
    printf("\n\r");
}

#ifdef __cplusplus
}
#endif
