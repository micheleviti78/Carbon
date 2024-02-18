/**
 ******************************************************************************
 * @file           main.cpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Jan. 2022
 * @brief          CM7 main program body
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

/* Includes ------------------------------------------------------------------*/
#include <carbon/diag.hpp>
#include <carbon/pin.hpp>

#include <stm32h7xx_hal.h>

#define BUFFER_SIZE 8388608

uint32_t testBuf[BUFFER_SIZE]
    __attribute__((aligned(4), section(".sdram_bank2")));

extern "C" {

void start_os(void);

#define GET_HAL_VERSION_MAIN ((HAL_GetHalVersion() >> 24) & 0xFFUL)
#define GET_HAL_VERSION_SUB1 ((HAL_GetHalVersion() >> 16) & 0xFFUL)
#define GET_HAL_VERSION_SUB2 ((HAL_GetHalVersion() >> 8) & 0xFFUL)
#define GET_HAL_VERSION_RC (HAL_GetHalVersion() & 0xFFUL)

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
    RAW_DIAG(SYSTEM_DIAG "CM7 ready");
    RAW_DIAG(SYSTEM_DIAG "Initialization complete");
    RAW_DIAG(SYSTEM_DIAG "Newlib version %d.%d.%d", __NEWLIB__,
             __NEWLIB_MINOR__, __NEWLIB_PATCHLEVEL__);
    RAW_DIAG(SYSTEM_DIAG "HAL version %lu.%lu.%lu.%lu", GET_HAL_VERSION_MAIN,
             GET_HAL_VERSION_SUB1, GET_HAL_VERSION_SUB2, GET_HAL_VERSION_RC);

    RAW_DIAG("testBuf address %lu", reinterpret_cast<uint32_t>(&testBuf[0]));

    for (unsigned i = 0; i < BUFFER_SIZE; i++) {
        testBuf[i] = i;
    }

    for (unsigned i = 0; i < BUFFER_SIZE; i++) {
        RAW_DIAG("%u", i);
        if (testBuf[i] != i) {
            RAW_DIAG("testBuf[%u] %lu != %u", i, testBuf[i], i);
            while (1) {
            }
        }
    }

    while (1) {
        BSP_LED_Toggle(LED_GREEN);
        HAL_Delay(1000);
    }
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line) {}
#endif /* USE_FULL_ASSERT */
}
