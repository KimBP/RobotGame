# Research Phase 0: Developer Package Creation

## Build System Detection Research

**Decision**: Use file existence checks and timestamp comparison for build detection
**Rationale**: Simple, reliable approach that handles all edge cases without complex dependencies
**Alternatives considered**: CMake --build commands, makefile dependency tracking, ninja build status

### Implementation Strategy
- Check for build directory existence
- Verify specific artifacts: `robot-arena`, `librunner.so`, `libscanner.so`
- Use timestamp comparison to detect outdated artifacts
- Check for running build processes to avoid conflicts
- Fall back to full rebuild when detection fails

## Development Kit Structure Research

**Decision**: Use standard SDK layout with relative paths and self-contained examples
**Rationale**: Extractable anywhere, immediate build capability, familiar to C++ developers
**Alternatives considered**: Flat structure, complex dependency management, system-wide installation

### Package Structure
```
robot-dev-kit/
├── README.md              # Quick start and build instructions
├── Robot.h               # Main API header
├── MyRobot.cpp           # Robot template
├── ROBOT_RULES.md        # Development rules and guidelines
├── bin/
│   └── robot-arena      # Main executable
├── lib/
│   ├── librunner.so     # Runner robot library
│   └── libscanner.so    # Scanner robot library
└── robots/
    ├── Runner/
    │   ├── Runner.cpp
    │   ├── Runner.h
    │   └── CMakeLists.txt
    └── Scanner/
        ├── Scanner.cpp
        ├── Scanner.h
        └── CMakeLists.txt
```

## CMake Integration Research

**Decision**: Simple CMake configuration for examples using relative paths
**Rationale**: Works when extracted anywhere, no complex package config needed
**Alternatives considered**: Full CMake package config, pkg-config, manual Makefiles

### Example CMakeLists.txt Structure
- Use `CMAKE_PREFIX_PATH` to find relative SDK location
- Simple `add_executable` and `target_link_libraries` for each robot
- Parallel build support with proper dependency management
- Cross-platform compatibility for Linux environments

## Documentation Best Practices Research

**Decision**: Comprehensive README with step-by-step examples
**Rationale**: Developer onboarding focus, clear progression from template to custom robots
**Alternatives considered**: API docs first, tutorial videos, interactive guides

### Documentation Components
- Quick start guide (under 30 minutes to first robot)
- Build instructions for each example
- Robot development rules and constraints
- API reference for Robot.h interface
- Troubleshooting common issues

## Packaging and Distribution Research

**Decision**: tar.gz with relative RPATH for shared libraries
**Rationale**: Portable, compression efficient, works across Linux distributions
**Alternatives considered**: zip, 7z, AppImage, Docker container

### Technical Requirements
- All paths relative to extraction point
- Shared libraries use `$ORIGIN` RPATH for portability
- No system dependencies beyond standard C++ toolchain
- Package size under 50MB for easy distribution
- Build completes in under 5 minutes

## Script Implementation Research

**Decision**: Pure bash script with repository file copying
**Rationale**: Constitution requirement (no heredocs), maintainability, transparency
**Alternatives considered**: Python script, make-based system, CMake packaging

### Script Design Principles
- Use only repository files, no inline generation
- All paths relative to script location
- Verbose output for debugging
- Error handling with meaningful messages
- Idempotent behavior (safe to run multiple times)

## File Management Research

**Decision**: Copy existing files from repository structure
**Rationale**: Constitution compliance, single source of truth, easy maintenance
**Alternatives considered**: Template generation, dynamic file creation, symbolic links

### Source File Locations
- Binaries: `build/robot-arena`, `build/Robots/*/lib*.so`
- Headers: `Robot.h` (root level)
- Robot sources: `Robots/Runner/`, `Robots/Scanner/`
- Documentation: Need to create `docs/dev-kit/` templates