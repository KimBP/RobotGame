cmake_minimum_required(VERSION 3.28)

# Set the project name
project(robot-arena VERSION 1.0 LANGUAGES CXX)

# Standard C++ version
set(CMAKE_CXX_STANDARD 17)

# (Crucial for clang-uml to work correctly)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# DEBUG: Enhanced logging configuration
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(-DDEBUG_MODE=1)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g -Wall -Wextra")
    message(STATUS "Debug build with enhanced logging enabled")
else()
    add_compile_definitions(-DDEBUG_MODE=0)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")
    message(STATUS "Release build")
endif()

# 1. Gather all .cpp files in the current directory
file(GLOB SOURCES "*.cpp")

# 2. Add the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# 3 Add external libraries
find_package(SDL2 REQUIRED CONFIG)
find_package(SDL2_image REQUIRED CONFIG)
find_package(SDL2_ttf  REQUIRED CONFIG)

# sdl2-gfx package is special
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2_GFX REQUIRED SDL2_gfx)

# 4. Export symbols for dynamic plugin loading
# Required so robot plugins can resolve Robot base class symbols at runtime
set_target_properties(${PROJECT_NAME} PROPERTIES
    LINK_FLAGS "-rdynamic"
)

target_link_libraries(${PROJECT_NAME} PRIVATE 
	SDL2::SDL2
	SDL2_image::SDL2_image
	SDL2_ttf::SDL2_ttf
	${SDL2_GFX_LIBRARIES}
	dl
)

add_subdirectory(Robots/Runner)
add_subdirectory(Robots/Scanner)
add_subdirectory(Robots/EdgePatrol)
add_subdirectory(Robots/EdgeRobot)
