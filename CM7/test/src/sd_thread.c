/**
 ******************************************************************************
 * @file           sd_thread.cpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Feb. 2024
 * @brief          thread to test the SD card, c code
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

static osThreadId sd_test_handle;

void sd_test_thread(const void *argument);

void start_sd_test_thread_imp(void) {
    osThreadDef(SD_Test, sd_test_thread, osPriorityNormal, 0,
                configMINIMAL_STACK_SIZE * 5);
    sd_test_handle = osThreadCreate(osThread(SD_Test), NULL);
}