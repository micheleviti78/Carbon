set(FTP_SOURCES
    src/ftp_file.c  
    src/ftp_server.c
    src/ftp.c
)

set(FTP_LIB ftp_${PROJECT_NAME})

add_library(${FTP_LIB} ${FTP_SOURCES})

target_include_directories(${FTP_LIB}
    PUBLIC
    src
    ${PROJECT_CONFIG_DIR}
)

target_link_libraries(${FTP_LIB} freertos_${PROJECT_NAME} lwip_${PROJECT_NAME} fatfs_${PROJECT_NAME} printf_${PROJECT_NAME})