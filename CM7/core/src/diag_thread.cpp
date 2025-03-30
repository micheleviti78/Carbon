/**
 ******************************************************************************
 * @file           diag_thread.cpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Dec. 2022
 * @brief          diag thread
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
#include <carbon/diag_thread.hpp>

DiagThread::DiagThread()
    : Thread("diag_thread", osPriorityBelowNormal,
             configMINIMAL_STACK_SIZE * 10) {}

void DiagThread::run() {
    DIAG(SYSTEM_DIAG "starting pulling thread");
    while (1) {
        carbon_diag_pull();
        osDelay(1);
    }
}
