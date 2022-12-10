set (PRINTF_SOURCES	printf.c)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DPRINTF_INCLUDE_CONFIG_H")
	
add_library(printf_${PROJECT_NAME} STATIC ${PRINTF_SOURCES})

target_include_directories(printf_${PROJECT_NAME}
	SYSTEM
	PUBLIC
	.
	)