/**
 ******************************************************************************
 * @file           rand.hpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Nov. 2022
 * @brief          random generator
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

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void carbon_rand_init(void);

uint32_t carbon_rand(void);

#ifdef __cplusplus
}
#endif
