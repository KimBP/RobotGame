# Quick Start Guide: Building Robot Plugins Standalone

**Feature**: 001-fix-undefined-symbols
**Updated**: 2026-01-13

---

## Overview

This guide shows you how to build robot plugins for RobotGame without access to the full arena source code. You only need the API headers: `Robot.h`, `RobotTypes.h`, `Logger.h`, and `Trigonometry.h`.

---

## Prerequisites

### Required Headers

Copy these four API headers to your development directory:

```
api-headers/
├── Robot.h           # Core robot interface
├── RobotTypes.h       # Type definitions and constants
├── Logger.h           # Logging functions
└── Trigonometry.h     # Navigation utilities
```

### Build Tools

- C++11 compatible compiler (g++ 4.9+, clang 3.4+)
- CMake 3.28+ (recommended) or Make
- POSIX-compliant system (Linux, macOS, BSD)

---

## Creating Your First Robot

### Step 1: Define Robot Header

Create `MyRobot.h`:

```cpp
#ifndef MYROBOT_H_
#define MYROBOT_H_

#include "Robot.h"

namespace RobotGame {

class MyRobot : public Robot {
public:
    MyRobot(RobCtrl* robCtrl);
    virtual ~MyRobot();

    // CRITICAL: Implement init() inline to avoid undefined symbols
    virtual struct tune_t init(std::string& name) {
        name = myName;
        return myTune;
    };

    virtual void run();
    virtual std::string name() const { return myName; };  // MUST be inline!

private:
    static const struct tune_t myTune;
    static const std::string myName;
};

} // namespace RobotGame

#endif // MYROBOT_H_
```

### Step 2: Implement Robot

Create `MyRobot.cpp`:

```cpp
#include "MyRobot.h"
#include "Logger.h"
#include "Trigonometry.h"
#include <cstdlib>

// Factory function - MUST have extern "C" linkage
extern "C" RobotGame::Robot* getRobot(RobotGame::RobCtrl* robCtrl) {
    static RobotGame::MyRobot* instance = new RobotGame::MyRobot(robCtrl);
    return instance;
}

// Static member definitions
const struct RobotGame::tune_t RobotGame::MyRobot::myTune = {1, 1};
const std::string RobotGame::MyRobot::myName("MyRobot");

// Constructor
RobotGame::MyRobot::MyRobot(RobCtrl* robCtrl)
: Robot(robCtrl) {
    Logger::Log(this, "MyRobot initialized");
}

// Destructor
RobotGame::MyRobot::~MyRobot() {
}

// Main robot logic
void RobotGame::MyRobot::run() {
    while (true) {
        // Example: Random movement
        int direction = rand() % 360;
        int speed = 50;

        // Check armor
        if (getArmor() > 50) {
            // Safe to move fast
            speed = 75;
        }

        // Log current state
        Logger::Log(this, std::string("Position: (") +
                         std::to_string(getX()) + "," +
                         std::to_string(getY()) + ")");

        // Move robot
        drive(direction, speed);
    }
}
```

### Step 3: Build with CMake

Create `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.28)

project(MyRobot VERSION 1.0 LANGUAGES CXX)

# Compile as shared library
add_library(myrobot SHARED MyRobot.cpp)

# Include path to API headers
target_include_directories(myrobot PRIVATE
    ${CMAKE_SOURCE_DIR}/api-headers
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# C++11 standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
```

### Step 4: Compile

```bash
# Configure
cmake -B build -S .

# Build
cmake --build build

# Output: build/libmyrobot.so
```

---

## Building with Make

If you prefer Make over CMake, create `Makefile`:

```makefile
CXX = g++
CXXFLAGS = -std=c++11 -fPIC -Wall -O2
LDFLAGS = -shared

INCLUDES = -Iapi-headers
TARGET = libmyrobot.so
SOURCES = MyRobot.cpp

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LDFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET)

install: $(TARGET)
	cp $(TARGET) /path/to/Robots/MyRobot/
```

Build with:
```bash
make
```

---

## Common Pitfalls and Solutions

### ❌ Problem: Undefined Symbol Error

**Error Message**:
```
Error loading plugin libmyrobot.so error :
libmyrobot.so: undefined symbol: _ZNK9RobotGame7MyRobot4nameB5cxx11Ev
```

**Cause**: The `name()` method is declared but not implemented.

**Solution**: Implement `name()` inline in the header file:

```cpp
// ❌ WRONG - Only declaration
virtual std::string name() const;

// ✅ CORRECT - Inline implementation
virtual std::string name() const { return myName; };
```

---

### ❌ Problem: Missing Factory Function

**Error Message**:
```
Error finding getRobot() in libmyrobot.so
```

**Cause**: The `getRobot()` function is not exported or has wrong linkage.

**Solution**: Ensure the function has `extern "C"` linkage:

```cpp
// ❌ WRONG - C++ linkage (name mangling)
RobotGame::Robot* getRobot(RobotGame::RobCtrl* robCtrl);

// ✅ CORRECT - C linkage (no name mangling)
extern "C" RobotGame::Robot* getRobot(RobotGame::RobCtrl* robCtrl) {
    static RobotGame::MyRobot* instance = new RobotGame::MyRobot(robCtrl);
    return instance;
}
```

---

### ❌ Problem: Cannot Find Headers

**Error Message**:
```
Robot.h: No such file or directory
```

**Cause**: Include path is not correctly set.

**Solution**:
- With CMake: Add header directory to `target_include_directories()`
- With Make: Add `-I/path/to/api-headers` to CXXFLAGS

---

### ❌ Problem: Architecture Mismatch

**Error Message**:
```
libmyrobot.so: wrong ELF class: ELFCLASS64
```

**Cause**: Plugin compiled for different architecture than arena.

**Solution**: Compile plugin with same target as arena:
```bash
# Check arena architecture
file robot-arena
# Output: ELF 64-bit LSB executable, x86-64

# Compile plugin for same architecture
g++ -m64 -std=c++11 -fPIC -shared -o libmyrobot.so MyRobot.cpp
```

---

## Testing Your Robot

### Step 1: Build the Robot

```bash
cmake -B build -S .
cmake --build build
```

### Step 2: Test Plugin Loading

```bash
# Check for undefined symbols
nm -D build/libmyrobot.so | grep " U "
# Should show no undefined symbols except those from arena

# Verify factory function is exported
nm -D build/libmyrobot.so | grep "getRobot"
# Should show T or W symbol
```

### Step 3: Run with Arena

```bash
# Navigate to arena build directory
cd /path/to/robot-arena/build

# Run arena with your robot plugin
./robot-arena /path/to/your/robot/libmyrobot.so
```

### Step 4: Check Logs

Robot logs are written to stdout/stderr during execution:
```
MyRobot initialized
Position: (5000,5000)
Position: (5071,5071)
...
```

---

## Debugging Tips

### Check Symbol Table

```bash
# List all symbols in plugin
nm build/libmyrobot.so

# Show dynamic symbols
nm -D build/libmyrobot.so

# Check for undefined symbols (grep for " U ")
nm -D build/libmyrobot.so | grep " U "
```

### Demangle C++ Symbols

```bash
# Demangle mangled symbol names
c++filt _ZNK9RobotGame7MyRobot4nameB5cxx11Ev
# Output: RobotGame::MyRobot::name[abi:cxx11]() const
```

### Check Library Dependencies

```bash
# Show libraries this plugin depends on
ldd build/libmyrobot.so
```

---

## Advanced: Building Multiple Robots

### Directory Structure

```
robots/
├── api-headers/          # Shared API headers
│   ├── Robot.h
│   ├── RobotTypes.h
│   ├── Logger.h
│   └── Trigonometry.h
├── AggressiveRobot/
│   ├── AggressiveRobot.h
│   ├── AggressiveRobot.cpp
│   └── CMakeLists.txt
├── DefensiveRobot/
│   ├── DefensiveRobot.h
│   ├── DefensiveRobot.cpp
│   └── CMakeLists.txt
└── CMakeLists.txt       # Top-level build
```

### Top-level CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.28)
project(robot-plugins VERSION 1.0 LANGUAGES CXX)

# Include all robots
add_subdirectory(AggressiveRobot)
add_subdirectory(DefensiveRobot)
```

### Build All Robots

```bash
cmake -B build -S .
cmake --build build
```

Output:
```
build/AggressiveRobot/libaggressiverobot.so
build/DefensiveRobot/libdefensiverobot.so
```

---

## API Reference Summary

### Required Headers

| Header | Purpose |
|--------|---------|
| Robot.h | Robot base class, virtual methods, action methods |
| RobotTypes.h | Type definitions, constants, tuning structs |
| Logger.h | Logging functions (Log, LogError, LogDebug) |
| Trigonometry.h | Navigation utilities (direction, distance, move) |

### Key Implementation Points

1. **name() MUST be inline** in header to avoid undefined symbols
2. **getRobot() MUST have extern "C"** linkage
3. **Compile with -fPIC** for position-independent code
4. **Link with -shared** to create shared library
5. **All three virtual methods** must be implemented (init, run, name)

---

## Example: Complete Working Robot

Here's a complete, working robot you can use as a template:

```cpp
// SimpleRobot.h
#ifndef SIMPLEROBOT_H_
#define SIMPLEROBOT_H_

#include "Robot.h"

namespace RobotGame {

class SimpleRobot : public Robot {
public:
    SimpleRobot(RobCtrl* robCtrl);
    virtual ~SimpleRobot();

    virtual struct tune_t init(std::string& name) {
        name = myName;
        return myTune;
    };

    virtual void run();
    virtual std::string name() const { return myName; };

private:
    static const struct tune_t myTune;
    static const std::string myName;
};

} // namespace RobotGame

#endif // SIMPLEROBOT_H_
```

```cpp
// SimpleRobot.cpp
#include "SimpleRobot.h"
#include "Logger.h"
#include <cstdlib>

extern "C" RobotGame::Robot* getRobot(RobotGame::RobCtrl* robCtrl) {
    static RobotGame::SimpleRobot* instance = new RobotGame::SimpleRobot(robCtrl);
    return instance;
}

const struct RobotGame::tune_t RobotGame::SimpleRobot::myTune = {2, 2};
const std::string RobotGame::SimpleRobot::myName("SimpleRobot");

RobotGame::SimpleRobot::SimpleRobot(RobCtrl* robCtrl)
: Robot(robCtrl) {
}

RobotGame::SimpleRobot::~SimpleRobot() {
}

void RobotGame::SimpleRobot::run() {
    int tickCount = 0;

    while (true) {
        tickCount++;

        // Log every 100 ticks
        if (tickCount % 100 == 0) {
            Logger::Log(this, std::string("Tick: ") + std::to_string(tickCount));
        }

        // Simple behavior: Move in random direction
        int direction = rand() % 360;
        drive(direction, 50);
    }
}
```

Build and test:
```bash
# Compile
g++ -std=c++11 -fPIC -shared -Iapi-headers \
    -o libsimplerobot.so SimpleRobot.cpp

# Test
./robot-arena libsimplerobot.so
```

---

## Troubleshooting Checklist

- [ ] All three virtual methods implemented (init, run, name)
- [ ] `name()` implemented inline in header
- [ ] `getRobot()` has `extern "C"` linkage
- [ ] Compiled with `-fPIC` flag
- [ ] Linked with `-shared` flag
- [ ] Include paths point to API headers
- [ ] No undefined symbols in plugin (check with `nm -D`)
- [ ] Same architecture as arena (check with `file`)
- [ ] Factory function exported (check with `nm -D`)

---

## Getting Help

If you encounter issues:

1. **Check the error messages** in the troubleshooting section
2. **Verify your symbols** with `nm -D libyourrobot.so`
3. **Review working examples**: Runner, Scanner robots in the arena
4. **Consult the API contract**: See `contracts/robot-plugin-api.md`

---

## Next Steps

- Explore more complex robot behaviors
- Add strategy patterns and state machines
- Implement team-based coordination
- Participate in robot competitions!
