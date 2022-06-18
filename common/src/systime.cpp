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

#include <diag.hpp>
#include <error.hpp>
#include <systime.hpp>

#include <stm32h7xx_hal.h>
#include <stm32h7xx_hal_tim.h>

#ifdef __cplusplus
extern "C" {
#endif

static TIM_HandleTypeDef hal_tick;

#ifdef CORE_CM7
#define TIMx TIM2
#define TIMx_IRQn TIM2_IRQn
#else
#define TIMx TIM5
#define TIMx_IRQn TIM5_IRQn
#endif

void low_level_system_time(void) {
#ifdef CORE_CM7
    /*
     *Setting DWT counter
     */
    DWT_Type *dwt = DWT;
    SET_BIT(dwt->CTRL, DWT_CTRL_CYCCNTENA_Msk);
#endif
    /*
     *Setting HAL tick
     */
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

    /* Enable the TIMx global Interrupt
     *  NO priority is set at this point.
     */

    HAL_NVIC_EnableIRQ(TIMx_IRQn);

#ifdef CORE_CM7
    __HAL_RCC_TIM2_CLK_ENABLE();
#else
    __HAL_RCC_TIM5_CLK_ENABLE();
#endif

    hal_tick.Instance = TIMx;
    hal_tick.Init.Prescaler = timPrescaler;
    hal_tick.Init.CounterMode = TIM_COUNTERMODE_UP;
    hal_tick.Init.Period = 999; // 1 ms period, 1 KHz
    hal_tick.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

    if (HAL_TIM_Base_Init(&hal_tick) != HAL_OK) {
        Error_Handler();
    } else {
        /* Start the TIM time Base generation in interrupt mode */
        if (HAL_TIM_Base_Start_IT(&hal_tick) != HAL_OK) {
            Error_Handler();
        } else {
            return;
        }
    }

    return;
}

HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority) {
    /*
     *Setting HAL tick prescaler
     */
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
        return HAL_ERROR;
    }

    if (timClk < 1000000ul) {
        Error_Handler();
    }

    auto timPrescaler =
        static_cast<uint32_t>((timClk / 1000000) - 1); // 1MHz clock

    __HAL_TIM_SET_PRESCALER(&hal_tick, timPrescaler);

    /*Configure the TIMx IRQ priority */
    HAL_NVIC_SetPriority(TIMx_IRQn, TickPriority, 0);

    return HAL_OK;
}

/**
 * @brief  Period elapsed callback in non blocking mode
 * @note   This function is called  when TIMx interrupt took place, inside
 * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
 * a global variable "uwTick" used as application time base.
 * @param  htim : TIM handle
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIMx) {
        HAL_IncTick();
    }
}

/**
 * @brief  Suspend Tick increment.
 * @note   Disable the tick increment by disabling TIMx update interrupt.
 * @param  None
 * @retval None
 */
void HAL_SuspendTick(void) {
    /* Disable TIM6 update Interrupt */
    __HAL_TIM_DISABLE_IT(&hal_tick, TIM_IT_UPDATE);
}

/**
 * @brief  Resume Tick increment.
 * @note   Enable the tick increment by Enabling TIMx update interrupt.
 * @param  None
 * @retval None
 */
void HAL_ResumeTick(void) {
    /* Enable TIMx Update interrupt */
    __HAL_TIM_ENABLE_IT(&hal_tick, TIM_IT_UPDATE);
}

void hal_tick_isr(void) { HAL_TIM_IRQHandler(&hal_tick); }

#ifdef __cplusplus
}
#endif
