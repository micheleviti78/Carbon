/**
 ******************************************************************************
 * @file           main_thread_sd.cpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Nov. 2022
 * @brief          CM7 main thread source for sd test
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

#include <carbon/diag_thread.hpp>
#include <carbon/error.hpp>
#include <carbon/main_thread.hpp>
#include <carbon/pin.hpp>
#include <carbon/stm32h747i_discovery_sd.h>
#include <carbon/trace_thread.hpp>

#include <cmsis_os.h>

#include <task.h>

#include <carbon_mp_test.h>

extern "C" {

static uint8_t writeBuf[512] __attribute__((aligned(32)));
static uint8_t readBuf[512] __attribute__((aligned(32)));
static BSP_SD_CardInfo cardInfo;
static BSP_SD_CardCID cardCID;

static void sd_test(uint32_t block_id);

void netif_config(void);

void mainThread(const void *argument) {
    start_diag_thread();

    DIAG(SYSTEM_DIAG "FreeRTOS version %d.%d.%d", tskKERNEL_VERSION_MAJOR,
         tskKERNEL_VERSION_MINOR, tskKERNEL_VERSION_BUILD);

    netif_config();
#ifdef FREERTOS_USE_TRACE
    start_trace_thread();
#endif

    if (BSP_SD_GetCardInfo(0, &cardInfo) < 0) {
        DIAG(SYSTEM_DIAG "Error reading SD card Info or no card");
    } else {
        DIAG(SYSTEM_DIAG "SD card type %lu", cardInfo.CardType);
        DIAG(SYSTEM_DIAG "SD card version %lu", cardInfo.CardVersion);
        DIAG(SYSTEM_DIAG "SD card class %lu", cardInfo.Class);
        DIAG(SYSTEM_DIAG "SD card number of blocks %lu", cardInfo.BlockNbr);
        DIAG(SYSTEM_DIAG "SD card block size %lu", cardInfo.BlockSize);
        DIAG(SYSTEM_DIAG "SD card number of logical blocks %lu",
             cardInfo.LogBlockNbr);
        DIAG(SYSTEM_DIAG "SD card logical block size %lu",
             cardInfo.LogBlockSize);
        DIAG(SYSTEM_DIAG "SD card speed %lu", cardInfo.CardSpeed);

        if (BSP_SD_GetCardCID(0, &cardCID) < 0) {
            DIAG(SYSTEM_DIAG "Error reading SD card CID or no card");
        } else {
            DIAG(SYSTEM_DIAG "SD card Manufactorer ID %lu",
                 cardCID.ManufacturerID);
            DIAG(SYSTEM_DIAG "SD card Manufactorer OEMID %lu",
                 cardCID.OEM_AppliID);
            DIAG(SYSTEM_DIAG "SD card Manufacturing Date %lu",
                 cardCID.ManufactDate);
        }

        DIAG(SYSTEM_DIAG "address of readBuf %lu",
             reinterpret_cast<uint32_t>(&readBuf[0]));
        DIAG(SYSTEM_DIAG "address of writeBuf %lu",
             reinterpret_cast<uint32_t>(&writeBuf[0]));

        for (unsigned i = 0; i < 128; i++) {
            writeBuf[i] = 170;
            readBuf[i] = 0;
        }
        for (unsigned i = 128; i < 256; i++) {
            writeBuf[i] = 255;
            readBuf[i] = 0;
        }
        for (unsigned i = 256; i < 384; i++) {
            writeBuf[i] = 165;
            readBuf[i] = 0;
        }
        for (unsigned i = 384; i < 512; i++) {
            writeBuf[i] = 90;
            readBuf[i] = 0;
        }

        for (unsigned i = 0; i < cardInfo.BlockNbr; i++) {
            DIAG(SYSTEM_DIAG "block %u", i);
            sd_test(i);
        }
    }
    while (1) {
        BSP_LED_Toggle(LED_GREEN);
        osDelay(1000);
    }
}

void sd_test(uint32_t block_id) {

    for (unsigned i = 0; i < 512; i++) {
        readBuf[i] = 0;
    }

    int32_t err;

    // SCB_CleanDCache_by_Addr(reinterpret_cast<uint32_t *>(&writeBuf[0]), 512);

    err = BSP_SD_WriteBlocks(0, reinterpret_cast<uint32_t *>(&writeBuf[0]),
                             block_id, 1);

    if (err < 0) {
        DIAG(SYSTEM_DIAG "Error writing SD card %ld", err);
        for (;;) {
            osDelay(1000);
        }
    }

    while (BSP_SD_GetCardState(0)) {
        osDelay(3);
    }

    err = BSP_SD_ReadBlocks(0, reinterpret_cast<uint32_t *>(&readBuf[0]),
                            block_id, 1);

    while (BSP_SD_GetCardState(0)) {
        osDelay(3);
    }

    // SCB_InvalidateDCache_by_Addr(reinterpret_cast<uint32_t *>(&readBuf[0]),
    //                              512);

    if (err < 0) {
        DIAG(SYSTEM_DIAG "Error reading SD card %ld", err);
        for (;;) {
            osDelay(1000);
        }
    } else {
        for (unsigned i = 0; i < 128; i++) {
            if (readBuf[i] != 170) {
                DIAG(SYSTEM_DIAG "read back data %lu, byte %u, block %lu",
                     static_cast<uint32_t>(readBuf[i]), i, block_id);
                for (;;) {
                    osDelay(1000);
                }
            }
        }
        for (unsigned i = 128; i < 256; i++) {
            if (readBuf[i] != 255) {
                DIAG(SYSTEM_DIAG "read back data %lu, byte %u, block %lu",
                     static_cast<uint32_t>(readBuf[i]), i, block_id);
                for (;;) {
                    osDelay(1000);
                }
            }
        }
        for (unsigned i = 256; i < 384; i++) {
            if (readBuf[i] != 165) {
                DIAG(SYSTEM_DIAG "read back data %lu, byte %u, block %lu",
                     static_cast<uint32_t>(readBuf[i]), i, block_id);
                for (;;) {
                    osDelay(1000);
                }
            }
        }
        for (unsigned i = 384; i < 512; i++) {
            if (readBuf[i] != 90) {
                DIAG(SYSTEM_DIAG "read back data %lu, byte %u, block %lu",
                     static_cast<uint32_t>(readBuf[i]), i, block_id);
                for (;;) {
                    osDelay(1000);
                }
            }
        }
    }

    osDelay(3);
}
}