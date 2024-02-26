/**
 ******************************************************************************
 * @file           sd_diskio.c
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Feb. 2024
 * @brief          SD Card FatFs interface
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
/**
 ******************************************************************************
 * @file    sd_diskio_dma_rtos_template_bspv2.c
 * @author  MCD Application Team
 * @brief   SD Disk I/O DMA with RTOS driver template using the BSPv2 API.
 *          This file needs to be copied at user project alongside
 *          the respective header file.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2017-2019 STMicroelectronics. All rights reserved.
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                       opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 **/

/* Includes ------------------------------------------------------------------*/
#include "sd_diskio.h"
#include "ff_gen_drv.h"

#include <string.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define QUEUE_SIZE (uint32_t)10
#define READ_CPLT_MSG (uint32_t)1
#define WRITE_CPLT_MSG (uint32_t)2
#define RW_ABORT_MSG (uint32_t)3
/*
 * the following Timeout is useful to give the control back to the applications
 * in case of errors in either BSP_SD_ReadCpltCallback() or
 * BSP_SD_WriteCpltCallback() the value by default is as defined in the BSP
 * platform driver otherwise 30 secs
 *
 */

#define SD_TIMEOUT 30 * 1000

#define SD_DEFAULT_BLOCK_SIZE 512

#ifndef BSP_SD_INSTANCE
#define BSP_SD_INSTANCE 0
#endif

/*
 * Depending on the usecase, the SD card initialization could be done at the
 * application level, if it is the case define the flag below to disable
 * the BSP_SD_Init() call in the SD_Initialize().
 */

#define DISABLE_SD_INIT

/*
 * when using cachable memory region, it may be needed to maintain the cache
 * validity. Enable the define below to activate a cache maintenance at each
 * read and write operation.
 * Notice: This is applicable only for cortex M7 based platform.
 */

/*
 * Some DMA requires 4-Byte aligned address buffer to correctly read/wite data,
 * in FatFs some accesses aren't thus we need a 4-byte aligned scratch buffer to
 * correctly transfer data
 */

static uint8_t writeBuf[BLOCKSIZE]
    __attribute__((aligned(32), section(".sdram_bank2")));
static uint8_t readBuf[BLOCKSIZE]
    __attribute__((aligned(32), section(".sdram_bank2")));

/* Disk status */
static volatile DSTATUS Stat = STA_NOINIT;

/* Private function prototypes -----------------------------------------------*/
static DSTATUS SD_CheckStatus(BYTE lun);
DSTATUS SD_initialize(BYTE);
DSTATUS SD_status(BYTE);
DRESULT SD_read(BYTE, BYTE *, DWORD, UINT);
#if _USE_WRITE == 1
DRESULT SD_write(BYTE, const BYTE *, DWORD, UINT);
#endif /* _USE_WRITE == 1 */
#if _USE_IOCTL == 1
DRESULT SD_ioctl(BYTE, BYTE, void *);
#endif /* _USE_IOCTL == 1 */

const Diskio_drvTypeDef SD_Driver = {
    SD_initialize, SD_status, SD_read,
#if _USE_WRITE == 1
    SD_write,
#endif /* _USE_WRITE == 1 */

#if _USE_IOCTL == 1
    SD_ioctl,
#endif /* _USE_IOCTL == 1 */
};

/* Private functions ---------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
static int SD_CheckStatusWithTimeout(uint32_t timeout) {
    uint32_t timer;
#if (osCMSIS < 0x20000U)
    timer = osKernelSysTick();
    /* block until SDIO IP is ready or a timeout occur */
    while (osKernelSysTick() - timer < timeout)
#else
    timer = osKernelGetTickCount();
    /* block until SDIO IP is ready or a timeout occur */
    while (osKernelGetTickCount() - timer < timeout)
#endif
    {
        if (BSP_SD_GetCardState(BSP_SD_INSTANCE) == SD_TRANSFER_OK) {
            return 0;
        }
    }

    return -1;
}

static DSTATUS SD_CheckStatus(BYTE lun) {
    Stat = STA_NOINIT;

    if (BSP_SD_GetCardState(BSP_SD_INSTANCE) == BSP_ERROR_NONE) {
        Stat &= ~STA_NOINIT;
    }

    return Stat;
}

/**
 * @brief  Initializes a Drive
 * @param  lun : not used
 * @retval DSTATUS: Operation status
 */
DSTATUS SD_initialize(BYTE lun) {
    Stat = STA_NOINIT;
    /*
     * check that the kernel has been started before continuing
     * as the osMessage API will fail otherwise
     */

    Stat = SD_CheckStatus(lun);

    return Stat;
}

/**
 * @brief  Gets Disk Status
 * @param  lun : not used
 * @retval DSTATUS: Operation status
 */
DSTATUS SD_status(BYTE lun) { return SD_CheckStatus(lun); }

/**
 * @brief  Reads Sector(s)
 * @param  lun : not used
 * @param  *buff: Data buffer to store read data
 * @param  sector: Sector address (LBA)
 * @param  count: Number of sectors to read (1..128)
 * @retval DRESULT: Operation result
 */
DRESULT SD_read(BYTE lun, BYTE *buff, DWORD sector, UINT count) {
    DRESULT res = RES_ERROR;
    int32_t returnSD;

    /*
     * ensure the SDCard is ready for a new operation
     */

    if (SD_CheckStatusWithTimeout(SD_TIMEOUT) < 0) {
        return res;
    }

    if (!((uint32_t)buff & 0x1F)) {
        if (BSP_SD_ReadBlocks_DMA(BSP_SD_INSTANCE, (uint32_t *)buff,
                                  (uint32_t)(sector),
                                  count) != BSP_ERROR_NONE) {
            return RES_ERROR;
        } else {
            return RES_OK;
        }
    } else {
        /* Slow path, fetch each sector a part and memcpy to destination
         * buffer */

        uint32_t i;

        for (i = 0; i < count; i++) {
            returnSD =
                BSP_SD_ReadBlocks_DMA(BSP_SD_INSTANCE, (uint32_t *)&readBuf[0],
                                      (uint32_t)sector++, 1);
            if (returnSD == BSP_ERROR_NONE) {
                memcpy(buff, &readBuf[0], BLOCKSIZE);
                buff += BLOCKSIZE;
            } else {
                break;
            }
        }
        if ((i == count) && (returnSD == BSP_ERROR_NONE))
            res = RES_OK;
    }
    return res;
}

/**
 * @brief  Writes Sector(s)
 * @param  lun : not used
 * @param  *buff: Data to be written
 * @param  sector: Sector address (LBA)
 * @param  count: Number of sectors to write (1..128)
 * @retval DRESULT: Operation result
 */
#if _USE_WRITE == 1
DRESULT SD_write(BYTE lun, const BYTE *buff, DWORD sector, UINT count) {
    DRESULT res = RES_ERROR;
    int32_t returnSD;

    /*
     * ensure the SDCard is ready for a new operation
     */

    if (SD_CheckStatusWithTimeout(SD_TIMEOUT) < 0) {
        return res;
    }

    if (!((uint32_t)buff & 0x1F)) {
        if (BSP_SD_WriteBlocks_DMA(BSP_SD_INSTANCE, (uint32_t *)buff,
                                   (uint32_t)(sector),
                                   count) != BSP_ERROR_NONE) {
            return RES_ERROR;
        } else {
            return RES_OK;
        }
    } else {
        /* Slow path, fetch each sector a part and memcpy to destination
         * buffer
         */
        uint32_t i;
        for (i = 0; i < count; i++) {
            memcpy((void *)&writeBuf[0], buff, BLOCKSIZE);
            buff += BLOCKSIZE;

            returnSD = BSP_SD_WriteBlocks_DMA(BSP_SD_INSTANCE,
                                              (uint32_t *)&writeBuf[0],
                                              (uint32_t)sector++, 1);
            if (returnSD != BSP_ERROR_NONE) {
                break;
            }
        }

        if ((i == count) && (returnSD == BSP_ERROR_NONE))
            res = RES_OK;
    }
    return res;
}
#endif /* _USE_WRITE == 1 */

/**
 * @brief  I/O control operation
 * @param  lun : not used
 * @param  cmd: Control code
 * @param  *buff: Buffer to send/receive control data
 * @retval DRESULT: Operation result
 */
#if _USE_IOCTL == 1
DRESULT SD_ioctl(BYTE lun, BYTE cmd, void *buff) {
    DRESULT res = RES_ERROR;
    BSP_SD_CardInfo CardInfo;

    if (Stat & STA_NOINIT)
        return RES_NOTRDY;

    switch (cmd) {
    /* Make sure that no pending write process */
    case CTRL_SYNC:
        res = RES_OK;
        break;

    /* Get number of sectors on the disk (DWORD) */
    case GET_SECTOR_COUNT:
        BSP_SD_GetCardInfo(BSP_SD_INSTANCE, &CardInfo);
        *(DWORD *)buff = CardInfo.LogBlockNbr;
        res = RES_OK;
        break;

    /* Get R/W sector size (WORD) */
    case GET_SECTOR_SIZE:
        BSP_SD_GetCardInfo(BSP_SD_INSTANCE, &CardInfo);
        *(WORD *)buff = CardInfo.LogBlockSize;
        res = RES_OK;
        break;

    /* Get erase block size in unit of sector (DWORD) */
    case GET_BLOCK_SIZE:
        BSP_SD_GetCardInfo(BSP_SD_INSTANCE, &CardInfo);
        *(DWORD *)buff = CardInfo.LogBlockSize / SD_DEFAULT_BLOCK_SIZE;
        res = RES_OK;
        break;

    default:
        res = RES_PARERR;
    }

    return res;
}
#endif /* _USE_IOCTL == 1 */
