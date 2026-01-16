# Robot Plugin API Contract

**Version**: 1.0
**Feature**: 001-fix-undefined-symbols
**Status**: Active

---

## Overview

This contract defines the interface for creating robot plugins that can be loaded dynamically into RobotGame. Robot plugins are shared libraries (.so files) that implement the Robot interface and export a factory function.

---

## Plugin Requirements

### Mandatory Components

1. **Shared Library Format**
   - Must be compiled as a shared object (.so file)
   - Must use position-independent code (-fPIC)
   - Must be loadable via dlopen()

2. **Factory Function**
   ```cpp
   extern "C" RobotGame::Robot* getRobot(RobotGame::RobCtrl* robCtrl);
   ```
   - Must be exported with C linkage
   - Must return a singleton instance of the robot
   - Function signature must match exactly

3. **Virtual Methods**
   - `virtual struct RobotGame::tune_t init(std::string& name) = 0`
   - `virtual void run() = 0`
   - `virtual std::string name() const = 0`
   - All three must be implemented

---

## Robot Base Class API

### Pure Virtual Methods (Must Implement)

#### init()

```cpp
virtual struct tune_t init(std::string& name) = 0;
```

**Purpose**: Initialize robot and return configuration

**Parameters**:
- `name` (std::string&): Output parameter - robot name will be set by this method

**Returns**:
- `tune_t` struct containing:
  - `tune_speed` (int): Speed tuning (0-2)
  - `tune_cannon` (int): Cannon range tuning (0-2)

**Constraints**:
- Must set `name` parameter
- Must return valid `tune_t` with sum ≤ 2

**Implementation Example**:
```cpp
struct RobotGame::tune_t MyRobot::init(std::string& name) {
    name = myName;
    return myTune;
}
```

---

#### run()

```cpp
virtual void run() = 0;
```

**Purpose**: Main robot execution loop

**Behavior**:
- Called once at robot startup
- Runs in infinite loop
- Must call at least one "ending" method per tick (see below)

**Ending Methods** (call one per tick):
- `skip()` - End tick without action
- `cannon()` - End tick with cannon shot
- `drive()` - End tick with movement (if speed > 0)

**Implementation Example**:
```cpp
void MyRobot::run() {
    while (true) {
        // Robot logic here
        if (shouldShoot) {
            cannon(direction, range);
        } else {
            drive(direction, speed);
        }
    }
}
```

---

#### name()

```cpp
virtual std::string name() const = 0;
```

**Purpose**: Return robot identifier

**Returns**: Robot name as string

**Implementation Pattern** (CRITICAL):
```cpp
// Option 1: Inline in header (RECOMMENDED)
virtual std::string name() const { return myName; };

// Option 2: Using string literal
virtual std::string name() const { return "MYROBOT"; };
```

**IMPORTANT**: Must be implemented inline in header to avoid undefined symbol errors.

---

## Robot Action Methods (Available in run())

### scan()

```cpp
range_t scan(angle_t direction, precision_t precision);
```

**Purpose**: Scan for enemies in given direction

**Parameters**:
- `direction` (angle_t): Compass direction (0-359 degrees)
  - 0 = North, 90 = East, 180 = South, 270 = West
- `precision` (precision_t): Scanning precision (±1 to ±15 degrees)

**Returns**:
- `range_t`: Distance to nearest enemy, or 0 if none detected

**Example**:
```cpp
range_t enemyDist = scan(45, 10);  // Scan NE with ±10° precision
if (enemyDist > 0) {
    // Enemy found!
}
```

---

### cannon()

```cpp
int cannon(angle_t direction, range_t range);
```

**Purpose**: Fire cannon in specified direction

**Parameters**:
- `direction` (angle_t): Firing direction (0-359 degrees)
- `range` (range_t): Firing range (typically 5000-7000)

**Returns**:
- `int`: 1 if cannon fired, 0 if cannot fire

**Constraints**:
- Costs 200 energy points
- Max 3 shots in air simultaneously
- Blast radius: 40 units
- Shell speed: 250 units/tick

**Example**:
```cpp
if (getEnergy() >= SHOOT_ENERGY_COST) {
    cannon(enemyDirection, enemyRange);
}
```

---

### drive()

```cpp
void drive(angle_t direction, speed_t speed);
```

**Purpose**: Move robot in given direction at given speed

**Parameters**:
- `direction` (angle_t): Movement direction (0-359 degrees)
- `speed` (speed_t): Movement speed (-75 to 100 units/tick)

**Behavior**:
- Acceleration: max 20 units/tick²
- Direction change: only allowed if speed ≤ 50 units/tick
- If speed > 50 and turn requested, robot decelerates to 50, turns, then accelerates

**Example**:
```cpp
drive(90, 75);  // Move East at 75 units/tick
```

---

### skip()

```cpp
void skip();
```

**Purpose**: End tick without performing action

**Use Cases**:
- Waiting for energy to recharge
- Observing without moving or shooting
- When no action is required

**Example**:
```cpp
if (getEnergy() < SHOOT_ENERGY_COST) {
    skip();  // Wait for energy
}
```

---

## Robot Query Methods

### getSpeed()

```cpp
speed_t getSpeed();
```
**Returns**: Current speed in units/tick

### getDirection()

```cpp
angle_t getDirection();
```
**Returns**: Current heading in degrees (0-359)

### getX()

```cpp
unsigned int getX();
```
**Returns**: Current X coordinate (0-9999)

### getY()

```cpp
unsigned int getY();
```
**Returns**: Current Y coordinate (0-9999)

### getTick()

```cpp
int getTick();
```
**Returns**: Current game tick number

### getArmor()

```cpp
armor_t getArmor();
```
**Returns**: Current armor (0-100 HP)
- Regenerates 1 HP/tick
- 0 HP = robot dead

### getEnergy()

```cpp
energy_t getEnergy();
```
**Returns**: Current energy (0-10000)
- Regenerates 50 energy/tick
- Movement costs energy proportional to speed
- Cannon costs 200 energy

---

## Energy/Armor Conversion

### armorToEnergy()

```cpp
void armorToEnergy(armor_t sell);
```
**Purpose**: Convert armor to energy
- Rate: 1 HP = 100 energy points
- **WARNING**: Reduces your armor!

### energyToArmor()

```cpp
void energyToArmor(energy_t sell);
```
**Purpose**: Convert energy to armor
- Rate: 100 energy points = 1 HP
- Maximum armor: 100 HP

---

## Logging API

### Log with Robot Reference

```cpp
static void Logger::Log(const Robot* robot, std::string msg);
```
**Purpose**: Log message with robot context

**Example**:
```cpp
Logger::Log(this, "Scanning for enemies");
```

### Log with Name

```cpp
static void Logger::Log(std::string name, std::string msg);
```
**Example**:
```cpp
Logger::Log("MyRobot", "Initialization complete");
```

### Log Levels

```cpp
static void Logger::LogError(std::string error);
static void Logger::LogWarning(std::string warning);
static void Logger::LogDebug(std::string msg);
```

---

## Trigonometry Utilities

### direction()

```cpp
static angle_t Trigonometry::direction(int x1, int y1, int x2, int y2);
```
**Purpose**: Calculate compass direction from point (x1,y1) to point (x2,y2)
**Returns**: Direction in degrees (0-359)

**Example**:
```cpp
angle_t toEnemy = Trigonometry::direction(getX(), getY(), enemyX, enemyY);
```

### distance()

```cpp
static range_t Trigonometry::distance(int x1, int y1, int x2, int y2);
```
**Purpose**: Calculate distance between two points
**Returns**: Distance in units

**Example**:
```cpp
range_t dist = Trigonometry::distance(getX(), getY(), targetX, targetY);
```

### move()

```cpp
static void Trigonometry::move(int x, int y, angle_t direction, range_t distance, int* newX, int* newY);
```
**Purpose**: Calculate new position after moving

**Parameters**:
- `x, y`: Starting position
- `direction`: Movement direction
- `distance`: Distance to move
- `newX, newY`: Output parameters for new position

**Example**:
```cpp
int newX, newY;
Trigonometry::move(getX(), getY(), getDirection(), 2000, &newX, &newY);
```

---

## Type Definitions

```cpp
typedef int angle_t;      // Direction in degrees (0-359)
typedef int precision_t;  // Scan precision (±1 to ±15)
typedef unsigned int range_t;  // Distance in units
typedef int speed_t;      // Speed in units/tick (-75 to 100)
typedef int armor_t;      // Health points (0-100)
typedef int energy_t;     // Energy points (0-10000)

struct tune_t {
    int tune_speed;   // 0-2
    int tune_cannon; // 0-2
};
```

---

## Constants

```cpp
const posx_t MAX_POS_X = 9999;
const posy_t MAX_POS_Y = 9999;

const armor_t MAX_ARMOR = 100;
const energy_t MAX_ENERGY = 10000;
const energy_t SHOOT_ENERGY_COST = 200;

const speed_t MAX_SPEED = 100;
const speed_t MAX_DIR_CHANGE_SPEED = 50;

const unsigned int MAX_SHOOT = 3;  // Max shells in air
```

---

## Error Handling

### Build-Time Errors
- **Undefined Symbol Error**: Virtual method declared but not implemented
  - **Fix**: Implement all virtual methods, preferably inline in header
- **Missing Factory Function**: `getRobot()` not found
  - **Fix**: Ensure `extern "C"` linkage and correct signature

### Runtime Errors
- **Plugin Load Failed**: dlopen() returns NULL
  - **Check**: Architecture mismatch, missing dependencies
- **Symbol Resolution Failed**: Undefined symbol at runtime
  - **Fix**: Ensure all required methods are implemented

---

## Best Practices

1. **Implement `name()` inline in header** to avoid undefined symbol errors
2. **Check energy before shooting** to avoid wasting shots
3. **Regenerate armor strategically** - don't reduce armor too low
4. **Use Trigonometry utilities** for navigation calculations
5. **Log important events** for debugging
6. **Handle boundaries** (0-9999 coordinate space)
7. **Consider acceleration** when planning movement
8. **Monitor armor** - 0 HP means death

---

## Example Robot Template

```cpp
#ifndef MYROBOT_H_
#define MYROBOT_H_

#include "Robot.h"  // API header

namespace RobotGame {

class MyRobot : public Robot {
public:
    MyRobot(RobCtrl* robCtrl);
    virtual ~MyRobot();

    // CRITICAL: Implement inline in header
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

#endif // MYROBOT_H_
```

```cpp
#include "MyRobot.h"
#include "Logger.h"
#include <cstdlib>

extern "C" RobotGame::Robot* getRobot(RobotGame::RobCtrl* robCtrl) {
    static RobotGame::MyRobot* instance = new RobotGame::MyRobot(robCtrl);
    return instance;
}

const struct RobotGame::tune_t RobotGame::MyRobot::myTune = {1, 1};
const std::string RobotGame::MyRobot::myName("MyRobot");

RobotGame::MyRobot::MyRobot(RobCtrl* robCtrl)
: Robot(robCtrl) {
}

RobotGame::MyRobot::~MyRobot() {
}

void RobotGame::MyRobot::run() {
    while (true) {
        // Robot logic here
        Logger::Log(this, "Running...");
        skip();
    }
}
```
