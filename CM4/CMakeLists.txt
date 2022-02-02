cmake_minimum_required(VERSION 3.16)

get_filename_component(PROJECT_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

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

set(LDSCRIPT
    ${PROJECT_ROOT_DIR}/CM4/STM32H747XIHX_FLASH.ld
)

include(${PROJECT_ROOT_DIR}/common/cmake/common.cmake)

SET(TARGET_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/core/include)
SET(TARGET_INCLUDE_CONF ${CMAKE_CURRENT_LIST_DIR}/core/include)

add_subdirectory(${PROJECT_ROOT_DIR}/lib/CMSIS cmsis)
add_subdirectory(${PROJECT_ROOT_DIR}/lib/hal hal)

SET(SOURCE
    ${CMAKE_CURRENT_LIST_DIR}/core/src/main.c
    ${CMAKE_CURRENT_LIST_DIR}/core/src/stm32h7xx_hal_msp.c
    ${CMAKE_CURRENT_LIST_DIR}/core/src/stm32h7xx_it.c
)

SET(STARTUP_SOURCE
    ${CMAKE_CURRENT_LIST_DIR}/startup/startup_stm32h747xihx.s
)

add_executable(${PROJECT_NAME} ${STARTUP_SOURCE} ${SOURCE} ${COMMON_SOURCE})

target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${TARGET_INCLUDE}
    ${TARGET_INCLUDE_CONF}
)

target_link_libraries(${PROJECT_NAME} cmsis_${PROJECT_NAME} hal_${PROJECT_NAME})




