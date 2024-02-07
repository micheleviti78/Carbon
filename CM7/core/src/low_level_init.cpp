/**
 ******************************************************************************
 * @file           low_level_init.cpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Dec. 2022
 * @brief          DISCO-STM32H747 low level initialization for CM7
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
#include <carbon/error.hpp>
#include <carbon/hsem.hpp>
#include <carbon/pin.hpp>
#include <carbon/rand.hpp>
#include <carbon/sdram.hpp>
#include <carbon/shared_memory.hpp>
#include <carbon/stm32h747i_discovery_sd.h>
#include <carbon/systime.hpp>
#include <carbon/uart.hpp>

#include <stm32h7xx_hal.h>

using namespace CARBON;

extern "C" {

void putchar_(char ch);

static void SystemClock_Config(void);

void low_level_init() {
    int32_t timeout;
    /* Wait until CPU2 boots and enters in stop mode or timeout*/
    timeout = 0xFFFF;
    while ((__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) != RESET) && (timeout-- > 0))
        ;

    if (timeout < 0) {
        Error_Handler();
    }

    /* activate cache */
    SCB_EnableICache();

    SCB_EnableDCache();

    /* HAL low level init */
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

    /* When system initialization is finished, Cortex-M7 will release Cortex-M4
     * by means of HSEM notification */
    /*init hardware semaphore*/
    hsemInit();
    /*Take HSEM */
    hSemInitSync.get();
    /*Release HSEM in order to notify the CPU2(CM4)*/
    hSemInitSync.release();
    /* wait until CPU2 wakes up from stop mode */
    timeout = 0xFFFF;
    while ((__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) == RESET) && (timeout-- > 0))
        ;
    if (timeout < 0) {
        Error_Handler();
    }

    /* init timer */
    low_level_system_time();

    /* init DIAG*/
    init_uart();

    putchar_('\r');
    putchar_('\n');

    /* init SDRAM */
    init_sdram();

    /* true random generator init */
    carbon_rand_init();

    /* Initialize Pin needed by the Error function */
    BSP_LED_Init(LED_GREEN);
    BSP_LED_Init(LED_ORANGE);

    /*init fifos*/

    /*DIAG FIFO*/
    FIFO_INIT(diag)

#ifdef FREERTOS_USE_TRACE
    /*DIAG TRACE*/
    FIFO_INIT(trace)
#endif

    BSP_SD_Init(0);

    setSyncFlag(SyncFlagBit::PeripherySync);
}

/**
 * @brief  System Clock Configuration
 *         The system Clock is configured as follow :
 *            System Clock source            = PLL (HSE)
 *            SYSCLK(Hz)                     = 400000000 (Cortex-M7 CPU Clock)
 *            HCLK(Hz)                       = 200000000 (Cortex-M4 CPU, Bus
 * matrix Clocks) AHB Prescaler                  = 2 D1 APB3 Prescaler = 2 (APB3
 * Clock  100MHz) D2 APB1 Prescaler              = 2 (APB1 Clock  100MHz) D2
 * APB2 Prescaler              = 2 (APB2 Clock  100MHz) D3 APB4 Prescaler = 2
 * (APB4 Clock  100MHz) HSE Frequency(Hz)              = 25000000 PLL_M = 4
 *            PLL_N                          = 128
 *            PLL_P                          = 2
 *            PLL_Q                          = 8
 *            PLL_R                          = 2
 *            PLL3_M                         = 5
 *            PLL3_N                         = 48
 *            PLL3_P                         = 2
 *            PLL3_Q                         = 5
 *            PLL3_R                         = 2
 *            VDD(V)                         = 3.3
 *            Flash Latency(WS)              = 4
 * @param  None
 * @retval None
 */
static void SystemClock_Config(void) {
    RCC_ClkInitTypeDef RCC_ClkInitStruct{};
    RCC_OscInitTypeDef RCC_OscInitStruct{};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct{};

    /*!< Supply configuration update enable */
    HAL_PWREx_ConfigSupply(PWR_DIRECT_SMPS_SUPPLY);

    /* The voltage scaling allows optimizing the power consumption when the
       device is clocked below the maximum system frequency, to update the
       voltage scaling value regarding system frequency refer to product
       datasheet.  */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {
    }

    // Enable HSE Oscillator and activate PLL with HSE as source
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
    RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
    RCC_OscInitStruct.CSIState = RCC_CSI_OFF;

    // PLL1 for system clock
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 4;
    RCC_OscInitStruct.PLL.PLLN = 128;
    RCC_OscInitStruct.PLL.PLLP = 2;
    RCC_OscInitStruct.PLL.PLLR = 2;
    RCC_OscInitStruct.PLL.PLLQ = 8;
    RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
    RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
    RCC_OscInitStruct.PLL.PLLFRACN = 0;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        assert_param(0);
    }

    // Select PLL as system clock source and configure bus clocks dividers
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
                                  RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 |
                                  RCC_CLOCKTYPE_D1PCLK1 | RCC_CLOCKTYPE_D3PCLK1;

    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
    RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK) {
        assert_param(0);
    }

    // PLL3
    PeriphClkInitStruct.PLL3.PLL3M = 5;
    PeriphClkInitStruct.PLL3.PLL3N = 48;
    PeriphClkInitStruct.PLL3.PLL3P = 2;
    PeriphClkInitStruct.PLL3.PLL3R = 2;
    PeriphClkInitStruct.PLL3.PLL3Q = 5;
    PeriphClkInitStruct.PLL3.PLL3RGE = RCC_PLL3VCIRANGE_2;
    PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOWIDE;
    PeriphClkInitStruct.PLL3.PLL3FRACN = 0;

    // PLL1Q 100MHZ for RNG
    PeriphClkInitStruct.PeriphClockSelection |= RCC_PERIPHCLK_RNG;
    PeriphClkInitStruct.RngClockSelection = RCC_RNGCLKSOURCE_PLL;

    // PLL1Q 100MHZ for SD
    PeriphClkInitStruct.PeriphClockSelection |= RCC_PERIPHCLK_SDMMC;
    PeriphClkInitStruct.SdmmcClockSelection = RCC_SDMMCCLKSOURCE_PLL;

    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
        assert_param(0);
    }

    /*
Note : The activation of the I/O Compensation Cell is recommended with
communication  interfaces (GPIO, SPI, FMC, QSPI ...)  when  operating at
high frequencies(please refer to product datasheet) The I/O Compensation
Cell activation  procedure requires :
  - The activation of the CSI clock
  - The activation of the SYSCFG clock
  - Enabling the I/O Compensation Cell : setting bit[0] of register
SYSCFG_CCCSR

    To do this please uncomment the following code
     */

    __HAL_RCC_CSI_ENABLE();

    __HAL_RCC_SYSCFG_CLK_ENABLE();

    HAL_EnableCompensationCell();
}
}
