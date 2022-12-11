/**
 ******************************************************************************
 * @file           netif_conf.c
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Nov. 2022
 * @brief          init and configuration Ethernet interface
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
#include <carbon/app_ethernet.h>
#include <carbon/diag.hpp>
#include <carbon/ethernetif.h>

#include <lwip/init.h>
#include <lwip/netif.h>
#include <lwip/tcpip.h>

struct netif gnetif; /* network interface structure */

void netif_config(void) {
    DIAG(SYSTEM_DIAG "LwIP version %d.%d.%d", LWIP_VERSION_MAJOR,
         LWIP_VERSION_MINOR, LWIP_VERSION_REVISION);

    tcpip_init(NULL, NULL);

    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;

#if LWIP_DHCP
    ip_addr_set_zero_ip4(&ipaddr);
    ip_addr_set_zero_ip4(&netmask);
    ip_addr_set_zero_ip4(&gw);
#else
    IP_ADDR4(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
    IP_ADDR4(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2,
             NETMASK_ADDR3);
    IP_ADDR4(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
#endif /* LWIP_DHCP */

    /* add the network interface */
    netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init,
              &tcpip_input);

    /*  Registers the default network interface. */
    netif_set_default(&gnetif);

    ethernet_link_status_updated(&gnetif);

#if LWIP_NETIF_LINK_CALLBACK
    netif_set_link_callback(&gnetif, ethernet_link_status_updated);

    osThreadDef(EthLink, ethernet_link_thread, osPriorityNormal, 0,
                configMINIMAL_STACK_SIZE * 6);
    osThreadCreate(osThread(EthLink), &gnetif);
#endif

#if LWIP_DHCP
    /* Start DHCPClient */
    osThreadDef(DHCP, DHCP_Thread, osPriorityHigh, 0,
                configMINIMAL_STACK_SIZE * 6);
    osThreadCreate(osThread(DHCP), &gnetif);
#endif
}
