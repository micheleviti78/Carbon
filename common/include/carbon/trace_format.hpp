/**
 ******************************************************************************
 * @file           trace_format.hpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Dec. 2022
 * @brief          common trace format used
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

#include <cstdint>
#include <limits>
#include <variant>

namespace CARBON {

#pragma pack(push, 1)

// ---------------------------------------------------------------------------
// CTF headers
// ---------------------------------------------------------------------------

struct TracePacketHeader {
    using MagicType = uint32_t;
    using StreamIdType = uint32_t;
    using CpuIdType = uint16_t;
    using SizeType = uint32_t;
    using TimestampType = uint64_t;
    using EventsDiscardedType = uint16_t;

    static constexpr auto MAX_PACKET_SIZE_BYTES =
        std::numeric_limits<SizeType>::max() >> 3;

    MagicType magic{0xc1fc1fc1};
    StreamIdType streamId{0};
    TimestampType timestamp{0};
    SizeType packetSizeBits{0};
    EventsDiscardedType eventsDiscarded{0};
    CpuIdType cpuId{0};
};

static_assert(sizeof(TracePacketHeader) == 24);

struct TraceEventHeader {
    using SizeType = uint16_t;
    using IdType = uint16_t;
    using TimestampType = uint64_t;

    static constexpr auto MAX_EVENT_SIZE_BYTES =
        std::numeric_limits<SizeType>::max() >> 3;

    SizeType eventSizeBits{0};
    IdType id{0};
    TimestampType timestamp{0};
};

static_assert(sizeof(TraceEventHeader) == 12);

// ---------------------------------------------------------------------------
// Common structures
// ---------------------------------------------------------------------------

struct TraceTaskInfo {
    char state{' '};
    char name[15]{0};
    uint32_t number{0};
    uint8_t currentPriority{0};
    uint8_t basePriority{0};
    uint16_t stackHighWaterMark{0};
    uint32_t runTimeCounter{0};
};

enum class TraceEventID : TraceEventHeader::IdType {
    Tasks = 0,
    Malloc = 1,
    Free = 2,
    TaskSwitchedIn = 3,
    TaskSwitchedOut = 4,
    PerfCnt = 20,
};

struct TraceTasksEvent {
    static constexpr TraceEventID ID = TraceEventID::Tasks;

    uint32_t totalRunTime{0};
    uint8_t nTasks{0};
    TraceTaskInfo tasks[0];
};

struct TraceMallocEvent {
    static constexpr TraceEventID ID = TraceEventID::Malloc;

    uint32_t address;
    uint32_t size;
};

struct TraceFreeEvent {
    static constexpr TraceEventID ID = TraceEventID::Free;

    uint32_t address;
    uint32_t size;
};

struct TraceTaskSwitchedInEvent {
    static constexpr TraceEventID ID = TraceEventID::TaskSwitchedIn;

    uint32_t number;
};

struct TraceTaskSwitchedOutEvent {
    static constexpr TraceEventID ID = TraceEventID::TaskSwitchedOut;

    uint32_t number;
};

struct TracePerfCntEvent {
    static constexpr TraceEventID ID = TraceEventID::PerfCnt;

    uint32_t id{0};
    uint32_t minCycles{0};
    uint32_t maxCycles{0};
    uint32_t avgCycles{0};
};

#pragma pack(pop)

using TraceEvent = std::variant<TraceTasksEvent, TraceMallocEvent,
                                TraceFreeEvent, TraceTaskSwitchedInEvent,
                                TraceTaskSwitchedOutEvent, TracePerfCntEvent>;

} // namespace CARBON
