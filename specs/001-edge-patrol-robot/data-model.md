# Data Model: Edge Patrol Robot

**Feature**: 001-edge-patrol-robot
**Date**: 2025-01-12

## Overview

This document defines the data entities, state structures, and relationships for the Edge Patrol Robot implementation. The robot maintains state for patrol behavior, threat detection, and targeting.

---

## Entities

### EdgePatrol

**Description**: Main robot class implementing edge patrol navigation and adaptive targeting behaviors.

**Base Class**: `RobotGame::Robot`

**Relationships**:
- Inherits from: `RobotGame::Robot`
- Uses: `RobotGame::Trigonometry` for calculations
- Controlled by: `RobotGame::RobCtrl` (provided by game framework)

---

## State Management

### Patrol State

**Description**: Current patrol behavior state determining robot's navigation mode.

**States**:
- `PATROL_TOP_EDGE` - Moving east or west along north edge of arena
- `PATROL_RIGHT_EDGE` - Moving north or south along east edge of arena
- `PATROL_BOTTOM_EDGE` - Moving east or west along south edge of arena
- `PATROL_LEFT_EDGE` - Moving north or south along west edge of arena
- `AVOID_OBSTACLE` - Temporarily navigating around obstacle
- `REACH_EDGE` - Moving from spawn position to nearest edge (initial state)

**State Machine Diagram**:
```
                    [REACH_EDGE]
                          |
                          v
               +-------------------------+
               | [PATROL_{TOP|RIGHT|    |
               |  BOTTOM|LEFT}_EDGE]    |
               +-------------------------+
                          |
                          | (obstacle detected)
                          v
               [AVOID_OBSTACLE]
                          |
                          | (obstacle cleared)
                          v
               [PATROL_{edge}_EDGE]
```

**Transitions**:
- `REACH_EDGE` → `PATROL_{edge}_EDGE` (when robot reaches edge)
- `PATROL_{edge}_EDGE` → `PATROL_{next_edge}_EDGE` (corner reached)
- `PATROL_{edge}_EDGE` → `AVOID_OBSTACLE` (obstacle detected in path)
- `AVOID_OBSTACLE` → `PATROL_{edge}_EDGE` (obstacle navigated)

---

### Movement State

**Description**: Robot's current movement parameters.

**Fields**:
- `currentDirection` (angle_t) - Current movement heading (0-359)
- `currentSpeed` (speed_t) - Current speed (0-100)
- `targetDirection` (angle_t) - Intended direction (may differ during acceleration)
- `targetSpeed` (speed_t) - Intended speed

**Validation Rules**:
- Direction must be normalized to [0,360) using `Trigonometry::compassUnify()`
- Speed must be ≤ robot's tuned maximum (75 for balanced tuning)
- Speed must be ≤50 to allow direction changes (per RobotTypes.h:38)

**Inherited from Base Class**:
- `getX()` - Current X position (0-9999)
- `getY()` - Current Y position (0-9999)
- `getSpeed()` - Current actual speed
- `getDirection()` - Current actual direction

---

### Targeting State

**Description**: Robot's current combat targeting behavior.

**States**:
- `TARGET_CENTER` - Shooting at arena center (default state)
- `TARGET_THREAT` - Shooting at detected enemy robot

**Transitions**:
- `TARGET_CENTER` → `TARGET_THREAT` (robot detected in ±10° detection angle)
- `TARGET_THREAT` → `TARGET_CENTER` (threat exits detection angle or range)

**Fields**:
- `targetMode` (enum) - `TARGET_CENTER` or `TARGET_THREAT`
- `lastThreatAngle` (angle_t) - Last known threat direction
- `lastThreatRange` (range_t) - Last known threat distance
- `shotsInAir` (int) - Count of active cannon shots (0-3)

**Validation Rules**:
- `shotsInAir` must be ≤3 (per RobotTypes.h:13)
- Cannot fire if `shotsInAir == 3`
- Cannot fire if `getEnergy() < SHOOT_ENERGY_COST` (200)

---

### Detection Parameters

**Description**: Configurable parameters for threat detection and edge patrol.

**Fields**:
- `detectionAngle` (precision_t) - Scan angle precision (default: 10 degrees)
- `detectionRange` (range_t) - Maximum detection range (default: 6000 units)
- `edgeDistance` (range_t) - Distance from wall during patrol (default: 800 units)
- `cornerThreshold` (range_t) - Distance from corner to trigger turn (default: 500 units)

**Validation Rules**:
- `detectionAngle` must be ≤15 (MAX_PRECISION from RobotTypes.h:31)
- `detectionRange` must be ≤ robot's tuned maximum (6000 for balanced tuning)
- `edgeDistance` must be < arena dimensions (9999)

**Constraints** (from RobotTypes.h):
- `MAX_PRECISION = 15` - Maximum scan precision
- `damageRanges = {100, 200, 300, 400}` - Damage ranges for blast radius

---

### Edge Patrol Configuration

**Description**: Arena boundaries and patrol route parameters.

**Fields**:
- `arenaWidth` (posx_t) - Arena X dimension (9999)
- `arenaHeight` (posy_t) - Arena Y dimension (9999)
- `centerX` (posx_t) - Arena center X (4999)
- `centerY` (posy_t) - Arena center Y (4999)

**Edge Boundaries**:
- North Edge: Y = `edgeDistance`
- South Edge: Y = `arenaHeight - edgeDistance`
- East Edge: X = `arenaWidth - edgeDistance`
- West Edge: X = `edgeDistance`

**Corner Coordinates**:
- North-West: (`edgeDistance`, `edgeDistance`)
- North-East: (`arenaWidth - edgeDistance`, `edgeDistance`)
- South-East: (`arenaWidth - edgeDistance`, `arenaHeight - edgeDistance`)
- South-West: (`edgeDistance`, `arenaHeight - edgeDistance`)

---

### Tuning Configuration

**Description**: Robot performance tuning parameters.

**Fields**:
- `tuneSpeed` (int) - Speed tuning level (0-2)
- `tuneCannon` (int) - Cannon range tuning level (0-2)

**Speed Levels** (from RobotTypes.h:44):
- Level 0: MAX_SPEED = 50
- Level 1: MAX_SPEED = 75
- Level 2: MAX_SPEED = 100

**Range Levels** (from RobotTypes.h:45):
- Level 0: MAX_RANGE = 5000
- Level 1: MAX_RANGE = 6000
- Level 2: MAX_RANGE = 7000

**Validation Rules**:
- Sum of `tuneSpeed + tuneCannon` must be ≤2 (RobotTypes.h:48)
- Recommended for EdgePatrol: `tuneSpeed = 1`, `tuneCannon = 1`

---

## Entity Relationships

### Class Hierarchy

```
RobotGame::Robot (base class)
    └── EdgePatrol
```

### Dependencies

```
EdgePatrol
    ├── RobotGame::Robot (base class)
    ├── RobotGame::RobCtrl (controller)
    └── RobotGame::Trigonometry (utilities)
```

### State Flow

```
RobotGame::RobCtrl
    ├── calls EdgePatrol::run() each tick
    └── manages robot position, armor, energy

EdgePatrol::run()
    ├── updates PatrolState
    ├── updates MovementState (via drive())
    ├── updates TargetingState (via scan() / cannon())
    └── calls skip() to end tick
```

---

## Data Transitions

### Patrol Lifecycle

**Initial State**: Robot spawned at random position by Scheduler

**Transition 1**: `REACH_EDGE`
- Calculate nearest edge based on spawn position
- Drive toward edge at maximum speed
- Transition to `PATROL_{edge}_EDGE` when edge distance reached

**Transition 2**: `PATROL_{edge}_EDGE`
- Maintain constant distance from wall
- Drive parallel to edge at patrol speed (75)
- Transition to next edge state when corner threshold reached

**Transition 3**: `AVOID_OBSTACLE`
- Scan forward with precision 10°
- If obstacle detected at range < 1500:
  - Turn 45° toward center
  - Drive forward 2000 units
  - Turn back toward edge
- Return to previous patrol state

### Targeting Lifecycle

**State 1**: `TARGET_CENTER`
- Calculate direction to arena center
- Fire cannon when energy ≥200
- Check for threats via scan() in movement direction

**Transition to Threat**:
- If scan() returns range >0 within detectionRange:
  - Set `targetMode = TARGET_THREAT`
  - Record threat direction and range

**State 2**: `TARGET_THREAT`
- Fire cannon at detected threat
- Re-scan each tick to update threat position

**Transition to Center**:
- If scan() returns 0 (no threat):
  - Set `targetMode = TARGET_CENTER`
- If threat angle deviates >10° from movement direction:
  - Set `targetMode = TARGET_CENTER`

---

## Data Flow

### Tick Processing Flow

```
Each Game Tick:
┌─────────────────────────────────────┐
│ 1. Get current state                │
│    - getSpeed(), getDirection()     │
│    - getX(), getY()                 │
└──────────────┬──────────────────────┘
               │
┌──────────────▼──────────────────────┐
│ 2. Update patrol navigation         │
│    - Check edge position            │
│    - Detect corners                 │
│    - Detect obstacles               │
│    - Update PatrolState            │
└──────────────┬──────────────────────┘
               │
┌──────────────▼──────────────────────┐
│ 3. Update movement                 │
│    - Calculate target direction     │
│    - Adjust speed for turns        │
│    - Call drive(direction, speed)   │
└──────────────┬──────────────────────┘
               │
┌──────────────▼──────────────────────┐
│ 4. Detect threats                  │
│    - scan(movementDir, ±10°)        │
│    - Update TargetingState          │
└──────────────┬──────────────────────┘
               │
┌──────────────▼──────────────────────┐
│ 5. Fire cannon                     │
│    - If TARGET_CENTER:              │
│      → Fire at arena center         │
│    - If TARGET_THREAT:             │
│      → Fire at threat              │
└──────────────┬──────────────────────┘
               │
┌──────────────▼──────────────────────┐
│ 6. End tick                        │
│    - Call skip()                   │
└─────────────────────────────────────┘
```

---

## Persistent Data

### Configuration Data (Compile-Time)

- Robot name: "EdgePatrol" (static constant)
- Tuning parameters: `{tune_speed=1, tune_cannon=1}` (static constant)
- Detection parameters (compile-time defaults)

### Runtime Data (Per-Tick)

- All state variables (PatrolState, MovementState, TargetingState)
- Inherited from base class: position, speed, direction, armor, energy

### No Persistent Storage Required

Robot state is managed entirely in memory by the RobotGame framework. No file I/O or database storage is needed.
