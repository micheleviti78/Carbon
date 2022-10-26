set(SOURCES
    tasks.c
    queue.c
    timers.c
    event_groups.c
    croutine.c
    list.c
    stream_buffer.c
)

set(FREERTOS_LIB freertos_${PROJECT_NAME})

if (CORE_CM7)
    set(PORTSOURCES
        portable/GCC/ARM_CM7/r0p1/port.c
        portable/MemMang/heap_4.c
    )
    set(PORTINCLUDES
        portable/GCC/ARM_CM7/r0p1
    )
else()
    set(PORTSOURCES
        portable/GCC/ARM_CM4F/port.c
        portable/MemMang/heap_4.c
    )
    set(PORTINCLUDES
        portable/GCC/ARM_CM4F
    )
endif()

set(PLFSOURCES
    CMSIS_RTOS/cmsis_os.c
)

add_library(${FREERTOS_LIB} ${SOURCES} ${PLFSOURCES} ${PORTSOURCES})

if (FREERTOS_USE_ASSERT)
    target_compile_definitions(${FREERTOS_LIB} PUBLIC -DFREERTOS_USE_ASSERT)
endif()

target_include_directories(${FREERTOS_LIB}
    SYSTEM
    PUBLIC
    include
    ${PORTINCLUDES}
    CMSIS_RTOS
    ${PROJECT_CONFIG_DIR}
)

target_link_libraries(${FREERTOS_LIB} hal_${PROJECT_NAME} cmsis_${PROJECT_NAME})
