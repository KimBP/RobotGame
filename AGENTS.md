# RobotGame Development Guidelines

Auto-generated from all feature plans. Last updated: 2026-01-12

## Active Technologies

- Shell Scripting (Bash), C++17 (for robots) + CMake build system, g++ compiler, tar utility (001-developer-package)
- C++17 (from existing codebase) + SDL2, SDL2_ttf, SDL2_gfx, SDL2_image (from CMake configuration) (001-graphics-fixes)
- N/A (real-time game with in-memory state) (001-graphics-fixes)


## Project Structure

```text
src/
Robots/
├── Runner/
├── Scanner/
└── EdgeRobot/ (001-edge-robot)

# Core game files
Robot.h, Robot.cpp, RobotTypes.h, etc.
```

## Commands

# Add commands for Shell Scripting (Bash), C++17 (for robots)

## Code Style

Shell Scripting (Bash), C++17 (for robots): Follow standard conventions

## Recent Changes
- 001-developer-package: Added Shell Scripting (Bash), C++17 (for robots) + CMake build system, g++ compiler, tar utility

- 001-fix-undefined-symbols: Added C++11 (based on existing .cproject configuration) + RobotGame framework (Robot, RobCtrl, Trigonometry classes), POSIX threading (pthread), SDL2 libraries (for game visualization)

- 001-edge-patrol-robot: Added C++11 (based on existing .cproject configuration) + RobotGame framework (Robot, RobCtrl, Trigonometry classes), POSIX threading (pthread), SDL2 libraries (for game visualization)

- 001-graphics-fixes: Added C++17 (from existing codebase) + SDL2, SDL2_ttf, SDL2_gfx, SDL2_image (from CMake configuration)

- 001-edge-robot: Added C++17 + SDL2, SDL2_ttf, SDL2_gfx, SDL2_image (CMake-based build system)

## Build Commands

### CMake Build System
- Build with `cmake -B build -S . && cmake --build build`
- Configure with: `cmake -DCMAKE_BUILD_TYPE=Release -B build -S .`

### Individual Robot Libraries
- Each robot compiles to shared library (.so)
- Robots are built as separate subdirectories
- Use `target_link_libraries()` for dependencies

### Main Executable
- Main game executable links all robot libraries
- Uses `add_subdirectory()` to include robot builds
- Links with SDL2 libraries: SDL2, SDL2_ttf, SDL2_gfx, SDL2_image

### Development Workflow
```bash
# Configure build
cmake -DCMAKE_BUILD_TYPE=Debug -B build -S .

# Build all components
cmake --build build

# Clean build
rm -rf build/
```

<!-- MANUAL ADDITIONS START -->
<!-- MANUAL ADDITIONS END -->
