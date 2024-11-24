/**
 ******************************************************************************
 * @file           main_thread.cpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Nov. 2022
 * @brief          CM7 main thread class
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

#include <carbon/diag_thread.hpp>
#include <carbon/display_matrix_spi.hpp>
#include <carbon/ftp_thread.hpp>
#include <carbon/main_thread.hpp>
#include <carbon/mp_thread.h>
#include <carbon/pin.hpp>
#include <carbon/sd_thread.hpp>
#include <carbon/tcp_test_thread.hpp>
#include <carbon/trace_thread.hpp>

#include <cmsis_os.h>

#if 0
static uint32_t buffer1 __attribute__((aligned(4))) = 0xA5A5A5A5;
static uint32_t buffer2 __attribute__((aligned(4))) = 0xAABBCCDD;
static uint32_t buffer3 __attribute__((aligned(4))) = 0xFFEEDDCC;
#endif

static DiagThread diagThread;
#ifdef FREERTOS_USE_TRACE
static TraceThread traceThread;
#endif
static SDThread sdThread;
static FTPThread ftpThread;
static TCPTestThread tcpTestThread;

extern "C" {
void netif_config(void);
void start_micropython();
}

MainThread::MainThread()
    : Thread("main_thread", osPriorityNormal, configMINIMAL_STACK_SIZE * 10) {}

void MainThread::run() {
    diagThread.start();

    netif_config();
#ifdef FREERTOS_USE_TRACE
    traceThread.start();
#endif

    /*init matrix display spi*/
    if (getDisplayMatrixSpi().init()) {
        Error_Handler();
    }
#if 0
    if (getDisplayMatrixSpi().DMATransmit(&buffer1, 1))
        DIAG(SYSTEM_DIAG "error transmitting the data");
    if (getDisplayMatrixSpi().DMATransmit(&buffer2, 1))
        DIAG(SYSTEM_DIAG "error transmitting the data");
    if (getDisplayMatrixSpi().DMATransmit(&buffer3, 1))
        DIAG(SYSTEM_DIAG "error transmitting the data");
#endif
    sdThread.start();

    osDelay(200);

    start_micropython();

    ftpThread.start();

    tcpTestThread.start();

    while (1) {
        BSP_LED_Toggle(LED_GREEN);
        osDelay(1000);
    }
}
