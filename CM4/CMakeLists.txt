cmake_minimum_required(VERSION 3.16)

get_filename_component(PROJECT_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

set(MAIN_DIR ${CMAKE_CURRENT_LIST_DIR})

set(CMAKE_TOOLCHAIN_FILE ${PROJECT_ROOT_DIR}/common/cmake/toolchain.cmake)

project(CARBON_CM4)

set(FLASH_ADDRESS 0x08100000)

set(ARM_FLAGS
    -DCORE_CM4
    -mcpu=cortex-m4
    -mthumb
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
)

set(CORE_CM4 ON)

set(LDSCRIPT
    ${PROJECT_ROOT_DIR}/CM4/STM32H747XIHx_FLASH_CM4.ld
)

include_directories(${CMAKE_CURRENT_LIST_DIR}/conf)

include(${PROJECT_ROOT_DIR}/common/cmake/common.cmake)

SET(SOURCE
    ${CMAKE_CURRENT_LIST_DIR}/core/src/hsem.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/src/low_level_init.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/src/main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/src/msp.c
    ${CMAKE_CURRENT_LIST_DIR}/core/src/interrupts.c
    ${CMAKE_CURRENT_LIST_DIR}/core/src/uart.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/src/matrix_display_spi.cpp
)

SET(STARTUP_SOURCE
    ${CMAKE_CURRENT_LIST_DIR}/startup/startup_stm32h747xihx.s
)

add_executable(${PROJECT_NAME} ${STARTUP_SOURCE} ${SOURCE} ${COMMON_SOURCE})

target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${TARGET_INCLUDE}
)

target_link_libraries(${PROJECT_NAME} cmsis_${PROJECT_NAME} hal_${PROJECT_NAME} printf_${PROJECT_NAME} freertos_${PROJECT_NAME} backtrace_${PROJECT_NAME})

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




