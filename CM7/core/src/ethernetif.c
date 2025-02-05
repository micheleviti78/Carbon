/**
 ******************************************************************************
 * @file           ethernetif.c
 * @author         Michele Viti <micheleviti78@gmail.com>
 * @date           Dez. 2024
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

#include <cmsis_os.h>

#include <lwip/ethip6.h>
#include <lwip/opt.h>
#include <lwip/tcpip.h>
#include <lwip/timeouts.h>
#include <netif/etharp.h>
#include <netif/ethernet.h>

#include <string.h>

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

#define ETH_MACCR_MASK ((uint32_t)0xFFFB7F7CU)
#define ETH_MACECR_MASK ((uint32_t)0x3F077FFFU)
#define ETH_MACPFR_MASK ((uint32_t)0x800007FFU)
#define ETH_MACWTR_MASK ((uint32_t)0x0000010FU)
#define ETH_MACTFCR_MASK ((uint32_t)0xFFFF00F2U)
#define ETH_MACRFCR_MASK ((uint32_t)0x00000003U)
#define ETH_MTLTQOMR_MASK ((uint32_t)0x00000072U)
#define ETH_MTLRQOMR_MASK ((uint32_t)0x0000007BU)

#define ETH_DMAMR_MASK ((uint32_t)0x00007802U)
#define ETH_DMASBMR_MASK ((uint32_t)0x0000D001U)
#define ETH_DMACCR_MASK ((uint32_t)0x00013FFFU)
#define ETH_DMACTCR_MASK ((uint32_t)0x003F1010U)
#define ETH_DMACRCR_MASK ((uint32_t)0x803F0000U)
#define ETH_MACPCSR_MASK                                                       \
    (ETH_MACPCSR_PWRDWN | ETH_MACPCSR_RWKPKTEN | ETH_MACPCSR_MGKPKTEN |        \
     ETH_MACPCSR_GLBLUCAST | ETH_MACPCSR_RWKPFE)

#define ETH_SWRESET_TIMEOUT ((uint32_t)500U)
#define ETH_MDIO_BUS_TIMEOUT ((uint32_t)1000U)
#define ETH_DMA_TRANSMIT_TIMEOUT ((uint32_t)2000U)

#define ETH_MAC_US_TICK ((uint32_t)1000000U)

#define ETH_RX_BUFFER_SIZE_ALIGNED ALIGN(ETH_RX_BUFFER_SIZE, CACHE_ALIGNMENT)

static Eth_Handle eth_handle;

static osSemaphoreId rxPktSemaphore =
    NULL; /* Semaphore to signal incoming packets */
static osSemaphoreId txPktSemaphore =
    NULL; /* Semaphore to signal outcome packets */
static osMutexId rx_ptk_mutex;
static osMutexId tx_ptk_mutex;

static volatile bool isInitialized = false;

/* Ethernet Rx DMA Descriptors */
ETH_DMADescTypeDef dmaRxDscrTab[ETH_RX_DESC_CNT]
    __attribute__((aligned(32), section(".RxDecripSection")));
/* Ethernet Tx DMA Descriptors */
ETH_DMADescTypeDef dmaTxDscrTab[ETH_TX_DESC_CNT]
    __attribute__((aligned(32), section(".TxDecripSection")));
/* Ethernet Receive Buffers */
static uint8_t rx_Buff[ETH_RX_DESC_CNT][ETH_RX_BUFFER_SIZE_ALIGNED]
    __attribute__((aligned(32)));
/* Ethernet Transmit Buffers */
static uint8_t tx_Buff[ETH_RX_BUFFER_SIZE_ALIGNED] __attribute__((aligned(32)));

#define INCREASE_TX_POINTER(pointer)                                           \
    if (pointer == &dmaTxDscrTab[ETH_TX_DESC_CNT - 1])                         \
        pointer = &dmaTxDscrTab[0];                                            \
    else                                                                       \
        pointer = pointer + 1;

#define INCREASE_RX_POINTER(pointer)                                           \
    if (pointer == &dmaRxDscrTab[ETH_RX_DESC_CNT - 1])                         \
        pointer = &dmaRxDscrTab[0];                                            \
    else                                                                       \
        pointer = pointer + 1;

struct carbon_pbuf_custom {
    struct pbuf_custom pbuf_custom;
    uint8_t *buffer;
};

LWIP_MEMPOOL_DECLARE(rx_pool, 10, sizeof(struct carbon_pbuf_custom),
                     "Zero-copy RX PBUF pool");

/*callbacks for the PHY -----------------------------------------------*/
int32_t carbon_hw_ethernet_phy_io_init(void);
int32_t carbon_hw_ethernet_phy_io_deinit(void);
int32_t carbon_hw_ethernet_phy_io_read_reg(uint32_t DevAddr, uint32_t RegAddr,
                                           uint32_t *pRegVal);
int32_t carbon_hw_ethernet_phy_io_write_reg(uint32_t DevAddr, uint32_t RegAddr,
                                            uint32_t RegVal);
int32_t carbon_hw_ethernet_phy_io_get_tick(void);

lan8742_Object_t lan8742;
lan8742_IOCtx_t lan8742_IOCtx = {
    carbon_hw_ethernet_phy_io_init, carbon_hw_ethernet_phy_io_deinit,
    carbon_hw_ethernet_phy_io_write_reg, carbon_hw_ethernet_phy_io_read_reg,
    carbon_hw_ethernet_phy_io_get_tick};

/*static functions declaration*/
static unsigned carbon_hw_ethernet_low_level_init(struct netif *netif);
static void carbon_hw_ethernet_msp_init(Eth_Handle *heth);
static void carbon_hw_ethernet_tx_dec_list_init(Eth_Handle *heth);
static void carbon_hw_ethernet_mac_mdio_clkconfig(Eth_Handle *heth);
static void carbon_hw_ethernet_mac_dma_config(Eth_Handle *heth);
static void carbon_hw_ethernet_set_mac_config(Eth_Handle *heth,
                                              ETH_MACConfigTypeDef *macconf);
static void carbon_hw_ethernet_get_mac_config(Eth_Handle *heth,
                                              ETH_MACConfigTypeDef *macconf);
static void carbon_hw_ethernet_set_dma_config(Eth_Handle *heth,
                                              ETH_DMAConfigTypeDef *dmaconf);
static void carbon_hw_ethernet_rx_dec_list_init(Eth_Handle *heth);
static void carbon_hw_ethernet_prepare_rx_desc(Eth_Handle *heth,
                                               ETH_DMADescTypeDef *dmarxdesc,
                                               uint32_t pBuffer1);
static void carbon_hw_ethernet_start(Eth_Handle *heth);
static void carbon_hw_ethernet_stop(Eth_Handle *heth);

static struct pbuf *carbon_lwip_low_level_input(struct netif *netif);
static err_t carbon_lwip_output(struct netif *netif, struct pbuf *p);
static void carbon_lwip_prepare_rx_descriptor(uint32_t bufferPtr);

/*functions declaration */
void pbuf_free_custom(struct pbuf *p);
void Error_Handler(void);

/*******************************************************************************
                       HAL Driver Interface
 *******************************************************************************/

void carbon_hw_ethernet_msp_init(Eth_Handle *heth) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (heth->Instance == ETH) {
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
PC1      ------> ETH_MDC
PA2      ------> ETH_MDIO
PA1      ------> ETH_REF_CLK
PA7      ------> ETH_CRS_DV
PC4      ------> ETH_RXD0
PC5      ------> ETH_RXD1
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

void carbon_hw_ethernet_isr() {
    if (__HAL_ETH_DMA_GET_IT(&eth_handle, ETH_DMACSR_RI)) {
        if (__HAL_ETH_DMA_GET_IT_SOURCE(&eth_handle, ETH_DMACIER_RIE)) {
            osSemaphoreRelease(rxPktSemaphore);
            /* Clear the Eth DMA Rx IT pending bits */
            __HAL_ETH_DMA_CLEAR_IT(&eth_handle, ETH_DMACSR_RI | ETH_DMACSR_NIS);
        }
    }
    if (__HAL_ETH_DMA_GET_IT(&eth_handle, ETH_DMACSR_TI)) {
        if (__HAL_ETH_DMA_GET_IT_SOURCE(&eth_handle, ETH_DMACIER_TIE)) {
            osSemaphoreRelease(txPktSemaphore);
            /* Clear the Eth DMA Tx IT pending bits */
            __HAL_ETH_DMA_CLEAR_IT(&eth_handle, ETH_DMACSR_TI | ETH_DMACSR_NIS);
        }
    }
    if (__HAL_ETH_DMA_GET_IT(&eth_handle,
                             ETH_DMACSR_AIS)) { /*Abnormal Interrupt Summary*/
        if (__HAL_ETH_DMA_GET_IT_SOURCE(&eth_handle, ETH_DMACIER_AIE)) {
            eth_handle.errorCode |= HAL_ETH_ERROR_DMA;
            /* set HAL state to ERROR */
            eth_handle.gState = HAL_ETH_STATE_ERROR;

            /* if fatal bus error occurred */
            if (__HAL_ETH_DMA_GET_IT(&eth_handle, ETH_DMACSR_FBE)) {
                /* check Fatal Bus Error (FBE), Transmit Process Stopped (TPS),
                 * Receive Process Stopped(RPS)  */
                eth_handle.dmaErrorCode = READ_BIT(
                    eth_handle.Instance->DMACSR,
                    (ETH_DMACSR_FBE | ETH_DMACSR_TPS | ETH_DMACSR_RPS));

                /* disable interrupts Normal Interrupt Summary Enable (NIE),
                 * Abnormal Interrupt Summary Enable (AIE)*/
                __HAL_ETH_DMA_DISABLE_IT(&eth_handle,
                                         ETH_DMACIER_NIE | ETH_DMACIER_AIE);
            }

            if (__HAL_ETH_DMA_GET_IT(&eth_handle, ETH_DMACSR_RBU))
                eth_handle.lostReceivedPackets++;

            /* check Context Descriptor Error (CDE), Early Transmit
             * Interrupt (ETI), Receive Watchdog Timeout (RWT), Receive
             * Buffer Unavailable (RBU), Abnormal Interrupt Summary (AIS)*/
            eth_handle.dmaErrorCode =
                READ_BIT(eth_handle.Instance->DMACSR,
                         (ETH_DMACSR_FBE | ETH_DMACSR_TPS | ETH_DMACSR_RPS |
                          ETH_DMACSR_CDE | ETH_DMACSR_ETI | ETH_DMACSR_RWT |
                          ETH_DMACSR_RBU | ETH_DMACSR_AIS));

            /* clear the interrupt summary flag */
            __HAL_ETH_DMA_CLEAR_IT(
                &eth_handle, (ETH_DMACSR_FBE | ETH_DMACSR_TPS | ETH_DMACSR_RPS |
                              ETH_DMACSR_CDE | ETH_DMACSR_ETI | ETH_DMACSR_RWT |
                              ETH_DMACSR_RBU | ETH_DMACSR_AIS));
        }
    }
    if (__HAL_ETH_MAC_GET_IT(&eth_handle,
                             (ETH_MACISR_RXSTSIS | ETH_MACISR_TXSTSIS))) {
        eth_handle.gState = HAL_ETH_STATE_ERROR;
        /* Get MAC Rx Tx status and clear Status register pending bit */
        eth_handle.macErrorCode = READ_REG(eth_handle.Instance->MACRXTXSR);
    }
}

unsigned carbon_hw_ethernet_low_level_init(struct netif *netif) {
    ETH_MACConfigTypeDef macConf;
    int32_t phyLinkState;
    uint32_t duplex, speed = 0;

    uint8_t MACAddr[6];
    eth_handle.Instance = ETH;
    MACAddr[0] = 0x00;
    MACAddr[1] = 0x80;
    MACAddr[2] = 0xE1;
    MACAddr[3] = 0x00;
    MACAddr[4] = 0x00;
    MACAddr[5] = 0x00;
    eth_handle.Init.MACAddr = &MACAddr[0];
    // eth_handle.Init.MediaInterface = HAL_ETH_RMII_MODE;
    eth_handle.Init.TxDesc = dmaTxDscrTab;
    eth_handle.Init.RxDesc = dmaRxDscrTab;
    eth_handle.Init.RxBuffLen = ETH_RX_BUFFER_SIZE_ALIGNED;

    carbon_hw_ethernet_msp_init(&eth_handle);

    __HAL_RCC_SYSCFG_CLK_ENABLE();

    HAL_SYSCFG_ETHInterfaceSelect(SYSCFG_ETH_RMII);
    SET_BIT(eth_handle.Instance->DMAMR, ETH_DMAMR_SWR);

    uint32_t tickstart = HAL_GetTick();

    while (READ_BIT(eth_handle.Instance->DMAMR, ETH_DMAMR_SWR) > 0U) {
        if (((HAL_GetTick() - tickstart) > ETH_SWRESET_TIMEOUT)) {
            eth_handle.errorCode = HAL_ETH_ERROR_TIMEOUT;
            eth_handle.gState = HAL_ETH_STATE_ERROR;
            DIAG(ETH_DIAG "init failed, timeout waiting for MAC reset");
            return 1;
        }
    }

    /*MDIO CSR Clock Range Configuration*/
    carbon_hw_ethernet_mac_mdio_clkconfig(&eth_handle);

    /*MAC LPI 1US Tic Counter Configuration*/
    WRITE_REG(eth_handle.Instance->MAC1USTCR,
              (((uint32_t)HAL_RCC_GetHCLKFreq() / ETH_MAC_US_TICK) - 1U));

    /*MAC, MTL and DMA default Configuration*/
    carbon_hw_ethernet_mac_dma_config(&eth_handle);

    /* SET DSL to 64 bit */
    MODIFY_REG(eth_handle.Instance->DMACCR, ETH_DMACCR_DSL,
               ETH_DMACCR_DSL_64BIT);

    /* Set Receive Buffers Length (must be a multiple of 4) */
    if ((eth_handle.Init.RxBuffLen % 0x4U) != 0x0U) {
        /* Set Error Code */
        eth_handle.errorCode = HAL_ETH_ERROR_PARAM;
        /* Set State as Error */
        eth_handle.gState = HAL_ETH_STATE_ERROR;
        DIAG(ETH_DIAG "init failed, receive buffer not 4 bit aligned");
        return 1;
    }

    /*DMA Tx Descriptors Configuration*/
    carbon_hw_ethernet_tx_dec_list_init(&eth_handle);

    /* DMA Rx Descriptors Configuration */
    carbon_hw_ethernet_rx_dec_list_init(&eth_handle);

    /* ETHERNET MAC Address Configuration */
    /* Set MAC addr bits 32 to 47 */
    eth_handle.Instance->MACA0HR =
        (((uint32_t)(eth_handle.Init.MACAddr[5]) << 8) |
         (uint32_t)eth_handle.Init.MACAddr[4]);
    /* Set MAC addr bits 0 to 31 */
    eth_handle.Instance->MACA0LR =
        (((uint32_t)(eth_handle.Init.MACAddr[3]) << 24) |
         ((uint32_t)(eth_handle.Init.MACAddr[2]) << 16) |
         ((uint32_t)(eth_handle.Init.MACAddr[1]) << 8) |
         (uint32_t)eth_handle.Init.MACAddr[0]);

    eth_handle.errorCode = HAL_ETH_ERROR_NONE;
    eth_handle.gState = HAL_ETH_STATE_READY;
    eth_handle.txState = HAL_ETH_STATE_READY;

    LWIP_MEMPOOL_INIT(rx_pool);

#if LWIP_ARP || LWIP_ETHERNET

    /* set MAC hardware address length */
    netif->hwaddr_len = ETH_HWADDR_LEN;

    /* set MAC hardware address */
    netif->hwaddr[0] = eth_handle.Init.MACAddr[0];
    netif->hwaddr[1] = eth_handle.Init.MACAddr[1];
    netif->hwaddr[2] = eth_handle.Init.MACAddr[2];
    netif->hwaddr[3] = eth_handle.Init.MACAddr[3];
    netif->hwaddr[4] = eth_handle.Init.MACAddr[4];
    netif->hwaddr[5] = eth_handle.Init.MACAddr[5];

    /* maximum transfer unit */
    netif->mtu = ETH_MAX_PAYLOAD;

    /* Accept broadcast address and ARP traffic */
    /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
#if LWIP_ARP
    netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;
#else
    netif->flags |= NETIF_FLAG_BROADCAST;
#endif /* LWIP_ARP */

    /* create a binary semaphore used for informing ethernetif of frame
     * reception/sending */
    rxPktSemaphore = xSemaphoreCreateBinary();
    txPktSemaphore = xSemaphoreCreateBinary();
    /*reset the semaphores*/
    osSemaphoreWait(rxPktSemaphore, 0);
    osSemaphoreWait(txPktSemaphore, 0);

    osMutexDef(rx_ptk_mutex);
    rx_ptk_mutex = osMutexCreate(osMutex(rx_ptk_mutex));
    osMutexDef(tx_ptk_mutex);
    tx_ptk_mutex = osMutexCreate(osMutex(tx_ptk_mutex));

    /* create the task that handles the ETH_MAC */
    osThreadDef(EthIf, carbon_lwip_input, osPriorityRealtime, 0,
                INTERFACE_THREAD_STACK_SIZE);
    osThreadCreate(osThread(EthIf), netif);

    /* Set PHY IO functions */
    LAN8742_RegisterBusIO(&lan8742, &lan8742_IOCtx);

    /* Initialize the LAN8742 ETH PHY */
    LAN8742_Init(&lan8742);

    phyLinkState = LAN8742_GetLinkState(&lan8742);

    /* Get link state */
    if (phyLinkState <= LAN8742_STATUS_LINK_DOWN) {
        netif_set_link_down(netif);
        netif_set_down(netif);
    } else {
        switch (phyLinkState) {
        case LAN8742_STATUS_100MBITS_FULLDUPLEX:
            duplex = ETH_FULLDUPLEX_MODE;
            speed = ETH_SPEED_100M;
            DIAG(ETH_DIAG "full duplex, 100 Mbit/sec");
            break;
        case LAN8742_STATUS_100MBITS_HALFDUPLEX:
            duplex = ETH_HALFDUPLEX_MODE;
            speed = ETH_SPEED_100M;
            DIAG(ETH_DIAG "half duplex, 100 Mbit/sec");
            break;
        case LAN8742_STATUS_10MBITS_FULLDUPLEX:
            duplex = ETH_FULLDUPLEX_MODE;
            speed = ETH_SPEED_10M;
            DIAG(ETH_DIAG "full duplex, 10 Mbit/sec");
            break;
        case LAN8742_STATUS_10MBITS_HALFDUPLEX:
            duplex = ETH_HALFDUPLEX_MODE;
            speed = ETH_SPEED_10M;
            DIAG(ETH_DIAG "half duplex, 100 Mbit/sec");
            break;
        default:
            duplex = ETH_FULLDUPLEX_MODE;
            speed = ETH_SPEED_100M;
            DIAG(ETH_DIAG "default configuration: full duplex, 100 Mbit/sec");
            break;
        }

        /* Get MAC Config MAC */
        carbon_hw_ethernet_get_mac_config(&eth_handle, &macConf);
        macConf.DuplexMode = duplex;
        macConf.Speed = speed;
        carbon_hw_ethernet_set_mac_config(&eth_handle, &macConf);
        carbon_hw_ethernet_start(&eth_handle);
        netif_set_up(netif);
        netif_set_link_up(netif);
    }
#endif /* LWIP_ARP || LWIP_ETHERNET */
    isInitialized = true;

    DIAG(ETH_DIAG "low level init successful");

    return 0;
}

void carbon_hw_ethernet_tx_dec_list_init(Eth_Handle *heth) {
    ETH_DMADescTypeDef *dmatxdesc;
    uint32_t i;

    /* Fill each DMATxDesc descriptor with the right values */
    for (i = 0; i < ETH_TX_DESC_CNT; i++) {
        dmatxdesc = heth->Init.TxDesc + i;

        WRITE_REG(dmatxdesc->DESC0, 0x0);
        WRITE_REG(dmatxdesc->DESC1, 0x0);
        WRITE_REG(dmatxdesc->DESC2, 0x0);
        WRITE_REG(dmatxdesc->DESC3, 0x0);

        WRITE_REG(heth->txDescList.txDesc[i], (uint32_t)dmatxdesc);
    }

    /* Set Transmit Descriptor Ring Length */
    WRITE_REG(heth->Instance->DMACTDRLR, (ETH_TX_DESC_CNT - 1));

    /* Set Transmit Descriptor List Address */
    WRITE_REG(heth->Instance->DMACTDLAR,
              (uint32_t)(heth->txDescList.txDesc[0]));

    /* Set Transmit Descriptor Tail pointer */
    WRITE_REG(heth->Instance->DMACTDTPR,
              (uint32_t)(heth->txDescList.txDesc[0]));
}

void carbon_hw_ethernet_mac_mdio_clkconfig(Eth_Handle *heth) {
    uint32_t tmpreg, hclk;

    /* Get the ETHERNET MACMDIOAR value */
    tmpreg = (heth->Instance)->MACMDIOAR;

    /* Clear CSR Clock Range bits */
    tmpreg &= ~ETH_MACMDIOAR_CR;

    /* Get hclk frequency value */
    hclk = HAL_RCC_GetHCLKFreq();

    /* Set CR bits depending on hclk value */
    if ((hclk >= 20000000U) && (hclk < 35000000U)) {
        /* CSR Clock Range between 20-35 MHz */
        tmpreg |= (uint32_t)ETH_MACMDIOAR_CR_DIV16;
    } else if ((hclk >= 35000000U) && (hclk < 60000000U)) {
        /* CSR Clock Range between 35-60 MHz */
        tmpreg |= (uint32_t)ETH_MACMDIOAR_CR_DIV26;
    } else if ((hclk >= 60000000U) && (hclk < 100000000U)) {
        /* CSR Clock Range between 60-100 MHz */
        tmpreg |= (uint32_t)ETH_MACMDIOAR_CR_DIV42;
    } else if ((hclk >= 100000000U) && (hclk < 150000000U)) {
        /* CSR Clock Range between 100-150 MHz */
        tmpreg |= (uint32_t)ETH_MACMDIOAR_CR_DIV62;
    } else /* (hclk >= 150000000)&&(hclk <= 200000000) */
    {
        /* CSR Clock Range between 150-200 MHz */
        tmpreg |= (uint32_t)ETH_MACMDIOAR_CR_DIV102;
    }

    /* Configure the CSR Clock Range */
    (heth->Instance)->MACMDIOAR = (uint32_t)tmpreg;
}

void carbon_hw_ethernet_mac_dma_config(Eth_Handle *heth) {
    ETH_MACConfigTypeDef macDefaultConf;
    ETH_DMAConfigTypeDef dmaDefaultConf;

    /*--------------- ETHERNET MAC registers default Configuration
     * --------------*/
    macDefaultConf.AutomaticPadCRCStrip = ENABLE;
    macDefaultConf.BackOffLimit = ETH_BACKOFFLIMIT_10;
    macDefaultConf.CarrierSenseBeforeTransmit = DISABLE;
    macDefaultConf.CarrierSenseDuringTransmit = DISABLE;
    macDefaultConf.ChecksumOffload = ENABLE;
    macDefaultConf.CRCCheckingRxPackets = ENABLE;
    macDefaultConf.CRCStripTypePacket = ENABLE;
    macDefaultConf.DeferralCheck = DISABLE;
    macDefaultConf.DropTCPIPChecksumErrorPacket = ENABLE;
    macDefaultConf.DuplexMode = ETH_FULLDUPLEX_MODE;
    macDefaultConf.ExtendedInterPacketGap = DISABLE;
    macDefaultConf.ExtendedInterPacketGapVal = 0x0;
    macDefaultConf.ForwardRxErrorPacket = DISABLE;
    macDefaultConf.ForwardRxUndersizedGoodPacket = DISABLE;
    macDefaultConf.GiantPacketSizeLimit = 0x618;
    macDefaultConf.GiantPacketSizeLimitControl = DISABLE;
    macDefaultConf.InterPacketGapVal = ETH_INTERPACKETGAP_96BIT;
    macDefaultConf.Jabber = ENABLE;
    macDefaultConf.JumboPacket = DISABLE;
    macDefaultConf.LoopbackMode = DISABLE;
    macDefaultConf.PauseLowThreshold = ETH_PAUSELOWTHRESHOLD_MINUS_4;
    macDefaultConf.PauseTime = 0x0;
    macDefaultConf.PreambleLength = ETH_PREAMBLELENGTH_7;
    macDefaultConf.ProgrammableWatchdog = DISABLE;
    macDefaultConf.ReceiveFlowControl = DISABLE;
    macDefaultConf.ReceiveOwn = ENABLE;
    macDefaultConf.ReceiveQueueMode = ETH_RECEIVESTOREFORWARD;
    macDefaultConf.RetryTransmission = ENABLE;
    macDefaultConf.SlowProtocolDetect = DISABLE;
    macDefaultConf.SourceAddrControl = ETH_SOURCEADDRESS_REPLACE_ADDR0;
    macDefaultConf.Speed = ETH_SPEED_100M;
    macDefaultConf.Support2KPacket = DISABLE;
    macDefaultConf.TransmitQueueMode = ETH_TRANSMITSTOREFORWARD;
    macDefaultConf.TransmitFlowControl = DISABLE;
    macDefaultConf.UnicastPausePacketDetect = DISABLE;
    macDefaultConf.UnicastSlowProtocolPacketDetect = DISABLE;
    macDefaultConf.Watchdog = ENABLE;
    macDefaultConf.WatchdogTimeout = ETH_MACWTR_WTO_2KB;
    macDefaultConf.ZeroQuantaPause = ENABLE;

    /* MAC default configuration */
    carbon_hw_ethernet_set_mac_config(heth, &macDefaultConf);

    /*--------------- ETHERNET DMA registers default Configuration
     * --------------*/
    dmaDefaultConf.AddressAlignedBeats = ENABLE;
    dmaDefaultConf.BurstMode = ETH_BURSTLENGTH_FIXED;
    dmaDefaultConf.DMAArbitration = ETH_DMAARBITRATION_RX1_TX1;
    dmaDefaultConf.FlushRxPacket = DISABLE;
    dmaDefaultConf.PBLx8Mode = DISABLE;
    dmaDefaultConf.RebuildINCRxBurst = DISABLE;
    dmaDefaultConf.RxDMABurstLength = ETH_RXDMABURSTLENGTH_32BEAT;
    dmaDefaultConf.SecondPacketOperate = DISABLE;
    dmaDefaultConf.TxDMABurstLength = ETH_TXDMABURSTLENGTH_32BEAT;
    dmaDefaultConf.TCPSegmentation = DISABLE;
    dmaDefaultConf.MaximumSegmentSize = 536;

    /* DMA default configuration */
    carbon_hw_ethernet_set_dma_config(heth, &dmaDefaultConf);
}

void carbon_hw_ethernet_set_mac_config(Eth_Handle *heth,
                                       ETH_MACConfigTypeDef *macconf) {
    uint32_t macregval;

    /*------------------------ MACCR Configuration --------------------*/
    macregval =
        (macconf->InterPacketGapVal | macconf->SourceAddrControl |
         ((uint32_t)macconf->ChecksumOffload << 27) |
         ((uint32_t)macconf->GiantPacketSizeLimitControl << 23) |
         ((uint32_t)macconf->Support2KPacket << 22) |
         ((uint32_t)macconf->CRCStripTypePacket << 21) |
         ((uint32_t)macconf->AutomaticPadCRCStrip << 20) |
         ((uint32_t)((macconf->Watchdog == DISABLE) ? 1U : 0U) << 19) |
         ((uint32_t)((macconf->Jabber == DISABLE) ? 1U : 0U) << 17) |
         ((uint32_t)macconf->JumboPacket << 16) | macconf->Speed |
         macconf->DuplexMode | ((uint32_t)macconf->LoopbackMode << 12) |
         ((uint32_t)macconf->CarrierSenseBeforeTransmit << 11) |
         ((uint32_t)((macconf->ReceiveOwn == DISABLE) ? 1U : 0U) << 10) |
         ((uint32_t)macconf->CarrierSenseDuringTransmit << 9) |
         ((uint32_t)((macconf->RetryTransmission == DISABLE) ? 1U : 0U) << 8) |
         macconf->BackOffLimit | ((uint32_t)macconf->DeferralCheck << 4) |
         macconf->PreambleLength);

    /* Write to MACCR */
    MODIFY_REG(heth->Instance->MACCR, ETH_MACCR_MASK, macregval);

    /*------------------------ MACECR Configuration --------------------*/
    macregval =
        ((macconf->ExtendedInterPacketGapVal << 25) |
         ((uint32_t)macconf->ExtendedInterPacketGap << 24) |
         ((uint32_t)macconf->UnicastSlowProtocolPacketDetect << 18) |
         ((uint32_t)macconf->SlowProtocolDetect << 17) |
         ((uint32_t)((macconf->CRCCheckingRxPackets == DISABLE) ? 1U : 0U)
          << 16) |
         macconf->GiantPacketSizeLimit);

    /* Write to MACECR */
    MODIFY_REG(heth->Instance->MACECR, ETH_MACECR_MASK, macregval);

    /*------------------------ MACWTR Configuration --------------------*/
    macregval = (((uint32_t)macconf->ProgrammableWatchdog << 8) |
                 macconf->WatchdogTimeout);

    /* Write to MACWTR */
    MODIFY_REG(heth->Instance->MACWTR, ETH_MACWTR_MASK, macregval);

    /*------------------------ MACTFCR Configuration --------------------*/
    macregval =
        (((uint32_t)macconf->TransmitFlowControl << 1) |
         macconf->PauseLowThreshold |
         ((uint32_t)((macconf->ZeroQuantaPause == DISABLE) ? 1U : 0U) << 7) |
         (macconf->PauseTime << 16));

    /* Write to MACTFCR */
    MODIFY_REG(heth->Instance->MACTFCR, ETH_MACTFCR_MASK, macregval);

    /*------------------------ MACRFCR Configuration --------------------*/
    macregval = ((uint32_t)macconf->ReceiveFlowControl |
                 ((uint32_t)macconf->UnicastPausePacketDetect << 1));

    /* Write to MACRFCR */
    MODIFY_REG(heth->Instance->MACRFCR, ETH_MACRFCR_MASK, macregval);

    /*------------------------ MTLTQOMR Configuration --------------------*/
    /* Write to MTLTQOMR */
    MODIFY_REG(heth->Instance->MTLTQOMR, ETH_MTLTQOMR_MASK,
               macconf->TransmitQueueMode);

    /*------------------------ MTLRQOMR Configuration --------------------*/
    macregval =
        (macconf->ReceiveQueueMode |
         ((uint32_t)((macconf->DropTCPIPChecksumErrorPacket == DISABLE) ? 1U
                                                                        : 0U)
          << 6) |
         ((uint32_t)macconf->ForwardRxErrorPacket << 4) |
         ((uint32_t)macconf->ForwardRxUndersizedGoodPacket << 3));

    /* Write to MTLRQOMR */
    MODIFY_REG(heth->Instance->MTLRQOMR, ETH_MTLRQOMR_MASK, macregval);
}

void carbon_hw_ethernet_get_mac_config(Eth_Handle *heth,
                                       ETH_MACConfigTypeDef *macconf) {
    /* Get MAC parameters */
    macconf->PreambleLength = READ_BIT(heth->Instance->MACCR, ETH_MACCR_PRELEN);
    macconf->DeferralCheck =
        ((READ_BIT(heth->Instance->MACCR, ETH_MACCR_DC) >> 4) > 0U) ? ENABLE
                                                                    : DISABLE;
    macconf->BackOffLimit = READ_BIT(heth->Instance->MACCR, ETH_MACCR_BL);
    macconf->RetryTransmission =
        ((READ_BIT(heth->Instance->MACCR, ETH_MACCR_DR) >> 8) == 0U) ? ENABLE
                                                                     : DISABLE;
    macconf->CarrierSenseDuringTransmit =
        ((READ_BIT(heth->Instance->MACCR, ETH_MACCR_DCRS) >> 9) > 0U) ? ENABLE
                                                                      : DISABLE;
    macconf->ReceiveOwn =
        ((READ_BIT(heth->Instance->MACCR, ETH_MACCR_DO) >> 10) == 0U) ? ENABLE
                                                                      : DISABLE;
    macconf->CarrierSenseBeforeTransmit =
        ((READ_BIT(heth->Instance->MACCR, ETH_MACCR_ECRSFD) >> 11) > 0U)
            ? ENABLE
            : DISABLE;
    macconf->LoopbackMode =
        ((READ_BIT(heth->Instance->MACCR, ETH_MACCR_LM) >> 12) > 0U) ? ENABLE
                                                                     : DISABLE;
    macconf->DuplexMode = READ_BIT(heth->Instance->MACCR, ETH_MACCR_DM);
    macconf->Speed = READ_BIT(heth->Instance->MACCR, ETH_MACCR_FES);
    macconf->JumboPacket =
        ((READ_BIT(heth->Instance->MACCR, ETH_MACCR_JE) >> 16) > 0U) ? ENABLE
                                                                     : DISABLE;
    macconf->Jabber =
        ((READ_BIT(heth->Instance->MACCR, ETH_MACCR_JD) >> 17) == 0U) ? ENABLE
                                                                      : DISABLE;
    macconf->Watchdog =
        ((READ_BIT(heth->Instance->MACCR, ETH_MACCR_WD) >> 19) == 0U) ? ENABLE
                                                                      : DISABLE;
    macconf->AutomaticPadCRCStrip =
        ((READ_BIT(heth->Instance->MACCR, ETH_MACCR_ACS) >> 20) > 0U) ? ENABLE
                                                                      : DISABLE;
    macconf->CRCStripTypePacket =
        ((READ_BIT(heth->Instance->MACCR, ETH_MACCR_CST) >> 21) > 0U) ? ENABLE
                                                                      : DISABLE;
    macconf->Support2KPacket =
        ((READ_BIT(heth->Instance->MACCR, ETH_MACCR_S2KP) >> 22) > 0U)
            ? ENABLE
            : DISABLE;
    macconf->GiantPacketSizeLimitControl =
        ((READ_BIT(heth->Instance->MACCR, ETH_MACCR_GPSLCE) >> 23) > 0U)
            ? ENABLE
            : DISABLE;
    macconf->InterPacketGapVal = READ_BIT(heth->Instance->MACCR, ETH_MACCR_IPG);
    macconf->ChecksumOffload =
        ((READ_BIT(heth->Instance->MACCR, ETH_MACCR_IPC) >> 27) > 0U) ? ENABLE
                                                                      : DISABLE;
    macconf->SourceAddrControl =
        READ_BIT(heth->Instance->MACCR, ETH_MACCR_SARC);

    macconf->GiantPacketSizeLimit =
        READ_BIT(heth->Instance->MACECR, ETH_MACECR_GPSL);
    macconf->CRCCheckingRxPackets =
        ((READ_BIT(heth->Instance->MACECR, ETH_MACECR_DCRCC) >> 16) == 0U)
            ? ENABLE
            : DISABLE;
    macconf->SlowProtocolDetect =
        ((READ_BIT(heth->Instance->MACECR, ETH_MACECR_SPEN) >> 17) > 0U)
            ? ENABLE
            : DISABLE;
    macconf->UnicastSlowProtocolPacketDetect =
        ((READ_BIT(heth->Instance->MACECR, ETH_MACECR_USP) >> 18) > 0U)
            ? ENABLE
            : DISABLE;
    macconf->ExtendedInterPacketGap =
        ((READ_BIT(heth->Instance->MACECR, ETH_MACECR_EIPGEN) >> 24) > 0U)
            ? ENABLE
            : DISABLE;
    macconf->ExtendedInterPacketGapVal =
        READ_BIT(heth->Instance->MACECR, ETH_MACECR_EIPG) >> 25;

    macconf->ProgrammableWatchdog =
        ((READ_BIT(heth->Instance->MACWTR, ETH_MACWTR_PWE) >> 8) > 0U)
            ? ENABLE
            : DISABLE;
    macconf->WatchdogTimeout = READ_BIT(heth->Instance->MACWTR, ETH_MACWTR_WTO);

    macconf->TransmitFlowControl =
        ((READ_BIT(heth->Instance->MACTFCR, ETH_MACTFCR_TFE) >> 1) > 0U)
            ? ENABLE
            : DISABLE;
    macconf->ZeroQuantaPause =
        ((READ_BIT(heth->Instance->MACTFCR, ETH_MACTFCR_DZPQ) >> 7) == 0U)
            ? ENABLE
            : DISABLE;
    macconf->PauseLowThreshold =
        READ_BIT(heth->Instance->MACTFCR, ETH_MACTFCR_PLT);
    macconf->PauseTime =
        (READ_BIT(heth->Instance->MACTFCR, ETH_MACTFCR_PT) >> 16);

    macconf->ReceiveFlowControl =
        (READ_BIT(heth->Instance->MACRFCR, ETH_MACRFCR_RFE) > 0U) ? ENABLE
                                                                  : DISABLE;
    macconf->UnicastPausePacketDetect =
        ((READ_BIT(heth->Instance->MACRFCR, ETH_MACRFCR_UP) >> 1) > 0U)
            ? ENABLE
            : DISABLE;

    macconf->TransmitQueueMode = READ_BIT(
        heth->Instance->MTLTQOMR, (ETH_MTLTQOMR_TTC | ETH_MTLTQOMR_TSF));

    macconf->ReceiveQueueMode = READ_BIT(heth->Instance->MTLRQOMR,
                                         (ETH_MTLRQOMR_RTC | ETH_MTLRQOMR_RSF));
    macconf->ForwardRxUndersizedGoodPacket =
        ((READ_BIT(heth->Instance->MTLRQOMR, ETH_MTLRQOMR_FUP) >> 3) > 0U)
            ? ENABLE
            : DISABLE;
    macconf->ForwardRxErrorPacket =
        ((READ_BIT(heth->Instance->MTLRQOMR, ETH_MTLRQOMR_FEP) >> 4) > 0U)
            ? ENABLE
            : DISABLE;
    macconf->DropTCPIPChecksumErrorPacket =
        ((READ_BIT(heth->Instance->MTLRQOMR, ETH_MTLRQOMR_DISTCPEF) >> 6) == 0U)
            ? ENABLE
            : DISABLE;
}

void carbon_hw_ethernet_set_dma_config(Eth_Handle *heth,
                                       ETH_DMAConfigTypeDef *dmaconf) {
    uint32_t dmaregval;

    /*------------------------ DMAMR Configuration --------------------*/
    MODIFY_REG(heth->Instance->DMAMR, ETH_DMAMR_MASK, dmaconf->DMAArbitration);

    /*------------------------ DMASBMR Configuration --------------------*/
    dmaregval =
        (((uint32_t)dmaconf->AddressAlignedBeats << 12) | dmaconf->BurstMode |
         ((uint32_t)dmaconf->RebuildINCRxBurst << 15));

    MODIFY_REG(heth->Instance->DMASBMR, ETH_DMASBMR_MASK, dmaregval);

    /*------------------------ DMACCR Configuration --------------------*/
    dmaregval =
        (((uint32_t)dmaconf->PBLx8Mode << 16) | dmaconf->MaximumSegmentSize);

    MODIFY_REG(heth->Instance->DMACCR, ETH_DMACCR_MASK, dmaregval);

    /*------------------------ DMACTCR Configuration --------------------*/
    dmaregval = (dmaconf->TxDMABurstLength |
                 ((uint32_t)dmaconf->SecondPacketOperate << 4) |
                 ((uint32_t)dmaconf->TCPSegmentation << 12));

    MODIFY_REG(heth->Instance->DMACTCR, ETH_DMACTCR_MASK, dmaregval);

    /*------------------------ DMACRCR Configuration --------------------*/
    dmaregval =
        (((uint32_t)dmaconf->FlushRxPacket << 31) | dmaconf->RxDMABurstLength);

    /* Write to DMACRCR */
    MODIFY_REG(heth->Instance->DMACRCR, ETH_DMACRCR_MASK, dmaregval);
}

void carbon_hw_ethernet_rx_dec_list_init(Eth_Handle *heth) {
    ETH_DMADescTypeDef *dmarxdesc;
    uint32_t i;

    for (i = 0; i < ETH_RX_DESC_CNT; i++) {
        dmarxdesc = heth->Init.RxDesc + i;

        /* Set Rx descritors addresses */
        WRITE_REG(heth->rxDescList.rxDesc[i], (uint32_t)dmarxdesc);

        carbon_hw_ethernet_prepare_rx_desc(heth, dmarxdesc,
                                           (uint32_t)(&(rx_Buff[i][0])));
    }

    WRITE_REG(heth->rxDescList.toProcessPointer, (uint32_t)heth->Init.RxDesc);

    /* Set Receive Descriptor Ring Length */
    WRITE_REG(heth->Instance->DMACRDRLR, ((uint32_t)(ETH_RX_DESC_CNT - 1)));

    /* Set Receive Descriptor List Address */
    WRITE_REG(heth->Instance->DMACRDLAR, (uint32_t)heth->Init.RxDesc);

    /* Set Receive Descriptor Tail pointer Address */
    WRITE_REG(
        heth->Instance->DMACRDTPR,
        ((uint32_t)(heth->Init.RxDesc + (uint32_t)(ETH_RX_DESC_CNT - 1))));
}

void carbon_hw_ethernet_prepare_rx_desc(Eth_Handle *heth,
                                        ETH_DMADescTypeDef *dmarxdesc,
                                        uint32_t pBuffer1) {
    WRITE_REG(dmarxdesc->DESC0, pBuffer1);
    WRITE_REG(dmarxdesc->DESC1, 0x0);
    WRITE_REG(dmarxdesc->DESC2, 0x0);
    WRITE_REG(dmarxdesc->DESC3, ETH_DMARXNDESCRF_BUF1V | ETH_DMARXNDESCRF_IOC |
                                    ETH_DMARXNDESCRF_OWN);
    WRITE_REG(dmarxdesc->BackupAddr0, pBuffer1);
    WRITE_REG(dmarxdesc->BackupAddr1, 0x0);
}

void carbon_hw_ethernet_start(Eth_Handle *heth) {
    if (heth->gState == HAL_ETH_STATE_READY) {

        /* Enable the MAC transmission */
        SET_BIT(heth->Instance->MACCR, ETH_MACCR_TE);

        /* Enable the MAC reception */
        SET_BIT(heth->Instance->MACCR, ETH_MACCR_RE);

        /* Set the Flush Transmit FIFO bit */
        SET_BIT(heth->Instance->MTLTQOMR, ETH_MTLTQOMR_FTQ);

        /* Enable the DMA transmission */
        SET_BIT(heth->Instance->DMACTCR, ETH_DMACTCR_ST);

        /* Enable the DMA reception */
        SET_BIT(heth->Instance->DMACRCR, ETH_DMACRCR_SR);

        /* Clear Tx and Rx process stopped flags */
        heth->Instance->DMACSR |= (ETH_DMACSR_TPS | ETH_DMACSR_RPS);

        /* Enable ETH DMA interrupts:
        - Tx complete interrupt
        - Rx complete interrupt
        - Fatal bus interrupt
        */
        __HAL_ETH_DMA_ENABLE_IT(heth, (ETH_DMACIER_NIE | ETH_DMACIER_RIE |
                                       ETH_DMACIER_TIE | ETH_DMACIER_FBEE |
                                       ETH_DMACIER_AIE));

        heth->gState = HAL_ETH_STATE_BUSY;
        osMutexWait(tx_ptk_mutex, osWaitForever);
        heth->txState = HAL_ETH_STATE_BUSY_TX;
        osMutexRelease(tx_ptk_mutex);
        DIAG(ETH_DIAG "ethernet started");
    } else {
        DIAG(ETH_DIAG "ethernet in error state could not be started");
    }
}

void carbon_hw_ethernet_stop(Eth_Handle *heth) {
    // ETH_DMADescTypeDef *dmarxdesc;
    // uint32_t descindex;

    /* Disable interrupts:
    - Tx complete interrupt
    - Rx complete interrupt
    - Fatal bus interrupt
    */
    __HAL_ETH_DMA_DISABLE_IT(heth, (ETH_DMACIER_NIE | ETH_DMACIER_RIE |
                                    ETH_DMACIER_TIE | ETH_DMACIER_FBEE |
                                    ETH_DMACIER_AIE));

    /* Disable the DMA transmission */
    CLEAR_BIT(heth->Instance->DMACTCR, ETH_DMACTCR_ST);

    /* Disable the DMA reception */
    CLEAR_BIT(heth->Instance->DMACRCR, ETH_DMACRCR_SR);

    /* Disable the MAC reception */
    CLEAR_BIT(heth->Instance->MACCR, ETH_MACCR_RE);

    /* Set the Flush Transmit FIFO bit */
    SET_BIT(heth->Instance->MTLTQOMR, ETH_MTLTQOMR_FTQ);

    /* Disable the MAC transmission */
    CLEAR_BIT(heth->Instance->MACCR, ETH_MACCR_TE);

    /* Clear IOC bit to all Rx descriptors */
    // for (descindex = 0; descindex < (uint32_t)ETH_RX_DESC_CNT; descindex++) {
    //     dmarxdesc = (ETH_DMADescTypeDef *)heth->rxDescList.rxDesc[descindex];
    //     CLEAR_BIT(dmarxdesc->DESC3, ETH_DMARXNDESCRF_IOC);
    // }

    heth->gState = HAL_ETH_STATE_READY;
    osMutexWait(tx_ptk_mutex, osWaitForever);
    heth->txState = HAL_ETH_STATE_READY;
    osMutexRelease(tx_ptk_mutex);
}

/*******************************************************************************
                       PHI IO Functions
 *******************************************************************************/

int32_t carbon_hw_ethernet_phy_io_init(void) {
    /* We assume that MDIO GPIO configuration is already done
       in the ETH_MspInit() else it should be done here*/

    /* Configure the MDIO Clock */
    carbon_hw_ethernet_mac_mdio_clkconfig(&eth_handle);

    return 0;
}

int32_t carbon_hw_ethernet_phy_io_deinit(void) { return 0; }

int32_t carbon_hw_ethernet_phy_io_read_reg(uint32_t PHYAddr, uint32_t PHYReg,
                                           uint32_t *pRegValue) {
    uint32_t tmpreg, tickstart;

    /* Check for the Busy flag */
    if (READ_BIT(eth_handle.Instance->MACMDIOAR, ETH_MACMDIOAR_MB) != 0U)
        return -1;

    /* Get the  MACMDIOAR value */
    WRITE_REG(tmpreg, eth_handle.Instance->MACMDIOAR);

    /* Prepare the MDIO Address Register value
       - Set the PHY device address
       - Set the PHY register address
       - Set the read mode
       - Set the MII Busy bit */

    MODIFY_REG(tmpreg, ETH_MACMDIOAR_PA, (PHYAddr << 21));
    MODIFY_REG(tmpreg, ETH_MACMDIOAR_RDA, (PHYReg << 16));
    MODIFY_REG(tmpreg, ETH_MACMDIOAR_MOC, ETH_MACMDIOAR_MOC_RD);
    SET_BIT(tmpreg, ETH_MACMDIOAR_MB);

    /* Write the result value into the MDII Address register */
    WRITE_REG(eth_handle.Instance->MACMDIOAR, tmpreg);

    tickstart = HAL_GetTick();

    /* Wait for the Busy flag */
    while (READ_BIT(eth_handle.Instance->MACMDIOAR, ETH_MACMDIOAR_MB) > 0U) {
        if (((HAL_GetTick() - tickstart) > ETH_MDIO_BUS_TIMEOUT))
            return -1;
    }

    /* Get MACMIIDR value */
    WRITE_REG(*pRegValue, (uint16_t)eth_handle.Instance->MACMDIODR);

    return 0;
}

int32_t carbon_hw_ethernet_phy_io_write_reg(uint32_t PHYAddr, uint32_t PHYReg,
                                            uint32_t RegValue) {
    uint32_t tmpreg, tickstart;

    /* Check for the Busy flag */
    if (READ_BIT(eth_handle.Instance->MACMDIOAR, ETH_MACMDIOAR_MB) != 0U)
        return -1;

    /* Get the  MACMDIOAR value */
    WRITE_REG(tmpreg, eth_handle.Instance->MACMDIOAR);

    /* Prepare the MDIO Address Register value
       - Set the PHY device address
       - Set the PHY register address
       - Set the write mode
       - Set the MII Busy bit */

    MODIFY_REG(tmpreg, ETH_MACMDIOAR_PA, (PHYAddr << 21));
    MODIFY_REG(tmpreg, ETH_MACMDIOAR_RDA, (PHYReg << 16));
    MODIFY_REG(tmpreg, ETH_MACMDIOAR_MOC, ETH_MACMDIOAR_MOC_WR);
    SET_BIT(tmpreg, ETH_MACMDIOAR_MB);

    /* Give the value to the MII data register */
    WRITE_REG(ETH->MACMDIODR, (uint16_t)RegValue);

    /* Write the result value into the MII Address register */
    WRITE_REG(ETH->MACMDIOAR, tmpreg);

    tickstart = HAL_GetTick();

    /* Wait for the Busy flag */
    while (READ_BIT(eth_handle.Instance->MACMDIOAR, ETH_MACMDIOAR_MB) > 0U) {
        if (((HAL_GetTick() - tickstart) > ETH_MDIO_BUS_TIMEOUT))
            return -1;
    }

    return 0;
}

int32_t carbon_hw_ethernet_phy_io_get_tick(void) { return HAL_GetTick(); }

/*******************************************************************************
                       LL Driver Interface ( LwIP stack --> ETH)
 *******************************************************************************/
static err_t carbon_lwip_output(struct netif *netif, struct pbuf *p);

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

u32_t sys_jiffies(void) { return HAL_GetTick(); }

u32_t sys_now(void) { return HAL_GetTick(); }

void pbuf_free_custom(struct pbuf *p) {
    struct carbon_pbuf_custom *custom_pbuf = (struct carbon_pbuf_custom *)p;
    uint32_t buffer = (uint32_t)custom_pbuf->buffer;
    osMutexWait(rx_ptk_mutex, osWaitForever);
    carbon_lwip_prepare_rx_descriptor(buffer);
    osMutexRelease(rx_ptk_mutex);
    LWIP_MEMPOOL_FREE(rx_pool, custom_pbuf);
}

err_t carbon_lwip_init(struct netif *netif) {

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
    /* The user should write its own code in low_level_output_arp_off
     * function
     */
    netif->output = low_level_output_arp_off;
#endif /* LWIP_ARP */
#endif /* LWIP_ARP || LWIP_ETHERNET */
#endif /* LWIP_IPV4 */

#if LWIP_IPV6
    netif->output_ip6 = ethip6_output;
#endif /* LWIP_IPV6 */

    netif->linkoutput = carbon_lwip_output;

    /* initialize the hardware */
    if (carbon_hw_ethernet_low_level_init(netif) != 0) {
        DIAG(ETH_DIAG "low level init failed");
        return ERR_IF;
    }

    return ERR_OK;
}

void carbon_lwip_link_thread(void const *argument) {
    ETH_MACConfigTypeDef macConf;
    int32_t phyLinkState;
    uint32_t linkchanged = 0, speed = 0, duplex = 0;

    struct netif *netif = (struct netif *)argument;

    for (;;) {
        if (!isInitialized)
            continue;
        phyLinkState = LAN8742_GetLinkState(&lan8742);
        if (netif_is_link_up(netif) &&
            (phyLinkState <= LAN8742_STATUS_LINK_DOWN)) {
            netif_set_down(netif);
            netif_set_link_down(netif);
            carbon_hw_ethernet_stop(&eth_handle);
        } else if (!netif_is_link_up(netif) &&
                   (phyLinkState > LAN8742_STATUS_LINK_DOWN)) {
            switch (phyLinkState) {
            case LAN8742_STATUS_100MBITS_FULLDUPLEX:
                duplex = ETH_FULLDUPLEX_MODE;
                speed = ETH_SPEED_100M;
                DIAG(ETH_DIAG "full duplex, 100 Mbit/sec");
                linkchanged = 1;
                break;
            case LAN8742_STATUS_100MBITS_HALFDUPLEX:
                duplex = ETH_HALFDUPLEX_MODE;
                speed = ETH_SPEED_100M;
                DIAG(ETH_DIAG "half duplex, 100 Mbit/sec");
                linkchanged = 1;
                break;
            case LAN8742_STATUS_10MBITS_FULLDUPLEX:
                duplex = ETH_FULLDUPLEX_MODE;
                speed = ETH_SPEED_10M;
                DIAG(ETH_DIAG "full duplex, 10 Mbit/sec");
                linkchanged = 1;
                break;
            case LAN8742_STATUS_10MBITS_HALFDUPLEX:
                duplex = ETH_HALFDUPLEX_MODE;
                speed = ETH_SPEED_10M;
                DIAG(ETH_DIAG "half duplex, 100 Mbit/sec");
                linkchanged = 1;
                break;
            default:
                DIAG(ETH_DIAG "no configuration found");
                break;
            }

            if (linkchanged) {
                /* Get MAC Config MAC */
                carbon_hw_ethernet_get_mac_config(&eth_handle, &macConf);
                macConf.DuplexMode = duplex;
                macConf.Speed = speed;
                carbon_hw_ethernet_set_mac_config(&eth_handle, &macConf);
                carbon_hw_ethernet_tx_dec_list_init(&eth_handle);
                carbon_hw_ethernet_start(&eth_handle);
                netif_set_up(netif);
                netif_set_link_up(netif);
            }
        }

        osDelay(100);
    }
}

void carbon_lwip_input(const void *argument) {
    struct pbuf *p;
    struct netif *netif = (struct netif *)argument;

    for (;;) {
        if (osSemaphoreWait(rxPktSemaphore, osWaitForever) != osOK)
            continue;
        // DIAG(ETH_DIAG "");
        while (1) {
            p = carbon_lwip_low_level_input(netif);
            if (p != NULL) {
                // DIAG(ETH_DIAG "pushing buffer %p", p->payload);
                err_t err = netif->input(p, netif);
                if (err == ERR_OK)
                    continue;
                if (err == ERR_MEM) {
                    DIAG(ETH_DIAG "warning, out of memory, pbuf %p, payload %p",
                         p, p->payload);

                } else {
                    DIAG(ETH_DIAG "error %d pushing pbuf %p, payload %p", err,
                         p, p->payload);
                }
                pbuf_free(p);
            } else
                break;
        }
    }
}

#define DISCARD_DESCRIPTOR(descr_pointer, bufferPtr)                           \
    osMutexWait(rx_ptk_mutex, osWaitForever);                                  \
    INCREASE_RX_POINTER(descr_pointer);                                        \
    eth_handle.rxDescList.toProcessPointer = (uint32_t)descr_pointer;          \
    carbon_lwip_prepare_rx_descriptor(bufferPtr);                              \
    osMutexRelease(rx_ptk_mutex);

struct pbuf *carbon_lwip_low_level_input(struct netif *netif) {
    struct pbuf *p = NULL;
    struct carbon_pbuf_custom *custom_pbuf;

    ETH_DMADescTypeDef *rx_desc_to_process =
        (ETH_DMADescTypeDef *)eth_handle.rxDescList.toProcessPointer;
    uint32_t bufferPtr = READ_REG(rx_desc_to_process->DESC0);
    uint32_t bufferLength =
        READ_BIT(rx_desc_to_process->DESC3, ETH_DMARXNDESCWBF_PL);
    ETH_DMADescTypeDef *current_dma_pointer =
        (ETH_DMADescTypeDef *)READ_REG(eth_handle.Instance->DMACCARDR);

    // DIAG(ETH_DIAG "bufferPtr %p, lenght %lu, descr %p, current ptr %p",
    //      (uint32_t *)bufferPtr, bufferLength, rx_desc_to_process,
    //      current_dma_pointer);

    if (rx_desc_to_process == current_dma_pointer) {
        // DIAG(ETH_DIAG "rx_desc_to_process == DMACCARDR");
        return p;
    }

    if (READ_BIT(rx_desc_to_process->DESC3, ETH_DMARXNDESCWBF_OWN) !=
        (uint32_t)RESET) {
        DIAG(ETH_DIAG "process does not own the descr");
        return p;
    }

    if (READ_BIT(rx_desc_to_process->DESC3, ETH_DMARXNDESCWBF_CTXT) !=
        (uint32_t)RESET) {
        DIAG(ETH_DIAG "discarding context descriptor");
        DISCARD_DESCRIPTOR(rx_desc_to_process, bufferPtr);
        return p;
    }
    if (READ_BIT(rx_desc_to_process->DESC3, ETH_DMARXNDESCWBF_FD) ==
        (uint32_t)RESET) {
        DIAG(ETH_DIAG "not first descriptor %lu", rx_desc_to_process->DESC3);
        DISCARD_DESCRIPTOR(rx_desc_to_process, bufferPtr);
        return p;
    }
    if (READ_BIT(rx_desc_to_process->DESC3, ETH_DMARXNDESCWBF_LD) ==
        (uint32_t)RESET) {
        DIAG(ETH_DIAG "not last descriptor %lu", rx_desc_to_process->DESC3);
        DISCARD_DESCRIPTOR(rx_desc_to_process, bufferPtr);
        return p;
    }
    if (bufferLength > (ETH_RX_BUFFER_SIZE_ALIGNED)) {
        DIAG(ETH_DIAG "bufferLength %lu > ETH_RX_BUFFER_SIZE_ALIGNED",
             bufferLength);
        DISCARD_DESCRIPTOR(rx_desc_to_process, bufferPtr);
        return p;
    }

    custom_pbuf = (struct carbon_pbuf_custom *)LWIP_MEMPOOL_ALLOC(rx_pool);

    if (custom_pbuf == NULL) {
        DIAG(ETH_DIAG "low level input: cannot allocate pbuf");
        return p;
    }

    SCB_InvalidateDCache_by_Addr((uint32_t *)bufferPtr,
                                 ETH_RX_BUFFER_SIZE_ALIGNED);

    pbuf_alloced_custom(PBUF_RAW, bufferLength, PBUF_REF,
                        ((struct pbuf_custom *)custom_pbuf),
                        (uint8_t *)bufferPtr, ETH_RX_BUFFER_SIZE_ALIGNED);

    ((struct pbuf_custom *)custom_pbuf)->custom_free_function =
        pbuf_free_custom;
    custom_pbuf->buffer = (uint8_t *)bufferPtr;

    p = (struct pbuf *)custom_pbuf;

    // osMutexWait(rx_ptk_mutex, osWaitForever);
    INCREASE_RX_POINTER(rx_desc_to_process);
    eth_handle.rxDescList.toProcessPointer = (uint32_t)rx_desc_to_process;
    // osMutexRelease(rx_ptk_mutex);

    // DIAG(ETH_DIAG "new descr %p", rx_desc_to_process);

    return p;
}

void carbon_lwip_prepare_rx_descriptor(uint32_t bufferPtr) {
    ETH_DMADescTypeDef *tail_pointer =
        (ETH_DMADescTypeDef *)READ_REG(eth_handle.Instance->DMACRDTPR);
    ETH_DMADescTypeDef *tail_pointer_increased = tail_pointer;
    INCREASE_RX_POINTER(tail_pointer_increased);
    ETH_DMADescTypeDef *rx_desc_to_process =
        (ETH_DMADescTypeDef *)eth_handle.rxDescList.toProcessPointer;
    ETH_DMADescTypeDef *current_dma_pointer =
        (ETH_DMADescTypeDef *)READ_REG(eth_handle.Instance->DMACCARDR);
    if (current_dma_pointer == tail_pointer_increased) {
        DIAG(ETH_DIAG
             "critical error: tail pointer increased == current pointer");
        osDelay(100);
        Error_Handler();
    }
    if (rx_desc_to_process == tail_pointer_increased) {
        DIAG(ETH_DIAG "critical error: tail pointer == rx_desc_to_process");
        osDelay(100);
        Error_Handler();
    }
    carbon_hw_ethernet_prepare_rx_desc(&eth_handle, tail_pointer, bufferPtr);

    /*issue a DMA transfer writing the new tail pointer */
    WRITE_REG(eth_handle.Instance->DMACRDTPR, (uint32_t)tail_pointer_increased);

    // DIAG(ETH_DIAG "");
    // DIAG(ETH_DIAG "tail pointer %p, current pointer %p, pointer to "
    //               "process %p, buffer %p",
    //      tail_pointer_increased, current_dma_pointer, rx_desc_to_process,
    //      (uint32_t *)bufferPtr);
    // for (unsigned i = 0; i < ETH_RX_DESC_CNT; i++) {
    //     ETH_DMADescTypeDef *descr_ptr = &(dmaRxDscrTab[i]);
    //     DIAG(ETH_DIAG "descr %p, buffer %p, DESC3 %lu", descr_ptr,
    //          (uint32_t *)descr_ptr->DESC0, descr_ptr->DESC3);
    // }
}

err_t carbon_lwip_output(struct netif *netif, struct pbuf *p) {
    if (!isInitialized)
        return ERR_IF;
    struct pbuf *q;

    osMutexWait(tx_ptk_mutex, osWaitForever);

    if (eth_handle.txState != HAL_ETH_STATE_BUSY_TX) {
        osMutexRelease(tx_ptk_mutex);
        return ERR_IF;
    }

    uint32_t size = 0;
    uint32_t total_size = p->tot_len;
    uint8_t *current_buf_ptr = (uint8_t *)&tx_Buff[0];

    if (total_size > (ETH_RX_BUFFER_SIZE_ALIGNED)) {
        DIAG(ETH_DIAG "tx_Buff too small, data lenght %lu", p->tot_len);
        osMutexRelease(tx_ptk_mutex);
        return ERR_IF;
    }

    for (q = p; q != NULL; q = q->next) {
        if (size >= total_size) {
            DIAG(ETH_DIAG "p->tot_len %lu not equal to actual data size %lu ",
                 total_size, size);
            osMutexRelease(tx_ptk_mutex);
            return ERR_IF;
        }

        memcpy(current_buf_ptr, q->payload, q->len);

        current_buf_ptr += q->len;
        size += q->len;
    }

    SCB_CleanDCache_by_Addr((uint32_t *)&tx_Buff[0],
                            ETH_RX_BUFFER_SIZE_ALIGNED);

    ETH_DMADescTypeDef *tail_pointer =
        (ETH_DMADescTypeDef *)READ_REG(eth_handle.Instance->DMACTDTPR);

    if ((READ_BIT(tail_pointer->DESC3, ETH_DMATXNDESCWBF_OWN) ==
         ETH_DMATXNDESCWBF_OWN)) {
        DIAG(ETH_DIAG "tail pointer owned by the DMA");
        osMutexRelease(tx_ptk_mutex);
        return ERR_IF;
    }

    /* Set header or buffer 1 address */
    WRITE_REG(tail_pointer->DESC0, (uint32_t)&tx_Buff[0]);
    /* Set header or buffer 1 Length */
    MODIFY_REG(tail_pointer->DESC2, ETH_DMATXNDESCRF_B1L, total_size);
    /* Mark it as First Descriptor */
    SET_BIT(tail_pointer->DESC3, ETH_DMATXNDESCRF_FD);
    /* Mark it as NORMAL descriptor */
    CLEAR_BIT(tail_pointer->DESC3, ETH_DMATXNDESCRF_CTXT);
    /* set OWN bit of FIRST descriptor */
    SET_BIT(tail_pointer->DESC3, ETH_DMATXNDESCRF_OWN);
    /* Set Interrupt on completion bit */
    SET_BIT(tail_pointer->DESC2, ETH_DMATXNDESCRF_IOC);
    /* Mark it as LAST descriptor */
    SET_BIT(tail_pointer->DESC3, ETH_DMATXNDESCRF_LD);

    // uint32_t descriptor_to_check = tail_pointer;

    // DIAG(ETH_DIAG "sending data %p, size %lu", tail_pointer, total_size);

    INCREASE_TX_POINTER(tail_pointer);

    /*issue a DMA transfer writing the new tail pointer */
    WRITE_REG(eth_handle.Instance->DMACTDTPR, (uint32_t)tail_pointer);

    if (osSemaphoreWait(txPktSemaphore, ETH_DMA_TRANSMIT_TIMEOUT) != osOK) {
        DIAG(ETH_DIAG "error sending data");
        osMutexRelease(tx_ptk_mutex);
        return ERR_IF;
    }

    osMutexRelease(tx_ptk_mutex);
    return ERR_OK;
}