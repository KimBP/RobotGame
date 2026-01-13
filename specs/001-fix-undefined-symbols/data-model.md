# Data Model: Fix Undefined Symbols in Robot Shared Libraries

**Feature**: 001-fix-undefined-symbols
**Date**: 2026-01-13

---

## Overview

This document describes the data model for robot plugins and the plugin loading system. The focus is on understanding the entities involved in building standalone robot shared libraries and the symbols that must be properly defined.

---

## Core Entities

### Robot Plugin

**Description**: A dynamically loadable shared library (.so file) containing a robot implementation that can be loaded into the RobotGame at runtime.

**Attributes**:
- `library_path` (string): Filesystem path to the .so file
- `name` (string): Robot identifier returned by `name()` method
- `version` (string, optional): Plugin version information

**Relationships**:
- Must implement all virtual methods from Robot base class
- Must export `getRobot(RobCtrl*)` function with C linkage
- Depends on API headers: Robot.h, RobotTypes.h, Logger.h, Trigonometry.h

**State Machine**:
```
[Compiled] → [Loading] → [Loaded] → [Running] → [Unloaded]
               ↓
            [Failed]
```

**Validation Rules**:
- Must define `name()` method (no undefined symbol allowed)
- Must define `init()` method
- Must define `run()` method
- Must export `getRobot()` function with `extern "C"` linkage
- Must compile as shared library (-shared flag)

---

### Robot Base Class

**Description**: Abstract base class that all robot plugins must inherit from. Defines the contract for robot behavior.

**Attributes**:
- `robCtrl` (RobCtrl*): Pointer to robot controller (private)

**Methods**:
**Pure Virtual (Must be implemented by plugins)**:
- `virtual struct tune_t init(std::string& name) = 0`
- `virtual void run() = 0`
- `virtual std::string name() const = 0`

**Implemented in base class (Available to all robots)**:
- Action methods: `scan()`, `cannon()`, `drive()`, `skip()`
- Query methods: `getSpeed()`, `getDirection()`, `getX()`, `getY()`, `getTick()`, `getArmor()`, `getEnergy()`
- Conversion methods: `armorToEnergy()`, `energyToArmor()`

**Relationships**:
- Base class for all robot implementations
- Has private association with RobCtrl controller

---

### Robot Instance

**Description**: Runtime instance of a robot plugin loaded into the game.

**Attributes**:
- `name` (string): Robot identifier
- `tune_speed` (int): Speed tuning parameter (0-2)
- `tune_cannon` (int): Cannon range tuning parameter (0-2)
- `position_x` (unsigned int): Current X coordinate
- `position_y` (unsigned int): Current Y coordinate
- `speed` (speed_t): Current speed (-75 to 100 units/tick)
- `direction` (angle_t): Current heading (0-359 degrees)
- `armor` (armor_t): Current armor (0-100 HP)
- `energy` (energy_t): Current energy (0-10000)

**Relationships**:
- Created by `getRobot()` factory function
- Controlled by RobCtrl instance
- Logs to Logger system

**State Machine**:
```
[Alive] → [Active] → [Dead]
   ↑            ↓
   └──────────┘
```

---

### API Headers

**Description**: Set of header files that define the complete API for building robot plugins without access to arena source code.

**Components**:

#### Robot.h
- Robot base class definition
- Virtual method declarations
- Inline method implementations
- Type forward declarations

#### RobotTypes.h
- Type definitions (angle_t, range_t, speed_t, armor_t, energy_t, posx_t, posy_t)
- Constants (MAX_POS_X, MAX_POS_Y, MAX_ARMOR, MAX_ENERGY, SHOOT_ENERGY_COST, etc.)
- Struct definitions (tune_t)
- Configuration arrays (speedVals, rangeVals, damageRanges)

#### Logger.h
- Static logging functions
- Log level methods (Log, LogError, LogWarning, LogDebug)
- Robot-specific logging overload

#### Trigonometry.h
- Static utility functions for navigation
- `direction(x1, y1, x2, y2)`: Calculate compass direction
- `distance(x1, y1, x2, y2)`: Calculate distance between points
- `move(x, y, direction, distance, newX, newY)`: Calculate new position
- `compassToUnity()` / `unityToCompass()`: Unit conversions

**Validation Rules**:
- Must be self-contained (no implementation details exposed)
- Must provide all necessary types and constants
- Must not require internal arena headers

---

## Symbol Table

### Exported Symbols (from Robot Plugin)

**Required Exports**:
| Symbol | Type | Description |
|--------|------|-------------|
| `getRobot` | `extern "C" Robot*(RobCtrl*)` | Factory function to create robot instance |

**Virtual Method Symbols** (defined in each robot):
| Symbol | Type | Implementation Location |
|--------|------|------------------------|
| `RobotName::name()` | virtual method | Header (inline) or .cpp |
| `RobotName::init()` | virtual method | .cpp file |
| `RobotName::run()` | virtual method | .cpp file |
| `RobotName::~RobotName()` | virtual method | .cpp file |

**Symbol Visibility**:
- `T` (Text): Defined in this library
- `W` (Weak): Can be overridden (inline functions)
- `U` (Undefined): Must be resolved at load time (ERROR if not found)
- `V` (Weak object): Weak object symbol

### Imported Symbols (from Arena)

Robot plugins import these symbols from the arena at runtime:
- All Robot base class methods (scan, cannon, drive, skip, etc.)
- All Logger methods (Log, LogError, etc.)
- All Trigonometry methods (direction, distance, move)

---

## Build Configuration

### Robot Plugin Build Settings

**Compiler Flags**:
- `-fPIC`: Position-independent code (required for shared library)
- `-std=c++11`: C++11 standard
- `-shared`: Create shared library

**Linker Flags**:
- `-rdynamic`: Export all symbols (may be needed)
- No additional libraries needed for standalone building

**Include Paths**:
- Path to API headers (Robot.h, RobotTypes.h, Logger.h, Trigonometry.h)
- Robot's own directory

**Output**:
- `.so` shared library file
- Dynamically loadable via dlopen()

### Plugin Loader Configuration

**dlopen Flags**:
- `RTLD_LAZY`: Delay symbol resolution until first use (default, recommended)
- `RTLD_NOW`: Resolve all symbols immediately (alternative for stricter checking)

**Symbol Resolution**:
1. Plugin symbols checked for `getRobot()` function
2. All undefined symbols resolved from arena
3. Virtual methods verified to be implemented

---

## Error States

### Undefined Symbol Error

**Description**: Robot plugin has a method declared but not implemented.

**Symptoms**:
- dlopen returns NULL
- dlerror() reports "undefined symbol: ..."
- Plugin fails to load

**Common Causes**:
- Virtual method declared in header but not implemented
- Inline implementation missing in header
- .cpp file missing implementation

**Example**:
```
Error loading plugin EdgePatrol.so error :
EdgePatrol.so: undefined symbol: _ZNK9RobotGame10EdgePatrol4nameB5cxx11Ev
(Demangled: RobotGame::EdgePatrol::name[abi:cxx11]() const)
```

### Missing Factory Function

**Description**: Plugin does not export `getRobot()` function.

**Symptoms**:
- dlsym() returns NULL for "getRobot"
- Error message: "Error finding getRobot() in ..."

**Solution**: Ensure `extern "C"` linkage and correct function signature.

### Wrong Architecture

**Description**: Plugin compiled for different CPU architecture than arena.

**Symptoms**:
- dlopen() returns NULL
- dlerror() reports "wrong ELF class" or similar

**Solution**: Compile plugin with same target architecture as arena.

---

## Data Flow

### Plugin Loading Flow

```
1. User executes: ./robot-arena Robots/EdgePatrol/libedgepatrol.so
                                    ↓
2. RobotGame.cpp calls: loadPlugin("libedgepatrol.so")
                                    ↓
3. dlopen("libedgepatrol.so", RTLD_LAZY)
   - Check for undefined symbols
   - Resolve symbols from arena
                                    ↓
4. dlsym(lib_handle, "getRobot")
   - Find factory function in plugin
                                    ↓
5. fn = getRobot(robCtrl)
   - Create robot instance
   - Call robot->init(name)
   - Start robot->run() thread
                                    ↓
6. Robot executes in game simulation
```

### Robot Execution Flow

```
1. getRobot(robCtrl) → Robot instance
   ↓
2. robot->init(name) → Returns tune_t configuration
   ↓
3. robot->run() → Main execution loop
   - Use scan(), cannon(), drive(), skip() actions
   - Query state with getSpeed(), getX(), etc.
   - Log events with Logger::Log()
   - Use Trigonometry utilities for navigation
   ↓
4. Loop continues until robot dies or game ends
```

---

## Relationships Diagram

```
┌─────────────────┐
│   Arena Exec    │
│  (RobotGame)    │
└────────┬────────┘
         │ loads via dlopen()
         ↓
┌─────────────────┐
│ Robot Plugin   │
│   (.so file)   │
│                │
│ - getRobot()   │←── extern "C" factory
│ - RobotName    │
│ - virtual methods │
└────────┬────────┘
         │ inherits
         ↓
┌─────────────────┐
│  Robot.h API   │
│                │
│ - base class   │
│ - methods      │
│ - types        │
└─────────────────┘

API Dependencies:
┌──────────────────┬──────────────────┬──────────────────┬──────────────────┐
│   Robot.h       │  RobotTypes.h    │   Logger.h       │  Trigonometry.h  │
├──────────────────┼──────────────────┼──────────────────┼──────────────────┤
│ Base class      │ Type defs        │ Logging API      │ Nav utilities    │
│ Virtual methods │ Constants        │ Static methods   │ Static methods   │
│ Inline impls    │ Structs         │                 │                 │
└──────────────────┴──────────────────┴──────────────────┴──────────────────┘
```

---

## Validation Rules Summary

1. **Robot Plugin Compilation**:
   - Must compile with -fPIC -shared flags
   - Must include Robot.h (or path to it)
   - Must implement all three pure virtual methods
   - Must export `getRobot()` function with extern "C"

2. **Symbol Resolution**:
   - No undefined symbols allowed in plugin
   - All virtual methods must be implemented
   - Factory function must be exported

3. **Runtime Loading**:
   - Plugin must be compiled for same architecture as arena
   - All dependent symbols must be available in arena
   - dlopen/dlsym must complete successfully

4. **Standalone Building**:
   - Only API headers required (Robot.h, RobotTypes.h, Logger.h, Trigonometry.h)
   - No access to arena implementation needed
   - No linking against arena binary required
