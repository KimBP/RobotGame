# RobotGame.BigPickle Development Guidelines

Auto-generated from all feature plans. Last updated: 2026-01-12

## Active Technologies

- C++17 + SDL2, SDL2_ttf, SDL2_gfx, SDL2_image (001-edge-robot)

## Project Structure

```text
Robots/
├── Runner/
├── Scanner/
└── EdgeRobot/ (001-edge-robot)

# Core game files
Robot.h, Robot.cpp, RobotTypes.h, etc.
```

## Commands

# Add commands for C++11

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

## Code Style

C++11: Follow standard conventions

## Recent Changes

- 001-edge-robot: Added C++17 + SDL2, SDL2_ttf, SDL2_gfx, SDL2_image (CMake-based build system)

<!-- MANUAL ADDITIONS START -->
<!-- MANUAL ADDITIONS END -->
