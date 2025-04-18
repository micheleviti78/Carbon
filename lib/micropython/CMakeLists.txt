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

set(SOURCE_SHARED
	${MICROPY_DIR}/shared/runtime/gchelper_thumb2.s
	${MICROPY_DIR}/shared/runtime/gchelper_native.c
	${MICROPY_DIR}/shared/runtime/pyexec.c
	${MICROPY_DIR}/shared/runtime/stdout_helpers.c
	${MICROPY_DIR}/shared/readline/readline.c
)

set(SOURCES_USER_MODE
	${CMAKE_CURRENT_LIST_DIR}/port/user_module/led.c
)

set(MICROPY_SOURCE_QSTR
	${MICROPY_SOURCE_EXTMOD}
    ${MICROPY_SOURCE_PY}
	${SOURCE_SHARED}
	${SOURCES_USER_MODE}
)

set(MICROPYTHON_LIB micropython_${PROJECT_NAME})

add_library(${MICROPYTHON_LIB} 
	${SOURCE_SHARED} 
	${MICROPY_SOURCE_PY} 
	${MICROPY_SOURCE_EXTMOD}
	${SOURCES_USER_MODE}
)

target_link_libraries(${MICROPYTHON_LIB} freertos_${PROJECT_NAME})

target_include_directories(${MICROPYTHON_LIB}
PUBLIC
${CMAKE_CURRENT_LIST_DIR}/port
${MICROPY_DIR}
${PROJECT_CONFIG_DIR}
${CMAKE_CURRENT_LIST_DIR}/../freertos/include
${CMAKE_CURRENT_LIST_DIR}/../freertos/portable/GCC/ARM_CM7/r0p1
${CURRENT_BUILD_DIR}
)

target_compile_options(${MICROPYTHON_LIB} PUBLIC -Wno-vla -Wno-undef -Wno-stack-usage)

SET(MICROPY_TARGET ${MICROPYTHON_LIB})

# Include the main MicroPython cmake rules.
include(${MICROPY_DIR}/py/mkrules.cmake)
