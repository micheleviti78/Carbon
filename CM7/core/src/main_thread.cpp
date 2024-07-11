/**
 ******************************************************************************
 * @file           main_thread.cpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Nov. 2022
 * @brief          CM7 main thread source
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
#include <carbon/main_thread.hpp>
#include <carbon/mp_thread.h>
#include <carbon/pin.hpp>
#include <carbon/sd_thread.hpp>
#include <carbon/trace_thread.hpp>

#include <cmsis_os.h>

#include <task.h>

extern "C" {

static uint32_t buffer __attribute__((aligned(4))) = 0xAAAAAAAA;

void netif_config(void);

void mainThread(const void *argument) {
    start_diag_thread();

    DIAG(SYSTEM_DIAG "buffer pointer %p", &buffer);

    /*init matrix display spi*/
    if (getDisplayMatrixSpi().init()) {
        DIAG(SYSTEM_DIAG "error init");
    } else {
        if (getDisplayMatrixSpi().DMATransmit(&buffer, 4))
            DIAG(SYSTEM_DIAG "error transmitting the data");
        if (getDisplayMatrixSpi().DMATransmit(&buffer, 4))
            DIAG(SYSTEM_DIAG "error transmitting the data");
    }

    netif_config();
#ifdef FREERTOS_USE_TRACE
    start_trace_thread();
#endif

    start_sd_thread();

    osDelay(200);

    start_micropython();

    while (1) {
        BSP_LED_Toggle(LED_GREEN);
        osDelay(1000);
    }
}
}
