/**
 ******************************************************************************
 * @file           io_utils.c
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Nov. 2024
 * @brief          io utilities
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

#include "io_utils.h"

#include <cmsis_os.h>

#define BUF_LEN 2048

FRESULT copy_file(const char *srcPath, const char *dstPath) {
    /* File system object corresponding to logical drive */
    FIL fsrc, fdst;
    /* File objects */
    uint8_t *buffer = (uint8_t *)pvPortMalloc(BUF_LEN); /* File copy buffer */
    FRESULT fr;
    /* FatFs function common result code */
    UINT br, bw;
    /* File read/write count */

    /* Open source file */
    fr = f_open(&fsrc, srcPath, FA_READ);
    if (fr) {
        vPortFree(buffer);
        return (int)fr;
    }

    /* Create destination file */
    fr = f_open(&fdst, dstPath, FA_WRITE | FA_CREATE_ALWAYS);
    if (fr) {
        vPortFree(buffer);
        return (int)fr;
    }

    /* Copy source to destination */
    while (1) {
        /* Read 'BUF_LEN' bytes from source file */
        fr = f_read(&fsrc, buffer, BUF_LEN, &br);
        if (fr != FR_OK || br == 0)
            break; /* Error condition or EOF */
        /* Write read data to the destination file */
        fr = f_write(&fdst, buffer, br, &bw);
        if (fr != FR_OK || bw < br)
            break; /* Error or disk full */
    }

    /* Close open files */
    f_close(&fsrc);
    f_close(&fdst);

    vPortFree(buffer);
    return fr;
}