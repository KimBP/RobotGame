# Robot Development Rules

## Overview

This document provides the rules and guidelines for creating robots for the RobotGame system. Following these rules ensures your robot will work correctly and integrate well with the game environment.

## Core Requirements

### 1. Plugin Architecture

- Every robot must be implemented as a shared library (.so file)
- Must inherit from the Robot class defined in Robot.h
- Must export a C-style factory function: `extern "C" Robot* getRobot(RobCtrl* robCtrl)`
- All robot behaviors must go through the RobCtrl interface (no direct system calls)

### 2. Required Methods

Your robot class must implement these virtual methods:

#### `init(std::string& name)`
- Called once at the start of the game
- Set your robot's name via the `name` parameter
- Return a `tune_t` struct specifying your robot's capabilities
- Initialize any internal state variables here

#### `run()`
- Called every game tick (main robot logic)
- This is where your robot's behavior is implemented
- Use Robot class methods to interact with the game world
- Must return void

#### `name() const`
- Returns your robot's name as a string
- Used for identification and logging

## Robot API Reference

### Movement and Control

#### `drive(angle_t direction, speed_t speed)`
- Move your robot in the specified direction at the given speed
- `direction`: 0-359 degrees (0=North, 90=East, 180=South, 270=West)
- `speed`: -75 to 100 units/tick (positive=forward, negative=backward)
- Max acceleration: 20 units/tick/tick
- Direction changes only possible when speed ≤ 50 units/tick
- Example: `drive(45, 30)` - Move Northeast at medium speed

#### `skip()`
- End the current tick without taking other actions
- Useful for waiting or strategic timing

### Combat

#### `scan(angle_t direction, precision_t precision)`
- Scan for enemies in the specified direction
- `direction`: 0-359 degrees
- `precision`: 1-15 degrees (± accuracy)
- Returns: range to nearest enemy, or 0 if no enemy detected
- Example: `scan(0, 10)` - Scan North with ±10 degree precision

#### `cannon(angle_t direction, range_t range)`
- Fire cannon in the specified direction
- `direction`: 0-359 degrees for firing direction
- `range`: distance to fire (up to your robot's max range)
- Returns: 1 if cannon fired, 0 if cannot fire
- Cannon shot travels 140 units/tick with blast radius 40
- Maximum 3 shots can be active at once
- Energy cost: 200 points per shot

### Status and Information

#### `getSpeed()` - Returns current speed (-75 to 100)
#### `getDirection()` - Returns current facing direction (0-359)
#### `getX()` - Returns current X position (0-9999)
#### `getY()` - Returns current Y position (0-9999)
#### `getTick()` - Returns current tick number
#### `getArmor()` - Returns current armor/health (0-100)
#### `getEnergy()` - Returns current energy points (0-10000)

### Resource Management

#### `armorToEnergy(armor_t amount)`
- Convert armor points to energy
- Rate: 1 armor → 100 energy points
- Useful when you have excess armor and need energy

#### `energyToArmor(energy_t amount)`
- Convert energy points to armor
- Rate: 100 energy → 1 armor point
- Useful for repairing damage

## Game Constants

### Physics and Movement
- Max speed: 100 units/tick
- Max acceleration: 20 units/tick/tick
- Max direction change speed: 50 units/tick
- Max direction change per tick: 45 degrees
- Max reverse speed: -50 units/tick

### Combat
- Cannon shot speed: 250 units/tick
- Cannon blast radius: 40 units
- Max active shots: 3
- Energy cost per shot: 200 points
- Damage ranges and armor costs:
  - 0-10 units: 8 armor damage
  - 11-20 units: 4 armor damage
  - 21-30 units: 2 armor damage
  - 31-40 units: 1 armor damage

### Resource Management
- Energy regeneration: 50 points/tick
- Energy maximum: 10,000 points
- Armor maximum: 100 points
- Fill-up energy: 50 points/tick (free energy from solar panels)

### Collision Damage
- Wall collision damage: (2 × speed) ÷ 10 armor points
- Robot collision damage: 2 × (speed₁ + speed₂) ÷ 10 armor points

## Development Best Practices

### 1. Energy Management
- Always check `getEnergy()` before firing cannon (costs 200 energy)
- Balance between aggressive behavior (cannon) and conservative (energy saving)
- Use `armorToEnergy()` when you have excess armor

### 2. Movement Strategy
- Use `scan()` before moving to detect threats
- Combine turning and forward movement for navigation
- Consider speed limits when planning complex maneuvers

### 3. Combat Tactics
- Fire cannon when enemies are within effective range (under 500 units)
- Lead targets based on their movement direction
- Manage your 3-shot limit wisely

### 4. Error Handling
- Check return values from all Robot methods
- Handle failed scans (range = 0) appropriately
- Don't assume cannon will always fire (check return value)

## Compilation Instructions

### Building Your Robot

```bash
# From package root:
g++ -shared -fPIC -o lib/MyRobot.so MyRobot.cpp -I.

# Or using CMake (if you have a CMakeLists.txt):
cd MyRobot
mkdir build
cd build
cmake ..
cmake --build .
```

### Testing Your Robot

```bash
# Run with included robots:
./bin/robot-arena --robots lib/librunner.so lib/libscanner.so lib/MyRobot.so

# Run with just your robot:
./bin/robot-arena --robots lib/MyRobot.so
```

## Common Pitfalls to Avoid

1. **Memory Leaks**: Ensure you don't leak memory in your robot code
2. **Infinite Loops**: Avoid blocking operations in `run()` method
3. **Invalid Ranges**: Always check scan results before using them
4. **Energy Management**: Don't fire cannon without checking energy levels
5. **Direction Calculation**: Ensure angles are within 0-359 degree range

## Support

For questions or issues:
- Check existing example robots (Runner, Scanner) for reference
- Review Robot.h and RobotTypes.h for complete API
- Test incrementally - start simple, then add complexity