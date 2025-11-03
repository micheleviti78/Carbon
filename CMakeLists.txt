cmake_minimum_required(VERSION 3.15)
project(MainProject)

# --- (CM7) ---
set(CM7_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/CM7")
set(CM7_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/CM7")

add_custom_target(Build_CM7 ALL
    COMMAND ${CMAKE_COMMAND} -S "${CM7_SRC_DIR}" -B "${CM7_BUILD_DIR}"
    COMMAND ${CMAKE_COMMAND} --build "${CM7_BUILD_DIR}"
    COMMENT "Building CM7 project"
)

# --- (CM4) ---
set(CM4_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/CM4")
set(CM4_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/CM4")

add_custom_target(Build_CM4 ALL
    COMMAND ${CMAKE_COMMAND} -S "${CM4_SRC_DIR}" -B "${CM4_BUILD_DIR}"
    COMMAND ${CMAKE_COMMAND} --build "${CM4_BUILD_DIR}"
    COMMENT "Building CM4 project"
)
