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

namespace CARBON {

static constexpr auto DIAG_ALIGNMENT = uint32_t{1};
static constexpr auto DIAG_FIFO_NELEMENTS = uint32_t{2048};
static constexpr auto DIAG_BUFFER_SIZE = DIAG_FIFO_NELEMENTS + 1;
static constexpr auto DIAG_BUFFER_SIZE_BYTES =
    (DIAG_FIFO_NELEMENTS + 1) * sizeof(uint8_t);

using DIAG_HSEM = HSEMSpinLock<HSEM_ID::NotifyDiag>;
using DiagFifo = Fifo<uint8_t, DIAG_ALIGNMENT, DIAG_HSEM, DIAG_FIFO_NELEMENTS>;

extern uint8_t diagBuffer[DIAG_BUFFER_SIZE]
    __attribute__((aligned(4), section(".diag_buffer")));
extern uint32_t diagBufferPtr;
extern DiagFifo diagFifo __attribute__((aligned(4), section(".diag_fifo")));

} // namespace CARBON
