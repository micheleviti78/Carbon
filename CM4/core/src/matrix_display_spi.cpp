/**
 ******************************************************************************
 * @file           matrix_display_spi.cpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Jul. 2024
 * @brief          matrix dispaly spi
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

#include <carbon/display_matrix_spi.hpp>

static BinarySemaphore semaphoreTx;
static BinarySemaphore semaphoreRx;

extern "C" {
static void carbon_hw_spi_tx_callback(SPI_HandleTypeDef *spiHandle);
static void carbon_hw_spi_rx_callback(SPI_HandleTypeDef *spiHandle);
}

static SPI_HandleTypeDef hspi2;
static DMA_HandleTypeDef hdma_spi2_tx;

class DisplayMatrixSpi : public Spi {
public:
    DisplayMatrixSpi() : Spi(hspi2, semaphoreTx, semaphoreRx) {
        HAL_SPI_RegisterCallback(&spiHandle_, HAL_SPI_TX_COMPLETE_CB_ID,
                                 carbon_hw_spi_tx_callback);
        HAL_SPI_RegisterCallback(&spiHandle_, HAL_SPI_RX_COMPLETE_CB_ID,
                                 carbon_hw_spi_rx_callback);
        HAL_SPI_RegisterCallback(&spiHandle_, HAL_SPI_TX_RX_COMPLETE_CB_ID,
                                 Spi::voidCallback);
        HAL_SPI_RegisterCallback(&spiHandle_, HAL_SPI_TX_HALF_COMPLETE_CB_ID,
                                 Spi::voidCallback);
        HAL_SPI_RegisterCallback(&spiHandle_, HAL_SPI_TX_HALF_COMPLETE_CB_ID,
                                 Spi::voidCallback);
        HAL_SPI_RegisterCallback(&spiHandle_, HAL_SPI_TX_RX_HALF_COMPLETE_CB_ID,
                                 Spi::voidCallback);
        HAL_SPI_RegisterCallback(&spiHandle_, HAL_SPI_ERROR_CB_ID,
                                 Spi::voidCallback);
        HAL_SPI_RegisterCallback(&spiHandle_, HAL_SPI_ABORT_CB_ID,
                                 Spi::voidCallback);
        HAL_SPI_RegisterCallback(&spiHandle_, HAL_SPI_MSPINIT_CB_ID,
                                 Spi::voidCallback);
        HAL_SPI_RegisterCallback(&spiHandle_, HAL_SPI_MSPDEINIT_CB_ID,
                                 Spi::voidCallback);
    }
    ~DisplayMatrixSpi() override = default;
    Error init() override { return Success; }
};

static DisplayMatrixSpi displayMatrixSpi;

Spi &getDisplayMatrixSpi() { return displayMatrixSpi; }

extern "C" {
void carbon_hw_matrix_display_dma_isr() { HAL_DMA_IRQHandler(&hdma_spi2_tx); }
void carbon_hw_matrix_display_spi_isr() { HAL_SPI_IRQHandler(&hspi2); }
void carbon_hw_spi_tx_callback(SPI_HandleTypeDef * /*spiHandle*/) {
    semaphoreTx.release();
}
void carbon_hw_spi_rx_callback(SPI_HandleTypeDef * /*spiHandle*/) {
    semaphoreRx.release();
}
}