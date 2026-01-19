cmake_minimum_required(VERSION 3.28)

# Set the project name
project(robot-arena VERSION 1.0 LANGUAGES CXX)

# Standard C++ version
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

# 1. Create shared library for core Robot interface (vtable emission)
add_library(robot-core SHARED 
    "Robot.cpp"
    "RobCtrl.cpp"
    "Logger.cpp"
)

# 2. Create static library for game engine (not needed by plugins)
add_library(robot-engine STATIC 
    "RobotDataEvent.cpp"
    "RobotPosEvent.cpp"
    "CannonPosEvent.cpp"
    "CannonShell.cpp"
    "Scheduler.cpp"
    "Trigonometry.cpp"
    "Viewer.cpp"
    "Explosion.cpp"
    "ShellPool.cpp"
)

# Export Robot class symbols from robot-core
set_target_properties(robot-core PROPERTIES
    CXX_VISIBILITY_PRESET default
)

# 3. Add the executable
add_executable(${PROJECT_NAME} "RobotGame.cpp")

# 3 Add external libraries
find_package(SDL2 REQUIRED CONFIG)
find_package(SDL2_image REQUIRED CONFIG)
find_package(SDL2_ttf  REQUIRED CONFIG)

# sdl2-gfx package is special
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2_GFX REQUIRED SDL2_gfx)

target_link_libraries(robot-engine PRIVATE robot-core)

target_link_libraries(${PROJECT_NAME} PRIVATE 
	robot-core
	robot-engine
	SDL2::SDL2
	SDL2_image::SDL2_image
	SDL2_ttf::SDL2_ttf
	${SDL2_GFX_LIBRARIES}
)

add_subdirectory(Robots/Runner)
add_subdirectory(Robots/Scanner)
add_subdirectory(Robots/EdgeRobot)

# 4. Ensure compile_commands.json is generated 
# (Crucial for clang-uml to work correctly)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
