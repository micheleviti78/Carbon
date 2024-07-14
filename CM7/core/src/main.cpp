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

#include <carbon/diag.hpp>
#include <carbon/main_thread.hpp>

#include <stm32h7xx_hal.h>

static MainThread mainThread;

extern "C" {

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
    RAW_DIAG(SYSTEM_DIAG "CM7 ready");

    mainThread.start();

    RAW_DIAG(SYSTEM_DIAG "starting OS");

    osKernelStart();

    RAW_DIAG(SYSTEM_DIAG "ERROR OS");

    while (1) {
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
