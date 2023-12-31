/**
 ******************************************************************************
 * @file           shared_memory.hpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Dec. 2022
 * @brief          Shared memory for the IPC
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

#include <carbon/fifo.hpp>
#include <carbon/hsem.hpp>
#include <carbon/trace_format.hpp>

#define FIFO_DECLARATION(NAME, TYPE, NELEMENTS, HSEM_INDEX)                    \
    using NAME##_ELEMENT_TYPE = TYPE;                                          \
    static constexpr auto NAME##_ELEMENT_ALIGNMENT =                           \
        sizeof(NAME##_ELEMENT_TYPE);                                           \
    static constexpr auto NAME##_ELEMENT_SIZE = sizeof(NAME##_ELEMENT_TYPE);   \
    static constexpr auto NAME##_FIFO_NELEMENTS = uint32_t{NELEMENTS};         \
    static constexpr auto NAME##_BUFFER_SIZE = NAME##_FIFO_NELEMENTS + 1;      \
    static constexpr auto NAME##_BUFFER_SIZE_BYTES =                           \
        (NAME##_FIFO_NELEMENTS + 1) * NAME##_ELEMENT_SIZE;                     \
    using NAME##_HSEM = HSEMSpinLock<HSEM_ID::HSEM_INDEX>;                     \
    using NAME##FifoClass =                                                    \
        Fifo<NAME##_ELEMENT_TYPE, NAME##_ELEMENT_ALIGNMENT, NAME##_HSEM,       \
             NAME##_FIFO_NELEMENTS>;                                           \
    extern NAME##_ELEMENT_TYPE NAME##Buffer[NAME##_BUFFER_SIZE]                \
        __attribute__((aligned(4), section("." #NAME "_buffer")));             \
    extern uint32_t NAME##BufferPtr;                                           \
    extern NAME##FifoClass NAME##Fifo                                          \
        __attribute__((aligned(4), section("." #NAME "_fifo")));

#define FIFO_DEFINITION(NAME)                                                  \
    NAME##_ELEMENT_TYPE NAME##Buffer[NAME##_BUFFER_SIZE];                      \
    uint32_t NAME##BufferPtr = reinterpret_cast<uint32_t>(&NAME##Buffer[0]);   \
    NAME##FifoClass NAME##Fifo;

#define FIFO_INIT(NAME)                                                        \
    NAME##Fifo.init(NAME##BufferPtr, NAME##_BUFFER_SIZE_BYTES);

namespace CARBON {

enum class SyncFlagBit : uint32_t { PeripherySync };

void waitForSyncFlag(SyncFlagBit syncFlagBit);
void setSyncFlag(SyncFlagBit syncFlagBit);
void resetSyncFlag();

/*DIAG FIFO*/

FIFO_DECLARATION(diag, uint8_t, 2048, NotifyDiag)

/*TRACE FIFO*/

/*Sync Flag*/

volatile extern uint32_t syncFlag
    __attribute__((aligned(4), section(".sync_flag")));

} // namespace CARBON
