/**
 ******************************************************************************
 * @file           ftp_thread.cpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Feb. 2024
 * @brief          ftp server thread
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
#include <carbon/ftp_thread.hpp>

#include <cmsis_os.h>

extern "C" {
#include <ftp.h>
}

FTPThread::FTPThread()
    : Thread("ftp_thread", osPriorityNormal, configMINIMAL_STACK_SIZE * 8) {}

void FTPThread::run() {
    DIAG(FTP "starting FTP server");
    ftp_server();
}
