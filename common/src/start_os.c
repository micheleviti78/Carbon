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

#include <diag.hpp>
#include <main_thread.hpp>

#include <cmsis_os.h>

static osThreadId main_task_handle;

void start_os(void) {
    osThreadDef(main_thread, mainThread, osPriorityNormal, 0, 256);
    main_task_handle = osThreadCreate(osThread(main_thread), NULL);
    RAW_DIAG("starting OS");
    osKernelStart();
    RAW_DIAG("ERROR OS");
}
