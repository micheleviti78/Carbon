enable_language(C CXX ASM)

set(WARN_FLAGS
    -Wall
    -Wextra
    -Werror
    -Werror=vla
    -Wundef
    -Wformat=2
    -Wformat-truncation
    -Wformat-overflow=2
    -Wformat-signedness
    -Wno-format-nonliteral
    -Wstack-usage=1024
    -Wno-unused-parameter
    -Wlogical-op
    -Wdouble-promotion
    -Wfloat-conversion
    -Warith-conversion
    -Wshadow=local
    -Wduplicated-cond
    -Wstringop-overflow=4
    -Wnull-dereference
)

set(MISC_FLAGS
    -fno-common
#    -fstack-usage
    -fdata-sections
    -ffunction-sections
    -funwind-tables
    -fno-omit-frame-pointer
    -fdelete-null-pointer-checks
)

set(HAL_COMMON
    -DUSE_MULTI_CORE_SHARED_CODE
    -DSTM32H747xx
)

set(CPP_FLAGS
    -std=c++17
    -fno-rtti
    -fno-exceptions
    -fno-threadsafe-statics
    -Wold-style-cast
)

add_compile_options(${ARM_FLAGS})
add_compile_options(${WARN_FLAGS})
add_compile_options(${MISC_FLAGS})
add_compile_options(${HAL_COMMON})

string(REPLACE ";" " " S_CPP_FLAGS "${CPP_FLAGS}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${S_CPP_FLAGS}")

set(LDSPECS nano.specs)

set(CMAKE_EXECUTABLE_SUFFIX ".elf")

string(REPLACE ";" " " S_ARM_FLAGS "${ARM_FLAGS}")

set(CMAKE_EXE_LINKER_FLAGS "${S_ARM_FLAGS} -specs=${LDSPECS} -T${LDSCRIPT}")

include_directories(${PROJECT_ROOT_DIR}/common/include)
include_directories(${MAIN_DIR}/core/include)
include_directories(${PROJECT_ROOT_DIR}/common/conf)

SET(TARGET_INCLUDE ${CMAKE_CURRENT_LIST_DIR}/core/include)

add_subdirectory(${PROJECT_ROOT_DIR}/lib/CMSIS cmsis)
add_subdirectory(${PROJECT_ROOT_DIR}/lib/hal hal)
add_subdirectory(${PROJECT_ROOT_DIR}/lib/printf printf)
add_subdirectory(${PROJECT_ROOT_DIR}/lib/freertos freertos)
add_subdirectory(${PROJECT_ROOT_DIR}/lib/backtrace backtrace)

SET(COMMON_SOURCE
    ${PROJECT_ROOT_DIR}/common/src/sys/syscalls.c
    ${PROJECT_ROOT_DIR}/common/src/sys/newlib.c
    ${PROJECT_ROOT_DIR}/common/src/sys/cpp.cpp
    ${PROJECT_ROOT_DIR}/common/src/sys/oshooks.cpp
    ${PROJECT_ROOT_DIR}/common/src/diag_thread.c
    ${PROJECT_ROOT_DIR}/common/src/diag.cpp
    ${PROJECT_ROOT_DIR}/common/src/error.cpp
    ${PROJECT_ROOT_DIR}/common/src/hsem.cpp
    ${PROJECT_ROOT_DIR}/common/src/irq.cpp
    ${PROJECT_ROOT_DIR}/common/src/mpu.cpp
    ${PROJECT_ROOT_DIR}/common/src/systime.cpp
    ${PROJECT_ROOT_DIR}/common/src/sdram.cpp
    ${PROJECT_ROOT_DIR}/common/src/shared_memory.cpp
    ${PROJECT_ROOT_DIR}/common/src/start_os.c
    ${PROJECT_ROOT_DIR}/common/src/pin.cpp
    ${PROJECT_ROOT_DIR}/common/src/rand.c
    ${PROJECT_ROOT_DIR}/common/src/system_stm32h7xx_dualcore_boot_cm4_cm7.c
)
