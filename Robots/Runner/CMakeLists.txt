cmake_minimum_required(VERSION 3.28)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

add_library(runner SHARED Runner.cpp)

target_include_directories(runner PRIVATE 
	${CMAKE_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR})

# Link only for vtable/RTTI symbols - standalone compilation works
target_link_libraries(runner PRIVATE robot-core)
