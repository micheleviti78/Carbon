/**
 ******************************************************************************
 * @file           main.cpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Jan. 2022
 * @brief          CM4 main program body
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

extern "C" {

void mainThread(const void */*argument*/){}

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
    DIAG(SYSTEM_DIAG "CM4 ready");

    /* Infinite loop */
    while (1) {
        HAL_Delay(500);
        BSP_LED_Toggle(LED_BLUE);
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
void assert_failed(uint8_t *file, uint32_t line) {
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line
       number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line)
     */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
}
