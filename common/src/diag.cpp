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
#include <carbon/hsem.hpp>
#include <carbon/shared_memory.hpp>
#include <carbon/sync.hpp>
#include <carbon/uart.hpp>

#include <stm32h7xx_hal.h>

#include <printf.h>

#ifdef __cplusplus
extern "C" {
#endif

extern UART_HandleTypeDef huart1;
static HSEMSpinLock<HSEM_ID::NotifyDiag> hsemDiag;

void putchar_(char ch) {
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
}

static void fifo_output(char character, void *arg) {
    DiagFifo::Context *context = reinterpret_cast<DiagFifo::Context *>(arg);
    context->push(character);
}

void carbon_raw_diag_print(const char *format, ...) {
    LockGuard<HSEMSpinLock<HSEM_ID::NotifyDiag>> Lock(hsemDiag);
    va_list vl;
    va_start(vl, format);
    vprintf_(format, vl);
    va_end(vl);
    printf_("\r\n");
}

void carbon_diag_push(const char *format, ...) {
    va_list vl;
    va_start(vl, format);
    /*getting stream length*/
    int len = vsnprintf_(nullptr, 0, format, vl);
    va_end(vl);
    DiagFifo::Context context(diagFifo, len + 2, hsemDiag);
    if (context.isOverflow()) {
        carbon_raw_diag_print("???????????");
        return;
    }
    /*writing into buffer, use vfctprintf (function to be implemented)*/
    vfctprintf(fifo_output, &context, format, vl);
    va_end(vl);
    fctprintf(fifo_output, &context, "\r\n");
    /*close fifo context*/
}

void carbon_diag_pull() {
    uint8_t ch;
    while (diagFifo.pop(ch, hsemDiag)) {
        HAL_UART_Transmit(&huart1, &ch, 1, 0xFFFF);
    }
}

#ifdef __cplusplus
}
#endif
