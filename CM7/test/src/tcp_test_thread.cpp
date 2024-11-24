/**
 ******************************************************************************
 * @file           tcp_test_thread.cpp
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

#include <carbon/tcp_test_thread.hpp>

#include <carbon/diag.hpp>
#include <carbon/error.hpp>

#define CARBON_TCP_TEST_PORT 9999

TCPTestThread::TCPTestThread()
    : Thread("tcp_test_thread", osPriorityNormal,
             configMINIMAL_STACK_SIZE * 64) {}

void TCPTestThread::run() {
    auto *conn_ = netconn_new(NETCONN_TCP);
    if (!conn_) {
        DIAG(ETH_TEST_DIAG "Failed to create new netconn");
        while (1)
            ;
    }

    auto port = uint16_t{CARBON_TCP_TEST_PORT};
    auto err = netconn_bind(conn_, nullptr, port);
    if (err != ERR_OK) {
        DIAG(ETH_TEST_DIAG "Can't bind to port %" PRIu16 ": %d", port, err);
        netconn_delete(conn_);
        while (1)
            ;
    }

    err = netconn_listen_with_backlog(conn_, 1);
    if (err != ERR_OK) {
        DIAG(ETH_TEST_DIAG "Can't listen to port %" PRIu16 ": %d", port, err);
        netconn_delete(conn_);
        while (1)
            ;
    }

    DIAG(ETH_TEST_DIAG "Accepting connections on port %" PRIu16, port);

    while (1) {
        err = netconn_accept(conn_, &newConn_);
        if (err == ERR_OK && newConn_) {
            DIAG(ETH_TEST_DIAG "start testing");
            receiveTestFile();
        }
        if (newConn_) {
            netconn_close(newConn_);
            netconn_delete(newConn_);
            newConn_ = nullptr;
        }
    }
}

void TCPTestThread::receiveTestFile() {
    uint32_t totalRecv = 0;
    recvNetbuf_ = nullptr;
    recvBuf_ = nullptr;
    recvBufSize_ = 0;
    uint8_t c = 0;
    uint32_t blockIndex = 0;
    nblock_ = 0;
    while (1) {
        err_t error = netconn_recv(newConn_, &recvNetbuf_);
        if (error == ERR_OK && recvNetbuf_) {
            netbuf_data(recvNetbuf_, &recvBuf_, &recvBufSize_);
            while (recvBufSize_ > 0) {
                c = *(reinterpret_cast<uint8_t *>(recvBuf_));
                block_[blockIndex] = c;
                blockIndex++;
                if (blockIndex == TCP_TEST_BLOCK_SIZE) {
                    blockIndex = 0;
                    if (!checkBlock())
                        DIAG(ETH_TEST_DIAG "ERROR block %lu corrupted",
                             nblock_);
                    nblock_++;
                    if (nblock_ % 10000 == 0)
                        DIAG(ETH_TEST_DIAG "checked %lu blocks", nblock_);
                }
                recvBuf_ = static_cast<uint8_t *>(recvBuf_) + 1;
                recvBufSize_--;
                totalRecv++;
            }
            netbuf_delete(recvNetbuf_);
            recvNetbuf_ = nullptr;
            recvBuf_ = nullptr;
        } else {
            break;
        }
    }

    DIAG(ETH_TEST_DIAG "data received %lu ", totalRecv);
    DIAG(ETH_TEST_DIAG "blocks received %lu ", nblock_);
}

bool TCPTestThread::checkBlock() {
    bool isOK = true;

    // Check the first 27 bytes are 0x00
    for (size_t i = 0; i < 27; ++i) {
        if (block_[i] != 0x00) {
            // printf("Error: Byte %zu is not 0x00, but %u\n", i, block_[i]);
            isOK = false;
        }
    }

    // Check the next 99 bytes are 0xAE
    for (size_t i = 27; i < 126; ++i) {
        if (block_[i] != 0xAE) {
            // printf("Error: Byte %zu is not 0xAE, but %u\n", i, block_[i]);
            isOK = false;
        }
    }

    // Check the next 129 bytes have the same index value
    for (size_t i = 126; i < 255; ++i) {
        if (block_[i] != static_cast<uint8_t>(nblock_)) {
            // printf("Error: Byte %zu is not equal to index value %u, but
            // %u\n", i, index_value, block_[i]);
            isOK = false;
        }
    }

    // Check the next 70 bytes are 0xAA
    for (size_t i = 255; i < 325; ++i) {
        if (block_[i] != 0xAA) {
            // printf("Error: Byte %zu is not 0xAA, but %u\n", i, block_[i]);
            isOK = false;
        }
    }

    // Check the remaining 187 bytes are 0x01
    for (size_t i = 325; i < TCP_TEST_BLOCK_SIZE; ++i) {
        if (block_[i] != 0x01) {
            // printf("Error: Byte %zu is not 0x01, but %u\n", i, block_[i]);
            isOK = false;
        }
    }

    return isOK;
}