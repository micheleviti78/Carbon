/**
 ******************************************************************************
 * @file           hsem.cpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Oct. 2022
 * @brief          DISCO-STM32H747 hardware semaphore for IPC
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

#include <diag.hpp>
#include <hsem.hpp>

__weak void hsem_notify_isr(uint32_t /*mask*/) {}

void hsemNVIC(void);

extern "C" void hsem_isr(void) {
    auto semMask = uint32_t{HSEM_COMMON->MISR};
#ifdef TEST_HSEM
    RAW_DIAG("in ISR MISR %lu, ISR %lu", HSEM_COMMON->MISR, HSEM_COMMON->ISR);
#endif
    HSEM_COMMON->ICR = semMask;
    hsem_notify_isr(semMask);
}

void hsemInit() {
    __HAL_RCC_HSEM_CLK_ENABLE();
    hsemNVIC();
}