/**
 ******************************************************************************
 * @file           semaphore
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Jul. 2024
 * @brief          c++ wrapper for semaphore
 ******************************************************************************
 * @attention
 * Copyright (c) 2024 Michele Viti.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#pragma once

#include <carbon/common.hpp>
#include <carbon/diag.hpp>
#include <carbon/irq.hpp>

#include <cmsis_os.h>

#include <stm32h7xx.h>

class Semaphore {
public:
    Semaphore(uint32_t count) : count_(count), isInit_(false) {}

    // CMSIS-RTOS doesn't provide a specific delete function for semaphores
    // Typically, resources should be properly managed by the application
    virtual ~Semaphore() = default;

    PREVENT_COPY_AND_MOVE(Semaphore)

    virtual void init() {
        CARBON::IRQ::lockRecursive();
        if (!isInit_) {
            osSemaphoreDef_t sem_def{0, NULL};
            semaphore_ = osSemaphoreCreate(&sem_def, count_);
            ASSERT(semaphore_ != NULL);
            isInit_ = true;
        }
        CARBON::IRQ::unLockRecursive();
    };

    bool acquire(uint32_t timeout = osWaitForever) {
        ASSERT(isInit_);
        return osSemaphoreWait(semaphore_, timeout) == osOK;
    }

    bool release() {
        ASSERT(isInit_);
        return osSemaphoreRelease(semaphore_) == osOK;
    }

protected:
    uint32_t count_;
    bool isInit_;
    osSemaphoreId semaphore_;
};

class BinarySemaphore : public Semaphore {
public:
    BinarySemaphore() : Semaphore(1) {}
    ~BinarySemaphore() override = default;
    void init() override {
        Semaphore::init();
        osSemaphoreWait(semaphore_, 0); // reset status of the Semaphore
    }
};