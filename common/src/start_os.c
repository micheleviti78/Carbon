/**
 ******************************************************************************
 * @file           start_os.c
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Nov. 2022
 * @brief          starting OS
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
#include <carbon/main_thread.hpp>

#include <cmsis_os.h>

#include <task.h>

// This is the static memory (TCB and stack) for the idle task
static StaticTask_t xIdleTaskTCB; /*__attribute__((section(".rtos_heap")));*/
static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE]
    __attribute__((aligned(8)));

static osThreadId main_task_handle;

void start_os(void) {
    osThreadDef(main_thread, mainThread, osPriorityNormal, 0,
                configMINIMAL_STACK_SIZE * 10);
    main_task_handle = osThreadCreate(osThread(main_thread), NULL);

    DIAG(SYSTEM_DIAG "starting OS");

    osKernelStart();

    RAW_DIAG(SYSTEM_DIAG "ERROR OS");

    while (1) {
    }
}

// We need this when configSUPPORT_STATIC_ALLOCATION is enabled
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                   StackType_t **ppxIdleTaskStackBuffer,
                                   uint32_t *pulIdleTaskStackSize) {
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
