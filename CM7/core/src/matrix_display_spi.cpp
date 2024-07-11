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

#define SPI2_SCK_Pin GPIO_PIN_12
#define SPI2_SCK_GPIO_Port GPIOA
#define SPI2_NSS_Pin GPIO_PIN_11
#define SPI2_NSS_GPIO_Port GPIOA
#define SPI2_MOSI_Pin GPIO_PIN_15
#define SPI2_MOSI_GPIO_Port GPIOB
#define Latch_Pin GPIO_PIN_14
#define Latch_GPIO_Port GPIOB

osSemaphoreDef(SEM_SPI2_TX_DEF);
static osSemaphoreId SEM_SPI2_TX;
osSemaphoreDef(SEM_SPI2_RX_DEF);
static osSemaphoreId SEM_SPI2_RX;
extern "C" {
static void voidCallback(SPI_HandleTypeDef * /*spiHandle*/);

static void carbon_hw_matrix_display_spi_init(SPI_HandleTypeDef *hspi);

static void carbon_hw_spi_tx_callback(SPI_HandleTypeDef *spiHandle);
static void carbon_hw_spi_rx_callback(SPI_HandleTypeDef *spiHandle);
}
static SPI_HandleTypeDef hspi2;
static DMA_HandleTypeDef hdma_spi2_tx;
static GPIO_InitTypeDef GPIO_InitStruct;

class DisplayMatrixSpi : public Spi {
public:
    DisplayMatrixSpi() : Spi() {
        HAL_SPI_RegisterCallback(&hspi2, HAL_SPI_MSPINIT_CB_ID,
                                 carbon_hw_matrix_display_spi_init);
        HAL_SPI_RegisterCallback(&hspi2, HAL_SPI_MSPDEINIT_CB_ID, voidCallback);
    };

    ~DisplayMatrixSpi() override = default;

    Error init() override {
        hspi2.Instance = SPI2;
        hspi2.Init.Mode = SPI_MODE_MASTER;
        hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
        hspi2.Init.Direction = SPI_DIRECTION_1LINE;
        hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
        hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
        hspi2.Init.DataSize = SPI_DATASIZE_32BIT;
        hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
        hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
        hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
        hspi2.Init.NSS = SPI_NSS_SOFT;
        hspi2.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
        hspi2.Init.MasterKeepIOState =
            SPI_MASTER_KEEP_IO_STATE_ENABLE; /* Recommended setting to avoid
                                                glitches */

        if (HAL_SPI_Init(&hspi2) != HAL_OK) {
            return InternalHardwareError;
        }

        HAL_SPI_RegisterCallback(&hspi2, HAL_SPI_TX_COMPLETE_CB_ID,
                                 carbon_hw_spi_tx_callback);
        HAL_SPI_RegisterCallback(&hspi2, HAL_SPI_RX_COMPLETE_CB_ID,
                                 carbon_hw_spi_rx_callback);
        HAL_SPI_RegisterCallback(&hspi2, HAL_SPI_TX_RX_COMPLETE_CB_ID,
                                 voidCallback);
        HAL_SPI_RegisterCallback(&hspi2, HAL_SPI_TX_HALF_COMPLETE_CB_ID,
                                 voidCallback);
        HAL_SPI_RegisterCallback(&hspi2, HAL_SPI_TX_HALF_COMPLETE_CB_ID,
                                 voidCallback);
        HAL_SPI_RegisterCallback(&hspi2, HAL_SPI_TX_RX_HALF_COMPLETE_CB_ID,
                                 voidCallback);
        HAL_SPI_RegisterCallback(&hspi2, HAL_SPI_ERROR_CB_ID, voidCallback);
        HAL_SPI_RegisterCallback(&hspi2, HAL_SPI_ABORT_CB_ID, voidCallback);

        SEM_SPI2_TX = osSemaphoreCreate(osSemaphore(SEM_SPI2_TX_DEF), 1);
        SEM_SPI2_RX = osSemaphoreCreate(osSemaphore(SEM_SPI2_RX_DEF), 1);

        osSemaphoreWait(SEM_SPI2_TX, 0);
        osSemaphoreWait(SEM_SPI2_RX, 0);

        DIAG(MATRIX_DIS_DIAG "dispaly matrix spi initialized");
        return Success;
    }

    Error DMATransmit(void *buffer, uint16_t bufferSize) override {
        __DSB();
        Error error = Success;
        auto res = HAL_SPI_Transmit_DMA(
            &hspi2, reinterpret_cast<uint8_t *>(buffer), bufferSize);
        if (res != 0) {
            DIAG(SPI_DIAG "Error DMA Trasmit: %u", res);
            DIAG(SPI_DIAG "SPI error code: %lu", HAL_SPI_GetError(&hspi2));
            DIAG(SPI_DIAG "DMA error code: %lu",
                 HAL_DMA_GetError(&hdma_spi2_tx));
            return InternalHardwareError;
        }
        if (!error && osSemaphoreWait(SEM_SPI2_TX, timeout) != osOK) {
            DIAG(SPI_DIAG "time out waiting TX DMA");
            error = InternalHardwareError;
        }
        if (HAL_SPI_GetError(&hspi2) != HAL_SPI_ERROR_NONE ||
            HAL_DMA_GetError(&hdma_spi2_tx) != HAL_DMA_ERROR_NONE) {
            DIAG(SPI_DIAG "SPI error code: %lu", HAL_SPI_GetError(&hspi2));
            DIAG(SPI_DIAG "DMA error code: %lu",
                 HAL_DMA_GetError(&hdma_spi2_tx));
            error = InternalHardwareError;
        }
        return error;
    }
};

static DisplayMatrixSpi displayMatrixSpi;

Spi &getDisplayMatrixSpi() { return displayMatrixSpi; }

extern "C" {
void carbon_hw_matrix_display_spi_init(SPI_HandleTypeDef *hspi) {
    if (hspi->Instance == SPI2) {
        /* Peripheral clock enable */

        __HAL_RCC_SPI2_CLK_ENABLE();
        __HAL_RCC_DMA1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();

        /**SPI2 GPIO Configuration
        PA12     ------> SPI2_SCK
        PA11     ------> SPI2_NSS
        PB15     ------> SPI2_MOSI
        */
        GPIO_InitStruct.Pin = SPI2_SCK_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = SPI2_MOSI_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
        HAL_GPIO_Init(SPI2_MOSI_GPIO_Port, &GPIO_InitStruct);

        /* SPI2 DMA Init */
        /* SPI2_TX Init */
        hdma_spi2_tx.Instance = DMA1_Stream0;
        hdma_spi2_tx.Init.Request = DMA_REQUEST_SPI2_TX;
        hdma_spi2_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
        hdma_spi2_tx.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_spi2_tx.Init.MemInc = DMA_MINC_ENABLE;
        hdma_spi2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
        hdma_spi2_tx.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
        hdma_spi2_tx.Init.Mode = DMA_NORMAL;
        hdma_spi2_tx.Init.Priority = DMA_PRIORITY_MEDIUM;
        hdma_spi2_tx.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
        hdma_spi2_tx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
        hdma_spi2_tx.Init.MemBurst = DMA_MBURST_SINGLE;
        hdma_spi2_tx.Init.PeriphBurst = DMA_PBURST_SINGLE;

        if (HAL_DMA_Init(&hdma_spi2_tx) != HAL_OK) {
            Error_Handler();
        }

        __HAL_LINKDMA(hspi, hdmatx, hdma_spi2_tx);

        /* DMA interrupt init */
        /* DMA1_Stream0_IRQn interrupt configuration */
        HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);

        /* SPI2 interrupt Init */
        HAL_NVIC_SetPriority(SPI2_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(SPI2_IRQn);

        DIAG(MATRIX_DIS_DIAG "SPI and DMA configured");
    }
}

void voidCallback(SPI_HandleTypeDef * /*spiHandle*/) {}

void carbon_hw_matrix_display_dma_isr() { HAL_DMA_IRQHandler(&hdma_spi2_tx); }
void carbon_hw_matrix_display_spi_isr() { HAL_SPI_IRQHandler(&hspi2); }
void carbon_hw_spi_tx_callback(SPI_HandleTypeDef * /*spiHandle*/) {
    osSemaphoreRelease(SEM_SPI2_TX);
}
void carbon_hw_spi_rx_callback(SPI_HandleTypeDef * /*spiHandle*/) {
    osSemaphoreRelease(SEM_SPI2_RX);
}
}