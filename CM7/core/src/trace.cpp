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
#ifdef FREERTOS_USE_TRACE

#include <carbon/diag.hpp>
#include <carbon/hsem.hpp>
#include <carbon/shared_memory.hpp>
#include <carbon/systime.hpp>
#include <carbon/trace.hpp>

#include <cstring>

#define CARBON_TRACE_TCP_PORT 18888
#define EVENT_PULL_PAUSE 5 // ms

namespace CARBON {

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
        pullEvent(newConn);
        DIAG(TRACE_DIAG "Client disconnected");
        netconn_close(newConn);
        netconn_delete(newConn);
    }
}

void Trace::pullEvent(struct netconn *conn) {
    while (1) {
        osDelay(EVENT_PULL_PAUSE);
        traceFifoClass::ContextPull context(traceFifo, hsemTrace);
        uint32_t len1;
        uint32_t len2;
        uint32_t ptr1;
        uint32_t ptr2;

        context.getDataLengthByte(len1, len2);
        context.getDataPtr(ptr1, ptr2);

        if (len1 + len2 > 0) {
            TracePacketHeader hdr;
            auto size = sizeof(TracePacketHeader) + len1 + len2;
            if (size > TracePacketHeader::MAX_PACKET_SIZE_BYTES) {
                RAW_DIAG("exceeded maximum size for trace packet header %lu",
                         size);
                return;
            }
            hdr.streamId =
                static_cast<TracePacketHeader::StreamIdType>(TraceStream::CM7);
            hdr.cpuId =
                static_cast<TracePacketHeader::CpuIdType>(TraceStream::CM7);
            hdr.timestamp = systimeUs();
            hdr.packetSizeBits = size << 3;

            auto res = netconn_write(conn, &hdr, sizeof(TracePacketHeader),
                                     NETCONN_COPY);
            if (res != ERR_OK) {
                RAW_DIAG("error sending trace packet header %d", res);
                return;
            }

            res = netconn_write(conn, reinterpret_cast<const void *>(ptr1),
                                len1, NETCONN_COPY);
            if (res != ERR_OK) {
                RAW_DIAG("error sending data1 from trace fifo %d", res);
                return;
            }

            if (len2 > 0) {
                res = netconn_write(conn, reinterpret_cast<const void *>(ptr2),
                                    len2, NETCONN_COPY);
                if (res != ERR_OK) {
                    RAW_DIAG("error sending data1 from trace fifo %d", res);
                    return;
                }
            }
        }
    }
}

static Trace traceSingleton;

Trace &Trace::instance() { return traceSingleton; }

} // namespace CARBON

#endif
