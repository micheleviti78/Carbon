/**
 ******************************************************************************
 * @file           tcp_test_thread.hpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Nov. 2024
 * @brief          test for tcp connection
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

#include <lwip/api.h>

#define TCP_TEST_BLOCK_SIZE 512

class TCPTestThread : public Thread {
public:
    TCPTestThread();
    ~TCPTestThread() = default;

protected:
    void run() override;
    void receiveTestFile();
    bool checkBlock();
    struct netconn *conn_{nullptr};
    struct netconn *newConn_{nullptr};
    struct netbuf *recvNetbuf_{nullptr};
    void *recvBuf_{nullptr};
    uint16_t recvBufSize_{0};
    uint8_t block_[TCP_TEST_BLOCK_SIZE]{0};
    uint32_t nblock_{0};
};