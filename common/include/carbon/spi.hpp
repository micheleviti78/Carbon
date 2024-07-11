/**
 ******************************************************************************
 * @file           spi.hpp
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

#pragma once

#include <carbon/common.hpp>
#include <carbon/error.hpp>
#include <carbon/semaphore.hpp>

#include <stm32h7xx_hal.h>

class Spi {
public:
    Spi() = default;
    // Spi(SPI_HandleTypeDef &spiHandle);
    // , BinarySemaphore &txSemaphore,
    //     BinarySemaphore &rxSemaphore);
    virtual ~Spi() = default;
    PREVENT_COPY_AND_MOVE(Spi)
    virtual Error init() = 0;
    virtual Error DMATransmit(void *buffer, uint16_t bufferSize) = 0;

protected:
    // SPI_HandleTypeDef &spiHandle_;
    // bool isInitialized_{false};

    static constexpr uint32_t timeout = 1000;

    // BinarySemaphore &txSemaphore_;
    // BinarySemaphore &rxSemaphore_;
};