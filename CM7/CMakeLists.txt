cmake_minimum_required(VERSION 3.16)

project(CARBON_CM7)

get_filename_component(PROJECT_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../" ABSOLUTE)

include($PROJCET_ROOT_DIR/cmake/toolchain.cmake)

set(ARM_FLAGS
    -DCORE_CM7
    -mcpu=cortex-m7
    -mthumb
    -mfpu=fpv5-d16
    -mfloat-abi=hard
    -mno-unaligned-access
)

include($PROJCET_ROOT_DIR/cmake/common.cmake)

