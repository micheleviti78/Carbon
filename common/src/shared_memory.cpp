/**
 ******************************************************************************
 * @file           shared_memory.cpp
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

#include <carbon/shared_memory.hpp>

namespace CARBON {

uint8_t diagBuffer[DIAG_BUFFER_SIZE];
uint32_t diagBufferPtr = reinterpret_cast<uint32_t>(&diagBuffer[0]);
DiagFifo diagFifo;

} // namespace CARBON
