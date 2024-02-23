/**
 ******************************************************************************
 * @file           sd_thread.cpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Feb. 2024
 * @brief          thread to test the SD card, cpp code
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

#include <carbon/diag.hpp>
#include <carbon/error.hpp>
#include <carbon/sd_card.h>

#include <cmsis_os.h>

#include <task.h>

#include <cmsis_os.h>

static constexpr auto BUFFER_SIZE = uint32_t{1024};

extern "C" {

static uint8_t writeBuf[BUFFER_SIZE]
    __attribute__((aligned(32), section(".sdram_bank2")));
static uint8_t readBuf[BUFFER_SIZE]
    __attribute__((aligned(32), section(".sdram_bank2")));
static BSP_SD_CardInfo cardInfo;
static BSP_SD_CardCID cardCID;

static void sd_test(uint32_t block_id);

void start_sd_test_thread_imp(void);

void start_sd_test_thread(void) { start_sd_test_thread_imp(); }

void sd_test_thread(const void * /*argument*/) {
    while (1) {
        BSP_SD_DeInit(0);

        int32_t res = BSP_SD_Init(0);

        if (res < 0) {
            DIAG(SD "SD card init failed: %ld", res);
            while (1) {
                osDelay(10000);
            }
        }

        DIAG("SD initialized");

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

            for (unsigned i = 0; i < 256; i++) {
                writeBuf[i] = 165;
            }
            for (unsigned i = 256; i < 512; i++) {
                writeBuf[i] = 90;
            }
            for (unsigned i = 512; i < 768; i++) {
                writeBuf[i] = 170;
            }
            for (unsigned i = 768; i < BUFFER_SIZE; i++) {
                writeBuf[i] = 255;
            }

            for (unsigned i = 0; i < cardInfo.LogBlockNbr; i = i + 2) {
                DIAG(SYSTEM_DIAG "block %u", i);
                sd_test(i);
            }
        }
        osDelay(1000);
    }
}

void sd_test(uint32_t block_id) {

    for (unsigned i = 0; i < BUFFER_SIZE; i++) {
        readBuf[i] = 11;
    }

    SCB_CleanDCache_by_Addr(reinterpret_cast<uint32_t *>(&readBuf[0]),
                            BUFFER_SIZE);

    int32_t err;

    err = BSP_SD_WriteBlocks_DMA(0, reinterpret_cast<uint32_t *>(&writeBuf[0]),
                                 block_id, 2);

    if (err < 0) {
        DIAG(SYSTEM_DIAG "Error writing SD card %ld", err);
        for (;;) {
            osDelay(1000);
        }
    }

    err = BSP_SD_ReadBlocks_DMA(0, reinterpret_cast<uint32_t *>(&readBuf[0]),
                                block_id, 2);

    if (err < 0) {
        DIAG(SYSTEM_DIAG "Error reading SD card %ld", err);
        for (;;) {
            osDelay(1000);
        }
    } else {
        for (unsigned i = 0; i < 256; i++) {
            if (readBuf[i] != 165) {
                for (unsigned j = 0; j < BUFFER_SIZE; j++) {
                    DIAG(SYSTEM_DIAG "read back data %lu, byte %u, block %lu ",
                         static_cast<uint32_t>(readBuf[j]), j, block_id);
                    osDelay(5);
                }
                for (;;) {
                    osDelay(1000);
                }
            }
        }
        for (unsigned i = 256; i < 512; i++) {
            if (readBuf[i] != 90) {
                for (unsigned j = 0; j < BUFFER_SIZE; j++) {
                    DIAG(SYSTEM_DIAG "read back data %lu, byte %u, block %lu ",
                         static_cast<uint32_t>(readBuf[j]), j, block_id);
                    osDelay(5);
                }
                for (;;) {
                    osDelay(1000);
                }
            }
        }
        for (unsigned i = 512; i < 768; i++) {
            if (readBuf[i] != 170) {
                for (unsigned j = 0; j < BUFFER_SIZE; j++) {
                    DIAG(SYSTEM_DIAG "read back data %lu, byte %u, block %lu ",
                         static_cast<uint32_t>(readBuf[j]), j, block_id);
                    osDelay(5);
                }
                for (;;) {
                    osDelay(1000);
                }
            }
        }
        for (unsigned i = 768; i < BUFFER_SIZE; i++) {
            if (readBuf[i] != 255) {
                for (unsigned j = 0; j < BUFFER_SIZE; j++) {
                    DIAG(SYSTEM_DIAG "read back data %lu, byte %u, block %lu ",
                         static_cast<uint32_t>(readBuf[j]), j, block_id);
                    osDelay(5);
                }
                for (;;) {
                    osDelay(1000);
                }
            }
        }
    }
    osDelay(3);
}
}