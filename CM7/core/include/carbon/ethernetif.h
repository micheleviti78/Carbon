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

/* Exported types ------------------------------------------------------------*/
/* Structure that include link thread parameters */
/* Exported functions ------------------------------------------------------- */
err_t ethernetif_init(struct netif *netif);
void ethernetif_input(const void *argument);
void ethernet_link_thread(void const *argument);
