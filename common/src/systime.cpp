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

// #include <diag.hpp>
// #include <error.hpp>
// #include <systime.hpp>

// #include <stm32h7xx_hal.h>
// #include <stm32h7xx_hal_tim.h>

// #ifdef __cplusplus
// extern "C" {
// #endif

// static TIM_HandleTypeDef hal_tick;

// #ifdef CORE_CM7
// #define TIMx TIM2
// #define TIMx_IRQn TIM2_IRQn
// #else
// #define TIMx TIM5
// #define TIMx_IRQn TIM5_IRQn
// #endif

// void low_level_system_time(void) {
// #ifdef CORE_CM7
//     /*
//      *Setting DWT counter
//      */
//     DWT_Type *dwt = DWT;
//     SET_BIT(dwt->CTRL, DWT_CTRL_CYCCNTENA_Msk);
// #endif
//     /*
//      *Setting HAL tick
//      */
//     RCC_ClkInitTypeDef clkConfig{};

//     uint32_t latency;
//     HAL_RCC_GetClockConfig(&clkConfig, &latency);

//     auto apb1Prescaler = clkConfig.APB1CLKDivider;

//     auto pclk1Freq = HAL_RCC_GetPCLK1Freq();

//     uint32_t timClk;
//     switch (apb1Prescaler) {
//     case RCC_APB1_DIV1:
//         timClk = pclk1Freq;
//         break;
//     case RCC_APB1_DIV2:
//         timClk = 2 * pclk1Freq;
//         break;
//     default:
//         Error_Handler();
//     }

//     if (timClk < 1000000ul) {
//         Error_Handler();
//     }

//     auto timPrescaler =
//         static_cast<uint32_t>((timClk / 1000000) - 1); // 1MHz clock

//     /* Enable the TIMx global Interrupt
//      *  NO priority is set at this point.
//      */

//     HAL_NVIC_EnableIRQ(TIMx_IRQn);

// #ifdef CORE_CM7
//     __HAL_RCC_TIM2_CLK_ENABLE();
// #else
//     __HAL_RCC_TIM5_CLK_ENABLE();
// #endif

//     hal_tick.Instance = TIMx;
//     hal_tick.Init.Prescaler = timPrescaler;
//     hal_tick.Init.CounterMode = TIM_COUNTERMODE_UP;
//     hal_tick.Init.Period = 999; // 1 ms period, 1 KHz
//     hal_tick.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

//     if (HAL_TIM_Base_Init(&hal_tick) != HAL_OK) {
//         Error_Handler();
//     } else {
//         /* Start the TIM time Base generation in interrupt mode */
//         if (HAL_TIM_Base_Start_IT(&hal_tick) != HAL_OK) {
//             Error_Handler();
//         } else {
//             return;
//         }
//     }

//     return;
// }

// HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority) {
//     /*
//      *Setting HAL tick prescaler
//      */
//     RCC_ClkInitTypeDef clkConfig{};

//     uint32_t latency;

//     HAL_RCC_GetClockConfig(&clkConfig, &latency);

//     auto apb1Prescaler = clkConfig.APB1CLKDivider;

//     auto pclk1Freq = HAL_RCC_GetPCLK1Freq();

//     uint32_t timClk;
//     switch (apb1Prescaler) {
//     case RCC_APB1_DIV1:
//         timClk = pclk1Freq;
//         break;
//     case RCC_APB1_DIV2:
//         timClk = 2 * pclk1Freq;
//         break;
//     default:
//         return HAL_ERROR;
//     }

//     if (timClk < 1000000ul) {
//         Error_Handler();
//     }

//     auto timPrescaler =
//         static_cast<uint32_t>((timClk / 1000000) - 1); // 1MHz clock

//     __HAL_TIM_SET_PRESCALER(&hal_tick, timPrescaler);

//     /*Configure the TIMx IRQ priority */
//     HAL_NVIC_SetPriority(TIMx_IRQn, TickPriority, 0);

//     return HAL_OK;
// }

// /**
//  * @brief  Period elapsed callback in non blocking mode
//  * @note   This function is called  when TIMx interrupt took place, inside
//  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
//  * a global variable "uwTick" used as application time base.
//  * @param  htim : TIM handle
//  * @retval None
//  */
// void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
//     if (htim->Instance == TIMx) {
//         HAL_IncTick();
//     }
// }

// /**
//  * @brief  Suspend Tick increment.
//  * @note   Disable the tick increment by disabling TIMx update interrupt.
//  * @param  None
//  * @retval None
//  */
// void HAL_SuspendTick(void) {
//     /* Disable TIM6 update Interrupt */
//     __HAL_TIM_DISABLE_IT(&hal_tick, TIM_IT_UPDATE);
// }

// /**
//  * @brief  Resume Tick increment.
//  * @note   Enable the tick increment by Enabling TIMx update interrupt.
//  * @param  None
//  * @retval None
//  */
// void HAL_ResumeTick(void) {
//     /* Enable TIMx Update interrupt */
//     __HAL_TIM_ENABLE_IT(&hal_tick, TIM_IT_UPDATE);
// }

// void hal_tick_isr(void) { HAL_TIM_IRQHandler(&hal_tick); }

// #ifdef __cplusplus
// }
// #endif
#include <diag.hpp>
#include <error.hpp>
#include <systime.hpp>

// #include <mwi/assert.h>
// #include <mwi/diag.h>
// #include <mwi/util/irq_lock.hpp>

#include <stm32h7xx_hal.h>
#include <stm32h7xx_ll_tim.h>

// NOTE: These functions are called during early init before static C++
// constructors. Do not access any static global classes from these
// functions!

#define CM7_SYSTIME_TIM TIM2
#define CM7_SYSTIME_TIM_PERIOD 0xFFFF // NOTE: always set to full span
#define CM7_SYSTIME_TIM_IRQ TIM2_IRQn
#define CM7_SYSTIME_TIM_CLK_EN() __HAL_RCC_TIM2_CLK_ENABLE();

#define CM4_SYSTIME_TIM TIM3
#define CM4_SYSTIME_TIM_PERIOD 0xFFFF // NOTE: always set to full span
#define CM4_SYSTIME_TIM_IRQ TIM3_IRQn
#define CM4_SYSTIME_TIM_CLK_EN() __HAL_RCC_TIM3_CLK_ENABLE()

static bool timRunning;

void low_level_system_time() {
#ifdef CORE_CM7
    RCC_ClkInitTypeDef clkConfig{};

    uint32_t latency;
    HAL_RCC_GetClockConfig(&clkConfig, &latency);

    auto apb1Prescaler = clkConfig.APB1CLKDivider;

    auto pclk1Freq = HAL_RCC_GetPCLK1Freq();

    uint32_t timClk;
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

    // auto apb2Prescaler = clkConfig.APB2CLKDivider;

    // auto pclk2Freq = HAL_RCC_GetPCLK2Freq();

    // uint32_t timClk;
    // switch (apb2Prescaler) {
    // case RCC_APB2_DIV1:
    //     timClk = pclk2Freq;
    //     break;
    // case RCC_APB2_DIV2:
    //     timClk = 2 * pclk2Freq;
    //     break;
    // default:
    //     Error_Handler();
    //     return;
    // }

    // auto timPrescaler =
    //     static_cast<uint32_t>((timClk / 1000000) - 1); // 1MHz clock

    LL_TIM_InitTypeDef timInit{};
    LL_TIM_OC_InitTypeDef ocInit{};

    CM7_SYSTIME_TIM_CLK_EN();

    LL_TIM_StructInit(&timInit);
    timInit.Autoreload = CM7_SYSTIME_TIM_PERIOD;
    timInit.Prescaler = timPrescaler;
    timInit.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    timInit.CounterMode = TIM_COUNTERMODE_UP;
    LL_TIM_Init(CM7_SYSTIME_TIM, &timInit);

    LL_TIM_OC_StructInit(&ocInit);
    ocInit.CompareValue = CM7_SYSTIME_TIM_PERIOD >> 1;
    LL_TIM_OC_Init(CM7_SYSTIME_TIM, TIM_CHANNEL_1, &ocInit);

    // CM4_SYSTIME_TIM_CLK_EN();

    // LL_TIM_StructInit(&timInit);
    // timInit.Autoreload = CM4_SYSTIME_TIM_PERIOD;
    // timInit.Prescaler = timPrescaler;
    // timInit.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    // timInit.CounterMode = TIM_COUNTERMODE_UP;
    // LL_TIM_Init(CM4_SYSTIME_TIM, &timInit);

    // LL_TIM_OC_StructInit(&ocInit);
    // ocInit.CompareValue = CM4_SYSTIME_TIM_PERIOD >> 1;
    // LL_TIM_OC_Init(CM4_SYSTIME_TIM, TIM_CHANNEL_1, &ocInit);

    timRunning = true;

    CM7_SYSTIME_TIM->DIER |= TIM_DIER_CC1IE;
    CM7_SYSTIME_TIM->CR1 |= TIM_CR1_CEN;

    HAL_NVIC_SetPriority(CM7_SYSTIME_TIM_IRQ, TICK_INT_PRIORITY, 0);

    HAL_NVIC_EnableIRQ(CM7_SYSTIME_TIM_IRQ);

    // #else // CORE_CM7

    //     timRunning = true;

    //     CM4_SYSTIME_TIM->DIER |= TIM_DIER_CC1IE;
    //     CM4_SYSTIME_TIM->CR1 |= TIM_CR1_CEN;

    //     HAL_NVIC_SetPriority(CM4_SYSTIME_TIM_IRQ, TICK_INT_PRIORITY, 0);

    //     HAL_NVIC_EnableIRQ(CM4_SYSTIME_TIM_IRQ);

#endif // CORE_CM4
}

#ifdef CORE_CM7
#define SYSTIME_TIM CM7_SYSTIME_TIM
#define SYSTIME_TIM_PERIOD CM7_SYSTIME_TIM_PERIOD
#else
#define SYSTIME_TIM CM4_SYSTIME_TIM
#define SYSTIME_TIM_PERIOD CM4_SYSTIME_TIM_PERIOD
#endif

static volatile uint32_t isrCount = 0;
static volatile uint32_t adjCount = 0;

static uint64_t usCounterAdjust(uint32_t usTimCnt) {
    static uint64_t usCounterBase = 0;
    static uint32_t usTimLastCount = 0;
    if (usTimCnt < usTimLastCount) {
        usCounterBase += SYSTIME_TIM_PERIOD;
        adjCount++;
    }
    usTimLastCount = usTimCnt;
    return usCounterBase + usTimCnt;
}

uint64_t systimeUs() {
    __disable_irq();
    return usCounterAdjust(SYSTIME_TIM->CNT);
    __enable_irq();
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

#ifdef CORE_CM7
void mwi_cm7_hw_us_systime_tim_isr() {
#else
void mwi_cm4_hw_us_systime_tim_isr() {
#endif
    RAW_DIAG("mismatch");
    uint32_t sr = SYSTIME_TIM->SR;
    if (0 != (sr & TIM_SR_UIF)) {
        SYSTIME_TIM->SR = ~TIM_DIER_UIE;
    }
    if (0 != (sr & TIM_SR_CC1IF)) {
        isrCount++;
        SYSTIME_TIM->SR = ~TIM_DIER_CC1IE;
        uint32_t cnt = SYSTIME_TIM->CNT;
        usCounterAdjust(cnt);
        // if (isrCount == adjCount){
        RAW_DIAG("mismatch");
        //}
        cnt += SYSTIME_TIM_PERIOD >> 1;
        if (cnt > SYSTIME_TIM_PERIOD)
            cnt -= SYSTIME_TIM_PERIOD;
        SYSTIME_TIM->CCR1 = cnt;
    }
}

// void mwi_freertos_run_time_stats_configure() {}

// uint32_t mwi_freertos_run_time_stats_counter() {
//     uint64_t us = systimeUs();
//     // Freertos run time stats counter must not wrap - good for first ~71
//     // minutes
//     if (us > 0xFFFFFFFFUL)
//         return 0xFFFFFFFFUL;
//     else
//         return static_cast<uint32_t>(us);
// }

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

} // extern "C"
