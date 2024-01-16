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

// static err_t sendEvent(TraceTasksEvent &event, struct netconn *conn);
//
// static err_t sendEvent(TraceMallocEvent &event, struct netconn *conn);
//
// static err_t sendEvent(TraceFreeEvent &event, struct netconn *conn);
//
// static err_t sendEvent(TraceTaskSwitchedInEvent &event, struct netconn
// *conn);
//
// static err_t sendEvent(TraceTaskSwitchedOutEvent &event, struct netconn
// *conn);
//
// static err_t sendEvent(TracePerfCntEvent &event, struct netconn *conn);

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

// void Trace::pullEvent(struct netconn *conn) {
//    TraceEvent event;
//    while (1) {
//        osDelay(EVENT_PULL_PAUSE);
//        bool res = traceFifo.pop(event, hsemTrace);
//        if (res) {
//
//            err_t err = std::visit(
//                [&](auto &traceEvent) -> err_t {
//                    return sendEvent(traceEvent, conn);
//                },
//                event);
//            if (err != ERR_OK) {
//                DIAG(TRACE_DIAG "Send error: %d", err);
//                return;
//            }
//        }
//    }
//}

void Trace::pullEvent(struct netconn *conn) {
    while (1) {
        osDelay(EVENT_PULL_PAUSE);
        uint8_t bufTemp[sizeof(TraceTaskSwitchedInEvent)]{0};
        uint32_t indexTemp = 0;
        bool res = traceFifo.pop(bufTemp[indexTemp], hsemTrace);
        if (res) {
            indexTemp++;
            while (indexTemp < sizeof(TraceTaskSwitchedInEvent)) {
                traceFifo.pop(bufTemp[indexTemp], hsemTrace);
                indexTemp++;
            }
            TraceTaskSwitchedInEvent *ptr =
                reinterpret_cast<TraceTaskSwitchedInEvent *>(bufTemp);
            // DIAG(TRACE_DIAG "in eventSizeBits %u",
            // ptr->header.eventSizeBits);
            DIAG(TRACE_DIAG "in event->id %u", ptr->header.id);
            // DIAG(TRACE_DIAG "in event->timestamp %llu",
            // ptr->header.timestamp); DIAG(TRACE_DIAG "in event->number %lu",
            // ptr->number);
        }
    }
}

// err_t sendEvent(TraceTasksEvent &event, struct netconn *conn) {
//    TracePacketHeader hdr;
//    auto size = sizeof(TracePacketHeader) + sizeof(TraceTasksEvent);
//    if (size > TracePacketHeader::MAX_PACKET_SIZE_BYTES)
//        return ERR_ARG;
//    hdr.streamId =
//        static_cast<TracePacketHeader::StreamIdType>(TraceStream::CM7);
//    hdr.cpuId = static_cast<TracePacketHeader::CpuIdType>(TraceStream::CM7);
//    hdr.timestamp = systimeUs();
//    hdr.packetSizeBits = size << 3;
//
//    //    DIAG(TRACE_DIAG "task eventSizeBits %u",
//    event.header.eventSizeBits);
//    //    DIAG(TRACE_DIAG "task event->id %u", event.header.id);
//    //    DIAG(TRACE_DIAG "task event->timestamp %llu",
//    event.header.timestamp);
//
//    auto res =
//        netconn_write(conn, &hdr, sizeof(TracePacketHeader), NETCONN_COPY);
//    if (res != ERR_OK) {
//        return res;
//    }
//    return netconn_write(conn, &event, sizeof(TraceTasksEvent), NETCONN_COPY);
//}
//
// err_t sendEvent(TraceMallocEvent &event, struct netconn *conn) {
//    TracePacketHeader hdr;
//    auto size = sizeof(TracePacketHeader) + sizeof(TraceMallocEvent);
//    if (size > TracePacketHeader::MAX_PACKET_SIZE_BYTES)
//        return ERR_ARG;
//    hdr.streamId =
//        static_cast<TracePacketHeader::StreamIdType>(TraceStream::CM7);
//    hdr.cpuId = static_cast<TracePacketHeader::CpuIdType>(TraceStream::CM7);
//    hdr.timestamp = systimeUs();
//    hdr.packetSizeBits = size << 3;
//
//    //    DIAG(TRACE_DIAG "malloc eventSizeBits %u",
//    //    event.header.eventSizeBits); DIAG(TRACE_DIAG "malloc event->id %u",
//    //    event.header.id); DIAG(TRACE_DIAG "malloc event->timestamp %llu",
//    //    event.header.timestamp);
//
//    auto res =
//        netconn_write(conn, &hdr, sizeof(TracePacketHeader), NETCONN_COPY);
//    if (res != ERR_OK) {
//        return res;
//    }
//    return netconn_write(conn, &event, sizeof(TraceMallocEvent),
//    NETCONN_COPY);
//}
//
// err_t sendEvent(TraceFreeEvent &event, struct netconn *conn) {
//    TracePacketHeader hdr;
//    auto size = sizeof(TracePacketHeader) + sizeof(TraceFreeEvent);
//    if (size > TracePacketHeader::MAX_PACKET_SIZE_BYTES)
//        return ERR_ARG;
//    hdr.streamId =
//        static_cast<TracePacketHeader::StreamIdType>(TraceStream::CM7);
//    hdr.cpuId = static_cast<TracePacketHeader::CpuIdType>(TraceStream::CM7);
//    hdr.timestamp = systimeUs();
//    hdr.packetSizeBits = size << 3;
//
//    //    DIAG(TRACE_DIAG "free eventSizeBits %u",
//    event.header.eventSizeBits);
//    //    DIAG(TRACE_DIAG "free event->id %u", event.header.id);
//    //    DIAG(TRACE_DIAG "free event->timestamp %llu",
//    event.header.timestamp);
//
//    auto res =
//        netconn_write(conn, &hdr, sizeof(TracePacketHeader), NETCONN_COPY);
//    if (res != ERR_OK) {
//        return res;
//    }
//    return netconn_write(conn, &event, sizeof(TraceFreeEvent), NETCONN_COPY);
//}
//
// err_t sendEvent(TraceTaskSwitchedInEvent &event, struct netconn *conn) {
//    TracePacketHeader hdr;
//    auto size = sizeof(TracePacketHeader) + sizeof(TraceTaskSwitchedInEvent);
//    if (size > TracePacketHeader::MAX_PACKET_SIZE_BYTES)
//        return ERR_ARG;
//    hdr.streamId =
//        static_cast<TracePacketHeader::StreamIdType>(TraceStream::CM7);
//    hdr.cpuId = static_cast<TracePacketHeader::CpuIdType>(TraceStream::CM7);
//    hdr.timestamp = systimeUs();
//    hdr.packetSizeBits = size << 3;
//
//    //    DIAG(TRACE_DIAG "in eventSizeBits %u", event.header.eventSizeBits);
//    //    DIAG(TRACE_DIAG "in event->id %u", event.header.id);
//    //    DIAG(TRACE_DIAG "in event->timestamp %llu", event.header.timestamp);
//    //    DIAG(TRACE_DIAG "in event->number %lu", event.number);
//
//    auto res =
//        netconn_write(conn, &hdr, sizeof(TracePacketHeader), NETCONN_COPY);
//    if (res != ERR_OK) {
//        return res;
//    }
//    return netconn_write(conn, &event, sizeof(TraceTaskSwitchedInEvent),
//                         NETCONN_COPY);
//}
//
// err_t sendEvent(TraceTaskSwitchedOutEvent &event, struct netconn *conn) {
//    TracePacketHeader hdr;
//    auto size = sizeof(TracePacketHeader) + sizeof(TraceTaskSwitchedOutEvent);
//    if (size > TracePacketHeader::MAX_PACKET_SIZE_BYTES)
//        return ERR_ARG;
//    hdr.streamId =
//        static_cast<TracePacketHeader::StreamIdType>(TraceStream::CM7);
//    hdr.cpuId = static_cast<TracePacketHeader::CpuIdType>(TraceStream::CM7);
//    hdr.timestamp = systimeUs();
//    hdr.packetSizeBits = size << 3;
//
//    //    DIAG(TRACE_DIAG "out eventSizeBits %u", event.header.eventSizeBits);
//    //    DIAG(TRACE_DIAG "out event->id %u", event.header.id);
//    //    DIAG(TRACE_DIAG "out event->timestamp %llu",
//    event.header.timestamp);
//    //    DIAG(TRACE_DIAG "in event->number %lu", event.number);
//
//    auto res =
//        netconn_write(conn, &hdr, sizeof(TracePacketHeader), NETCONN_COPY);
//    if (res != ERR_OK) {
//        return res;
//    }
//    return netconn_write(conn, &event, sizeof(TraceTaskSwitchedOutEvent),
//                         NETCONN_COPY);
//}
//
// err_t sendEvent(TracePerfCntEvent &event, struct netconn *conn) {
//    TracePacketHeader hdr;
//    auto size = sizeof(TracePacketHeader) + sizeof(TracePerfCntEvent);
//    if (size > TracePacketHeader::MAX_PACKET_SIZE_BYTES)
//        return ERR_ARG;
//    hdr.streamId =
//        static_cast<TracePacketHeader::StreamIdType>(TraceStream::CM7);
//    hdr.cpuId = static_cast<TracePacketHeader::CpuIdType>(TraceStream::CM7);
//    hdr.timestamp = systimeUs();
//    hdr.packetSizeBits = size << 3;
//
//    //    DIAG(TRACE_DIAG "PerfCnt eventSizeBits %u",
//    //    event.header.eventSizeBits); DIAG(TRACE_DIAG "PerfCnt event->id %u",
//    //    event.header.id); DIAG(TRACE_DIAG "PerfCnt event->timestamp %llu",
//    //    event.header.timestamp);
//
//    auto res =
//        netconn_write(conn, &hdr, sizeof(TracePacketHeader), NETCONN_COPY);
//    if (res != ERR_OK) {
//        return res;
//    }
//    return netconn_write(conn, &event, sizeof(TracePerfCntEvent),
//    NETCONN_COPY);
//}

static Trace traceSingleton;

Trace &Trace::instance() { return traceSingleton; }

} // namespace CARBON

#endif
