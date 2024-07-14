/**
 ******************************************************************************
 * @file    interrupts.c
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
#include <carbon/diag.hpp>
#include <carbon/sd_card.hpp>

#include <stm32h7xx_hal.h>

#include <cmsis_os.h>

extern ETH_HandleTypeDef heth;

void fmc_isr(void);
void carbon_hw_us_systime_tim_isr(void);
void carbon_hw_matrix_display_spi_isr(void);
void carbon_hw_matrix_display_dma_isr(void);
void hsem_isr(void);

#include <backtrace.h>

#define BACKTRACE_SIZE 128

static inline void __attribute__((always_inline)) do_backtrace(uint32_t pc) {
    static backtrace_t backtrace_buf[BACKTRACE_SIZE];
    int count = backtrace_unwind(backtrace_buf, BACKTRACE_SIZE);
    RAW_DIAG("backtrace unwind count %d", count);
    int i = 0;
    for (; i < count; ++i) {
        if ((uint32_t)backtrace_buf[i].address == pc)
            break;
    }
    for (; i < count; ++i) {
        RAW_DIAG("## %p %p", backtrace_buf[i].address,
                 backtrace_buf[i].function);
    }
}

/**
 * stack_unfold_hard_fault:
 * This is called from the HardFault_HandlerAsm with a pointer the Fault stack
 * as the parameter. We can then read the values from the stack and place them
 * into local variables for ease of reading.
 * We then read the various Fault Status and Address Registers to help decode
 * cause of the fault.
 */

void stack_unfold_hard_fault(unsigned long *hardfault_args) {

    volatile unsigned long stacked_r0;
    volatile unsigned long stacked_r1;
    volatile unsigned long stacked_r2;
    volatile unsigned long stacked_r3;
    volatile unsigned long stacked_r12;
    volatile unsigned long stacked_lr;
    volatile unsigned long stacked_pc;
    volatile unsigned long stacked_psr;

    volatile unsigned long _CFSR;
    volatile unsigned long _HFSR;
    volatile unsigned long _DFSR;
    volatile unsigned long _AFSR;
    volatile unsigned long _BFAR;
    volatile unsigned long _MMAR;

    stacked_r0 = ((unsigned long)hardfault_args[0]);
    stacked_r1 = ((unsigned long)hardfault_args[1]);
    stacked_r2 = ((unsigned long)hardfault_args[2]);
    stacked_r3 = ((unsigned long)hardfault_args[3]);
    stacked_r12 = ((unsigned long)hardfault_args[4]);
    stacked_lr = ((unsigned long)hardfault_args[5]);
    stacked_pc = ((unsigned long)hardfault_args[6]);
    stacked_psr = ((unsigned long)hardfault_args[7]);

    // Configurable Fault Status Register
    // Consists of MMSR, BFSR and UFSR
    _CFSR = (*((volatile unsigned long *)(0xE000ED28)));
    // Hard Fault Status Register
    _HFSR = (*((volatile unsigned long *)(0xE000ED2C)));
    // Debug Fault Status Register
    _DFSR = (*((volatile unsigned long *)(0xE000ED30)));
    // Auxiliary Fault Status Register
    _AFSR = (*((volatile unsigned long *)(0xE000ED3C)));

    // Read the Fault Address Registers. These may not contain valid values.
    // Check BFARVALID/MMARVALID to see if they are valid values

    // MemManage Fault Address Register
    _MMAR = (*((volatile unsigned long *)(0xE000ED34)));
    // Bus Fault Address Register
    _BFAR = (*((volatile unsigned long *)(0xE000ED38)));

    RAW_DIAG("hard fault R0 %lu", stacked_r0);
    RAW_DIAG("hard fault R1 %lu", stacked_r1);
    RAW_DIAG("hard fault R2 %lu", stacked_r2);
    RAW_DIAG("hard fault R3 %lu", stacked_r3);
    RAW_DIAG("hard fault R12 %lu", stacked_r12);
    RAW_DIAG("hard fault LR %lu", stacked_lr);
    RAW_DIAG("hard fault PC %lu", stacked_pc);
    RAW_DIAG("hard fault PSR %lu", stacked_psr);

    RAW_DIAG(" ");

    RAW_DIAG("hard fault _CFSR %lu", _CFSR);
    RAW_DIAG("hard fault _HFSR %lu", _HFSR);
    RAW_DIAG("hard fault _DFSR %lu", _DFSR);
    RAW_DIAG("hard fault _AFSR %lu", _AFSR);
    RAW_DIAG("hard fault _BFAR %lu", _BFAR);
    RAW_DIAG("hard fault _MMAR %lu", _MMAR);

    do_backtrace(stacked_pc);

    while (1) {
    }
}

void stack_unfold_mem_fault(unsigned long *hardfault_args) {
    volatile unsigned long stacked_pc = ((unsigned long)hardfault_args[6]);

    RAW_DIAG("Mem fault PC %lu", stacked_pc);

    // do_backtrace(stacked_pc);

    while (1) {
    }
}

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
__attribute__((naked)) void HardFault_Handler(void) {
    /*
     * Get the appropriate stack pointer, depending on our mode,
     * and use it as the parameter to the C handler. This function
     * will never return
     */
    __asm(".syntax unified\n"
          "MOVS R0, #4 \n"
          "MOV R1, LR \n"
          "TST R0, R1 \n"
          "BEQ _MSP \n"
          "MRS R0, PSP \n"
          "B stack_unfold_hard_fault \n"
          "_MSP: \n"
          "MRS R0, MSP \n"
          "B stack_unfold_hard_fault \n"
          ".syntax divided\n");
}

/**
 * @brief This function handles Memory management fault.
 */
__attribute__((naked)) void MemManage_Handler(void) {
    /*
     * Get the appropriate stack pointer, depending on our mode,
     * and use it as the parameter to the C handler. This function
     * will never return
     */
    __asm(".syntax unified\n"
          "MOVS R0, #4 \n"
          "MOV R1, LR \n"
          "TST R0, R1 \n"
          "BEQ _MSP_MEM \n"
          "MRS R0, PSP \n"
          "B stack_unfold_mem_fault \n"
          "_MSP_MEM: \n"
          "MRS R0, MSP \n"
          "B stack_unfold_mem_fault \n"
          ".syntax divided\n");
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
#if !defined FIFO_TEST && !defined HSEM_TEST && !defined SDRAM_TEST
void ETH_IRQHandler(void) { HAL_ETH_IRQHandler(&heth); }
#endif

/**
 * @brief This function handles SDMMC1
 */

void SDMMC1_IRQHandler(void) { BSP_SD_IRQHandler(0); }

/**
 * @brief This function handles EXTI9_5
 */

void EXTI9_5_IRQHandler(void) { HAL_EXTI_IRQHandler(&hsd_exti[0]); }

/**
 * @brief This function handles DMA1 stream0 global interrupt.
 */
void DMA1_Stream0_IRQHandler(void) { carbon_hw_matrix_display_dma_isr(); }

/**
 * @brief This function handles SPI2 global interrupt.
 */
void SPI2_IRQHandler(void) { carbon_hw_matrix_display_spi_isr(); }
