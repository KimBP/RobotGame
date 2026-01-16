cmake_minimum_required(VERSION 3.28)

add_library(runner SHARED Runner.cpp)

target_include_directories(runner PRIVATE 
	${CMAKE_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR})
