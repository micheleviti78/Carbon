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

#include <sd_diskio.h>

#include <carbon_mp.h>
#include <carbon_mp_test.h>

#include <cmsis_os.h>
#include <task.h>

extern "C" {

static char sdPath[4];
static FATFS sdFATFS;

static const char msg[] =
    "Dopo quella diamantata pazzesca la contessina Serbelloni Mazzanti Vien "
    "Dal mare gli fece conoscere alcuni amici e gli presentò "
    "nell'ordine:\n\rla "
    "signora Bolla, i coniugi Bertani, la contessa Ruffino, i fratelli Gancia, "
    "Donna Folonari, il barone Ricasoli, il marchese Antinori, i Serristori "
    "Branca e i Moretti, quelli della birra.\n\rA metà di quel giro di "
    "presentazioni Fantozzi era già completamente ubriaco!";

static uint8_t workBuffer[_MAX_SS]
    __attribute__((aligned(32), section(".sdram_bank2")));

#define MICROPYTHON_HEAP_SIZE 2097152U /*2 MB size micropython heap*/
static uint8_t micropython_heap[MICROPYTHON_HEAP_SIZE]
    __attribute__((aligned(32), section(".sdram_bank2")));

static BSP_SD_CardInfo cardInfo;
static BSP_SD_CardCID cardCID;

void start_sd_thread_imp(void);

void start_sd_thread(void) { start_sd_thread_imp(); }

void sd_thread(const void * /*argument*/) {
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
        DIAG(SD "Error reading SD card Info or no card");
    } else {
        DIAG(SD "SD card type %lu", cardInfo.CardType);
        DIAG(SD "SD card version %lu", cardInfo.CardVersion);
        DIAG(SD "SD card class %lu", cardInfo.Class);
        DIAG(SD "SD card number of blocks %lu", cardInfo.BlockNbr);
        DIAG(SD "SD card block size %lu", cardInfo.BlockSize);
        DIAG(SD "SD card number of logical blocks %lu", cardInfo.LogBlockNbr);
        DIAG(SD "SD card logical block size %lu", cardInfo.LogBlockSize);
        DIAG(SD "SD card speed %lu", cardInfo.CardSpeed);

        if (BSP_SD_GetCardCID(0, &cardCID) < 0) {
            DIAG(SD "Error reading SD card CID or no card");
        } else {
            DIAG(SD "SD card Manufactorer ID %lu", cardCID.ManufacturerID);
            DIAG(SD "SD card Manufactorer OEMID %lu", cardCID.OEM_AppliID);
            DIAG(SD "SD card Manufacturing Date %lu", cardCID.ManufactDate);
        }
    }

    if (FATFS_LinkDriver(&SD_Driver, &sdPath[0]) != 0) {
        DIAG(SD "cannot register diskio driver");
        while (1) {
            osDelay(10000);
        }
    } else {
        DIAG(SD "Logical path for sd card volume %s", &sdPath[0]);
    }

    FRESULT fres = f_mount(&sdFATFS, &sdPath[0], 1);

    if (fres == 0) {
        DIAG(SD "Logical volume %s for sd card mounted", &sdPath[0]);
    } else if (fres == 13) {
        fres = f_mkfs(&sdPath[0], FM_ANY, 0, workBuffer, sizeof(workBuffer));
        if (fres == 0) {
            DIAG(SD "Logical volume %s formatted", &sdPath[0]);
            fres = f_mount(&sdFATFS, &sdPath[0], 1);
            if (fres == 0) {
                DIAG(SD "Logical volume %s mounted after formatting",
                     &sdPath[0]);
            } else {
                DIAG(SD "Error mounting logical volume %s: %d", &sdPath[0],
                     fres);
                while (1) {
                    osDelay(10000);
                }
            }
        } else {
            DIAG(SD "Logical volume %s error %d while formatting", &sdPath[0],
                 fres);
            while (1) {
                osDelay(10000);
            }
        }
    } else {
        DIAG(SD "Error mounting logical volume %s: %d", &sdPath[0], fres);
        while (1) {
            osDelay(10000);
        }
    }

    FIL fs_write;
    UINT bw;

    fres = f_open(&fs_write, "boot.py", FA_WRITE | FA_CREATE_ALWAYS);

    if (fres != 0) {
        DIAG(SD "error opening file %d", fres);
        while (1) {
            osDelay(10000);
        }
    }

    size_t msg_size = (sizeof(msg) / sizeof(msg[0])) - 1;

    DIAG(SD "message size %u", msg_size);

    fres = f_write(&fs_write, msg, msg_size, &bw);

    if (fres != 0) {
        DIAG(SD "error writing file %d", fres);
        while (1) {
            osDelay(10000);
        }
    }

    DIAG(SD "bytes written %u", bw);

    f_close(&fs_write);

    DIAG(SD "FAT file system test complete");

    osDelay(100);

    mp_embed_init(&micropython_heap[0], MICROPYTHON_HEAP_SIZE);

    DIAG(SYSTEM_DIAG "Micropython initialized, heap at %p, size %u",
         &micropython_heap[0], MICROPYTHON_HEAP_SIZE);

    carbon_mp_test();

    while (1) {
        osDelay(10000);
    }
}
}