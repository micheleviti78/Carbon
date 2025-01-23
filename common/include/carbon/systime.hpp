/**
 ******************************************************************************
 * @file           systime.hpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Feb. 2022
 * @brief          DISCO-STM32H747 system time
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

#ifdef __cplusplus
#include <cstdint>
#else
#include <stdint.h>
#endif

#include <stm32h7xx_hal.h>

#define DWT_CLOCKS DWT->CYCCNT;

#ifdef __cplusplus
extern "C" {
#endif

void low_level_system_time(void);

uint64_t systimeUs();

void delayUs(uint32_t us);

#ifdef __cplusplus
}
#endif
