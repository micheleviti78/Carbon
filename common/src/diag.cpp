/**
  ******************************************************************************
  * @file           diag.cpp
  * @author         Michele Viti <micheleviti78@gmail.com>
  * @date           Feb. 2022
  * @brief          DISCO-STM32H747 Diagnostic Console, source file
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

#include <printf.h>
#include <stm32h7xx_hal.h>

#ifdef __cplusplus
 extern "C" {
#endif

void Error_Handler(void);

UART_HandleTypeDef huart1;

void init_diag(){
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
}

void _putchar(char ch){
	 HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
}

void carbon_raw_diag_print(const char *format, ...){
	va_list vl;
	va_start(vl,format);
	vprintf(format, vl);
	va_end(vl);
	printf("\n\r");
}

#ifdef __cplusplus
 }
#endif  