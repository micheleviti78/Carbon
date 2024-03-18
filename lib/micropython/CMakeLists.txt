# Set location of base MicroPython directory.
if(NOT MICROPY_DIR)
    get_filename_component(MICROPY_DIR ${CMAKE_CURRENT_LIST_DIR}/micropython ABSOLUTE)
endif()

if(NOT MICROPY_DIR_PARENT)
    get_filename_component(MICROPY_DIR_PARENT ${CMAKE_CURRENT_LIST_DIR} ABSOLUTE)
endif()

SET(MICROPY_GENHDR_DIR ${CURRENT_BUILD_DIR}/genhdr/)

include(${MICROPY_DIR}/py/py.cmake)

include(${MICROPY_DIR}/extmod/extmod.cmake)

set(SOURCES_PORT
    ${CMAKE_CURRENT_LIST_DIR}/port/mpcarbon.c  
    ${CMAKE_CURRENT_LIST_DIR}/port/gccollect.c  
    ${CMAKE_CURRENT_LIST_DIR}/port/mphalport.c
    ${CMAKE_CURRENT_LIST_DIR}/port/mpthreadport.c
    ${CMAKE_CURRENT_LIST_DIR}/port/cortex_m7_get_sp.s
)

set(SOURCE_SHARED
	${MICROPY_DIR}/shared/runtime/gchelper_thumb2.s
	${MICROPY_DIR}/shared/runtime/gchelper_native.c
	${MICROPY_DIR}/shared/runtime/pyexec.c
	${MICROPY_DIR}/shared/runtime/sys_stdio_mphal.c
	${MICROPY_DIR}/shared/runtime/interrupt_char.c
	${MICROPY_DIR}/shared/runtime/stdout_helpers.c
	${MICROPY_DIR}/shared/readline/readline.c
)

set(MICROPY_SOURCE_QSTR
	${MICROPY_SOURCE_EXTMOD}
    ${MICROPY_SOURCE_PY}
	${SOURCE_SHARED}
	${SOURCES_PORT}
)

set(MICROPYTHON_LIB micropython_${PROJECT_NAME})

add_library(${MICROPYTHON_LIB} ${SOURCES_PORT} ${SOURCE_SHARED} ${MICROPY_SOURCE_PY} ${MICROPY_SOURCE_EXTMOD})

target_compile_options(${MICROPYTHON_LIB} PUBLIC -Wno-vla -Wno-undef -Wno-stack-usage)

SET(MICROPY_TARGET ${MICROPYTHON_LIB})

target_link_libraries(${MICROPYTHON_LIB} freertos_${PROJECT_NAME} printf_${PROJECT_NAME})

target_include_directories(${MICROPYTHON_LIB}
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/port
	${MICROPY_DIR}
    ${PROJECT_CONFIG_DIR}
	${CMAKE_CURRENT_LIST_DIR}/../freertos/include
	${CMAKE_CURRENT_LIST_DIR}/../freertos/portable/GCC/ARM_CM7/r0p1
	${CMAKE_CURRENT_LIST_DIR}/../printf
	${CMAKE_CURRENT_LIST_DIR}/../freertos/CMSIS_RTOS/
	${CURRENT_BUILD_DIR}
)

# Include the main MicroPython cmake rules.
include(${MICROPY_DIR}/py/mkrules.cmake)
