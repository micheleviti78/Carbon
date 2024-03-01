/**
 ******************************************************************************
 * @file           sdram.cpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Feb. 2022
 * @brief          DISCO-STM32H747 FMC, source file
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
#include <carbon/sdram.hpp>

#include <stm32h7xx_hal.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FMC_D28_Pin GPIO_PIN_6
#define FMC_D28_GPIO_Port GPIOI
#define FMC_NBL3_Pin GPIO_PIN_5
#define FMC_NBL3_GPIO_Port GPIOI
#define FMC_NBL2_Pin GPIO_PIN_4
#define FMC_NBL2_GPIO_Port GPIOI
#define FMC_D25_Pin GPIO_PIN_1
#define FMC_D25_GPIO_Port GPIOI
#define FMC_D24_Pin GPIO_PIN_0
#define FMC_D24_GPIO_Port GPIOI
#define FMC_D29_Pin GPIO_PIN_7
#define FMC_D29_GPIO_Port GPIOI
#define FMC_NBL1_Pin GPIO_PIN_1
#define FMC_NBL1_GPIO_Port GPIOE
#define FMC_D26_Pin GPIO_PIN_2
#define FMC_D26_GPIO_Port GPIOI
#define FMC_D23_Pin GPIO_PIN_15
#define FMC_D23_GPIO_Port GPIOH
#define FMC_D22_Pin GPIO_PIN_14
#define FMC_D22_GPIO_Port GPIOH
#define FMC_NBL0_Pin GPIO_PIN_0
#define FMC_NBL0_GPIO_Port GPIOE
#define FMC_D27_Pin GPIO_PIN_3
#define FMC_D27_GPIO_Port GPIOI
#define FMC_SDCAS_Pin GPIO_PIN_15
#define FMC_SDCAS_GPIO_Port GPIOG
#define FMC_D2_Pin GPIO_PIN_0
#define FMC_D2_GPIO_Port GPIOD
#define FMC_D21_Pin GPIO_PIN_13
#define FMC_D21_GPIO_Port GPIOH
#define FMC_D30_Pin GPIO_PIN_9
#define FMC_D30_GPIO_Port GPIOI
#define FMC_D3_Pin GPIO_PIN_1
#define FMC_D3_GPIO_Port GPIOD
#define FMC_D31_Pin GPIO_PIN_10
#define FMC_D31_GPIO_Port GPIOI
#define FMC_SDCLK_Pin GPIO_PIN_8
#define FMC_SDCLK_GPIO_Port GPIOG
#define FMC_A2_Pin GPIO_PIN_2
#define FMC_A2_GPIO_Port GPIOF
#define FMC_A1_Pin GPIO_PIN_1
#define FMC_A1_GPIO_Port GPIOF
#define FMC_A0_Pin GPIO_PIN_0
#define FMC_A0_GPIO_Port GPIOF
#define FMC_A3_Pin GPIO_PIN_3
#define FMC_A3_GPIO_Port GPIOF
#define FMC_BA0_Pin GPIO_PIN_4
#define FMC_BA0_GPIO_Port GPIOG
#define FMC_BA1_Pin GPIO_PIN_5
#define FMC_BA1_GPIO_Port GPIOG
#define FMC_A12_Pin GPIO_PIN_2
#define FMC_A12_GPIO_Port GPIOG
#define FMC_A5_Pin GPIO_PIN_5
#define FMC_A5_GPIO_Port GPIOF
#define FMC_A4_Pin GPIO_PIN_4
#define FMC_A4_GPIO_Port GPIOF
#define FMC_D7_Pin GPIO_PIN_10
#define FMC_D7_GPIO_Port GPIOE
#define FMC_SDNWE_Pin GPIO_PIN_5
#define FMC_SDNWE_GPIO_Port GPIOH
#define FMC_A7_Pin GPIO_PIN_13
#define FMC_A7_GPIO_Port GPIOF
#define FMC_A8_Pin GPIO_PIN_14
#define FMC_A8_GPIO_Port GPIOF
#define FMC_D6_Pin GPIO_PIN_9
#define FMC_D6_GPIO_Port GPIOE
#define FMC_D8_Pin GPIO_PIN_11
#define FMC_D8_GPIO_Port GPIOE
#define FMC_D18_Pin GPIO_PIN_10
#define FMC_D18_GPIO_Port GPIOH
#define FMC_D19_Pin GPIO_PIN_11
#define FMC_D19_GPIO_Port GPIOH
#define FMC_D1_Pin GPIO_PIN_15
#define FMC_D1_GPIO_Port GPIOD
#define FMC_D0_Pin GPIO_PIN_14
#define FMC_D0_GPIO_Port GPIOD
#define FMC_A6_Pin GPIO_PIN_12
#define FMC_A6_GPIO_Port GPIOF
#define FMC_A9_Pin GPIO_PIN_15
#define FMC_A9_GPIO_Port GPIOF
#define FMC_D9_Pin GPIO_PIN_12
#define FMC_D9_GPIO_Port GPIOE
#define FMC_D12_Pin GPIO_PIN_15
#define FMC_D12_GPIO_Port GPIOE
#define FMC_D17_Pin GPIO_PIN_9
#define FMC_D17_GPIO_Port GPIOH
#define FMC_D20_Pin GPIO_PIN_12
#define FMC_D20_GPIO_Port GPIOH
#define FMC_SDRAS_Pin GPIO_PIN_11
#define FMC_SDRAS_GPIO_Port GPIOF
#define FMC_A10_Pin GPIO_PIN_0
#define FMC_A10_GPIO_Port GPIOG
#define FMC_D5_Pin GPIO_PIN_8
#define FMC_D5_GPIO_Port GPIOE
#define FMC_D10_Pin GPIO_PIN_13
#define FMC_D10_GPIO_Port GPIOE
#define FMC_SDNE1_Pin GPIO_PIN_6
#define FMC_SDNE1_GPIO_Port GPIOH
#define FMC_D16_Pin GPIO_PIN_8
#define FMC_D16_GPIO_Port GPIOH
#define FMC_D15_Pin GPIO_PIN_10
#define FMC_D15_GPIO_Port GPIOD
#define FMC_D14_Pin GPIO_PIN_9
#define FMC_D14_GPIO_Port GPIOD
#define FMC_A11_Pin GPIO_PIN_1
#define FMC_A11_GPIO_Port GPIOG
#define FMC_D4_Pin GPIO_PIN_7
#define FMC_D4_GPIO_Port GPIOE
#define FMC_D11_Pin GPIO_PIN_14
#define FMC_D11_GPIO_Port GPIOE
#define FMC_SDCKE1_Pin GPIO_PIN_7
#define FMC_SDCKE1_GPIO_Port GPIOH
#define FMC_D13_Pin GPIO_PIN_8
#define FMC_D13_GPIO_Port GPIOD

#define SDRAM_REFRESH_PERIOD 64.
#define SDRAM_ROWS 4096lu
#define SDRAM_CLOCK 100lu
#define SDRAM_REFRESH_COUNT 0x00000603

static uint32_t FMC_Initialized = 0;
static GPIO_InitTypeDef GPIO_InitStruct;
static RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;
static SDRAM_HandleTypeDef hsdram1;
static FMC_SDRAM_TimingTypeDef SdramTiming;

/* FMC initialization function */
static bool init_fmc(void) {
    /** Perform the SDRAM1 memory initialization sequence
     */
    hsdram1.Instance = FMC_SDRAM_DEVICE;
    /* hsdram1.Init */
    hsdram1.Init.SDBank = FMC_SDRAM_BANK2;
    hsdram1.Init.ColumnBitsNumber = FMC_SDRAM_COLUMN_BITS_NUM_9;
    hsdram1.Init.RowBitsNumber = FMC_SDRAM_ROW_BITS_NUM_12;
    hsdram1.Init.MemoryDataWidth = FMC_SDRAM_MEM_BUS_WIDTH_32;
    hsdram1.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;
    hsdram1.Init.CASLatency = FMC_SDRAM_CAS_LATENCY_2;
    hsdram1.Init.WriteProtection = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
    hsdram1.Init.SDClockPeriod = FMC_SDRAM_CLOCK_PERIOD_2;
    hsdram1.Init.ReadBurst = FMC_SDRAM_RBURST_ENABLE;
    hsdram1.Init.ReadPipeDelay = FMC_SDRAM_RPIPE_DELAY_0;
    /* SdramTiming */
    SdramTiming.LoadToActiveDelay = 2;    // from manual 2 TMRD
    SdramTiming.ExitSelfRefreshDelay = 6; // from manual 7 TXSR
    SdramTiming.SelfRefreshTime = 4;      // from manual 5 TRAS
    SdramTiming.RowCycleDelay = 6;        // from manual 2 TRCD
    SdramTiming.WriteRecoveryTime = 2;    // from manual 3 TWR
    SdramTiming.RPDelay = 2;              // from manual 2 TRP
    SdramTiming.RCDDelay = 2;             // from manual 6 TRC

    if (HAL_SDRAM_Init(&hsdram1, &SdramTiming) != HAL_OK) {
        return false;
    }

    return true;
}

void HAL_FMC_MspInit() {
    if (FMC_Initialized) {
        return;
    }
    FMC_Initialized = 1;

    /** Initializes the peripherals clock
     */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_FMC;
    PeriphClkInitStruct.FmcClockSelection = RCC_FMCCLKSOURCE_D1HCLK;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
        Error_Handler();
    }

    /* Peripheral clock enable */
    __HAL_RCC_FMC_CLK_ENABLE();

    /** FMC GPIO Configuration
    PI0   ------> FMC_D24_Pin
    PI1   ------> FMC_D25_Pin
    PI2   ------> FMC_D26_Pin
    PI3   ------> FMC_D27_Pin
    PI4   ------> FMC_NBL2_Pin
    PI5   ------> FMC_NBL3_Pin
    PI6   ------> FMC_D28_Pin
    PI7   ------> FMC_D29_Pin
    PI9   ------> FMC_D30_Pin
    PI10  ------> FMC_D31_Pin
    PE0   ------> FMC_NBL0_Pin
    PE1   ------> FMC_NBL1_Pin
    PE7   ------> FMC_D4_Pin
    PE8   ------> FMC_D5_Pin
    PE9   ------> FMC_D6_Pin
    PE10  ------> FMC_D7_Pin
    PE11  ------> FMC_D8_Pin
    PE12  ------> FMC_D9_Pin
    PE13  ------> FMC_D10_Pin
    PE14  ------> FMC_D11_Pin
    PE15  ------> FMC_D12_Pin
    PH5   ------> FMC_SDNWE_Pin
    PH6   ------> FMC_SDNE1_Pin
    PH7   ------> FMC_SDCKE1_Pin
    PH8   ------> FMC_D16_Pin
    PH9   ------> FMC_D17_Pin
    PH10  ------> FMC_D18_Pin
    PH11  ------> FMC_D19_Pin
    PH12  ------> FMC_D20_Pin
    PH13  ------> FMC_D21_Pin
    PH14  ------> FMC_D22_Pin
    PH15  ------> FMC_D23_Pin
    PG0   ------> FMC_A10_Pin
    PG1   ------> FMC_A11_Pin
    PG2   ------> FMC_A12_Pin
    PG4   ------> FMC_BA0_Pin
    PG5   ------> FMC_BA1_Pin
    PG8   ------> FMC_SDCLK_Pin
    PG15  ------> FMC_SDNCAS_Pin
    PD0   ------> FMC_D2_Pin
    PD1   ------> FMC_D3_Pin
    PD8   ------> FMC_D13_Pin
    PD9   ------> FMC_D14_Pin
    PD10  ------> FMC_D15_Pin
    PD14  ------> FMC_D0_Pin
    PD15  ------> FMC_D1_Pin
    PF0   ------> FMC_A0_Pin
    PF1   ------> FMC_A1_Pin
    PF2   ------> FMC_A2_Pin
    PF3   ------> FMC_A3_Pin
    PF4   ------> FMC_A4_Pin
    PF5   ------> FMC_A5_Pin
    PF11  ------> FMC_SDNRAS_Pin
    PF12  ------> FMC_A6_Pin
    PF13  ------> FMC_A7_Pin
    PF14  ------> FMC_A8_Pin
    PF15  ------> FMC_A9_Pin
    */

    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOI_CLK_ENABLE();

    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_FMC;

    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
                          GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 |
                          GPIO_PIN_9 | GPIO_PIN_10;
    HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_7 | GPIO_PIN_8 |
                          GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 |
                          GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 |
                          GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 |
                          GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4 |
                          GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_8 | GPIO_PIN_9 |
                          GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
                          GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_11 | GPIO_PIN_12 |
                          GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
}
void HAL_SDRAM_MspInit(SDRAM_HandleTypeDef *hsdram) { HAL_FMC_MspInit(); }

void init_sdram() {
    if (!init_fmc()) {
        RAW_DIAG("error initialization FMC");
    }

    uint32_t tmpreg = 0, timeout = 0xFFFF;
    __IO uint32_t index;

    /* SDRAM initialization sequence */

    /*Enabling Clock and Bank 2*/
    SET_BIT(hsdram1.Instance->SDCMR, FMC_SDCMR_MODE_0 | FMC_SDCMR_CTB2);

    tmpreg = FMC_Bank5_6_R->SDSR & FMC_SDSR_MODES2_Msk;
    while ((tmpreg != 0) && (timeout-- > 0)) {
        tmpreg = FMC_Bank5_6_R->SDSR & FMC_SDSR_MODES2_Msk;
    }

    /* Delay */
    for (index = 0; index < 1000; index++)
        ;

    /*issuing command precharge all for Bank2*/
    SET_BIT(hsdram1.Instance->SDCMR, FMC_SDCMR_MODE_1 | FMC_SDCMR_CTB2);
    tmpreg = FMC_Bank5_6_R->SDSR & FMC_SDSR_MODES2_Msk;
    timeout = 0xFFFF;
    while ((tmpreg != 0) && (timeout-- > 0)) {
        tmpreg = FMC_Bank5_6_R->SDSR & FMC_SDSR_MODES2_Msk;
    }

    /*issuing command auto charge (8 cycles)*/
    SET_BIT(hsdram1.Instance->SDCMR, FMC_SDCMR_MODE_0 | FMC_SDCMR_MODE_1 |
                                         FMC_SDCMR_CTB2 | FMC_SDCMR_NRFS_0 |
                                         FMC_SDCMR_NRFS_1 | FMC_SDCMR_NRFS_2);
    // FMC_Bank5_6_R->SDCMR = 0x000000EB;
    timeout = 0xFFFF;
    tmpreg = FMC_Bank5_6_R->SDSR & FMC_SDSR_MODES2_Msk;
    while ((tmpreg != 0) && (timeout-- > 0)) {
        tmpreg = FMC_Bank5_6_R->SDSR & FMC_SDSR_MODES2_Msk;
    }

/*issuing command Load Mode Register*/
/*RAM configuration
    Mode Register
       M0 = M1 = M2 = 0 Burst Length 1
       M5 = 1 CAS Latency 2
       M9 = Single Location Access
*/
#define SDRAM_MODE_REGISTER 0x220UL
#define SDRAM_SDCMR_MRD SDRAM_MODE_REGISTER << FMC_SDCMR_MRD_Pos
#define FMC_SDCMR_LOAD_MODE 0x4UL
    SET_BIT(hsdram1.Instance->SDCMR,
            FMC_SDCMR_LOAD_MODE | FMC_SDCMR_CTB2 | SDRAM_SDCMR_MRD);
    tmpreg = FMC_Bank5_6_R->SDSR & FMC_SDSR_MODES2_Msk;
    timeout = 0xFFFF;
    while ((tmpreg != 0) && (timeout-- > 0)) {
        tmpreg = FMC_Bank5_6_R->SDSR & FMC_SDSR_MODES2_Msk;
    }

    /*Set refresh count*/
    if (HAL_SDRAM_ProgramRefreshRate(&hsdram1, SDRAM_REFRESH_COUNT) != HAL_OK) {
        RAW_DIAG("Error setting SDRAM refresh rate");
    }

    /*write protection not activated*/
    // if (HAL_SDRAM_WriteProtection_Disable(&hsdram1) != HAL_OK) {
    //     RAW_DIAG("Error disabling SDRAM write protection %d");
    // }

    /*1 ms delay*/
    HAL_Delay(1);

    /*RES interrupt enable*/
    __FMC_SDRAM_ENABLE_IT(hsdram1.Instance, FMC_SDRTR_REIE);

    /*enable FMC interrupt*/
    HAL_NVIC_SetPriority(FMC_IRQn, 15, 0);
    HAL_NVIC_EnableIRQ(FMC_IRQn);

    __FMC_ENABLE();

    return;
}

void fmc_isr(void) { HAL_SDRAM_IRQHandler(&hsdram1); }

#ifdef __cplusplus
}
#endif
