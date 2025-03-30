/**
 ******************************************************************************
 * @file           sd_thread.cpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Feb. 2024
 * @brief          thread to mount/unmount the SD card
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
#include <carbon/sd_thread.hpp>

#include <io_utils.h>
#include <sd_diskio.h>

#include <cmsis_os.h>
#include <task.h>

static char sdPath[4];
static FATFS sdFATFS;

static uint8_t workBuffer[_MAX_SS]
    __attribute__((aligned(32), section(".sdram_bank2")));

static BSP_SD_CardInfo cardInfo;
static BSP_SD_CardCID cardCID;

extern "C" {
FRESULT copy_file(const char *srcPath, const char *dstPath);
}

SDThread::SDThread()
    : Thread("sd_thread", osPriorityNormal, configMINIMAL_STACK_SIZE * 64) {}

void SDThread::run() {
    DIAG(SD "starting SD thread");
    while (1) {
        int32_t res = BSP_SD_Init(0);

        if (res < 0) {
            DIAG(SD "SD card init failed: %ld", res);
            while (1) {
                osDelay(10000);
            }
        }

        DIAG(SD "SD initialized");

        if (BSP_SD_GetCardInfo(0, &cardInfo) < 0) {
            DIAG(SD "Error reading SD card Info or no card");
        } else {
            DIAG(SD "SD card type %lu", cardInfo.CardType);
            DIAG(SD "SD card version %lu", cardInfo.CardVersion);
            DIAG(SD "SD card class %lu", cardInfo.Class);
            DIAG(SD "SD card number of blocks %lu", cardInfo.BlockNbr);
            DIAG(SD "SD card block size %lu", cardInfo.BlockSize);
            DIAG(SD "SD card number of logical blocks %lu",
                 cardInfo.LogBlockNbr);
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
            fres =
                f_mkfs(&sdPath[0], FM_ANY, 0, workBuffer, sizeof(workBuffer));
            if (fres == 0) {
                DIAG(SD "Logical volume %s formatted", &sdPath[0]);
                fres = f_mount(&sdFATFS, &sdPath[0], 1);
                if (fres == 0) {
                    DIAG(SD "Logical volume %s mounted after formatting",
                         &sdPath[0]);
                } else {
                    DIAG(SD "Error mounting logical volume %s: %d", &sdPath[0],
                         fres);
                }
            } else {
                DIAG(SD "Logical volume %s error %d while formatting",
                     &sdPath[0], fres);
            }
        } else {
            DIAG(SD "Error mounting logical volume %s: %d", &sdPath[0], fres);
        }
#if 0
        FRESULT test_res = copy_file("0:/original/ocean_birds.avi",
                                     "0:/test1/ocean_birds.avi");
        DIAG(SD "copying file 1 error %lu", test_res);

        test_res =
            copy_file("0:/original/rain_drops.avi", "0:/test1/rain_drops.avi");
        DIAG(SD "copying file 2 error %lu", test_res);

        test_res = copy_file("0:/original/Stream.avi", "0:/test1/Stream.avi");
        DIAG(SD "copying file 3 error %lu", test_res);

        test_res = copy_file("0:/original/tropicalfish.avi",
                             "0:/test1/tropicalfish.avi");
        DIAG(SD "copying file 4 error %lu", test_res);

        test_res = copy_file("0:/original/ocean_birds.avi",
                                     "0:/test2/ocean_birds.avi");
        DIAG(SD "copying file 1 error %lu", test_res);

        test_res =
            copy_file("0:/original/rain_drops.avi", "0:/test2/rain_drops.avi");
        DIAG(SD "copying file 2 error %lu", test_res);

        test_res = copy_file("0:/original/Stream.avi", "0:/test2/Stream.avi");
        DIAG(SD "copying file 3 error %lu", test_res);

        test_res = copy_file("0:/original/tropicalfish.avi",
                             "0:/test2/tropicalfish.avi");
        DIAG(SD "copying file 4 error %lu", test_res);

        test_res = copy_file("0:/original/ocean_birds.avi",
                                     "0:/test3/ocean_birds.avi");
        DIAG(SD "copying file 1 error %lu", test_res);

        test_res =
            copy_file("0:/original/rain_drops.avi", "0:/test3/rain_drops.avi");
        DIAG(SD "copying file 2 error %lu", test_res);

        test_res = copy_file("0:/original/Stream.avi", "0:/test3/Stream.avi");
        DIAG(SD "copying file 3 error %lu", test_res);

        test_res = copy_file("0:/original/tropicalfish.avi",
                             "0:/test3/tropicalfish.avi");
        DIAG(SD "copying file 4 error %lu", test_res);
#endif
        BSP_SD_DeInit(0);
        /* Unmount volume */
        f_mount(NULL, &sdPath[0], 0);
        DIAG(SD "volume %s unmounted", &sdPath[0]);
        if (FATFS_UnLinkDriverEx(&sdPath[0], 0) != 0) {
            DIAG(SD "cannot unregister diskio driver");
        }
    }
}
