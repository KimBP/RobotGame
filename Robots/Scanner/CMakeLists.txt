cmake_minimum_required(VERSION 3.28)

add_library(scanner SHARED Scanner.cpp)

target_include_directories(scanner PRIVATE 
	${CMAKE_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR})
