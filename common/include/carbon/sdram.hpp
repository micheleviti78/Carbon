/**
 ******************************************************************************
 * @file           sdram.hpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Feb. 2022
 * @brief          DISCO-STM32H747 FMC, header file
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
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void init_sdram();
#define SDRAM_BANK2_ADDR 0xD0000000

#ifdef __cplusplus
}
#endif
