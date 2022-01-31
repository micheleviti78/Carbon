SET(CMSIS_LIB cmsis_${PROJECT_NAME})

add_library(${CMSIS_LIB} INTERFACE)

target_include_directories(${CMSIS_LIB}
    SYSTEM
    INTERFACE include
    INTERFACE device/ST/STM32H7xx/include
)