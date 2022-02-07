set (PRINTF_SOURCES	printf.c)
	
add_library(printf_${PROJECT_NAME} STATIC ${PRINTF_SOURCES})

target_include_directories(printf_${PROJECT_NAME}
	SYSTEM
	PUBLIC
	.
	)