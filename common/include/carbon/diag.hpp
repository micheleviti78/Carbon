/**
 ******************************************************************************
 * @file           diag.hpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Feb. 2022
 * @brief          DISCO-STM32H747 Diagnostic Console, header file
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

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void carbon_raw_diag_print(const char *format, ...)
    __attribute__((format(printf, 1, 2)));
;

void carbon_diag_push(const char *format, ...)
    __attribute__((format(printf, 1, 2)));
;

void carbon_diag_pull();

#ifdef __cplusplus
}
#endif

#define LWIP_DIAG "[lwip] "
#define SYSTEM_DIAG "[system] "
#define TRACE_DIAG "[trace] "
#define TEST_DIAG "[test] "
#define SD "[sd] "
#define MP "[uPy] "
#define SPI_DIAG "[spi] "
#define MATRIX_DIS_DIAG "[md] "
#define FTP "[ftp] "
#define ETH_DIAG "[eth] "
#define ETH_TEST_DIAG "[ethtest] "

#ifdef CORE_CM7
#define DIAG_CPU "[CM7] "
#else
#define DIAG_CPU "[CM4] "
#endif

#define RAW_DIAG(...) carbon_raw_diag_print(DIAG_CPU __VA_ARGS__)

#define DIAG_HELPER(fmt, ...)                                                  \
    carbon_diag_push(DIAG_CPU fmt "\n\r", ##__VA_ARGS__)
#define DIAG(...) DIAG_HELPER(__VA_ARGS__)
