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
[[maybe_unused]] static void SystemInit_ExtMemCtl(void);

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
    SystemInit_ExtMemCtl();
    // init_sdram();

    /* true random generator init */
    carbon_rand_init();

    /* Initialize Pin needed by CM7 */
    BSP_LED_Init(LED_GREEN);
    BSP_LED_Init(LED_ORANGE);

    /* Initialize Pin needed by CM4 */
    BSP_LED_Init(LED_BLUE);
    BSP_LED_Init(LED_RED);

    /*init fifos*/

    /*DIAG FIFO*/
    FIFO_INIT(diag)

#ifdef FREERTOS_USE_TRACE
    /*DIAG TRACE*/
    FIFO_INIT(trace)
#endif
#ifdef SD_TEST
    int32_t err = BSP_SD_Init(0);
    if (err < 0) {
        RAW_DIAG("error SD init %ld", err);
        for (;;) {
        }
    }
#endif
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

/**
 * @brief  Setup the external memory controller.
 *         Called in startup_stm32h7xx.s before jump to main.
 *         This function configures the external memories SDRAM
 *         This SDRAM will be used as program data memory (including heap and
 * stack).
 * @param  None
 * @retval None
 */
void SystemInit_ExtMemCtl(void) {
    __IO uint32_t tmp = 0;
    uint32_t tmpreg = 0, timeout = 0xFFFF;
    __IO uint32_t index;

    /* Enable GPIOD, GPIOE, GPIOF, GPIOG, GPIOH and GPIOI interface
        clock */
    RCC->AHB4ENR |= 0x000001F8;

    /* Delay after an RCC peripheral clock enabling */
    tmp = READ_BIT(RCC->AHB4ENR, RCC_AHB4ENR_GPIOEEN);

    /* Connect PDx pins to FMC Alternate function */
    GPIOD->AFR[0] = 0x000000CC;
    GPIOD->AFR[1] = 0xCC000CCC;
    /* Configure PDx pins in Alternate function mode */
    GPIOD->MODER = 0xAFEAFFFA;
    /* Configure PDx pins speed to 100 MHz */
    GPIOD->OSPEEDR = 0xF03F000F;
    /* Configure PDx pins Output type to push-pull */
    GPIOD->OTYPER = 0x00000000;
    /* Configure PDx pins in Pull-up */
    GPIOD->PUPDR = 0x50150005;

    /* Connect PEx pins to FMC Alternate function */
    GPIOE->AFR[0] = 0xC00000CC;
    GPIOE->AFR[1] = 0xCCCCCCCC;
    /* Configure PEx pins in Alternate function mode */
    GPIOE->MODER = 0xAAAABFFA;
    /* Configure PEx pins speed to 100 MHz */
    GPIOE->OSPEEDR = 0xFFFFC00F;
    /* Configure PEx pins Output type to push-pull */
    GPIOE->OTYPER = 0x00000000;
    /* Configure PEx pins in Pull-up */
    GPIOE->PUPDR = 0x55554005;

    /* Connect PFx pins to FMC Alternate function */
    GPIOF->AFR[0] = 0x00CCCCCC;
    GPIOF->AFR[1] = 0xCCCCC000;
    /* Configure PFx pins in Alternate function mode */
    GPIOF->MODER = 0xAABFFAAA;
    /* Configure PFx pins speed to 100 MHz */
    GPIOF->OSPEEDR = 0xFFC00FFF;
    /* Configure PFx pins Output type to push-pull */
    GPIOF->OTYPER = 0x00000000;
    /* Configure PFx pins in Pull-up */
    GPIOF->PUPDR = 0x55400555;

    /* Connect PGx pins to FMC Alternate function */
    GPIOG->AFR[0] = 0x00CCCCCC;
    GPIOG->AFR[1] = 0xC000000C;
    /* Configure PGx pins in Alternate function mode */
    GPIOG->MODER = 0xBFFEFAAA;
    /* Configure PGx pins speed to 100 MHz */
    GPIOG->OSPEEDR = 0xC0030FFF;
    /* Configure PGx pins Output type to push-pull */
    GPIOG->OTYPER = 0x00000000;
    /* Configure PGx pins in Pull-up */
    GPIOG->PUPDR = 0x40010555;

    /* Connect PHx pins to FMC Alternate function */
    GPIOH->AFR[0] = 0xCCC00000;
    GPIOH->AFR[1] = 0xCCCCCCCC;
    /* Configure PHx pins in Alternate function mode */
    GPIOH->MODER = 0xAAAAABFF;
    /* Configure PHx pins speed to 100 MHz */
    GPIOH->OSPEEDR = 0xFFFFFC00;
    /* Configure PHx pins Output type to push-pull */
    GPIOH->OTYPER = 0x00000000;
    /* Configure PHx pins in Pull-up */
    GPIOH->PUPDR = 0x55555400;

    /* Connect PIx pins to FMC Alternate function */
    GPIOI->AFR[0] = 0xCCCCCCCC;
    GPIOI->AFR[1] = 0x00000CC0;
    /* Configure PIx pins in Alternate function mode */
    GPIOI->MODER = 0xFFEBAAAA;
    /* Configure PIx pins speed to 100 MHz */
    GPIOI->OSPEEDR = 0x003CFFFF;
    /* Configure PIx pins Output type to push-pull */
    GPIOI->OTYPER = 0x00000000;
    /* Configure PIx pins in Pull-up */
    GPIOI->PUPDR = 0x00145555;

    /*-- FMC Configuration
     * ------------------------------------------------------*/
    /* Enable the FMC interface clock */
    (RCC->AHB3ENR |= (RCC_AHB3ENR_FMCEN));
    /*SDRAM Timing and access interface configuration*/
    /*LoadToActiveDelay  = 2
      ExitSelfRefreshDelay = 6
      SelfRefreshTime      = 4
      RowCycleDelay        = 6
      WriteRecoveryTime    = 2
      RPDelay              = 2
      RCDDelay             = 2
      SDBank             = FMC_SDRAM_BANK2
      ColumnBitsNumber   = FMC_SDRAM_COLUMN_BITS_NUM_9
      RowBitsNumber      = FMC_SDRAM_ROW_BITS_NUM_12
      MemoryDataWidth    = FMC_SDRAM_MEM_BUS_WIDTH_32
      InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4
      CASLatency         = FMC_SDRAM_CAS_LATENCY_2
      WriteProtection    = FMC_SDRAM_WRITE_PROTECTION_DISABLE
      SDClockPeriod      = FMC_SDRAM_CLOCK_PERIOD_2
      ReadBurst          = FMC_SDRAM_RBURST_ENABLE
      ReadPipeDelay      = FMC_SDRAM_RPIPE_DELAY_0*/

    FMC_Bank5_6_R->SDCR[0] = 0x00001800;
    FMC_Bank5_6_R->SDCR[1] = 0x00000165;
    FMC_Bank5_6_R->SDTR[0] = 0x00105000;
    FMC_Bank5_6_R->SDTR[1] = 0x01010351;

    /* SDRAM initialization sequence */
    /* Clock enable command */
    FMC_Bank5_6_R->SDCMR = 0x00000009;
    tmpreg = FMC_Bank5_6_R->SDSR & 0x00000020;
    while ((tmpreg != 0) && (timeout-- > 0)) {
        tmpreg = FMC_Bank5_6_R->SDSR & 0x00000020;
    }

    /* Delay */
    for (index = 0; index < 1000; index++)
        ;

    /* PALL command */
    FMC_Bank5_6_R->SDCMR = 0x0000000A;
    timeout = 0xFFFF;
    while ((tmpreg != 0) && (timeout-- > 0)) {
        tmpreg = FMC_Bank5_6_R->SDSR & 0x00000020;
    }

    FMC_Bank5_6_R->SDCMR = 0x000000EB;
    timeout = 0xFFFF;
    while ((tmpreg != 0) && (timeout-- > 0)) {
        tmpreg = FMC_Bank5_6_R->SDSR & 0x00000020;
    }

    FMC_Bank5_6_R->SDCMR = 0x0004400C;
    timeout = 0xFFFF;
    while ((tmpreg != 0) && (timeout-- > 0)) {
        tmpreg = FMC_Bank5_6_R->SDSR & 0x00000020;
    }
    /* Set refresh count */
    tmpreg = FMC_Bank5_6_R->SDRTR;
    FMC_Bank5_6_R->SDRTR = (tmpreg | (0x00000603 << 1));

    /* Disable write protection */
    tmpreg = FMC_Bank5_6_R->SDCR[1];
    FMC_Bank5_6_R->SDCR[1] = (tmpreg & 0xFFFFFDFF);

    /*FMC controller Enable*/
    FMC_Bank1_R->BTCR[0] |= 0x80000000;

    (void)(tmp);
}
}
