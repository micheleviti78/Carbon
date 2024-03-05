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

#include <mptask.h>

#include <cmsis_os.h>
#include <task.h>

extern "C" {

// This is example 1 script, which will be compiled and executed.
[[maybe_unused]] static const char *example_1 =
    "print('hello world!')\n"
    "print(list(x + 1 for x in range(10)))\n"
    "import gc\n"
    "print('run GC collect')\n"
    "gc.collect()\n"
    "\n"
    "print('finish')\n";

// This is example 2 script, which will be compiled and executed.
[[maybe_unused]] static const char *example_2 =
    "for i in range(10):\n"
    "    print('iter {:08}'.format(i))\n"
    "\n"
    "import gc\n"
    "print('run GC collect')\n"
    "gc.collect()\n"
    "\n"
    "print('finish')\n";

[[maybe_unused]] static const char *example_3 =
    "def addition(number1, number2):\n"
    "   result = number1 + number2\n"
    "   print(' Addition result : ',result)\n"
    "def area(radius):\n"
    "   result = 3.1415 * radius * radius\n"
    "   return result\n"
    "addition(5, 3)\n"
    "print(' Area of the circle is ',area(2))\n"
    // "import gc\n"
    // "print('run GC collect')\n"
    // "gc.collect()\n"
    // "\n"
    "print('finish')\n";

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

    FIL fs_read;
    UINT byte_read;

    fres = f_open(&fs_read, "boot.py", FA_READ);

    if (fres != 0) {
        DIAG(SD "error opening file %d", fres);
    }

    FILINFO file_info;

    fres = f_stat("boot.py", &file_info);

    if (fres != 0) {
        DIAG(SD "error reading file stats %d", fres);
    } else {
        DIAG(SD "file size %lu", file_info.fsize);
    }

    uint8_t *readBuffer =
        reinterpret_cast<uint8_t *>(pvPortMalloc(file_info.fsize + 1));

    if (readBuffer) {
        size_t byte_to_read = file_info.fsize;
        uint8_t *head = readBuffer;
        while (byte_to_read > 0) {
            fres = f_read(&fs_read, head, byte_to_read, &byte_read);
            if (fres != 0) {
                DIAG(SD "error copying file %d", fres);
                break;
            }
            byte_to_read -= byte_read;
            head += byte_read;
        }

        *(readBuffer + file_info.fsize) = 0;

        startMicropython((void *)example_2);
    }

    while (1) {
        osDelay(10000);
    }
}
}