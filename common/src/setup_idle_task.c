/**
 ******************************************************************************
 * @file           setup_idle_task.c
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Jul. 2024
 * @brief          setup idle task
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
#include <cmsis_os.h>

#include <FreeRTOS.h>

#ifdef configSUPPORT_STATIC_ALLOCATION
// This is the static memory (TCB and stack) for the idle task
static StaticTask_t xIdleTaskTCB; /*__attribute__((section(".rtos_heap")));*/
static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE]
    __attribute__((aligned(8)));

// We need this when configSUPPORT_STATIC_ALLOCATION is enabled
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                   StackType_t **ppxIdleTaskStackBuffer,
                                   uint32_t *pulIdleTaskStackSize) {
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
#else
#warning "supporting only dynamic allocation"
#endif