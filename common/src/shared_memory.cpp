/**
 ******************************************************************************
 * @file           shared_memory.cpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Dec. 2022
 * @brief          Shared memory for the IPC
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
#include <carbon/error.hpp>
#include <carbon/shared_memory.hpp>

#include <stm32h7xx_hal.h>

namespace CARBON {

void waitForSyncFlag(SyncFlagBit syncFlagBit) {
    uint32_t timeout = 0xFFFFFFFF;
    uint32_t mask = 0x1 << static_cast<uint32_t>(syncFlagBit);
    while ((syncFlag & mask) == 0 && --timeout > 0) {
    }
    if (timeout == 0)
        Error_Handler();
}

void setSyncFlag(SyncFlagBit syncFlagBit) {
    uint32_t mask = 0x1 << static_cast<uint32_t>(syncFlagBit);
    syncFlag |= mask;
    __DSB();
}

void resetSyncFlag() {
    syncFlag = 0;
    __DSB();
}

volatile uint32_t syncFlag;

/*DIAG FIFO*/

FIFO_DEFINITION(diag)

#ifdef FREERTOS_USE_TRACE
/*TRACE FIFO*/

FIFO_DEFINITION(trace)
#endif

} // namespace CARBON
