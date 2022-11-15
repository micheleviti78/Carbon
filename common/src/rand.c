/**
 ******************************************************************************
 * @file           rand.c
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

#include <diag.hpp>
#include <rand.hpp>

#include <stm32h7xx_hal.h>
#include <stm32h7xx_hal_rng.h>

#include <cmsis_os.h>

static RNG_HandleTypeDef hrng;
static osMutexId rng_mx;

void HAL_RNG_MspInit(RNG_HandleTypeDef *hrng) { __HAL_RCC_RNG_CLK_ENABLE(); }

void HAL_RNG_MspDeInit(RNG_HandleTypeDef *hrng) { __HAL_RCC_RNG_CLK_DISABLE(); }

void carbon_rand_init(void) {
    hrng.Instance = RNG;
    if (HAL_RNG_DeInit(&hrng) != HAL_OK) {
        RAW_DIAG("error while deinitializing RNG");
    }
    if (HAL_RNG_Init(&hrng) != HAL_OK) {
        RAW_DIAG("error while initializing RNG");
    }
    osMutexDef(rng_mx);
    rng_mx = osMutexCreate(osMutex(rng_mx));
}

uint32_t carbon_rand(void) {
    uint32_t val;
    osMutexWait(rng_mx, osWaitForever);
    if (HAL_RNG_GenerateRandomNumber(&hrng, &val) != HAL_OK) {
        RAW_DIAG("error for random generator");
    }
    osMutexRelease(rng_mx);
    return val;
}
