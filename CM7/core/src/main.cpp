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

#include <stm32h7xx_hal.h>

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
    DIAG(SYSTEM_DIAG "CM7 ready");

    DIAG(SYSTEM_DIAG "Initialization complete");
    DIAG(SYSTEM_DIAG "Newlib version %d.%d.%d", __NEWLIB__, __NEWLIB_MINOR__,
         __NEWLIB_PATCHLEVEL__);
    DIAG(SYSTEM_DIAG "HAL version %lu.%lu.%lu.%lu", GET_HAL_VERSION_MAIN,
         GET_HAL_VERSION_SUB1, GET_HAL_VERSION_SUB2, GET_HAL_VERSION_RC);

    /* starting OS */
    start_os();

    while (1)
        ;
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
