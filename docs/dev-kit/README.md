# Robot Development Kit

## Overview

This package contains everything you need to develop, test, and deploy robot behaviors for the RobotGame system. Extract this package anywhere and start building robots immediately - no additional downloads or repository access required.

## Package Contents

```
robot-dev-kit/
├── README.md              # This file - quick start guide
├── MyRobot.cpp            # Robot template (your starting point)
├── ROBOT_RULES.md        # Complete robot development rules
├── include/
│   ├── Logger.h         # Logging utilities
│   ├── Robot.h           # Robot API header
│   ├── RobotTypes.h     # Constants and type definitions
│   ├── safequeue.h      # Thread-safe queue implementation
│   └── Trigonometry.h   # Trigonometry helper functions
├── bin/
│   └── robot-arena      # Game executable for testing
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

## 30-Minute Quick Start

### Step 1: Extract and Explore (5 minutes)

```bash
# Extract anywhere
tar -xzf robot-dev-kit.tar.gz
cd robot-dev-kit

# Verify contents
ls -la
# Should see README.md, include/, MyRobot.cpp, etc.
```

### Step 2: Test Existing Robots (5 minutes)

```bash
# Build example robots
cd robots/Runner
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH=../..
cmake --build .

cd ../../Scanner
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH=../..
cmake --build .

cd ../..

# Test with game executable
./bin/robot-arena --robots lib/librunner.so lib/libscanner.so
```

### Step 3: Create Your First Robot (10 minutes)

```bash
# Copy template and customize
cp MyRobot.cpp MyFirstRobot.cpp

# Edit the file with your favorite editor
nano MyFirstRobot.cpp  # or vim, emacs, etc.

# Build your robot
g++ -shared -fPIC -o lib/MyFirstRobot.so MyFirstRobot.cpp -Iinclude
```

### Step 4: Test Your Robot (10 minutes)

```bash
# Run with existing robots
./bin/robot-arena --robots lib/librunner.so lib/libscanner.so lib/MyFirstRobot.so

# Or run only your robot
./bin/robot-arena --robots lib/MyFirstRobot.so
```

## Building Robots

### Method 1: Simple Compilation (Recommended for testing)

```bash
# From package root:
g++ -shared -fPIC -o YourRobot.so YourRobot.cpp -Iinclude

# The -Iinclude flag tells compiler to look for all headers in include/ directory
# The -Iinclude flag tells compiler to look for headers in include/ directory
# -shared creates a shared library (.so file)
# -fPIC ensures position-independent code (required for shared libraries)
```

### Method 2: CMake Build (Recommended for complex robots)

```bash
# Create CMakeLists.txt for your robot:
cmake_minimum_required(VERSION 3.15)
project(YourRobot)

# Find required headers
find_path(RobotHeader PATHS ${CMAKE_CURRENT_SOURCE_DIR}/.. NAMES Robot.h REQUIRED)

# Create shared library
add_library(YourRobot SHARED YourRobot.cpp)
target_include_directories(YourRobot PRIVATE ${RobotHeader_PATH}/include)

# Set output name
set_target_properties(YourRobot PROPERTIES
    OUTPUT_NAME "YourRobot"
    POSITION_INDEPENDENT_CODE ON
)
```

```bash
# Build with CMake:
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH=..
cmake --build .
```

## Robot Development Workflow

### 1. Start with Template
```bash
cp MyRobot.cpp YourNewRobot.cpp
# Edit to implement your behavior
```

### 2. Implement Required Methods
Every robot needs these methods:
- `init()`: Initialize your robot and set capabilities
- `run()`: Main behavior logic (called each tick)
- `name()`: Return robot name
- `getRobot()`: Factory function for plugin loading

### 3. Test Iteratively
```bash
# Build and test frequently
g++ -shared -fPIC -o YourNewRobot.so YourNewRobot.cpp -Iinclude
./bin/robot-arena --robots lib/YourNewRobot.so

# Add complexity gradually
# Start simple, then add features
```

### 4. Debug with Game Executable
```bash
# Run game to see robot behavior
./bin/robot-arena --robots lib/YourNewRobot.so lib/libscanner.so

# Observe movement, combat, and energy management
```

## Robot API Reference

### Key Methods (see ROBOT_RULES.md for details):

```cpp
// Movement
drive(direction, speed)     // Move robot
skip()                      // End current tick

// Combat
scan(direction, precision)     // Look for enemies
cannon(direction, range)       // Fire cannon

// Status
getSpeed()                   // Current speed
getDirection()                // Current direction
getX(), getY()                // Current position
getEnergy()                   // Current energy
getArmor()                   // Current armor

// Resource Management
armorToEnergy(amount)        // Convert armor to energy
energyToArmor(amount)        // Convert energy to armor
```

## Testing Your Robot

### Unit Testing
```bash
# Test compilation in isolation
g++ -shared -fPIC -o TestRobot.so TestRobot.cpp -Iinclude 2>/dev/null
echo $?
```

### Integration Testing
```bash
# Test with different robot combinations
./bin/robot-arena --robots lib/YourRobot.so lib/librunner.so
./bin/robot-arena --robots lib/YourRobot.so lib/libscanner.so
./bin/robot-arena --robots lib/YourRobot.so lib/YourRobot.so  # Multiple instances
```

### Performance Testing
```bash
# Monitor behavior over time
# Check energy efficiency
# Test movement patterns
# Validate combat effectiveness
```

## Common Issues and Solutions

### Compilation Errors
**Problem**: `Robot.h: No such file or directory`
**Solution**: Ensure you're compiling from the correct directory with `-I.` flag

**Problem**: Undefined reference to Robot methods
**Solution**: Check you're inheriting from `RobotGame::Robot` class

**Problem**: Cannot create shared library
**Solution**: Use `-shared -fPIC` flags with g++

### Runtime Errors
**Problem**: Robot not loading
**Solution**: Verify `getRobot()` function is exported with `extern "C"`

**Problem**: Robot shows no behavior
**Solution**: Check your `run()` method implementation

**Problem**: Segmentation fault
**Solution**: Check for null pointer access or array bounds issues

## Advanced Topics

### Energy Management Strategy
- Balance aggressive vs. conservative behavior
- Know when to use armorToEnergy() for energy boost
- Plan cannon shots carefully (200 energy cost each)

### Movement Patterns
- Combine scanning and movement
- Use direction changes strategically
- Account for acceleration limits (20 units/tick/tick)

### Combat Tactics
- Lead targets based on movement prediction
- Manage the 3-shot limit effectively
- Use scan results for tactical decisions

### Multi-Robot Coordination
- Test your robot against different opponents
- Consider team strategies if applicable
- Optimize for various scenarios

## Getting Help

### Documentation Resources
- `ROBOT_RULES.md`: Complete API reference and development rules
- `include/Robot.h`: Header file with class definitions
- `include/RobotTypes.h`: Constants and type definitions
- `include/Logger.h`: Logging utilities
- `include/Trigonometry.h`: Trigonometry helper functions
- Example robots: `robots/Runner/` and `robots/Scanner/`

### Troubleshooting
1. Check compilation includes: `-Iinclude` flag is required
2. Verify shared library creation: use `-shared -fPIC`
3. Test with simple robots before complex ones
4. Use the game executable's debug output for issues
5. Check energy management in your robot logic

### Next Steps
- Study the example robots for advanced patterns
- Experiment with different strategies
- Join the community for shared knowledge
- Contribute your robots back to the ecosystem

---

**Enjoy building robots!** 🤖

This development kit provides everything needed to create sophisticated robot behaviors. Start with the template, study the examples, and iterate based on your results.