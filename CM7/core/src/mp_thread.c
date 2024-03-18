/**
 ******************************************************************************
 * @file           mp_thread.c
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Mar. 2024
 * @brief          source file for the micropython task
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
#include <mpcarbon.h>

#include <cmsis_os.h>
#include <task.h>

#include <carbon/diag.hpp>

#define MICROPY_TASK_PRIORITY (1)
#define MICROPY_TASK_STACK_SIZE 2097152U /*2 MB size micropython stack*/
#define MICROPY_TASK_STACK_LEN (MICROPY_TASK_STACK_SIZE / sizeof(StackType_t))
#define MICROPYTHON_HEAP_SIZE 2097152U /*2 MB size micropython heap*/

// This is the static memory (TCB and stack) for the main MicroPython task
static StaticTask_t mpTaskTCB
    __attribute__((aligned(8), section(".sdram_bank2")));

static StackType_t mpTaskStack[MICROPY_TASK_STACK_LEN]
    __attribute__((aligned(8), section(".sdram_bank2")));

static uint8_t micropython_heap[MICROPYTHON_HEAP_SIZE]
    __attribute__((aligned(32), section(".sdram_bank2")));

uintptr_t cortex_m7_get_sp(void);
static uint8_t *sp;
static void TASK_MicroPython(void *pvParameters);

void TASK_MicroPython(void *pvParameters) {
    sp = (uint8_t *)cortex_m7_get_sp();

    DIAG(MP "starting micropython");
    DIAG(MP "stack at %p size %u", mpTaskStack, MICROPY_TASK_STACK_SIZE);
    DIAG(MP "heap at %p size %u", micropython_heap, MICROPYTHON_HEAP_SIZE);
    DIAG(MP "executing script at %p", pvParameters);

    mp_carbon_init(&mpTaskStack[0], MICROPY_TASK_STACK_LEN,
                   &micropython_heap[0], MICROPYTHON_HEAP_SIZE, sp);

    if (pvParameters) {
        mp_carbon_exec_str((const char *)pvParameters);
    } else {
        DIAG(MP "passed a null ptr");
    }

    DIAG(MP "python script execution terminated");

    for (;;) {
        osDelay(10000);
    }
}

void startMicropython(void *exec) {
    TaskHandle_t taskHandle =
        xTaskCreateStatic(TASK_MicroPython, "MicroPy", MICROPY_TASK_STACK_LEN,
                          exec, MICROPY_TASK_PRIORITY, mpTaskStack, &mpTaskTCB);
    if (taskHandle == NULL) {
        DIAG(MP "failed to start the micropython task");
    } else {
        DIAG(MP "micropython task started");
    }
}
