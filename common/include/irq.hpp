/**
 ******************************************************************************
 * @file           irq.hpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Sep. 2022
 * @brief          DISCO-STM32H747 basic IRQ interface
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

#include <common.hpp>

#include <stm32h7xx.h>

class IRQ {
public:
    PREVENT_COPY_AND_MOVE(IRQ)

    enum LockStatus : bool { Locked = true, Unlocked = false };

    static inline void lock() { __disable_irq(); }

    static inline void unlock() { __enable_irq(); }

    static inline void lockRecursive() {
        auto startStatus = isLocked();
        __disable_irq();
        if (++irqLockCounter == 1)
            irqLockStartStatus = startStatus;
    }

    static inline void unLockRecursive() {
        if (irqLockCounter != 0) {
            if (--irqLockCounter == 0) {
                if (irqLockStartStatus == LockStatus::Unlocked)
                    __enable_irq();
            }
        }
    }

    static inline LockStatus isLocked() {
        return ((__get_PRIMASK() & 0x01) ? LockStatus::Locked
                                         : LockStatus::Unlocked);
    }

    static inline bool isInIRQ() { return 0 != __get_IPSR(); }

private:
    IRQ() = default;

    static LockStatus irqLockStartStatus;
    static uint32_t irqLockCounter;
};

class IRQLockRecursive {
public:
    inline void get() { IRQ::lockRecursive(); }

    inline void release() { IRQ::unLockRecursive(); }
};