/**
 ******************************************************************************
 * @file           oshooks.cpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Nov. 2022
 * @brief          os hooks for debugging
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

#include <carbon/diag.hpp>

#include <FreeRTOS.h>
#include <task.h>

extern "C" {

const uint8_t freeRTOSMemoryScheme __attribute__((unused)) = 4;

void vApplicationStackOverflowHook(TaskHandle_t /*xTask*/, char *taskName) {
    RAW_DIAG("Stack overflow in task %s !", taskName);
    for (;;)
        ;
}

void vApplicationMallocFailedHook(void) { RAW_DIAG("Malloc failed !"); }

} // extern "C"
