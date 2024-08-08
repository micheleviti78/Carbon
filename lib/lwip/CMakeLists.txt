set(SOURCES
	src/api/if_api.c
    src/api/api_lib.c
    src/api/api_msg.c
    src/api/err.c
    src/api/netbuf.c
    src/api/netdb.c
    src/api/netifapi.c
    src/api/sockets.c
    src/api/tcpip.c
    src/netif/ethernet.c
	src/core/altcp.c
	src/core/altcp_alloc.c
	src/core/altcp_tcp.c
    src/core/init.c
    src/core/ip.c
    src/core/mem.c
    src/core/netif.c
    src/core/raw.c
    src/core/stats.c
    src/core/sys.c
    src/core/tcp.c
    src/core/tcp_in.c
    src/core/tcp_out.c
    src/core/timeouts.c
    src/core/udp.c
    src/core/ipv4/ip4_addr.c
    src/core/ipv4/dhcp.c
    src/core/ipv4/ip4_frag.c
    src/core/ipv4/icmp.c
    src/core/ipv4/ip4.c
    src/core/ipv4/igmp.c
    src/core/ipv4/autoip.c
    src/core/ipv4/etharp.c
    src/core/ipv6/inet6.c
    src/core/ipv6/ip6_addr.c
    src/core/ipv6/ethip6.c
    src/core/ipv6/mld6.c
    src/core/ipv6/ip6_frag.c
    src/core/ipv6/nd6.c
    src/core/ipv6/dhcp6.c
    src/core/ipv6/ip6.c
    src/core/ipv6/icmp6.c
    src/core/def.c
    src/core/memp.c
    src/core/pbuf.c
    src/core/inet_chksum.c
    src/core/dns.c
    system/OS/sys_arch.c
    #system/arch/lwip_checksum.c
    #system/arch/lwip_memcpy.c
    #src/apps/mdns/mdns.c
    #src/apps/lwiperf/lwiperf.c
)

set(LWIP_LIB lwip_${PROJECT_NAME})

add_library(${LWIP_LIB} ${SOURCES})

target_include_directories(${LWIP_LIB}
    SYSTEM
    PUBLIC
    src/include
    system/
    ${PROJECT_CONFIG_DIR}
)

if (DEBUG_LWIP)
    target_compile_definitions(${LWIP_LIB} PRIVATE -DLWIP_DEBUG)
endif (DEBUG_LWIP)

target_compile_options(${LWIP_LIB}
    PRIVATE
    -Wformat=0
    -Wno-unused-variable
)

target_link_libraries(${LWIP_LIB} freertos_${PROJECT_NAME})
