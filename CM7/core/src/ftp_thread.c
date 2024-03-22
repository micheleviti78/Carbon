/**
 ******************************************************************************
 * @file           ftp_thread.c
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Feb. 2024
 * @brief          thread to init the SD card, c code
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

#include <ftp.h>

static osThreadId ftp_handle;

static void ftp_server_wrapper(const void *) { ftp_server(); }

void start_ftp_thread(void) {
    osThreadDef(FTP_Thread, ftp_server_wrapper, osPriorityNormal, 0,
                configMINIMAL_STACK_SIZE * 8);
    ftp_handle = osThreadCreate(osThread(FTP_Thread), NULL);
}