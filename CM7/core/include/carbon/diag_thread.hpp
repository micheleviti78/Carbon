/**
 ******************************************************************************
 * @file           diag_thread.hpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Dec. 2022
 * @brief          starting diag thread
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

#include <carbon/thread.hpp>

class DiagThread : public Thread {
public:
    DiagThread();
    ~DiagThread() override = default;

protected:
    void run() override;
};
