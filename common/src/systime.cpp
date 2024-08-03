/**
 ******************************************************************************
 * @file           sysytime.cpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Feb. 2022
 * @brief          DISCO-STM32H747 system time
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
#include <carbon/irq.hpp>
#include <carbon/sync.hpp>
#include <carbon/systime.hpp>

#include <stm32h7xx_hal.h>
#include <stm32h7xx_ll_tim.h>

// NOTE: These functions are called during early init before static C++
// constructors. Do not access any static global classes from these
// functions!

using namespace CARBON;

#ifdef CORE_CM7
#define SYSTIME_TIM TIM2
#define SYSTIME_TIM_PERIOD 0xFFFFFFFF // NOTE: always set to full span
#define SYSTIME_TIM_IRQ TIM2_IRQn
#define SYSTIME_TIM_CLK_EN() __HAL_RCC_TIM2_CLK_ENABLE();
#else
#define SYSTIME_TIM TIM5
#define SYSTIME_TIM_PERIOD 0xFFFFFFFF // NOTE: always set to full span
#define SYSTIME_TIM_IRQ TIM5_IRQn
#define SYSTIME_TIM_CLK_EN() __HAL_RCC_TIM5_CLK_ENABLE()
#endif

static bool timRunning;

static IRQLockRecursive irqLockRecursive;

void low_level_system_time() {
#ifdef CORE_CM7
    /*
     *Setting DWT counter
     */
    DWT_Type *dwt = DWT;
    SET_BIT(dwt->CTRL, DWT_CTRL_CYCCNTENA_Msk);
#endif
    RCC_ClkInitTypeDef clkConfig{};

    uint32_t latency;
    HAL_RCC_GetClockConfig(&clkConfig, &latency);

    auto apb1Prescaler = clkConfig.APB1CLKDivider;

    auto pclk1Freq = HAL_RCC_GetPCLK1Freq();

    uint32_t timClk{0};
    switch (apb1Prescaler) {
    case RCC_APB1_DIV1:
        timClk = pclk1Freq;
        break;
    case RCC_APB1_DIV2:
        timClk = 2 * pclk1Freq;
        break;
    default:
        Error_Handler();
    }

    if (timClk < 1000000ul) {
        Error_Handler();
    }

    auto timPrescaler =
        static_cast<uint32_t>((timClk / 1000000) - 1); // 1MHz clock

    LL_TIM_InitTypeDef timInit{};
    LL_TIM_OC_InitTypeDef ocInit{};

    SYSTIME_TIM_CLK_EN();

    LL_TIM_StructInit(&timInit);
    timInit.Autoreload = SYSTIME_TIM_PERIOD;
    timInit.Prescaler = timPrescaler;
    timInit.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    timInit.CounterMode = TIM_COUNTERMODE_UP;
    LL_TIM_Init(SYSTIME_TIM, &timInit);

    LL_TIM_OC_StructInit(&ocInit);
    ocInit.CompareValue = SYSTIME_TIM_PERIOD >> 1;
    ErrorStatus result =
        LL_TIM_OC_Init(SYSTIME_TIM, LL_TIM_CHANNEL_CH1, &ocInit);

    if (result == ERROR) {
        while (1)
            ;
    }

    timRunning = true;

    SYSTIME_TIM->DIER |= TIM_DIER_CC1IE;
    SYSTIME_TIM->CR1 |= TIM_CR1_CEN;

    HAL_NVIC_SetPriority(SYSTIME_TIM_IRQ, TICK_INT_PRIORITY, 0);

    HAL_NVIC_EnableIRQ(SYSTIME_TIM_IRQ);
}

static volatile uint64_t usCounterBase = 0;
static volatile uint32_t usTimLastCount = 0;

static uint64_t usCounterAdjust(uint32_t usTimCnt) {
    if (usTimCnt < usTimLastCount) {
        usCounterBase += SYSTIME_TIM_PERIOD;
    }
    usTimLastCount = usTimCnt;
    return usCounterBase + usTimCnt;
}

uint64_t systimeUs() {
    LockGuard<IRQLockRecursive> lock(irqLockRecursive);
    uint64_t count = usCounterAdjust(SYSTIME_TIM->CNT);
    return count;
}

void delayUs(uint32_t us) {
    uint32_t CNT1 = SYSTIME_TIM->CNT;
    uint32_t CNT2;
    uint32_t diff;
    while (true) {
        do {
            CNT2 = SYSTIME_TIM->CNT;
        } while (CNT2 == CNT1);
        diff = CNT2 > CNT1 ? (CNT2 - CNT1) : (SYSTIME_TIM_PERIOD - CNT1 + CNT2);
        if (diff >= us)
            break;
        us -= diff;
        CNT1 = CNT2;
    }
}

extern "C" {

void carbon_hw_us_systime_tim_isr() {
    uint32_t sr = SYSTIME_TIM->SR;
    if (0 != (sr & TIM_SR_UIF)) {
        SYSTIME_TIM->SR = ~TIM_DIER_UIE;
    }
    if (0 != (sr & TIM_SR_CC1IF)) {
        SYSTIME_TIM->SR = ~TIM_DIER_CC1IE;
        uint32_t cnt = SYSTIME_TIM->CNT;
        usCounterAdjust(cnt);
        cnt += SYSTIME_TIM_PERIOD >> 1;
        if (cnt > SYSTIME_TIM_PERIOD)
            cnt -= SYSTIME_TIM_PERIOD;
        SYSTIME_TIM->CCR1 = cnt;
    }
}

/***** HAL Tick withSysTick *****/

HAL_StatusTypeDef HAL_InitTick(uint32_t /*TickPriority*/) {
    if (uwTickFreq != HAL_TICK_FREQ_1KHZ)
        return HAL_ERROR;
    return HAL_OK;
}

void HAL_IncTick() { Error_Handler(); }

uint32_t HAL_GetTick() {
    if (timRunning) {
        return static_cast<uint32_t>(systimeUs() / 1000);
    } else {
        return 0;
    }
}

void HAL_Delay(uint32_t delay) {
    if (timRunning) {
        delayUs(delay * 1000);
    }
}

void HAL_SuspendTick() {}

void HAL_ResumeTick() {}

/***** RTOS Runtime Stats *****/

void carbon_conf_timer_runtime_stats() {}

uint32_t carbon_time_counter_value() {
    uint64_t us = systimeUs();
    // Freertos run time stats counter must not wrap - good for first ~71
    // minutes
    if (us > 0xFFFFFFFFUL)
        return 0xFFFFFFFFUL;
    return static_cast<uint32_t>(us);
}

/**********/

} // extern "C"
