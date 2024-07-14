/**
 ******************************************************************************
 * @file           thread.cpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Jul. 2024
 * @brief          C++ wrapper for CMSIS threads
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

#include <carbon/common.hpp>
#include <carbon/diag.hpp>

#include <cmsis_os.h>

class Thread {
public:
    // Constructor to initialize the thread attributes
    Thread(const char *name, osPriority priority = osPriorityNormal,
           uint32_t stackSize = 1024)
        : name_(name), priority_(priority), stackSize_(stackSize),
          id_(nullptr) {}

    // Destructor to terminate the thread
    virtual ~Thread() {
        if (id_ != nullptr) {
            osThreadTerminate(id_);
        }
    }

    // Function to start the thread
    void start() {
        osThreadDef_t thread_def;
        thread_def.pthread = &Thread::threadEntry;
        thread_def.tpriority = priority_;
        thread_def.instances = 0; // Single instance of the thread
        thread_def.stacksize = stackSize_;
        thread_def.name = const_cast<char *>(name_); // Cast away constness
        thread_def.buffer = NULL;
        thread_def.controlblock = NULL;
        id_ = osThreadCreate(&thread_def, this);
        ASSERT(id_ != nullptr);
    }

    // Function to get the thread id
    osThreadId getId() const { return id_; }

protected:
    // Pure virtual method to be implemented by derived classes
    virtual void run() = 0;

private:
    static void threadEntry(void const *argument) {
        Thread *thread = static_cast<Thread *>(const_cast<void *>(argument));
        thread->run();
    }

    const char *name_;    // Thread name
    osPriority priority_; // Thread priority
    uint32_t stackSize_;  // Stack size for the thread
    osThreadId id_;       // Thread ID
};
