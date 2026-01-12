# Research: Edge Patrol Robot Implementation

**Feature**: 001-edge-patrol-robot
**Date**: 2025-01-12

## Overview

This document consolidates research findings for implementing the Edge Patrol Robot sample plugin. All technical decisions and architectural choices are documented here.

---

## Robot Architecture

### Plugin System

**Decision**: Robots are implemented as shared libraries (.so files) loaded dynamically at runtime.

**Rationale**: The RobotGame framework uses dlopen/dlsym to load robot plugins dynamically. Each robot exports a C-compatible `getRobot` function that returns a singleton robot instance. This allows the game to load any number of robot plugins via command-line arguments.

**Evidence**:
- `RobotGame.cpp:27-43` shows the `loadPlugin` function using `dlopen` and `dlsym`
- Robots are added via command-line arguments in `main()` loop
- Each robot is compiled as a separate shared library

**Alternatives Considered**:
- Static linking to main executable: Would require recompilation for new robots, less flexible
- Multiple inheritance patterns: More complex, unnecessary for this use case

---

## Robot Base Class API

### Available Methods

The `RobotGame::Robot` base class provides the following methods for robot implementations:

**Movement & Navigation**:
- `drive(angle_t direction, speed_t speed)` - Order robot to drive in direction at speed
- `getSpeed()` - Returns current actual speed
- `getDirection()` - Returns current actual movement direction
- `getX()` / `getY()` - Returns current position
- `skip()` - End current tick without taking action

**Combat**:
- `scan(angle_t direction, precision_t precision)` - Scan for enemies, returns range or 0 if none detected
- `cannon(angle_t direction, range_t range)` - Fire cannon at direction with range, returns 1 if fired

**State**:
- `getArmor()` - Returns remaining HP (0 = dead)
- `getEnergy()` - Returns current energy level
- `getTick()` - Returns current game tick number
- `armorToEnergy(armor_t)` / `energyToArmor(energy_t)` - Convert between armor and energy

**Constants** (from RobotTypes.h):
- `MAX_POS_X = 9999`, `MAX_POS_Y = 9999` - Arena dimensions
- `MAX_SPEED = 100` - Maximum speed
- `MAX_SHOOT = 3` - Maximum shots in air simultaneously
- `SHOOT_ENERGY_COST = 200` - Energy required to fire
- `SHELL_SPEED = 250` - Projectile speed
- `MAX_PRECISION = 15` - Maximum scan precision
- `MAX_ARMOR = 100`, `MAX_ENERGY = 10000`
- `MAX_DIR_CHANGE_SPEED = 50` - Max speed to allow direction change
- `MAX_DIR_CHANGE = 45` - Maximum direction change per tick

**Evidence**: `Robot.h` and `RobotTypes.h` define the complete API

---

## Trigonometry Utilities

### Available Static Methods

The `RobotGame::Trigonometry` class provides utility functions:

- `direction(x1, y1, x2, y2)` - Returns compass direction from (x1,y1) to (x2,y2)
- `distance(x1, y1, x2, y2)` - Returns distance between two points
- `compassUnify(angle_t)` - Normalizes angle to [0,360[ range
- `compassToUnity(angle_t)` / `unityToCompass(double)` - Angle conversions

**Compass Direction Convention**:
- North = 0°, East = 90°, South = 180°, West = 270°
- Y-axis is reversed compared to standard unit circle

**Evidence**: `Trigonometry.h` and `Trigonometry.cpp` implement these functions

---

## Edge Patrol Algorithm

### Requirements

1. Navigate along arena edges maintaining consistent distance from walls
2. Navigate smoothly around corners without stopping/reversing
3. Detect and avoid obstacles near edges
4. Handle different arena sizes (configurable)

### Implementation Strategy

**Decision**: Use finite state machine (FSM) with patrol states for each edge.

**Rationale**: Edge patrol requires distinct behaviors for different edges and corners. An FSM provides clear state transitions and is easy to debug and test.

**States**:
1. `PATROL_TOP_EDGE` - Moving east or west along top edge
2. `PATROL_RIGHT_EDGE` - Moving north or south along right edge
3. `PATROL_BOTTOM_EDGE` - Moving east or west along bottom edge
4. `PATROL_LEFT_EDGE` - Moving north or south along left edge
5. `AVOID_OBSTACLE` - Temporary state when obstacle detected

**Algorithm**:
1. Start at nearest edge from spawn position
2. Move along edge at constant distance from wall (e.g., 500-1000 units)
3. Detect upcoming corners via position monitoring
4. Transition to next edge state smoothly (direction change ≤45° per tick)
5. If obstacle detected in path, temporarily switch to AVOID_OBSTACLE state
6. After obstacle avoidance, return to edge patrol

**Edge Distance**: Use configurable parameter (default 800 units from wall)

**Corner Navigation**:
- When approaching corner (within 500 units), calculate direction to next edge
- Reduce speed to ≤50 to enable direction change
- Turn toward new edge direction
- Accelerate back to patrol speed

**Obstacle Avoidance**:
- Use scan() in movement direction with precision 5°
- If obstacle detected at range < 1500 units:
  - Turn 45° away from wall (toward center of arena)
  - Move forward 2000 units
  - Turn back toward edge
  - Resume patrol

**Evidence**: Runner robot demonstrates edge detection logic in `Runner.cpp:81-94`

---

## Center-Focused Shooting Algorithm

### Requirements

1. Continuously fire projectiles toward arena center when no threats detected
2. Maintain consistent fire rate (≥2 shots per second per spec)
3. Aim at center from current patrol position
4. Adjust shooting angle as robot moves

### Implementation Strategy

**Decision**: Calculate center direction dynamically each tick and fire when energy allows.

**Rationale**: Simple and efficient approach. The center is fixed at (4999.5, 4999.5) for 9999x9999 arena. Trigonometry::direction() provides accurate bearing.

**Algorithm**:
```cpp
angle_t centerDirection = Trigonometry::direction(getX(), getY(), MAX_POS_X/2, MAX_POS_Y/2);
angle_t predictedFutureDirection = centerDirection;

// Adjust for robot movement to lead the target
if (getSpeed() > 50) {
    // Calculate where robot will be in ~5 ticks
    int futureX, futureY;
    Trigonometry::move(getX(), getY(), getDirection(), getSpeed() * 5, &futureX, &futureY);
    predictedFutureDirection = Trigonometry::direction(futureX, futureY, MAX_POS_X/2, MAX_POS_Y/2);
}

// Fire at center if enough energy
if (getEnergy() >= SHOOT_ENERGY_COST) {
    cannon(predictedFutureDirection, 6000); // Maximum effective range
}
```

**Range Selection**: Use 6000 units (near maximum of 7000) to cover most of arena from edges.

**Evidence**: Scanner robot demonstrates scan-shoot pattern in `Scanner.cpp:33-59`

---

## Threat Detection Algorithm

### Requirements

1. Detect other robots within ±10 degrees of movement direction
2. Detect robots within configurable range (default 6000 units)
3. Switch targeting from center to threat when detected
4. Resume center targeting when threat exits detection angle or range

### Implementation Strategy

**Decision**: Use bidirectional scanning around movement direction to detect threats.

**Rationale**: The scan API provides a sector scan capability. Scanning in movement direction with ±10° precision efficiently detects threats in the robot's path.

**Algorithm**:
```cpp
angle_t moveDir = getDirection();
angle_t scanDir = moveDir;
precision_t precision = 10; // ±10 degrees
range_t threatRange = scan(scanDir, precision);

if (threatRange > 0 && threatRange <= detectionRange) {
    // Threat detected - fire at threat
    cannon(scanDir, threatRange);
} else {
    // No threat - fire at center
    fireAtCenter();
}
```

**Detection Range**: Configurable, default 6000 units (covers most of arena).

**Precision Limitation**: Scan API maximum precision is 15° (from RobotTypes.h), so 10° is within limits.

**Energy Management**: Check energy before each fire to avoid wasting energy on missed shots.

**Evidence**: Robot.h:40 describes scan API: "scans in direction with +/- precision"

---

## Tuning Configuration

### Robot Tuning Parameters

Each robot provides a `tune_t` struct with two parameters:

```cpp
struct tune_t {
    int tune_speed;   // 0..2 => max speed (50, 75, or 100)
    int tune_cannon;  // 0..2 => max range (5000, 6000, or 7000)
};
```

**Constraint**: Sum of parameters must be ≤2 (from RobotTypes.h:48)

### Recommended Tuning for Edge Patrol

**Decision**: Use balanced tuning: `tune_speed = 1` (75), `tune_cannon = 1` (6000)

**Rationale**:
- Speed of 75 allows fast edge patrol while maintaining control for corner turns
- Range of 6000 covers most of arena from edge positions
- Sum = 2, satisfies constraint

**Alternative**: `tune_speed = 2` (100), `tune_cannon = 0` (5000) - faster movement but shorter range

**Evidence**: Tune constraints in `RobotTypes.h:47-52`, example tunings in Runner (`{2,0}`) and Scanner (`{0,2}`)

---

## Build Integration

### Build System

**Decision**: Modify Eclipse CDT project configuration to include EdgePatrol robot in build.

**Rationale**: The project uses Eclipse CDT with GNU Make builder. The Robots/ directory is currently excluded from source entries, indicating robots are built separately.

**Build Configuration**:
1. Add EdgePatrol directory to project source entries
2. Create Makefile or modify existing build script to compile EdgePatrol as shared library
3. Link against RobotGame base classes and utilities
4. Output EdgePatrol.so to appropriate location

**Compiler Flags**: `-c -fmessage-length=0 -std=c++11 -pthread` (from .cproject:26)

**Linker Flags**: `-rdynamic -pthread -ldl -lSDL2 -lSDL2_gfx -lSDL2_image -lSDL2_ttf` (from .cproject:37-44)

**Compiler Command Pattern** (based on existing robots):
```bash
g++ -std=c++11 -fPIC -c -I../ EdgePatrol.cpp -o EdgePatrol.o
g++ -shared -o EdgePatrol.so EdgePatrol.o -ldl
```

**Evidence**: .cproject defines build configuration for Debug and Release configurations

---

## Integration with Existing Robots

### Directory Structure

**Decision**: Create `Robots/EdgePatrol/` directory following existing pattern.

**Rationale**: Maintains consistency with existing sample robots (Runner, Scanner) and makes discovery easy for users.

**Files to Create**:
- `Robots/EdgePatrol/EdgePatrol.h` - Class declaration
- `Robots/EdgePatrol/EdgePatrol.cpp` - Implementation including `getRobot()` factory function

**Pattern to Follow** (from Runner):
```cpp
// Header
class EdgePatrol: public RobotGame::Robot {
public:
    EdgePatrol(RobotGame::RobCtrl* robCtrl);
    virtual ~EdgePatrol();
    virtual struct RobotGame::tune_t init(std::string& name);
    virtual void run();
    virtual std::string name() const;
private:
    static const struct RobotGame::tune_t myTune;
    static const std::string myName;
};

// Implementation
extern "C" RobotGame::Robot* getRobot(RobotGame::RobCtrl* robCtrl) {
    static EdgePatrol* instance = new EdgePatrol(robCtrl);
    return instance;
}

const struct RobotGame::tune_t EdgePatrol::myTune = {1, 1};
const std::string EdgePatrol::myName("EdgePatrol");
```

**Evidence**: Runner and Scanner demonstrate this exact pattern

---

## Testing Strategy

### Manual Testing Approach

**Decision**: Test by running game with EdgePatrol robot and observing behavior.

**Rationale**: RobotGame is a visual simulation. Manual testing allows verification of complex behaviors like edge patrol and adaptive targeting in real game conditions.

**Test Scenarios**:
1. **Empty Arena**: Verify robot completes full circuit of edges
2. **Center Shooting**: Verify robot fires consistently toward center while patrolling
3. **Obstacle Avoidance**: Place obstacles near edges, verify robot navigates around them
4. **Threat Detection**: Add second robot, verify targeting switches when threat enters detection angle
5. **Multiple Arenas**: Test with different arena sizes (if configurable)
6. **Corner Navigation**: Verify smooth corner turns without stopping

**Running Tests**:
```bash
# Build EdgePatrol.so
# Run game with EdgePatrol robot
./RobotGame Robots/EdgePatrol/EdgePatrol.so

# Add second robot to test threat detection
./RobotGame Robots/EdgePatrol/EdgePatrol.so Robots/Runner/Runner.so
```

**Verification Criteria**:
- Robot completes at least 5 edge circuits without collision
- Fire rate ≥2 shots per second
- Targeting switches within 100ms when threat enters/exits detection angle
- Edge patrol maintained ≥95% of session time

**Evidence**: Spec success criteria (SC-001 through SC-008) define measurable outcomes

---

## Unknowns Resolved

All technical unknowns from the spec have been resolved through this research:

1. ✅ Language/Version: C++11 (from .cproject)
2. ✅ Build System: Eclipse CDT with GNU Make
3. ✅ Robot Architecture: Shared library plugins with dlopen
4. ✅ API Methods: Documented from Robot.h and RobotTypes.h
5. ✅ Trigonometry Utilities: Available from Trigonometry class
6. ✅ Directory Structure: Follows Runner/Scanner pattern
7. ✅ Build Integration: Modify .cproject or add Makefile
8. ✅ Testing: Manual testing via game execution

**Status**: Ready to proceed to Phase 1 (Design & Contracts)
