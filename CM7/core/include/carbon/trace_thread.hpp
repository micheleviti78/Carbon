/**
 ******************************************************************************
 * @file           trace_thread.hpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Jan. 2024
 * @brief          trace thread
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

#include <carbon/thread.hpp>
class TraceThread : public Thread {
public:
    TraceThread();
    ~TraceThread() = default;

protected:
    void run() override;
};