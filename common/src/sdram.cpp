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

#include <diag.hpp>
#include <error.hpp>
#include <main.hpp>
#include <sdram.hpp>

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
#define SDRAM_REFRESH_COUNT 603lu

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
  SdramTiming.LoadToActiveDelay = 2;    // TMRD
  SdramTiming.ExitSelfRefreshDelay = 7; // TXSR
  SdramTiming.SelfRefreshTime = 5;      // TRAS
  SdramTiming.RowCycleDelay = 2;        // TRCD
  SdramTiming.WriteRecoveryTime = 3;    // TWR
  SdramTiming.RPDelay = 2;              // TRP
  SdramTiming.RCDDelay = 6;             // TRC

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
  PI6   ------> FMC_D28
  PI5   ------> FMC_NBL3
  PI4   ------> FMC_NBL2
  PI1   ------> FMC_D25
  PI0   ------> FMC_D24
  PI7   ------> FMC_D29
  PE1   ------> FMC_NBL1
  PI2   ------> FMC_D26
  PH15  ------> FMC_D23
  PH14  ------> FMC_D22
  PE0   ------> FMC_NBL0
  PI3   ------> FMC_D27
  PG15  ------> FMC_SDNCAS
  PD0   ------> FMC_D2
  PH13  ------> FMC_D21
  PI9   ------> FMC_D30
  PD1   ------> FMC_D3
  PI10  ------> FMC_D31
  PG8   ------> FMC_SDCLK
  PF2   ------> FMC_A2
  PF1   ------> FMC_A1
  PF0   ------> FMC_A0
  PF3   ------> FMC_A3
  PG4   ------> FMC_BA0
  PG2   ------> FMC_A12
  PF5   ------> FMC_A5
  PF4   ------> FMC_A4
  PE10  ------> FMC_D7
  PH5   ------> FMC_SDNWE
  PF13  ------> FMC_A7
  PF14  ------> FMC_A8
  PE9   ------> FMC_D6
  PE11  ------> FMC_D8
  PH10  ------> FMC_D18
  PH11  ------> FMC_D19
  PD15  ------> FMC_D1
  PD14  ------> FMC_D0
  PF12  ------> FMC_A6
  PF15  ------> FMC_A9
  PE12  ------> FMC_D9
  PE15  ------> FMC_D12
  PH9   ------> FMC_D17
  PH12  ------> FMC_D20
  PF11  ------> FMC_SDNRAS
  PG0   ------> FMC_A10
  PE8   ------> FMC_D5
  PE13  ------> FMC_D10
  PH6   ------> FMC_SDNE1
  PH8   ------> FMC_D16
  PD10  ------> FMC_D15
  PD9   ------> FMC_D14
  PG1   ------> FMC_A11
  PE7   ------> FMC_D4
  PE14  ------> FMC_D11
  PH7   ------> FMC_SDCKE1
  PD8   ------> FMC_D13
  */

  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();

  GPIO_InitStruct.Pin = FMC_D28_Pin | FMC_NBL3_Pin | FMC_NBL2_Pin |
                        FMC_D25_Pin | FMC_D24_Pin | FMC_D29_Pin | FMC_D26_Pin |
                        FMC_D27_Pin | FMC_D30_Pin | FMC_D31_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = FMC_NBL1_Pin | FMC_NBL0_Pin | FMC_D7_Pin | FMC_D6_Pin |
                        FMC_D8_Pin | FMC_D9_Pin | FMC_D12_Pin | FMC_D5_Pin |
                        FMC_D10_Pin | FMC_D4_Pin | FMC_D11_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = FMC_D23_Pin | FMC_D22_Pin | FMC_D21_Pin |
                        FMC_SDNWE_Pin | FMC_D18_Pin | FMC_D19_Pin |
                        FMC_D17_Pin | FMC_D20_Pin | FMC_SDNE1_Pin |
                        FMC_D16_Pin | FMC_SDCKE1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = FMC_SDCAS_Pin | FMC_SDCLK_Pin | FMC_BA0_Pin |
                        FMC_A12_Pin | FMC_A10_Pin | FMC_A11_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = FMC_D2_Pin | FMC_D3_Pin | FMC_D1_Pin | FMC_D0_Pin |
                        FMC_D15_Pin | FMC_D14_Pin | FMC_D13_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = FMC_A2_Pin | FMC_A1_Pin | FMC_A0_Pin | FMC_A3_Pin |
                        FMC_A5_Pin | FMC_A4_Pin | FMC_A7_Pin | FMC_A8_Pin |
                        FMC_A6_Pin | FMC_A9_Pin | FMC_SDRAS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF12_FMC;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
}
void HAL_SDRAM_MspInit(SDRAM_HandleTypeDef *hsdram) { HAL_FMC_MspInit(); }

void init_sdram() {
  if (!init_fmc()) {
    RAW_DIAG("error initialization FMC");
  }

  /*Enabling Clock and Bank 2*/
  SET_BIT(hsdram1.Instance->SDCMR, FMC_SDCMR_MODE_0 | FMC_SDCMR_CTB2);

  /*1 ms delay*/
  HAL_Delay(1);

  /*issuing command precharge all for Bank2*/
  SET_BIT(hsdram1.Instance->SDCMR, FMC_SDCMR_MODE_1 | FMC_SDCMR_CTB2);

  /*1 ms delay*/
  HAL_Delay(1);

  /*issuing command auto charge (8 cycles)*/
  SET_BIT(hsdram1.Instance->SDCMR, FMC_SDCMR_MODE_0 | FMC_SDCMR_MODE_1 |
                                       FMC_SDCMR_CTB2 | FMC_SDCMR_NRFS_0 |
                                       FMC_SDCMR_NRFS_1 | FMC_SDCMR_NRFS_3);

  /*1 ms delay*/
  HAL_Delay(1);

/*issuing command Load Mode Register*/
#define FMC_SDCMR_MODE_2_FIX 0x3UL
  /* Mode Registerg
     M0 = M1 = M2 = 0 Burst Length 1
     M5 = 1 CAS Latency 2
     M9 = Single Location Access
  */

#define SDRAM_MODE_REGISTER 0x220UL << FMC_SDCMR_MRD_Pos
  SET_BIT(hsdram1.Instance->SDCMR,
          FMC_SDCMR_MODE_2_FIX | FMC_SDCMR_CTB2 | SDRAM_MODE_REGISTER);

  /*1 ms delay*/
  HAL_Delay(1);

  /*Set refresh count*/
  if (HAL_SDRAM_ProgramRefreshRate(&hsdram1, SDRAM_REFRESH_COUNT) != HAL_OK) {
    RAW_DIAG("Error setting SDRAM refresh rate");
  }

  /*RES interrupt enable*/
  __FMC_SDRAM_ENABLE_IT(hsdram1.Instance, FMC_SDRTR_REIE);

  /*enable FMC interrupt*/
  HAL_NVIC_SetPriority(FMC_IRQn, 15, 0);
  HAL_NVIC_EnableIRQ(FMC_IRQn);

  /*FMC controller Enable*/
  FMC_Bank1_R->BTCR[0] |= 0x80000000; // TODO use low level API

  return;
}

void fmc_isr(void) { HAL_SDRAM_IRQHandler(&hsdram1); }

#ifdef __cplusplus
}
#endif