/**
 ******************************************************************************
 * @file           hsem.hpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Oct. 2022
 * @brief          DISCO-STM32H747 hardware semaphore for IPC, CM4 CPU
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

extern "C" {

void hsemNVIC(void) {
    HAL_NVIC_SetPriority(HSEM2_IRQn, 0x0F, 0);
    HAL_NVIC_EnableIRQ(HSEM2_IRQn);
}

void hsem_notify_isr(uint32_t /*mask*/) {}
}
