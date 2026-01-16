# Data Model: Edge Navigation Robot

## Robot State Management

### Core Robot State

**Position & Movement**
- `posx_t x` - Current X coordinate (0-9999)
- `posy_t y` - Current Y coordinate (0-9999)  
- `angle_t direction` - Current heading (0-359 degrees)
- `speed_t speed` - Current velocity (-75 to 100 units/tick)

**Navigation State**
- `NavigationMode mode` - Current navigation behavior (EDGE_FOLLOW, THREAT_ENGAGE, AVOIDANCE)
- `posx_t targetX` - Target position for navigation
- `posy_t targetY` - Target position for navigation
- `angle_t patrolDirection` - Direction for perimeter patrol (0=North, 90=East, etc.)

**Combat State**
- `bool threatDetected` - Whether enemy robot detected in threat cone
- `angle_t threatDirection` - Direction to detected threat
- `range_t threatRange` - Distance to detected threat
- `bool shootingAtCenter` - Whether currently aiming at arena center

## Navigation Algorithm Data

### Edge Following Logic
```cpp
enum EdgeNavigationState {
    MOVING_TO_EDGE,
    FOLLOWING_EDGE,
    CORNER_HANDLING,
    AVOIDING_OBSTACLE
};
```

### Distance Calculations
- `distanceToNorthEdge = y` (distance to top boundary)
- `distanceToSouthEdge = MAX_POS_Y - y` (distance to bottom boundary)
- `distanceToEastEdge = MAX_POS_X - x` (distance to right boundary)
- `distanceToWestEdge = x` (distance to left boundary)
- `safeDistanceFromEdge = 200` (minimum safe distance from walls)

## Threat Detection System

### Scan Pattern
- `angle_t scanCenter = direction` (center of ±10° detection cone)
- `precision_t scanPrecision = 10` (±10 degrees)
- `range_t detectionRange = 75-100` (medium detection range)

### Threat Prioritization
```cpp
struct ThreatInfo {
    angle_t direction;
    range_t range;
    int priority;  // 1=highest, lower=less urgent
};
```

## Arena Center Targeting

### Center Coordinates
- `posx_t centerX = MAX_POS_X / 2 = 4999`
- `posy_t centerY = MAX_POS_Y / 2 = 4999`

### Center Angle Calculation
```cpp
angle_t angleToCenter = atan2(centerY - y, centerX - x) * 180 / PI;
// Convert to compass degrees (0=North, 90=East, 180=South, 270=West)
```

## Robot Configuration

### Tune Parameters
```cpp
const struct RobotGame::tune_t myTune = {
    .tune_speed = 2,    // MAX_SPEED (100 units/tick) for good maneuverability
    .tune_cannon = 2    // MAX_RANGE (7000 units) for shooting to center
};
// Sum = 2 (within MAX_TUNE = 2)
```

### Robot Identity
- `std::string myName = "EdgeRobot"`

## State Transitions

### Navigation Mode Transitions
```
EDGE_FOLLOW → THREAT_ENGAGE: when threat detected in ±10° cone
THREAT_ENGAGE → EDGE_FOLLOW: when no threats detected for 5 ticks
EDGE_FOLLOW → AVOIDANCE: when obstacle detected in path
AVOIDANCE → EDGE_FOLLOW: when obstacle avoided and edge regained
```

### Decision Logic Flow
```
1. Check for threats in ±10° cone
   - If detected: engage threat (THREAT_ENGAGE mode)
   - If none: proceed to step 2

2. Check for obstacles in planned path
   - If detected: avoidance maneuver (AVOIDANCE mode)
   - If none: proceed to step 3

3. Continue edge navigation (EDGE_FOLLOW mode)
   - Calculate distance to nearest edge
   - Adjust movement to maintain safe distance
   - Shoot at arena center continuously
```

## Performance Considerations

### Computation Efficiency
- Simple trigonometric calculations for angles
- Basic distance formulas for edge detection
- Minimal state tracking for performance

### Memory Footprint
- Small number of state variables
- No complex data structures
- Suitable for real-time robot control

## Integration Points

### Robot Base Class Integration
- Inherits all core functionality from `RobotGame::Robot`
- Uses existing `scan()`, `cannon()`, `drive()` methods
- Follows established `run()` main loop pattern

### Game World Constants
- Uses `MAX_POS_X`, `MAX_POS_Y` from `RobotTypes.h`
- Respects `MAX_SPEED`, `SHELL_SPEED` constraints
- Follows energy/armor management rules