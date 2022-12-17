set(SOURCES
    backtrace.c
)

add_library(backtrace_${PROJECT_NAME} ${SOURCES})

target_include_directories(backtrace_${PROJECT_NAME} SYSTEM PUBLIC ./)
