/**
 ******************************************************************************
 * @file    stm32h7xx_it.c
 * @brief   Interrupt Service Routines.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <stm32h7xx_hal.h>

#include <diag.hpp>
#include <stm32h7xx_it.h>

#include <cmsis_os.h>

extern ETH_HandleTypeDef heth;

void fmc_isr(void);
void carbon_hw_us_systime_tim_isr(void);
void hsem_isr(void);

/******************************************************************************/
/*           Cortex Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
 * @brief This function handles Non maskable interrupt.
 */
void NMI_Handler(void) {
    RAW_DIAG("NMI_Handler");
    while (1) {
    }
}

/**
 * @brief This function handles Hard fault interrupt.
 */
void HardFault_Handler(void) {
    RAW_DIAG("HardFault_Handler");
    while (1) {
    }
}

/**
 * @brief This function handles Memory management fault.
 */
void MemManage_Handler(void) {
    RAW_DIAG("MemManage_Handler");
    while (1) {
    }
}

/**
 * @brief This function handles Pre-fetch fault, memory access fault.
 */
void BusFault_Handler(void) {
    RAW_DIAG("BusFault_Handler");
    while (1) {
    }
}

/**
 * @brief This function handles Undefined instruction or illegal state.
 */
void UsageFault_Handler(void) {
    RAW_DIAG("UsageFault_Handler");
    while (1) {
    }
}

/**
 * @brief This function handles Debug monitor.
 */
void DebugMon_Handler(void) {}

/**
 * @brief This function handles System tick timer.
 */
void SysTick_Handler(void) { osSystickHandler(); }

/**
 * @brief This function handles FMC IRQ.
 */
void FMC_IRQHandler(void) { fmc_isr(); }

/**
 * @brief This function handles TIM2.
 */

void TIM2_IRQHandler(void) { carbon_hw_us_systime_tim_isr(); }

/**
 * @brief This function handles HSEM1.
 */

void HSEM1_IRQHandler(void) { hsem_isr(); }

/**
 * @brief This function handles Ethernet.
 */
#if !defined TEST_FIFO && !defined TEST_HSEM
void ETH_IRQHandler(void) { HAL_ETH_IRQHandler(&heth); }
#endif
