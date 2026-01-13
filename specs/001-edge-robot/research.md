# Research: Edge Navigation Robot Integration

## Decision: Follow Existing Robot Sample Pattern

**Rationale**: The project has established patterns for robot samples that must be followed for consistency and build integration.

## Technical Context Analysis

### Robot Framework Structure
- **Language**: C++11 (confirmed from .cproject file)
- **Build System**: Eclipse CDT with separate build configurations
- **Platform**: Linux executable with SDL2 graphics dependencies
- **Robot Base**: `RobotGame::Robot` abstract base class with virtual methods
- **Sample Location**: `Robots/` directory parallel to existing `Runner` and `Scanner` samples

### Robot Class Integration Pattern
Based on analysis of existing samples:

```cpp
class NewRobot: public RobotGame::Robot {
public:
    NewRobot(RobotGame::RobCtrl* robCtrl);
    virtual ~NewRobot();
    
    virtual struct RobotGame::tune_t init(std::string& name) override;
    virtual void run() override;
    virtual std::string name() const override;
    
private:
    static const struct RobotGame::tune_t myTune;
    static const std::string myName;
};

// Factory function for dynamic loading
extern "C" RobotGame::Robot* getRobot(RobotGame::RobCtrl* robCtrl) {
    static NewRobot* instance = new NewRobot(robCtrl);
    return instance;
}
```

### Build System Integration
- Robots directory is excluded from main build (`sourceEntries` excludes "Robots")
- Each robot compiles to separate binary/library
- Eclipse CDT manages build configurations
- Uses SDL2, SDL2_ttf, SDL2_gfx, SDL2_image libraries

### Available Robot API
From `Robot.h` and `RobotTypes.h`:
- **Navigation**: `drive(direction, speed)`, `getSpeed()`, `getDirection()`, `getX()`, `getY()`
- **Combat**: `cannon(direction, range)`, `scan(direction, precision)`
- **Status**: `getArmor()`, `getEnergy()`, `getTick()`
- **Resource Management**: `armorToEnergy()`, `energyToArmor()`

### Game World Constants
- **Arena Size**: 10000x10000 units (0-9999 for both X and Y)
- **Max Speed**: 100 units/tick
- **Shell Speed**: 250 units/tick  
- **Detection Range**: Up to 7000 units (depends on robot tune)
- **Energy**: 200 points per shot, 50 points regenerated per tick

## Implementation Requirements

### File Structure
```
Robots/EdgeRobot/
├── EdgeRobot.h
├── EdgeRobot.cpp
└── (Makefile or build integration)
```

### Edge Navigation Logic Requirements
1. **Edge Detection**: Calculate distance to arena boundaries (0,0 to 9999,9999)
2. **Threat Detection**: Scan ±10° cone, 75-100 unit range as specified
3. **Movement Strategy**: Follow perimeter while maintaining safe distance from walls
4. **Combat Strategy**: Shoot at arena center when safe, engage detected threats

### Build Integration
- Follow existing pattern of separate compilation units
- Ensure dynamic loading through `getRobot()` factory function
- Maintain consistent naming and structure with Runner/Scanner samples

## Alternatives Considered

### Alternative 1: Inline with Main Game Code
- **Rejected**: Would violate project's modular sample architecture
- **Issue**: Robots directory intentionally excluded from main build

### Alternative 2: Different Inheritance Pattern  
- **Rejected**: Existing `RobotGame::Robot` base class provides all needed functionality
- **Issue**: Would break compatibility with existing framework

### Alternative 3: Different Build System
- **Rejected**: Eclipse CDT is established and working  
- **Issue**: Unnecessary complexity and maintenance burden

## Conclusion

The Edge Navigation Robot should follow the established sample robot pattern, inheriting from `RobotGame::Robot` and implementing the required navigation and combat behaviors using the existing API. The robot will be placed in `Robots/EdgeRobot/` directory parallel to existing samples and integrated through the build system's existing mechanisms.