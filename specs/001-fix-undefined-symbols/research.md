# Research: Fix Undefined Symbols in Robot Shared Libraries

**Feature**: 001-fix-undefined-symbols
**Date**: 2026-01-13
**Status**: Complete

---

## Research Summary

This research investigates the undefined symbol error when loading the EdgePatrol robot plugin and determines the solution approach to support standalone robot building with minimal API dependencies.

## Decision 1: Root Cause of Undefined Symbol Error

**Problem**: EdgePatrol fails to load with undefined symbol `RobotGame::EdgePatrol::name[abi:cxx11]() const`

**Root Cause**: The `name()` method is declared in `EdgePatrol.h:23` as `virtual std::string name() const;` but never implemented in the `.cpp` file. The Robot base class (`Robot.h:30`) declares this as a pure virtual method that all robot plugins must implement.

**Evidence**:
```
$ nm -D build/Robots/EdgePatrol/libedgepatrol.so | grep -E "U.*EdgePatrol"
U _ZNK9RobotGame10EdgePatrol4nameB5cxx11Ev
# Demangled: RobotGame::EdgePatrol::name[abi:cxx11]() const
```

---

## Decision 2: Working Robot Pattern (Runner & Scanner)

**Decision**: Runner and Scanner plugins work correctly because they implement `name()` inline in header files.

**Analysis**:

Runner.h:23:
```cpp
virtual std::string name() const { return "RUNNER"; };
```

Scanner.h:23:
```cpp
virtual std::string name() const { return myName; };
```

When compiled with inline implementation in header, these become weak symbols in the shared library:
```
$ nm build/Robots/Scanner/libscanner.so | grep name
W _ZNK7Scanner4nameB5cxx11Ev
```

**Rationale**: Inline header implementation ensures the symbol is defined at compile time without requiring implementation in `.cpp` file. This is the simplest pattern for robot plugins.

**Alternatives Considered**:
1. **Implement in .cpp file**: Would require explicit definition but adds file complexity
2. **Use default implementation in base class**: Not viable as each robot needs unique name
3. **Macro-based name generation**: Adds complexity, reduces readability

---

## Decision 3: Standalone Robot API Requirements

**Decision**: Robots must be compiled with Robot.h, RobotTypes.h, Logger.h, and Trigonometry.h to support standalone development.

**Analysis**:

Required headers for EdgePatrol:
```cpp
#include "EdgePatrol.h"      // Robot class declaration
#include "Logger.h"          // Logging functions
#include "Trigonometry.h"    // Utility functions (direction, move)
```

EdgePatrol.cpp uses these Trigonometry methods:
- `Trigonometry::direction(int x1, int y1, int x2, int y2)` - line 184
- `Trigonometry::move(int x, int y, angle_t dir, range_t dist, int* newx, int* newy)` - line 132

**API Dependencies**:

1. **Robot.h** - Core robot interface
   - Base class with virtual methods: `init()`, `run()`, `name()`
   - Action methods: `scan()`, `cannon()`, `drive()`, `skip()`
   - Query methods: `getSpeed()`, `getDirection()`, `getX()`, `getY()`, `getArmor()`, `getEnergy()`
   - Conversion methods: `armorToEnergy()`, `energyToArmor()`

2. **RobotTypes.h** - Type definitions
   - All type aliases: `angle_t`, `range_t`, `speed_t`, `armor_t`, `energy_t`, `posx_t`, `posy_t`
   - Constants: `MAX_POS_X`, `MAX_POS_Y`, `MAX_ARMOR`, `MAX_ENERGY`, `SHOOT_ENERGY_COST`
   - Struct: `tune_t`

3. **Logger.h** - Logging API
   - `Logger::Log(const Robot* robot, std::string msg)` - for robot-specific logging
   - `Logger::Log(std::string name, std::string msg)` - named logging
   - `Logger::LogError/LogWarning/LogDebug` - level-based logging

4. **Trigonometry.h** - Utility functions
   - `Trigonometry::direction(int x1, int y1, int x2, int y2)` - calculate compass direction
   - `Trigonometry::distance(int x1, int y1, int x2, int y2)` - calculate distance
   - `Trigonometry::move(int x, int y, angle_t dir, range_t dist, int* newX, int* newY)` - calculate new position

**Rationale**: The four headers provide a complete API for robot development without requiring access to arena internals (RobCtrl.h, Scheduler.cpp, etc.). This enables third-party robot developers to build plugins using only these API headers.

**Alternatives Considered**:
1. **Only Robot.h and Logger.h** - Insufficient, robots need Trigonometry utilities for navigation
2. **All headers including implementation** - Violates encapsulation, exposes internal logic
3. **Link against static library** - Not truly standalone, requires compiled arena binary

---

## Decision 4: Symbol Visibility and Linking

**Decision**: Robot shared libraries should use default symbol visibility with RTLD_LAZY loading mode.

**Analysis**:

Current RobotGame.cpp:27-43 plugin loader:
```cpp
getRobotFunc loadPlugin(const char* plugin)
{
    void* lib_handle;
    lib_handle = dlopen(plugin, RTLD_LAZY);  // Lazy symbol resolution
    if (!lib_handle) {
        std::cout << "Error loading plugin " << plugin << " error : " << dlerror() << std::endl;
        return NULL;
    }

    getRobotFunc fn = reinterpret_cast<getRobotFunc>(dlsym(lib_handle, "getRobot"));
    char* error = dlerror();
    if (error != NULL) {
        std::cout << "Error finding getRobot() in " << plugin << std::endl;
        return NULL;
    }
    return fn;
}
```

**Symbol Types**:
- `T` - Text (code) section symbols (defined in this library)
- `W` - Weak symbols (can be overridden, typically inline functions)
- `U` - Undefined symbols (must be resolved at load time)
- `V` - Weak object symbols

**Key Requirements**:
- Robot plugins must export `getRobot(RobCtrl* robCtrl)` function with C linkage (`extern "C"`)
- All virtual methods must be implemented to avoid `U` symbols
- Static symbols can stay internal to plugin

**Rationale**: RTLD_LAZY allows symbol resolution when symbols are first referenced rather than at load time, improving startup performance. Default visibility ensures symbols are available to the dynamic linker.

---

## Decision 5: Build System Requirements

**Decision**: Robot plugins should be built as shared libraries with proper include paths.

**Current EdgePatrol CMakeLists.txt**:
```cmake
add_library(edgepatrol SHARED EdgePatrol.cpp)

target_include_directories(edgepatrol PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR})
```

**Build Commands**:
```bash
# From repository root
cmake -B build -S .
cmake --build build

# Robot libraries built as:
build/Robots/EdgePatrol/libedgepatrol.so
build/Robots/Runner/librunner.so
build/Robots/Scanner/libscanner.so
```

**For Standalone Building** (without arena source):
```cmake
# Minimal robot CMakeLists.txt
cmake_minimum_required(VERSION 3.28)

# Robot-specific
project(edgedemo VERSION 1.0 LANGUAGES CXX)

add_library(edgedemo SHARED EdgeDemo.cpp)

target_include_directories(edgedemo PRIVATE
    /path/to/api/headers  # Robot.h, RobotTypes.h, Logger.h, Trigonometry.h
)
```

**Rationale**: Shared library (.so) format enables dynamic loading via dlopen. Proper include paths ensure headers are found during compilation.

---

## Unknowns Resolved

| Unknown | Resolution |
|----------|------------|
| Plugin Architecture Pattern | dlopen-based shared library with extern "C" factory function |
| Virtual Method Linkage | Implement inline in header (W symbol) or in .cpp (T symbol) |
| API Header Isolation | Robot.h + RobotTypes.h + Logger.h + Trigonometry.h |
| Symbol Visibility | Default visibility, RTLD_LAZY loading |
| Working Pattern Analysis | Runner/Scanner use inline header implementation |

---

## Technical Decisions Summary

1. **Fix EdgePatrol**: Implement `name()` method inline in EdgePatrol.h (line 23)
   ```cpp
   virtual std::string name() const { return myName; };
   ```

2. **API Headers for Standalone Building**: Provide these four headers to external robot developers:
   - `Robot.h` - Core robot interface
   - `RobotTypes.h` - Type definitions and constants
   - `Logger.h` - Logging API
   - `Trigonometry.h` - Navigation utility functions

3. **Document Pattern**: Update quickstart guide to show inline virtual method implementation pattern

4. **Verify All Robots**: Ensure Runner, Scanner, and EdgePatrol all follow consistent pattern

---

## Alternatives Considered and Rejected

| Alternative | Why Rejected |
|-------------|--------------|
| Implement name() in EdgePatrol.cpp | Adds file complexity, deviates from established pattern |
| Provide only Robot.h and Logger.h | Insufficient - robots need Trigonometry utilities |
| Link robot plugins with static arena library | Not truly standalone, violates plugin architecture |
| Use function pointers instead of virtual methods | Over-engineered, breaks C++ OOP pattern |
| Hide Trigonometry as internal detail | Useful for robot navigation logic, should be API |

---

## Next Steps

1. Fix EdgePatrol by implementing `name()` inline in header
2. Create API package with Robot.h, RobotTypes.h, Logger.h, Trigonometry.h
3. Update documentation to reflect standalone building requirements
4. Verify all robots load successfully without undefined symbols
