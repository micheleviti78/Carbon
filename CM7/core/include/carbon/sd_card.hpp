/**
 ******************************************************************************
 * @file           sd_card.hpp
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Feb. 2024
 * @brief          SD Card Interface
 ******************************************************************************
 * @attention
 * Copyright (c) 2024 Michele Viti.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/**
 ******************************************************************************
 * @file    stm32h747i_discovery_sd.h
 * @author  MCD Application Team
 * @brief   This file contains the common defines and functions prototypes for
 *          the stm32h747i_discovery_sd.c driver.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2018 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#pragma once

#include <carbon/diag.hpp>
#include <carbon/sd_errno.h>

#include <stm32h7xx_hal_conf.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief SD Card information structure
 */
#define BSP_SD_CardInfo HAL_SD_CardInfoTypeDef
#define BSP_SD_CardCID HAL_SD_CardCIDTypeDef

#if (USE_HAL_SD_REGISTER_CALLBACKS == 1)
typedef struct {
    void (*pMspInitCb)(SD_HandleTypeDef *);
    void (*pMspDeInitCb)(SD_HandleTypeDef *);
} BSP_SD_Cb_t;
#endif /* (USE_HAL_SD_REGISTER_CALLBACKS == 1) */

#define SD_INSTANCES_NBR 1UL

#ifndef SD_WRITE_TIMEOUT
#define SD_WRITE_TIMEOUT 100U
#endif

#ifndef SD_READ_TIMEOUT
#define SD_READ_TIMEOUT 100U
#endif

/**
 * @brief SD interface bus width selection
 *        4-bit wide data bus can be disabled to avoid conflict with
 *        camera pins (PC9 and PC11) on the Disco board.
 *        1-bit (SDMMC_D0) databus width will be used instead.
 */
#ifndef USE_SD_BUS_WIDE_4B
#define USE_SD_BUS_WIDE_4B 1U
#endif

/**
 * @brief  SD transfer state definition
 */
#define SD_TRANSFER_OK 0U
#define SD_TRANSFER_BUSY 1U

/**
 * @brief SD-detect signal
 */
#define SD_PRESENT 1UL
#define SD_NOT_PRESENT 0UL

#define SD_DETECT_PIN GPIO_PIN_8
#define SD_DETECT_GPIO_PORT GPIOI
#define SD_DETECT_GPIO_CLK_ENABLE() __HAL_RCC_GPIOI_CLK_ENABLE()
#define SD_DETECT_GPIO_CLK_DISABLE() __HAL_RCC_GPIOI_CLK_DISABLE()
#define SD_DETECT_EXTI_IRQn EXTI9_5_IRQn

#define SD_DETECT_EXTI_LINE EXTI_LINE_8
#define SD_DetectIRQHandler() HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8)

extern SD_HandleTypeDef hsd_sdmmc[];
extern EXTI_HandleTypeDef hsd_exti[];

int32_t BSP_SD_Init(uint32_t Instance);
int32_t BSP_SD_DeInit(uint32_t Instance);
#if (USE_HAL_SD_REGISTER_CALLBACKS == 1)
int32_t BSP_SD_RegisterMspCallbacks(uint32_t Instance, BSP_SD_Cb_t *CallBacks);
int32_t BSP_SD_RegisterDefaultMspCallbacks(uint32_t Instance);
#endif /* (USE_HAL_SD_REGISTER_CALLBACKS == 1) */
int32_t BSP_SD_DetectITConfig(uint32_t Instance);
int32_t BSP_SD_ReadBlocks(uint32_t Instance, uint32_t *pData, uint32_t BlockIdx,
                          uint32_t BlocksNbr);
int32_t BSP_SD_WriteBlocks(uint32_t Instance, uint32_t *pData,
                           uint32_t BlockIdx, uint32_t NbrOfBlocks);
int32_t BSP_SD_ReadBlocks_DMA(uint32_t Instance, uint32_t *pData,
                              uint32_t BlockIdx, uint32_t NbrOfBlocks);
int32_t BSP_SD_WriteBlocks_DMA(uint32_t Instance, uint32_t *pData,
                               uint32_t BlockIdx, uint32_t NbrOfBlocks);
int32_t BSP_SD_ReadBlocks_IT(uint32_t Instance, uint32_t *pData,
                             uint32_t BlockIdx, uint32_t NbrOfBlocks);
int32_t BSP_SD_WriteBlocks_IT(uint32_t Instance, uint32_t *pData,
                              uint32_t BlockIdx, uint32_t NbrOfBlocks);
int32_t BSP_SD_Erase(uint32_t Instance, uint32_t BlockIdx, uint32_t BlocksNbr);
int32_t BSP_SD_GetCardState(uint32_t Instance);
int32_t BSP_SD_GetCardInfo(uint32_t Instance, BSP_SD_CardInfo *CardInfo);
int32_t BSP_SD_GetCardCID(uint32_t Instance, BSP_SD_CardCID *CardCID);
int32_t BSP_SD_IsDetected(uint32_t Instance);
int32_t BSP_SD_Init_Detect_Notify(uint32_t Instance);

void BSP_SD_DETECT_IRQHandler(uint32_t Instance);
void BSP_SD_IRQHandler(uint32_t Instance);

/* These functions can be modified in case the current settings (e.g. DMA stream
   or IT) need to be changed for specific application needs */
void BSP_SD_AbortCallback(uint32_t Instance);
void BSP_SD_WriteCpltCallback(uint32_t Instance);
void BSP_SD_ReadCpltCallback(uint32_t Instance);
void BSP_SD_DetectCallback(uint32_t Instance);
void HAL_SD_DriveTransciver_1_8V_Callback(FlagStatus status);
HAL_StatusTypeDef MX_SDMMC1_SD_Init(SD_HandleTypeDef *hsd);

#ifdef __cplusplus
}
#endif
