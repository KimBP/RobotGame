cmake_minimum_required(VERSION 3.28)

# Set the project name
project(robot-arena VERSION 1.0 LANGUAGES CXX)

# Standard C++ version
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

target_link_libraries(${PROJECT_NAME} PRIVATE 
	SDL2::SDL2
	SDL2_image::SDL2_image
	SDL2_ttf::SDL2_ttf
add_subdirectory(Robots/EdgePatrol)
)

add_subdirectory(Robots/Runner)
add_subdirectory(Robots/Scanner)

# 4. Ensure compile_commands.json is generated 
# (Crucial for clang-uml to work correctly)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
