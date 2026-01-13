# API Contracts: Edge Navigation Robot

## Robot Interface Contract

### Class Definition
```cpp
class EdgeRobot: public RobotGame::Robot {
public:
    EdgeRobot(RobotGame::RobCtrl* robCtrl);
    virtual ~EdgeRobot();

    virtual struct RobotGame::tune_t init(std::string& name) override;
    virtual void run() override;
    virtual std::string name() const override;

private:
    static const struct RobotGame::tune_t myTune;
    static const std::string myName;
    
    // Navigation state
    enum NavigationMode { EDGE_FOLLOW, THREAT_ENGAGE, AVOIDANCE };
    NavigationMode currentMode;
    
    // Helper methods
    angle_t calculateAngleToCenter();
    bool detectThreatInCone(angle_t& threatDirection, range_t& threatRange);
    void navigateEdges();
    void engageThreat(angle_t direction, range_t range);
    void avoidObstacle();
};
```

### Factory Function Contract
```cpp
extern "C" RobotGame::Robot* getRobot(RobotGame::RobCtrl* robCtrl) {
    static EdgeRobot* instance = new EdgeRobot(robCtrl);
    return instance;
}
```

## Navigation Algorithm Contract

### Edge Distance Calculations
```cpp
// Returns distance to nearest arena edge
range_t calculateDistanceToNearestEdge(posx_t x, posy_t y);

// Determines optimal edge-following direction based on current position
angle_t calculateEdgeFollowDirection(posx_t x, posy_t y, angle_t currentDirection);

// Checks if position is safe distance from edges (minimum 200 units)
bool isSafeFromEdges(posx_t x, posy_t y);
```

### Threat Detection Contract
```cpp
// Scans ±10° cone around current heading for threats
// Returns true if threat detected, outputs direction and range
bool detectThreatInCone(
    angle_t currentHeading,           // Input: robot's current direction
    precision_t scanPrecision,        // Input: ±10 degrees
    range_t maxRange,                 // Input: 75-100 units
    angle_t& threatDirection,         // Output: direction to threat
    range_t& threatRange              // Output: distance to threat
);
```

### Center Targeting Contract
```cpp
// Calculates angle from current position to arena center (4999, 4999)
angle_t calculateAngleToCenter(posx_t currentX, posy_t currentY);

// Determines if robot should shoot at center (no threats detected)
bool shouldShootAtCenter(bool threatDetected, energy_t currentEnergy);
```

## State Management Contract

### Navigation States
```cpp
enum NavigationMode {
    EDGE_FOLLOW,      // Default state: move along arena perimeter
    THREAT_ENGAGE,     // Combat state: prioritize enemy engagement
    AVOIDANCE         // Emergency state: avoid immediate collision
};
```

### State Transition Rules
```cpp
NavigationMode determineNextState(
    NavigationMode currentMode,
    bool threatDetected,
    bool obstacleAhead,
    int ticksSinceLastThreat
);
```

## Performance Requirements

### Real-time Constraints
- **Update Frequency**: Must complete logic in < 16ms (60+ ticks/second)
- **Memory Usage**: < 1MB additional memory overhead
- **CPU Usage**: < 5% of single thread per robot

### Response Times
- **Threat Detection**: < 5ms calculation time
- **Navigation Update**: < 10ms calculation time
- **State Transition**: < 2ms calculation time

## Error Handling Contract

### Exception Safety
- No exceptions thrown from robot logic
- Graceful degradation when calculations fail
- Default to safe behaviors (stop movement, avoid shooting)

### Invalid State Recovery
```cpp
// Ensures robot remains in valid state even with unexpected inputs
void validateAndCorrectState(NavigationMode& mode, angle_t& direction, speed_t& speed);
```

## Integration Test Contracts

### Navigation Test Cases
```cpp
// Test edge following behavior
TEST(EdgeNavigation, FollowsPerimeterClockwise);
TEST(EdgeNavigation, HandlesCornersCorrectly);
TEST(EdgeNavigation, MaintainsSafeDistanceFromWalls);

// Test threat detection
TEST(ThreatDetection, DetectsEnemyInCone);
TEST(ThreatDetection, IgnoresEnemyOutsideCone);
TEST(ThreatDetection, PrioritizesClosestThreat);

// Test state transitions
TEST(StateTransitions, TransitionsToThreatEngage);
TEST(StateTransitions, ReturnsToEdgeFollowing);
TEST(StateTransitions, AvoidsObstaclesCorrectly);
```

### Combat Test Cases
```cpp
// Test shooting behavior
TEST(Combat, ShootsAtCenterWhenSafe);
TEST(Combat, EngagesDetectedThreats);
TEST(Combat, ManagesEnergyEfficiently);
```

## Configuration Contracts

### Robot Tuning Parameters
```cpp
const struct RobotGame::tune_t myTune = {
    .tune_speed = 2,    // Maximum speed: 100 units/tick
    .tune_cannon = 2     // Maximum range: 7000 units
};
```

### Navigation Constants
```cpp
const range_t SAFE_EDGE_DISTANCE = 200;      // Minimum distance from walls
const precision_t THREAT_DETECTION_CONE = 10; // ±10 degrees
const range_t THREAT_DETECTION_RANGE = 75;   // Medium detection range
const angle_t CENTER_X = 4999;               // Arena center X
const angle_t CENTER_Y = 4999;               // Arena center Y
```