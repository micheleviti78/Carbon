/**
 ******************************************************************************
 * @file           ethernetif.c
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Sept. 2024
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

#include <carbon/common.hpp>
#include <carbon/diag.hpp>
#include <carbon/ethernetif.h>

#include <lan8742.h>

#include <stm32h7xx_hal.h>

#include <cmsis_os.h>

#include <lwip/ethip6.h>
#include <lwip/opt.h>
#include <lwip/tcpip.h>
#include <lwip/timeouts.h>
#include <netif/etharp.h>
#include <netif/ethernet.h>

#include <string.h>

/* Private define ------------------------------------------------------------*/
#define ETH_TX_EN_Pin GPIO_PIN_11
#define ETH_TX_EN_GPIO_Port GPIOG
#define OSC32_OUT_Pin GPIO_PIN_15
#define OSC32_OUT_GPIO_Port GPIOC
#define OSC32_IN_Pin GPIO_PIN_14
#define OSC32_IN_GPIO_Port GPIOC
#define ETH_TXD1_Pin GPIO_PIN_12
#define ETH_TXD1_GPIO_Port GPIOG
#define ETH_TXD0_Pin GPIO_PIN_13
#define ETH_TXD0_GPIO_Port GPIOG
#define CEC_CK_MCO1_Pin GPIO_PIN_8
#define CEC_CK_MCO1_GPIO_Port GPIOA
#define OSC_OUT_Pin GPIO_PIN_1
#define OSC_OUT_GPIO_Port GPIOH
#define OSC_IN_Pin GPIO_PIN_0
#define OSC_IN_GPIO_Port GPIOH
#define ETH_MDC_SAI4_D1_Pin GPIO_PIN_1
#define ETH_MDC_SAI4_D1_GPIO_Port GPIOC
#define ETH_MDIO_Pin GPIO_PIN_2
#define ETH_MDIO_GPIO_Port GPIOA
#define ETH_REF_CLK_Pin GPIO_PIN_1
#define ETH_REF_CLK_GPIO_Port GPIOA
#define ETH_CRS_DV_Pin GPIO_PIN_7
#define ETH_CRS_DV_GPIO_Port GPIOA
#define ETH_RXD0_Pin GPIO_PIN_4
#define ETH_RXD0_GPIO_Port GPIOC
#define ETH_RXD1_Pin GPIO_PIN_5
#define ETH_RXD1_GPIO_Port GPIOC
/* The time to block waiting for input. */
#define TIME_WAITING_FOR_INPUT (portMAX_DELAY)
/* USER CODE BEGIN OS_THREAD_STACK_SIZE_WITH_RTOS */
/* Stack size of the interface thread */
#define INTERFACE_THREAD_STACK_SIZE (350)
/* USER CODE END OS_THREAD_STACK_SIZE_WITH_RTOS */
/* Network interface name */
#define IFNAME0 's'
#define IFNAME1 't'

/* ETH Setting  */
#define ETH_DMA_TRANSMIT_TIMEOUT (20U)

/*
@Note: This interface is implemented to operate in zero-copy mode only:
        - Rx buffers are allocated statically and passed directly to the LwIP
stack they will return back to ETH DMA after been processed by the stack.
        - Tx Buffers will be allocated from LwIP stack memory heap,
          then passed to ETH HAL driver.

@Notes:
  1.a. ETH DMA Rx descriptors must be contiguous, the default count is 4,
       to customize it please redefine ETH_RX_DESC_CNT in ETH GUI (Rx Descriptor
Length) so that updated value will be generated in stm32xxxx_hal_conf.h 1.b. ETH
DMA Tx descriptors must be contiguous, the default count is 4, to customize it
please redefine ETH_TX_DESC_CNT in ETH GUI (Tx Descriptor Length) so that
updated value will be generated in stm32xxxx_hal_conf.h

  2.a. Rx Buffers number must be between ETH_RX_DESC_CNT and 2*ETH_RX_DESC_CNT
  2.b. Rx Buffers must have the same size: ETH_RX_BUFFER_SIZE, this value must
       passed to ETH DMA in the init field (heth.Init.RxBuffLen)
  2.c  The RX Buffers addresses and sizes must be properly defined to be aligned
       to L1-CACHE line size (32 bytes).
 */

#define ETH_RX_BUFFER_SIZE_ALIGNED ALIGN(ETH_RX_BUFFER_SIZE, CACHE_ALIGNMENT)

/* Ethernet Rx DMA Descriptors */
ETH_DMADescTypeDef DMARxDscrTab[ETH_RX_DESC_CNT]
    __attribute__((aligned(32), section(".RxDecripSection")));
/* Ethernet Tx DMA Descriptors */
ETH_DMADescTypeDef DMATxDscrTab[ETH_TX_DESC_CNT]
    __attribute__((aligned(32), section(".TxDecripSection")));
/* Ethernet Receive Buffers */
static uint8_t Rx_Buff[ETH_RX_DESC_CNT][ETH_RX_BUFFER_SIZE_ALIGNED]
    __attribute__((aligned(32)));
/* Ethernet Transmit Buffers */
static uint8_t Tx_Buff[ETH_RX_BUFFER_SIZE_ALIGNED] __attribute__((aligned(32)));

osSemaphoreId RxPktSemaphore = NULL; /* Semaphore to signal incoming packets */

/* Global Ethernet handle */
ETH_HandleTypeDef heth;
ETH_TxPacketConfig TxConfig;

/* Memory Pool Declaration */
LWIP_MEMPOOL_DECLARE(RX_POOL, 10, sizeof(struct pbuf_custom),
                     "Zero-copy RX PBUF pool");

/* Private function prototypes -----------------------------------------------*/
int32_t ETH_PHY_IO_Init(void);
int32_t ETH_PHY_IO_DeInit(void);
int32_t ETH_PHY_IO_ReadReg(uint32_t DevAddr, uint32_t RegAddr,
                           uint32_t *pRegVal);
int32_t ETH_PHY_IO_WriteReg(uint32_t DevAddr, uint32_t RegAddr,
                            uint32_t RegVal);
int32_t ETH_PHY_IO_GetTick(void);

lan8742_Object_t LAN8742;
lan8742_IOCtx_t LAN8742_IOCtx = {ETH_PHY_IO_Init, ETH_PHY_IO_DeInit,
                                 ETH_PHY_IO_WriteReg, ETH_PHY_IO_ReadReg,
                                 ETH_PHY_IO_GetTick};

/* Private functions ---------------------------------------------------------*/
void pbuf_free_custom(struct pbuf *p);
void Error_Handler(void);

void HAL_ETH_MspInit(ETH_HandleTypeDef *ethHandle) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (ethHandle->Instance == ETH) {
        /* Enable Peripheral clock */
        __HAL_RCC_ETH1MAC_CLK_ENABLE();
        __HAL_RCC_ETH1TX_CLK_ENABLE();
        __HAL_RCC_ETH1RX_CLK_ENABLE();

        __HAL_RCC_GPIOG_CLK_ENABLE();
        __HAL_RCC_GPIOC_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        /**ETH GPIO Configuration
PG11     ------> ETH_TX_EN
PG12     ------> ETH_TXD1
PG13     ------> ETH_TXD0
PC1     ------> ETH_MDC
PA2     ------> ETH_MDIO
PA1     ------> ETH_REF_CLK
PA7     ------> ETH_CRS_DV
PC4     ------> ETH_RXD0
PC5     ------> ETH_RXD1
         */
        GPIO_InitStruct.Pin = ETH_TX_EN_Pin | ETH_TXD1_Pin | ETH_TXD0_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
        HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = ETH_MDC_SAI4_D1_Pin | ETH_RXD0_Pin | ETH_RXD1_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = ETH_MDIO_Pin | ETH_REF_CLK_Pin | ETH_CRS_DV_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* Peripheral interrupt init */
        HAL_NVIC_SetPriority(ETH_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(ETH_IRQn);
    }
}

void HAL_ETH_MspDeInit(ETH_HandleTypeDef *ethHandle) {
    if (ethHandle->Instance == ETH) {
        /* Disable Peripheral clock */
        __HAL_RCC_ETH1MAC_CLK_DISABLE();
        __HAL_RCC_ETH1TX_CLK_DISABLE();
        __HAL_RCC_ETH1RX_CLK_DISABLE();

        /**ETH GPIO Configuration
PG11     ------> ETH_TX_EN
PG12     ------> ETH_TXD1
PG13     ------> ETH_TXD0
PC1     ------> ETH_MDC
PA2     ------> ETH_MDIO
PA1     ------> ETH_REF_CLK
PA7     ------> ETH_CRS_DV
PC4     ------> ETH_RXD0
PC5     ------> ETH_RXD1
         */
        HAL_GPIO_DeInit(GPIOG, ETH_TX_EN_Pin | ETH_TXD1_Pin | ETH_TXD0_Pin);

        HAL_GPIO_DeInit(GPIOC,
                        ETH_MDC_SAI4_D1_Pin | ETH_RXD0_Pin | ETH_RXD1_Pin);

        HAL_GPIO_DeInit(GPIOA, ETH_MDIO_Pin | ETH_REF_CLK_Pin | ETH_CRS_DV_Pin);

        /* Peripheral interrupt Deinit*/
        HAL_NVIC_DisableIRQ(ETH_IRQn);
    }
}

/**
 * @brief  Ethernet Rx Transfer completed callback
 * @param  heth: ETH handle
 * @retval None
 */
void HAL_ETH_RxCpltCallback(ETH_HandleTypeDef *heth) {
    osSemaphoreRelease(RxPktSemaphore);
}

/*******************************************************************************
                       LL Driver Interface ( LwIP stack --> ETH)
 *******************************************************************************/
/**
 * @brief In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
static void low_level_init(struct netif *netif) {
    HAL_StatusTypeDef hal_eth_init_status = HAL_OK;
    uint32_t idx = 0;
    ETH_MACConfigTypeDef MACConf;
    int32_t PHYLinkState;
    uint32_t duplex, speed = 0;
    /* Start ETH HAL Init */

    uint8_t MACAddr[6];
    heth.Instance = ETH;
    MACAddr[0] = 0x00;
    MACAddr[1] = 0x80;
    MACAddr[2] = 0xE1;
    MACAddr[3] = 0x00;
    MACAddr[4] = 0x00;
    MACAddr[5] = 0x00;
    heth.Init.MACAddr = &MACAddr[0];
    heth.Init.MediaInterface = HAL_ETH_RMII_MODE;
    heth.Init.TxDesc = DMATxDscrTab;
    heth.Init.RxDesc = DMARxDscrTab;
    heth.Init.RxBuffLen = ETH_RX_BUFFER_SIZE_ALIGNED;

    hal_eth_init_status = HAL_ETH_Init(&heth);

    memset(&TxConfig, 0, sizeof(ETH_TxPacketConfig));
    // TxConfig.Attributes =
    //     ETH_TX_PACKETS_FEATURES_CSUM | ETH_TX_PACKETS_FEATURES_CRCPAD;
    // TxConfig.ChecksumCtrl = ETH_CHECKSUM_IPHDR_PAYLOAD_INSERT_PHDR_CALC;
    // TxConfig.CRCPadCtrl = ETH_CRC_PAD_INSERT;

    /* End ETH HAL Init */

    /* Initialize the RX POOL */
    LWIP_MEMPOOL_INIT(RX_POOL);

#if LWIP_ARP || LWIP_ETHERNET

    /* set MAC hardware address length */
    netif->hwaddr_len = ETH_HWADDR_LEN;

    /* set MAC hardware address */
    netif->hwaddr[0] = heth.Init.MACAddr[0];
    netif->hwaddr[1] = heth.Init.MACAddr[1];
    netif->hwaddr[2] = heth.Init.MACAddr[2];
    netif->hwaddr[3] = heth.Init.MACAddr[3];
    netif->hwaddr[4] = heth.Init.MACAddr[4];
    netif->hwaddr[5] = heth.Init.MACAddr[5];

    /* maximum transfer unit */
    netif->mtu = ETH_MAX_PAYLOAD;

    /* Accept broadcast address and ARP traffic */
    /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
#if LWIP_ARP
    netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;
#else
    netif->flags |= NETIF_FLAG_BROADCAST;
#endif /* LWIP_ARP */

    for (idx = 0; idx < ETH_RX_DESC_CNT; idx++) {
        HAL_ETH_DescAssignMemory(&heth, idx, Rx_Buff[idx], NULL);
    }

    /* create a binary semaphore used for informing ethernetif of frame
     * reception */
    RxPktSemaphore = xSemaphoreCreateBinary();
    /*reset the semaphore*/
    osSemaphoreWait(RxPktSemaphore, 0);

    /* create the task that handles the ETH_MAC */
    osThreadDef(EthIf, ethernetif_input, osPriorityAboveNormal, 0,
                INTERFACE_THREAD_STACK_SIZE);
    osThreadCreate(osThread(EthIf), netif);

    /* Set PHY IO functions */
    LAN8742_RegisterBusIO(&LAN8742, &LAN8742_IOCtx);

    /* Initialize the LAN8742 ETH PHY */
    LAN8742_Init(&LAN8742);

    if (hal_eth_init_status == HAL_OK) {
        PHYLinkState = LAN8742_GetLinkState(&LAN8742);

        /* Get link state */
        if (PHYLinkState <= LAN8742_STATUS_LINK_DOWN) {
            netif_set_link_down(netif);
            netif_set_down(netif);
        } else {
            switch (PHYLinkState) {
            case LAN8742_STATUS_100MBITS_FULLDUPLEX:
                duplex = ETH_FULLDUPLEX_MODE;
                speed = ETH_SPEED_100M;
                break;
            case LAN8742_STATUS_100MBITS_HALFDUPLEX:
                duplex = ETH_HALFDUPLEX_MODE;
                speed = ETH_SPEED_100M;
                break;
            case LAN8742_STATUS_10MBITS_FULLDUPLEX:
                duplex = ETH_FULLDUPLEX_MODE;
                speed = ETH_SPEED_10M;
                break;
            case LAN8742_STATUS_10MBITS_HALFDUPLEX:
                duplex = ETH_HALFDUPLEX_MODE;
                speed = ETH_SPEED_10M;
                break;
            default:
                duplex = ETH_FULLDUPLEX_MODE;
                speed = ETH_SPEED_100M;
                break;
            }

            /* Get MAC Config MAC */
            HAL_ETH_GetMACConfig(&heth, &MACConf);
            MACConf.DuplexMode = duplex;
            MACConf.Speed = speed;
            HAL_ETH_SetMACConfig(&heth, &MACConf);

            HAL_ETH_Start_IT(&heth);
            netif_set_up(netif);
            netif_set_link_up(netif);
        }

    } else {
        Error_Handler();
    }
#endif /* LWIP_ARP || LWIP_ETHERNET */
}

/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and
 * type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become available since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */

static err_t low_level_output(struct netif *netif, struct pbuf *p) {
    struct pbuf *q;
    ETH_BufferTypeDef TxETH_Buffer;
    err_t errval = ERR_OK;

    if (p->tot_len >= (ETH_RX_BUFFER_SIZE_ALIGNED)) {
        DIAG(ETH_DIAG "low level output: Tx_Buff too small, data lenght %lu",
             p->tot_len);
        return ERR_IF;
    }

    TxETH_Buffer.buffer = (uint8_t *)&Tx_Buff[0];
    TxETH_Buffer.next = NULL;
    TxETH_Buffer.len = p->tot_len;

    TxConfig.TxBuffer = &TxETH_Buffer;
    TxConfig.Length = p->tot_len;

    uint32_t size = 0;
    uint8_t *current_buf_ptr = (uint8_t *)TxETH_Buffer.buffer;

    for (q = p; q != NULL; q = q->next) {
        if (size >= TxETH_Buffer.len) {
            DIAG(ETH_DIAG "low level output: p->tot_len %lu not equal to "
                          "actual data size %lu",
                 TxETH_Buffer.len, size);
            return ERR_IF;
        }

        memcpy(current_buf_ptr, q->payload, q->len);

        current_buf_ptr += q->len;
        size += q->len;
    }

    if (((uintptr_t)(TxETH_Buffer.buffer) & 0x1F) != 0) {
        DIAG(ETH_DIAG "low level output: tx buffer not 32-byte aligned");
        return ERR_IF;
    }

    SCB_CleanDCache_by_Addr((uint32_t *)TxETH_Buffer.buffer,
                            ETH_RX_BUFFER_SIZE_ALIGNED);

    int result = HAL_ETH_Transmit(&heth, &TxConfig, ETH_DMA_TRANSMIT_TIMEOUT);

    if (result != HAL_OK) {
        DIAG(ETH_DIAG "low level output: error data transmitting: %d", result);
        return ERR_IF;
    }

    return errval;
}

/**
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
 */
static struct pbuf *low_level_input(struct netif *netif) {
    struct pbuf *p = NULL;
    ETH_BufferTypeDef RxBuff[ETH_RX_DESC_CNT];
    uint32_t i = 0;
    struct pbuf_custom *custom_pbuf;
    HAL_StatusTypeDef status;

    memset(RxBuff, 0, ETH_RX_DESC_CNT * sizeof(ETH_BufferTypeDef));

    for (i = 0; i < ETH_RX_DESC_CNT - 1; i++) {
        RxBuff[i].next = &RxBuff[i + 1];
    }

    status = HAL_ETH_GetRxDataBuffer(&heth, RxBuff);

    if (status != HAL_OK || RxBuff[0].buffer == NULL) {
        return p;
    }

    custom_pbuf = (struct pbuf_custom *)LWIP_MEMPOOL_ALLOC(RX_POOL);
    if (custom_pbuf == NULL) {
        DIAG(ETH_DIAG "low level input: cannot allocate pbuf");
        return p;
    }
    custom_pbuf->custom_free_function = pbuf_free_custom;

    p = (struct pbuf *)custom_pbuf;
    ETH_BufferTypeDef *ethBuf = &RxBuff[0];
    i = 0;

    while (i < ETH_RX_DESC_CNT) {
        SCB_InvalidateDCache_by_Addr((uint32_t *)ethBuf->buffer,
                                     ETH_RX_BUFFER_SIZE_ALIGNED);

        if (!pbuf_alloced_custom(PBUF_RAW, ethBuf->len, PBUF_REF, custom_pbuf,
                                 ethBuf->buffer, ETH_RX_BUFFER_SIZE_ALIGNED)) {
            DIAG(ETH_DIAG "low level input: pbuf not initialized");
            if (p) {
                pbuf_free(p);
                p = NULL;
            }
            return p;
        }

        ethBuf = ethBuf->next;
        if (!ethBuf->buffer) {
            break;
        }

        custom_pbuf->pbuf.next = (struct pbuf *)LWIP_MEMPOOL_ALLOC(RX_POOL);
        custom_pbuf = (struct pbuf_custom *)custom_pbuf->pbuf.next;
        if (custom_pbuf == NULL) {
            DIAG(ETH_DIAG "low level input: cannot allocate pbuf");
            if (p) {
                pbuf_free(p);
                p = NULL;
            }
            return p;
        }
        custom_pbuf->custom_free_function = pbuf_free_custom;

        i++;
    }

    return p;
}

/**
 * This function should be called when a packet is ready to be read
 * from the interface. It uses the function low_level_input() that
 * should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */
void ethernetif_input(const void *argument) {
    struct pbuf *p;
    struct netif *netif = (struct netif *)argument;

    for (;;) {
        if (osSemaphoreWait(RxPktSemaphore, TIME_WAITING_FOR_INPUT) == osOK) {
            do {
                p = low_level_input(netif);
                if (p != NULL) {
                    err_t err = netif->input(p, netif);
                    if (err != ERR_OK) {
                        DIAG(
                            ETH_DIAG
                            "error %d pushing pbuf %p, payload %p to the stack",
                            err, p, p->payload);
                        pbuf_free(p);
                    }
                    // Build Rx descriptor to be ready for next data reception
                    HAL_ETH_BuildRxDescriptors(&heth);
                }
            } while (p != NULL);
        }
    }
}

#if !LWIP_ARP
/**
 * This function has to be completed by user in case of ARP OFF.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if ...
 */
static err_t low_level_output_arp_off(struct netif *netif, struct pbuf *q,
                                      const ip4_addr_t *ipaddr) {
    err_t errval;
    errval = ERR_OK;

    return errval;
}
#endif /* LWIP_ARP */

/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t ethernetif_init(struct netif *netif) {
    LWIP_ASSERT("netif != NULL", (netif != NULL));

#if LWIP_NETIF_HOSTNAME
    /* Initialize interface hostname */
    netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

    netif->name[0] = IFNAME0;
    netif->name[1] = IFNAME1;
    /* We directly use etharp_output() here to save a function call.
     * You can instead declare your own function an call etharp_output()
     * from it if you have to do some checks before sending (e.g. if link
     * is available...) */

#if LWIP_IPV4
#if LWIP_ARP || LWIP_ETHERNET
#if LWIP_ARP
    netif->output = etharp_output;
#else
    /* The user should write its own code in low_level_output_arp_off function
     */
    netif->output = low_level_output_arp_off;
#endif /* LWIP_ARP */
#endif /* LWIP_ARP || LWIP_ETHERNET */
#endif /* LWIP_IPV4 */

#if LWIP_IPV6
    netif->output_ip6 = ethip6_output;
#endif /* LWIP_IPV6 */

    netif->linkoutput = low_level_output;

    /* initialize the hardware */
    low_level_init(netif);

    return ERR_OK;
}

/**
 * @brief  Custom Rx pbuf free callback
 * @param  pbuf: pbuf to be freed
 * @retval None
 */
void pbuf_free_custom(struct pbuf *p) {
    struct pbuf_custom *custom_pbuf = (struct pbuf_custom *)p;

    LWIP_MEMPOOL_FREE(RX_POOL, custom_pbuf);
}

/**
 * @brief  Returns the current time in milliseconds
 *         when LWIP_TIMERS == 1 and NO_SYS == 1
 * @param  None
 * @retval Current Time value
 */
u32_t sys_jiffies(void) { return HAL_GetTick(); }

/**
 * @brief  Returns the current time in milliseconds
 *         when LWIP_TIMERS == 1 and NO_SYS == 1
 * @param  None
 * @retval Current Time value
 */
u32_t sys_now(void) { return HAL_GetTick(); }

/*******************************************************************************
                       PHI IO Functions
 *******************************************************************************/
/**
 * @brief  Initializes the MDIO interface GPIO and clocks.
 * @param  None
 * @retval 0 if OK, -1 if ERROR
 */
int32_t ETH_PHY_IO_Init(void) {
    /* We assume that MDIO GPIO configuration is already done
 in the ETH_MspInit() else it should be done here
     */

    /* Configure the MDIO Clock */
    HAL_ETH_SetMDIOClockRange(&heth);

    return 0;
}

/**
 * @brief  De-Initializes the MDIO interface .
 * @param  None
 * @retval 0 if OK, -1 if ERROR
 */
int32_t ETH_PHY_IO_DeInit(void) { return 0; }

/**
 * @brief  Read a PHY register through the MDIO interface.
 * @param  DevAddr: PHY port address
 * @param  RegAddr: PHY register address
 * @param  pRegVal: pointer to hold the register value
 * @retval 0 if OK -1 if Error
 */
int32_t ETH_PHY_IO_ReadReg(uint32_t DevAddr, uint32_t RegAddr,
                           uint32_t *pRegVal) {
    if (HAL_ETH_ReadPHYRegister(&heth, DevAddr, RegAddr, pRegVal) != HAL_OK) {
        return -1;
    }

    return 0;
}

/**
 * @brief  Write a value to a PHY register through the MDIO interface.
 * @param  DevAddr: PHY port address
 * @param  RegAddr: PHY register address
 * @param  RegVal: Value to be written
 * @retval 0 if OK -1 if Error
 */
int32_t ETH_PHY_IO_WriteReg(uint32_t DevAddr, uint32_t RegAddr,
                            uint32_t RegVal) {
    if (HAL_ETH_WritePHYRegister(&heth, DevAddr, RegAddr, RegVal) != HAL_OK) {
        return -1;
    }

    return 0;
}

/**
 * @brief  Get the time in millisecons used for internal PHY driver process.
 * @retval Time value
 */
int32_t ETH_PHY_IO_GetTick(void) { return HAL_GetTick(); }

/**
 * @brief  Check the ETH link state then update ETH driver and netif link
 * accordingly.
 * @param  argument: netif
 * @retval None
 */
void ethernet_link_thread(void const *argument) {
    ETH_MACConfigTypeDef MACConf;
    int32_t PHYLinkState;
    uint32_t linkchanged = 0, speed = 0, duplex = 0;

    struct netif *netif = (struct netif *)argument;

    for (;;) {
        PHYLinkState = LAN8742_GetLinkState(&LAN8742);

        if (netif_is_link_up(netif) &&
            (PHYLinkState <= LAN8742_STATUS_LINK_DOWN)) {
            HAL_ETH_Stop_IT(&heth);
            netif_set_down(netif);
            netif_set_link_down(netif);
        } else if (!netif_is_link_up(netif) &&
                   (PHYLinkState > LAN8742_STATUS_LINK_DOWN)) {
            switch (PHYLinkState) {
            case LAN8742_STATUS_100MBITS_FULLDUPLEX:
                duplex = ETH_FULLDUPLEX_MODE;
                speed = ETH_SPEED_100M;
                linkchanged = 1;
                break;
            case LAN8742_STATUS_100MBITS_HALFDUPLEX:
                duplex = ETH_HALFDUPLEX_MODE;
                speed = ETH_SPEED_100M;
                linkchanged = 1;
                break;
            case LAN8742_STATUS_10MBITS_FULLDUPLEX:
                duplex = ETH_FULLDUPLEX_MODE;
                speed = ETH_SPEED_10M;
                linkchanged = 1;
                break;
            case LAN8742_STATUS_10MBITS_HALFDUPLEX:
                duplex = ETH_HALFDUPLEX_MODE;
                speed = ETH_SPEED_10M;
                linkchanged = 1;
                break;
            default:
                break;
            }

            if (linkchanged) {
                /* Get MAC Config MAC */
                HAL_ETH_GetMACConfig(&heth, &MACConf);
                MACConf.DuplexMode = duplex;
                MACConf.Speed = speed;
                HAL_ETH_SetMACConfig(&heth, &MACConf);

                HAL_ETH_Start_IT(&heth);
                netif_set_up(netif);
                netif_set_link_up(netif);
            }
        }

        osDelay(100);
    }
}
