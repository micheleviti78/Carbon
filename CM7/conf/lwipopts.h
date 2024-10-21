// clang-format off

#ifndef MWI_LWIPOPTS_H
#define MWI_LWIPOPTS_H

#define ETH_RX_BUFFER_SIZE          1528

#define WITH_RTOS                   1
#define NO_SYS                      0
#define LWIP_NETIF_API              1
#define MEM_ALIGNMENT               4
#define MEM_SIZE                    (50*1024)
#define MEMP_NUM_PBUF               32
#define MEMP_NUM_RAW_PCB            8
#define MEMP_NUM_UDP_PCB            8
#define MEMP_NUM_TCP_PCB            10
#define MEMP_NUM_TCP_PCB_LISTEN     16
#define MEMP_NUM_TCP_SEG            32
#define MEMP_NUM_REASSDATA          10
#define MEMP_NUM_FRAG_PBUF          30
//#define MEMP_NUM_ARP_QUEUE          30
//#define MEMP_NUM_IGMP_GROUP         8
#define MEMP_NUM_NETBUF             4
#define MEMP_NUM_NETCONN            8
#define MEMP_NUM_TCPIP_MSG_API      16
#define MEMP_NUM_TCPIP_MSG_INPKT    16
#define MEMP_NUM_SYS_TIMEOUT        30
//#define MEMP_NUM_NETDB              1
//#define PBUF_POOL_SIZE              16
//#define PBUF_POOL_BUFSIZE           1528
#define LWIP_SUPPORT_CUSTOM_PBUF    1
#define LWIP_IPV4                   1
#define LWIP_TCP                    1
#define TCP_QUEUE_OOSEQ             0
#define TCP_MSS                     (1500 - 40)	  // TCP_MSS = (Ethernet MTU - IP header size - TCP header size)
#define TCP_SND_BUF                 (4*TCP_MSS)
#define TCP_SND_QUEUELEN            (2*TCP_SND_BUF/TCP_MSS)
#define TCP_WND                     (2*TCP_MSS)
#define ARP_QUEUEING                1
#define LWIP_ICMP                   1
#define LWIP_UDP                    1
#define LWIP_IGMP                   1
#define LWIP_AUTOIP                 1
#define LWIP_DHCP                   1
#define LWIP_DHCP_AUTOIP_COOP       1
#define LWIP_DHCP_AUTOIP_COOP_TRIES 4
#define LWIP_MDNS_RESPONDER         1
#define LWIP_NETIF_LINK_CALLBACK    1
#define LWIP_NETIF_STATUS_CALLBACK  1
#define LWIP_NUM_NETIF_CLIENT_DATA  1
#define LWIP_STATS                  0
#define MDNS_MAX_SERVICES           2
#define LWIP_TCP_KEEPALIVE          1

//#define CHECKSUM_BY_HARDWARE

#ifdef CHECKSUM_BY_HARDWARE
  #define CHECKSUM_GEN_IP           0
  #define CHECKSUM_GEN_UDP          1
  #define CHECKSUM_GEN_TCP          1
  #define CHECKSUM_GEN_ICMP         1
  #define CHECKSUM_CHECK_IP         0
  #define CHECKSUM_CHECK_UDP        0
  #define CHECKSUM_CHECK_TCP        0
#else
  #define CHECKSUM_GEN_IP           1
  #define CHECKSUM_GEN_UDP          1
  #define CHECKSUM_GEN_TCP          1
  #define CHECKSUM_GEN_ICMP         1
  #define CHECKSUM_CHECK_IP         1
  #define CHECKSUM_CHECK_UDP        1
  #define CHECKSUM_CHECK_TCP        1
#endif

#define LWIP_NETCONN                1
#define LWIP_NETCONN_FULLDUPLEX     1
#define LWIP_SO_RCVTIMEO            1
#define LWIP_SOCKET                 1
#define LWIP_COMPAT_SOCKETS         0
#define LWIP_DNS                    1
#define SO_REUSE                    1

#define TCPIP_THREAD_NAME           "TCP/IP"
#define TCPIP_THREAD_STACKSIZE      1000
#define TCPIP_MBOX_SIZE             6
#define DEFAULT_UDP_RECVMBOX_SIZE   6
#define DEFAULT_TCP_RECVMBOX_SIZE   6
#define DEFAULT_ACCEPTMBOX_SIZE     6
#define DEFAULT_THREAD_STACKSIZE    500
#define TCPIP_THREAD_PRIO           osPriorityAboveNormal

//#define LWIP_NOASSERT               1

#define LWIP_DEBUG                  0
#define ETHARP_DEBUG                LWIP_DBG_OFF
#define NETIF_DEBUG                 LWIP_DBG_OFF
#define PBUF_DEBUG                  LWIP_DBG_OFF
#define API_LIB_DEBUG               LWIP_DBG_OFF
#define API_MSG_DEBUG               LWIP_DBG_OFF
#define SOCKETS_DEBUG               LWIP_DBG_OFF
#define ICMP_DEBUG                  LWIP_DBG_OFF
#define IGMP_DEBUG                  LWIP_DBG_OFF
#define INET_DEBUG                  LWIP_DBG_OFF
#define IP_DEBUG                    LWIP_DBG_OFF
#define IP_REASS_DEBUG              LWIP_DBG_OFF
#define RAW_DEBUG                   LWIP_DBG_OFF
#define MEM_DEBUG                   LWIP_DBG_OFF
#define MEMP_DEBUG                  LWIP_DBG_OFF
#define SYS_DEBUG                   LWIP_DBG_OFF
#define TIMERS_DEBUG                LWIP_DBG_OFF
#define TCP_DEBUG                   LWIP_DBG_OFF
#define TCP_INPUT_DEBUG             LWIP_DBG_OFF
#define TCP_FR_DEBUG                LWIP_DBG_OFF
#define TCP_RTO_DEBUG               LWIP_DBG_OFF
#define TCP_CWND_DEBUG              LWIP_DBG_OFF
#define TCP_WND_DEBUG               LWIP_DBG_OFF
#define TCP_OUTPUT_DEBUG            LWIP_DBG_OFF
#define TCP_RST_DEBUG               LWIP_DBG_OFF
#define TCP_QLEN_DEBUG              LWIP_DBG_OFF
#define UDP_DEBUG                   LWIP_DBG_OFF
#define TCPIP_DEBUG                 LWIP_DBG_OFF
#define SLIP_DEBUG                  LWIP_DBG_OFF
#define DHCP_DEBUG                  LWIP_DBG_OFF
#define AUTOIP_DEBUG                LWIP_DBG_OFF
#define DNS_DEBUG                   LWIP_DBG_OFF
#define IP6_DEBUG                   LWIP_DBG_OFF

#endif // MWI_LWIPOPTS_H