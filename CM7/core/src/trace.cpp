/**
 ******************************************************************************
 * @file           trace.cpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Jan. 2024
 * @brief          class to handle and send trace packets
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

#include <carbon/diag.hpp>
#include <carbon/trace.hpp>

#include <cstring>

#define CARBON_TRACE_TCP_PORT 18888

namespace CARBON {

#ifdef FREERTOS_USE_TRACE
Trace::~Trace() {}

bool Trace::init() {
    if (conn_) {
        return true;
    }

    auto *conn = netconn_new(NETCONN_TCP);
    if (!conn) {
        DIAG(TRACE_DIAG "Failed to create new netconn");
        return false;
    }

    auto port = uint16_t{CARBON_TRACE_TCP_PORT};
    auto err = netconn_bind(conn, nullptr, port);
    if (err != ERR_OK) {
        DIAG(TRACE_DIAG "Can't bind to port %" PRIu16 ": %d", port, err);
        netconn_delete(conn);
        return false;
    }

    err = netconn_listen_with_backlog(conn, 1);
    if (err != ERR_OK) {
        DIAG(TRACE_DIAG "Can't listen to port %" PRIu16 ": %d", port, err);
        netconn_delete(conn);
        return false;
    }

    DIAG(TRACE_DIAG "Accepting connections on port %" PRIu16, port);

    conn_ = conn;

    return true;
}

void Trace::runConnection() {
    int acceptTry = 0;
    while (true) {
        struct netconn *newConn = nullptr;
        auto err = netconn_accept(conn_, &newConn);
        if (err != ERR_OK) {
            DIAG(TRACE_DIAG "Accept failed: %d", err);
            osDelay(1000);
            if (++acceptTry == 5) {
                DIAG(TRACE_DIAG "accept failed 5 times, returning");
                netconn_close(newConn);
                netconn_delete(newConn);
                return;
            }
            continue;
        }
        acceptTry = 0;
        DIAG(TRACE_DIAG "Client connected");
        sendTrace(newConn);
        DIAG(TRACE_DIAG "Client disconnected");
        netconn_close(newConn);
        netconn_delete(newConn);
    }
}

void Trace::sendTrace(struct netconn *conn) {
    const char *test = "test\n\r";
    auto mysize = strlen(test);
    while (1) {
        auto err = netconn_write(conn, test, mysize, NETCONN_COPY);
        if (err != ERR_OK) {
            DIAG(TRACE_DIAG "Send error: %d", err);
            return;
        }
    }
    return;
}

static Trace traceSingleton;

Trace &Trace::instance() { return traceSingleton; }
#endif

} // namespace CARBON
