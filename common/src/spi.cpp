/**
 ******************************************************************************
 * @file           spi.cpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Jul. 2024
 * @brief          c++ spi interface
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
#include <carbon/spi.hpp>

Spi::Spi(SPI_HandleTypeDef &spiHandle, BinarySemaphore &txSemaphore,
         BinarySemaphore &rxSemaphore)
    : spiHandle_(spiHandle), txSemaphore_(txSemaphore),
      rxSemaphore_(rxSemaphore) {}

Error Spi::DMATransmit(void *buffer, uint16_t bufferSize) {
    __DSB();
    DIAG(SPI_DIAG "Starting DMA transfer");
    auto res = HAL_SPI_Transmit_DMA(
        &spiHandle_, reinterpret_cast<uint8_t *>(buffer), bufferSize);
    if (res != 0) {
        DIAG(SPI_DIAG "Error DMA Trasmit: %u", res);
        DIAG(SPI_DIAG "SPI error code: %lu", HAL_SPI_GetError(&spiHandle_));
        return InternalHardwareError;
    }
    DIAG(SPI_DIAG "DMA started");
    if (!txSemaphore_.acquire(timeout)) {
        DIAG(SPI_DIAG "time out waiting TX DMA");
        return InternalHardwareError;
    }
    DIAG(SPI_DIAG "SPI error code: %lu", HAL_SPI_GetError(&spiHandle_));
    return Success;
}
