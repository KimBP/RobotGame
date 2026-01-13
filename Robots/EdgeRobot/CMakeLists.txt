cmake_minimum_required(VERSION 3.28)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

add_library(edgeRobot SHARED EdgeRobot.cpp)

target_include_directories(edgeRobot PRIVATE 
	${CMAKE_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR})