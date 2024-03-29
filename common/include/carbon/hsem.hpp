/**
 ******************************************************************************
 * @file           hsem.hpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Oct. 2022
 * @brief          DISCO-STM32H747 hardware semaphore for IPC
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

#pragma once

#include <carbon/irq.hpp>

#include <stm32h7xx_hal.h>

#include <cstdint>

extern "C" void hsemInit();

namespace CARBON {

enum class HSEM_ID : uint32_t { InitSync, Diag, Trace };

template <HSEM_ID hsemID> class HSEMSpinLock {
public:
    HSEMSpinLock() = default;

    PREVENT_COPY_AND_MOVE(HSEMSpinLock);

    static void get() {
        IRQ::lockRecursive();
        uint32_t id = static_cast<uint32_t>(hsemID);
        while (HSEM->RLR[id] != (HSEM_CR_COREID_CURRENT | HSEM_RLR_LOCK)) {
            __NOP();
        }
        __DMB();
    }

    static void release() {
        uint32_t id = static_cast<uint32_t>(hsemID);
        __DMB();
        HSEM->R[id] = HSEM_CR_COREID_CURRENT;
        IRQ::unLockRecursive();
    }

    static inline void enableNotification() {
        uint32_t semId = static_cast<uint32_t>(hsemID);
        HSEM_COMMON->IER |= 1U << semId;
    }

    static inline void disableNotification() {
        uint32_t semId = static_cast<uint32_t>(hsemID);
        HSEM_COMMON->IER &= ~(1U << semId);
    }

    static inline void clearNotification() {
        uint32_t semId = static_cast<uint32_t>(hsemID);
        HSEM_COMMON->ICR = (1U << semId);
    }
};

extern HSEMSpinLock<HSEM_ID::InitSync> hSemInitSync;

extern HSEMSpinLock<HSEM_ID::Trace> hsemTrace;

} // namespace CARBON
