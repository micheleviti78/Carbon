/**
 ******************************************************************************
 * @file           ftp_thread.hpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Feb. 2024
 * @brief          ftp thread
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
class FTPThread : public Thread {
public:
    FTPThread();
    ~FTPThread() = default;

protected:
    void run() override;
};