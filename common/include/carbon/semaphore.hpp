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

#include <cmsis_os.h>

#include <stm32h7xx.h>

class Semaphore {
public:
    Semaphore(uint32_t count) : count_(count), isInit_(false) {}

    // CMSIS-RTOS doesn't provide a specific delete function for semaphores
    // Typically, resources should be properly managed by the application
    virtual ~Semaphore() = default;

    PREVENT_COPY_AND_MOVE(Semaphore)

    void init() {
        __disable_irq();
        if (!isInit_) {
            osSemaphoreDef_t sem_def;
            _semaphore = osSemaphoreCreate(&sem_def, count);
            ASSERT(_semaphore != NULL);
            isInit_ = true;
        }
        __enable_irq();
    };

    bool acquire(uint32_t timeout = osWaitForever) {
        ASSERT(isInit);
        return osSemaphoreWait(_semaphore, timeout) == osOK;
    }

    bool release() {
        ASSERT(isInit);
        return osSemaphoreRelease(_semaphore) == osOK;
    }
}

protected :

    uint32_t count_;
bool isInit_;
osSemaphoreId _semaphore;

class BinarySemaphore : public Semaphore {
public:
    BinarySemaphore() : Semaphore(1) {
        osSemaphoreWait(_semaphore, 0); // reset status of the Semaphore
    }
    ~BinarySemaphore() override = default;
};