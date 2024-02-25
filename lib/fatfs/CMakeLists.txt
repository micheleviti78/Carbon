set(FATFS_SOURCES
    src/diskio.c  
    src/ff.c  
    src/ff_gen_drv.c  
    src/sd_diskio.c
)

set(FATFS_OPTIONS
    src/option/ccsbcs.c  
    src/option/syscall.c  
    src/option/unicode.c
)

set(FATFS_LIB fatfs_${PROJECT_NAME})

add_library(${FATFS_LIB} ${FATFS_SOURCES} ${FATFS_OPTIONS})

target_include_directories(${FATFS_LIB}
    PUBLIC
    src
    ${PROJECT_CONFIG_DIR}
)

target_link_libraries(${FATFS_LIB} hal_${PROJECT_NAME} freertos_${PROJECT_NAME})
