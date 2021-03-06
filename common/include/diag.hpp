/**
 ******************************************************************************
 * @file           diag.hpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Feb. 2022
 * @brief          DISCO-STM32H747 Diagnostic Console, header file
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
extern "C" {
#endif

void init_diag();
void carbon_raw_diag_print(const char *format, ...);

#define RAW_DIAG(...) carbon_raw_diag_print(__VA_ARGS__)

#ifdef __cplusplus
}
#endif
