cmake_minimum_required(VERSION 3.16)

get_filename_component(PROJECT_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)

project(trace_test)

set(CPP_FLAGS
    -std=c++17
)

string(REPLACE ";" " " S_CPP_FLAGS "${CPP_FLAGS}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${S_CPP_FLAGS}")

include_directories(${PROJECT_ROOT_DIR}/common/include)

SET (SOURCE 
	test.cpp
	)

add_executable(${PROJECT_NAME} ${SOURCE})