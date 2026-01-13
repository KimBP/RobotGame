# Contracts: Edge Patrol Robot

**Feature**: 001-edge-patrol-robot
**Date**: 2025-01-12

## Overview

This directory contains API contracts for the Edge Patrol Robot feature.

## Note

The Edge Patrol Robot is implemented as a C++ shared library plugin, not a web service or API. Therefore, no REST/GraphQL API contracts are needed.

### Robot Plugin Interface

The robot plugin follows the existing RobotGame framework contract defined by:

- **Base Class API**: `Robot.h` - Defines the robot interface that all plugins must implement
- **Factory Function**: `extern "C" RobotGame::Robot* getRobot(RobotGame::RobCtrl* robCtrl)`
- **Type System**: `RobotTypes.h` - Defines all data types and constants

### Contract Details

The robot must implement the following interface:

```cpp
class EdgePatrol: public RobotGame::Robot {
public:
    EdgePatrol(RobotGame::RobCtrl* robCtrl);
    virtual ~EdgePatrol();

    // Required base class methods
    virtual struct RobotGame::tune_t init(std::string& name);
    virtual void run();
    virtual std::string name() const;
};
```

And export a C-compatible factory function:

```cpp
extern "C" RobotGame::Robot* getRobot(RobotGame::RobCtrl* robCtrl) {
    static EdgePatrol* instance = new EdgePatrol(robCtrl);
    return instance;
}
```

### Runtime Contract

- **Loading**: Robot is loaded via `dlopen()` by RobotGame
- **Initialization**: `getRobot()` is called once to create singleton instance
- **Execution**: `run()` method is called continuously in separate thread
- **Termination**: Robot is destroyed when game ends or robot dies

## References

- **Robot API**: `Robot.h` (base class interface)
- **Types**: `RobotTypes.h` (data types and constants)
- **Trigonometry**: `Trigonometry.h` (utility functions)
- **Examples**: `Robots/Runner/`, `Robots/Scanner/` (reference implementations)
