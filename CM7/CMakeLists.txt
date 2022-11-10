cmake_minimum_required(VERSION 3.16)

get_filename_component(PROJECT_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

set(MAIN_DIR ${CMAKE_CURRENT_LIST_DIR})

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

set(LDSCRIPT
    ${PROJECT_ROOT_DIR}/CM7/STM32H747XIHx_FLASH_CM7.ld
)

include(${PROJECT_ROOT_DIR}/common/cmake/common.cmake)

SET(SOURCE
    ${CMAKE_CURRENT_LIST_DIR}/core/src/hsem.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/src/stm32h7xx_hal_msp.c
    ${CMAKE_CURRENT_LIST_DIR}/core/src/stm32h7xx_it.c
)

            
if(TEST_FIFO)
    SET(SOURCE_MAIN ${CMAKE_CURRENT_LIST_DIR}/test/src/main_fifo.cpp)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DTEST_FIFO")
    message("testing fifo interface")
elseif(TEST_HSEM)
    SET(SOURCE_MAIN ${CMAKE_CURRENT_LIST_DIR}/test/src/main_hsem.cpp)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DTEST_HSEM")
    message("testing hsem interface")
else()
    SET(SOURCE_MAIN 
    ${CMAKE_CURRENT_LIST_DIR}/core/src/main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/core/src/main_thread.cpp)
endif(TEST_FIFO)

add_subdirectory(${PROJECT_ROOT_DIR}/lib/lwip lwip)

SET(STARTUP_SOURCE
    ${CMAKE_CURRENT_LIST_DIR}/startup/startup_stm32h747xihx.s
)

add_executable(${PROJECT_NAME} ${STARTUP_SOURCE} ${SOURCE_MAIN} ${SOURCE} ${COMMON_SOURCE})

target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${TARGET_INCLUDE}
)

target_link_libraries(${PROJECT_NAME} cmsis_${PROJECT_NAME} hal_${PROJECT_NAME} printf_${PROJECT_NAME} freertos_${PROJECT_NAME} lwip_${PROJECT_NAME})

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