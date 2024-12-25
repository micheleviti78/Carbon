/**
 ******************************************************************************
 * @file           ethernetif.h
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Nov. 2022
 * @brief          Ethernet driver
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

#pragma once

#include <lwip/err.h>
#include <lwip/netif.h>

#include <stm32h7xx_hal.h>

err_t carbon_lwip_init(struct netif *netif);
void carbon_lwip_input(const void *argument);
void carbon_lwip_link_thread(void const *argument);

typedef struct {
    uint32_t txDesc[ETH_TX_DESC_CNT];
} Eth_Tx_Desc_List_TypeDef;
typedef struct {
    uint32_t rxDesc[ETH_RX_DESC_CNT];
    __IO uint32_t toProcessPointer;
} Eth_Rx_Desc_List_TypeDef;

typedef struct {
    ETH_TypeDef *Instance;
    ETH_InitTypeDef Init;
    Eth_Tx_Desc_List_TypeDef txDescList;
    Eth_Rx_Desc_List_TypeDef rxDescList;
    __IO HAL_ETH_StateTypeDef gState;
    __IO HAL_ETH_StateTypeDef rxState;
    __IO uint32_t lostReceivedPackets;
    __IO uint32_t errorCode;
    __IO uint32_t dmaErrorCode;
    __IO uint32_t macErrorCode;
} Eth_Handle;
