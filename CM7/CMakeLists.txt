cmake_minimum_required(VERSION 3.16)

get_filename_component(PROJECT_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

set(MAIN_DIR ${CMAKE_CURRENT_LIST_DIR})

set(CURRENT_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR})

message(${CURRENT_BUILD_DIR})

set(CMAKE_TOOLCHAIN_FILE ${PROJECT_ROOT_DIR}/common/cmake/toolchain.cmake)

project(CARBON_CM7)

set(FLASH_ADDRESS 0x08000000)

set(ARM_FLAGS
    -DCORE_CM7
    -mcpu=cortex-m7
    -mthumb
    -mfpu=fpv5-d16
    -mfloat-abi=hard
    -mno-unaligned-access
)

set(CORE_CM7 ON)

set(LDSCRIPT
    ${PROJECT_ROOT_DIR}/CM7/STM32H747XIHx_FLASH_CM7.ld
)

include_directories(${CMAKE_CURRENT_LIST_DIR}/conf)

include(${PROJECT_ROOT_DIR}/common/cmake/common.cmake)

if(FIFO_TEST)
    SET(SOURCE_MAIN ${CMAKE_CURRENT_LIST_DIR}/test/src/main_fifo.cpp)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DFIFO_TEST")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DFIFO_TEST")
    message("testing fifo interface")
elseif(HSEM_TEST)
    SET(SOURCE_MAIN ${CMAKE_CURRENT_LIST_DIR}/test/src/main_hsem.cpp)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DHSEM_TEST")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DHSEM_TEST")
    message("testing hsem interface")
elseif(SD_TEST)
    SET(SOURCE_MAIN 
    ${CMAKE_CURRENT_LIST_DIR}/core/src/main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/test/src/main_thread_sd.cpp
    ${CMAKE_CURRENT_LIST_DIR}/test/src/sd_thread.c
    ${CMAKE_CURRENT_LIST_DIR}/test/src/sd_thread.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/src/app_ethernet.c
    ${CMAKE_CURRENT_LIST_DIR}/core/src/ethernetif.c
    ${CMAKE_CURRENT_LIST_DIR}/core/src/lan8742.c
    ${CMAKE_CURRENT_LIST_DIR}/core/src/netif_conf.c)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSD_TEST")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSD_TEST")
    message("testing sd interface")
elseif(SDRAM_TEST)
    SET(SOURCE_MAIN ${CMAKE_CURRENT_LIST_DIR}/test/src/main_sdram.cpp)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSDRAM_TEST")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSDRAM_TEST") 
    message("testing sdram")
else()
    SET(SOURCE_MAIN 
    ${CMAKE_CURRENT_LIST_DIR}/core/src/main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/src/main_thread.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/src/sd_thread.c
    ${CMAKE_CURRENT_LIST_DIR}/core/src/sd_thread.cpp 
    ${CMAKE_CURRENT_LIST_DIR}/core/src/app_ethernet.c
    ${CMAKE_CURRENT_LIST_DIR}/core/src/ethernetif.c
    ${CMAKE_CURRENT_LIST_DIR}/core/src/lan8742.c
    ${CMAKE_CURRENT_LIST_DIR}/core/src/netif_conf.c
    ${CMAKE_CURRENT_LIST_DIR}/core/src/mp_thread.c)
endif(FIFO_TEST)

SET(SOURCE
    ${CMAKE_CURRENT_LIST_DIR}/core/src/hsem.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/src/low_level_init.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/src/sd_card.c
    ${CMAKE_CURRENT_LIST_DIR}/core/src/msp.c
    ${CMAKE_CURRENT_LIST_DIR}/core/src/interrupts.c
    ${CMAKE_CURRENT_LIST_DIR}/core/src/trace.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/src/trace_thread.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/src/trace_thread.c
    ${CMAKE_CURRENT_LIST_DIR}/core/src/uart.cpp
)

add_subdirectory(${PROJECT_ROOT_DIR}/lib/lwip lwip)
add_subdirectory(${PROJECT_ROOT_DIR}/lib/micropython micropython)
add_subdirectory(${PROJECT_ROOT_DIR}/lib/fatfs fatfs)

SET(STARTUP_SOURCE
    ${CMAKE_CURRENT_LIST_DIR}/startup/startup_stm32h747xihx.s
)

add_executable(${PROJECT_NAME} ${STARTUP_SOURCE} ${SOURCE_MAIN} ${SOURCE} ${COMMON_SOURCE})

target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${TARGET_INCLUDE}
)

target_link_libraries(${PROJECT_NAME} 
    cmsis_${PROJECT_NAME} 
    hal_${PROJECT_NAME} 
    printf_${PROJECT_NAME} 
    freertos_${PROJECT_NAME} 
    lwip_${PROJECT_NAME} 
    backtrace_${PROJECT_NAME} 
    micropython_${PROJECT_NAME}
    fatfs_${PROJECT_NAME}
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${ARM_SIZE_UTIL} ${PROJECT_NAME}.elf
    COMMAND ${ARM_OBJCOPY} -O binary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin
    )

add_custom_target(flash_ocd)

add_dependencies(flash_ocd ${PROJECT_NAME})

add_custom_command(
    TARGET flash_ocd
    COMMAND ${OPENOCD} -f board/stm32h747i-disco.cfg -c "program ${PROJECT_NAME}.bin reset exit ${FLASH_ADDRESS}"
 	)