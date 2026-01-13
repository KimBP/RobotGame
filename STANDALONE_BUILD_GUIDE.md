# Standalone Robot Development Guide

## Overview

This guide demonstrates how to develop and build robots independently from the arena codebase. Robots can be compiled with only the Robot.h API header file - no arena source code required.

## Requirements for Standalone Development

### Required Files
- `Robot.h` - The complete API interface for robot development
- `RobotTypes.h` - Type definitions and constants
- `Logger.h` - Debug logging interface for robot development
- Your robot implementation files

### NOT Required
- `Robot.cpp` or any other arena implementation files
- Arena source code
- Internal arena headers (except Robot.h, RobotTypes.h, Logger.h)

## Building Robots Standalone

### Step 1: Setup Development Environment

```bash
# Create a directory for your robot
mkdir my_robot
cd my_robot

# Copy the API headers (only these needed!)
cp /path/to/arena/Robot.h .
cp /path/to/arena/RobotTypes.h .
```

### Step 2: Create Your Robot

Create a header file (e.g., `MyRobot.h`):

```cpp
#include "Robot.h"

class MyRobot: public RobotGame::Robot {
public:
    MyRobot(RobotGame::RobCtrl* robCtrl);
    virtual ~MyRobot();

    virtual struct RobotGame::tune_t init(std::string& name);
    virtual void run();
    virtual std::string name() const;

private:
    static const struct RobotGame::tune_t myTune;
    static const std::string myName;
};
```

Create implementation file (e.g., `MyRobot.cpp`):

```cpp
#include "MyRobot.h"
#include <ctime>
#include <cstdlib>

extern "C" __attribute__((visibility("default"))) RobotGame::Robot* getRobot(RobotGame::RobCtrl* robCtrl) {
    static MyRobot* instance = new MyRobot(robCtrl);
    return instance;
}

// Robot implementation...
```

### Step 3: Compile Your Robot

```bash
# Compile to object file (works with only Robot.h)
g++ -std=c++17 -fPIC -c MyRobot.cpp -o MyRobot.o

# Create shared library
g++ -std=c++17 -shared MyRobot.o -o libmyrobot.so
```

## Runtime Requirements

While robots can be compiled standalone, at runtime they need:

1. **Robot Implementation**: The `librobot-core.so` from the arena provides Robot method implementations
2. **Arena Environment**: The robot arena loads and manages robot instances

## Example: Complete Standalone Build

```bash
#!/bin/bash
# Complete standalone robot build example

# Setup
mkdir -p /tmp/robot_build
cd /tmp/robot_build

# Get API files (only these!)
cp /path/to/robot-arena/Robot.h .
cp /path/to/robot-arena/RobotTypes.h .
cp /path/to/robot-arena/Logger.h .
cp /path/to/robot-arena/safequeue.h .

# Create robot files
cat > TestRobot.h << 'EOF'
#include "Robot.h"

class TestRobot: public RobotGame::Robot {
public:
    TestRobot(RobotGame::RobCtrl* robCtrl);
    virtual ~TestRobot();
    
    virtual struct RobotGame::tune_t init(std::string& name) {
        name = myName;
        return myTune;
    }
    virtual void run();
    virtual std::string name() const { return myName; }

private:
    static const struct RobotGame::tune_t myTune;
    static const std::string myName;
};
EOF

cat > TestRobot.cpp << 'EOF'
#include "TestRobot.h"

extern "C" __attribute__((visibility("default"))) RobotGame::Robot* getRobot(RobotGame::RobCtrl* robCtrl) {
    static TestRobot* instance = new TestRobot(robCtrl);
    return instance;
}

const struct RobotGame::tune_t TestRobot::myTune = {2, 2};
const std::string TestRobot::myName("TestRobot");

TestRobot::TestRobot(RobotGame::RobCtrl* robCtrl)
: RobotGame::Robot(robCtrl) {}

TestRobot::~TestRobot() {}

void TestRobot::run() {
    while (true) {
        drive(0, 50);  // Drive north at speed 50
        skip();
    }
}
EOF

# Compile (works with only Robot.h!)
g++ -std=c++17 -fPIC -c TestRobot.cpp -o TestRobot.o
g++ -std=c++17 -shared TestRobot.o -o libtestrobot.so

echo "✅ Robot built successfully with only Robot.h!"
echo "📁 Ready for deployment to robot arena"
```

## Architecture Benefits

### For Robot Developers
- **No Arena Dependencies**: Develop robots without accessing arena source code
- **Clean API**: Only need Robot.h interface definitions
- **Isolated Development**: Work independently from arena development
- **Standard C++**: Uses only standard C++17 and Robot API

### For Arena System  
- **Plugin Architecture**: Robots are dynamically loaded shared libraries
- **Version Compatibility**: Robot.h provides stable API interface
- **Runtime Flexibility**: Multiple robots can coexist in same arena

## Loading Robots in Arena

```bash
# Run arena with your robot
./robot-arena /path/to/your/libmyrobot.so

# Multiple robots
./robot-arena robot1.so robot2.so robot3.so
```

## Troubleshooting

### Compilation Errors
- **"Robot.h not found"**: Ensure Robot.h is in your include path
- **"Logger.h not found"**: Ensure Logger.h and safequeue.h are in your include path
- **"Undefined symbols"**: Normal during standalone compile, resolved at runtime

### Runtime Errors  
- **"undefined symbol"**: Ensure arena's librobot-core.so is available
- **"cannot open shared object"**: Check library path and permissions

## API Reference

See `Robot.h` for complete API documentation including:

- **Movement**: `drive()`, `getSpeed()`, `getDirection()`, `getX()`, `getY()`
- **Combat**: `cannon()`, `scan()`
- **Status**: `getArmor()`, `getEnergy()`, `getTick()`
- **Resource Management**: `armorToEnergy()`, `energyToArmor()`
- **Control**: `skip()`

All methods are documented with parameters, return values, and usage examples.