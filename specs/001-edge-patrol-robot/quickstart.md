# Quickstart: Edge Patrol Robot

**Feature**: 001-edge-patrol-robot
**Date**: 2025-01-12

## Overview

This guide provides step-by-step instructions for building, integrating, and testing the Edge Patrol Robot sample plugin for the RobotGame framework.

---

## Prerequisites

### Required Software

- **Linux Operating System** (the project uses Linux-specific build tools)
- **GCC/G++ Compiler** with C++11 support
- **Eclipse CDT** (optional, for IDE-based building)
- **Make** (GNU Make)
- **SDL2 Libraries**:
  - `libSDL2-dev`
  - `libSDL2-gfx-dev`
  - `libSDL2-image-dev`
  - `libSDL2-ttf-dev`

### Installing Dependencies

On Ubuntu/Debian:
```bash
sudo apt-get update
sudo apt-get install build-essential g++
sudo apt-get install libsdl2-dev libsdl2-gfx-dev libsdl2-image-dev libsdl2-ttf-dev
```

On Fedora/RHEL:
```bash
sudo dnf install gcc-c++ make
sudo dnf install SDL2-devel SDL2_gfx-devel SDL2_image-devel SDL2_ttf-devel
```

---

## Project Structure

### Directory Layout

```
RobotGame.GLM-4.7/
├── Robot.h                # Base robot class API
├── Robot.cpp              # Base robot implementation
├── RobotTypes.h           # Type definitions and constants
├── Trigonometry.h          # Utility functions
├── Trigonometry.cpp
├── RobotGame.cpp          # Main executable entry point
├── Scheduler.h/cpp        # Robot scheduler
├── Robots/               # Sample robot plugins
│   ├── Runner/            # Existing sample robot
│   │   ├── Runner.h
│   │   └── Runner.cpp
│   ├── Scanner/           # Existing sample robot
│   │   ├── Scanner.h
│   │   └── Scanner.cpp
│   └── EdgePatrol/       # New sample robot (to be created)
│       ├── EdgePatrol.h   # (to be created)
│       └── EdgePatrol.cpp # (to be created)
└── .cproject             # Eclipse CDT project configuration
```

### Key Files to Create

1. `Robots/EdgePatrol/EdgePatrol.h` - EdgePatrol class declaration
2. `Robots/EdgePatrol/EdgePatrol.cpp` - EdgePatrol implementation
3. Build configuration (Makefile or updated .cproject)

---

## Building Edge Patrol Robot

### CMake Build (Recommended)

**Build all robots and main executable**:

```bash
mkdir -p build && cd build
cmake ..
make
```

**Expected Output**:
```
Building CXX objects...
Linking CXX executable robot-arena
Linking CXX shared library libedgepatrol.so
Linking CXX shared library librunner.so
Linking CXX shared library libscanner.so
Built targets: robot-arena, edgepatrol, runner, scanner
```

**Build specific robot**:

```bash
cd build
make edgepatrol
make runner
make scanner
```

**Generated Files**:
- `build/robot-arena` - Main executable
- `build/Robots/libedgepatrol.so` - Edge Patrol robot plugin
- `build/Robots/librunner.so` - Runner robot plugin
- `build/Robots/libscanner.so` - Scanner robot plugin

### Alternative: Eclipse CDT Build

If using Eclipse IDE:

1. **Import Project**:
   - File → Import → General → Existing Projects into Workspace
   - Select `RobotGame.GLM-4.7` directory

2. **Configure Build**:
   - Right-click project → Properties → C/C++ Build
   - In "Source Entries", add `Robots/EdgePatrol` folder

3. **Build Robot**:
   - Right-click `EdgePatrol.cpp` → Build
   - Output: `EdgePatrol.so` in project root

4. **Or Build Entire Project**:
   - Project → Build All
   - All robots will be compiled

---

## Running Edge Patrol Robot

### Basic Execution

Run game with Edge Patrol robot as single contestant:

```bash
# From project root
./RobotGame Robots/EdgePatrol/EdgePatrol.so
```

### Multi-Robot Matches

Run with multiple robots:

```bash
# Edge Patrol vs Runner
./RobotGame Robots/EdgePatrol/EdgePatrol.so Robots/Runner/Runner.so

# Edge Patrol vs Scanner
./RobotGame Robots/EdgePatrol/EdgePatrol.so Robots/Scanner/Scanner.so

# Three-way battle
./RobotGame Robots/EdgePatrol/EdgePatrol.so Robots/Runner/Runner.so Robots/Scanner/Scanner.so
```

### Multiple Edge Patrol Robots

Test multiple Edge Patrol instances:

```bash
# Two Edge Patrol robots
./RobotGame Robots/EdgePatrol/EdgePatrol.so Robots/EdgePatrol/EdgePatrol.so
```

Note: Each .so file creates a singleton instance, but the game spawns multiple robot controllers using the same factory function.

---

## Verifying Edge Patrol Behavior

### Observation Checklist

Open the game window and observe the following:

#### Edge Patrol Movement
- [ ] Robot moves along arena edges (not through center)
- [ ] Robot maintains consistent distance from walls
- [ ] Robot turns smoothly at corners without stopping
- [ ] Robot completes full circuits of the arena

#### Center-Focused Shooting
- [ ] Robot fires projectiles toward arena center while patrolling
- [ ] Fire rate is consistent (approximately 2+ shots per second)
- [ ] Shooting angle adjusts as robot moves along edges
- [ ] Projectiles travel toward center from various edge positions

#### Threat Detection
- [ ] Add a second robot (e.g., Runner) to the match
- [ ] When threat enters ±10° detection angle, Edge Patrol switches targeting
- [ ] Robot fires at detected threat instead of center
- [ ] When threat exits detection angle, robot resumes center targeting

#### Obstacle Avoidance
- [ ] Place obstacles near arena edges (if supported)
- [ ] Robot detects obstacles and navigates around them
- [ ] After obstacle avoidance, robot returns to edge patrol

---

## Testing Scenarios

### Test 1: Empty Arena (Baseline)

**Objective**: Verify edge patrol and center shooting

**Command**:
```bash
./RobotGame Robots/EdgePatrol/EdgePatrol.so
```

**Expected**: Robot should:
- Spawn at random position
- Move to nearest edge
- Complete at least 5 full edge circuits
- Continuously shoot at arena center
- No collisions with walls

**Success Criteria**:
- Robot stays alive indefinitely (no self-damage)
- Smooth corner navigation
- Consistent shooting toward center

---

### Test 2: Edge Patrol vs Runner

**Objective**: Test threat detection and targeting switching

**Command**:
```bash
./RobotGame Robots/EdgePatrol/EdgePatrol.so Robots/Runner/Runner.so
```

**Expected**: Edge Patrol should:
- Patrol edges and shoot at center initially
- When Runner enters ±10° detection angle, switch targeting to Runner
- Fire at Runner instead of center
- When Runner exits detection angle, resume center targeting

**Watch For**:
- Targeting switches within ~100ms
- Smooth transitions between target modes
- Edge patrol continues regardless of targeting state

---

### Test 3: Multi-Robot Free-For-All

**Objective**: Test behavior with multiple threats

**Command**:
```bash
./RobotGame Robots/EdgePatrol/EdgePatrol.so Robots/Runner/Runner.so Robots/Scanner/Scanner.so
```

**Expected**: Edge Patrol should:
- Prioritize threats in detection angle over other robots
- Continue edge patrol while engaging threats
- Survive competitively against multiple opponents

---

### Test 4: Edge Patrol Mirror Match

**Objective**: Test Edge Patrol vs Edge Patrol

**Command**:
```bash
./RobotGame Robots/EdgePatrol/EdgePatrol.so Robots/EdgePatrol/EdgePatrol.so
```

**Expected**: Both robots should:
- Patrol edges independently
- Switch targeting when opponent enters detection angle
- Demonstrate symmetric behavior

---

## Troubleshooting

### Build Errors

**Error**: `Robot.h: No such file or directory`

**Solution**: Ensure include path is correct: `-I../../` in Makefile

**Error**: `undefined reference to 'dlopen'`

**Solution**: Add `-ldl` to linker flags: `LDFLAGS = -shared -ldl`

**Error**: `multiple definition of 'getRobot'`

**Solution**: Ensure only one definition per .cpp file; the extern "C" declaration should only be in .cpp, not .h

---

### Runtime Errors

**Error**: `Error loading plugin Robots/EdgePatrol/EdgePatrol.so`

**Cause**: Shared library not compiled or wrong architecture

**Solution**:
- Ensure .so file exists in `Robots/EdgePatrol/` directory
- Check file permissions: `chmod +x Robots/EdgePatrol/EdgePatrol.so`
- Verify architecture: `file Robots/EdgePatrol/EdgePatrol.so`

**Error**: `Error finding getRobot() in Robots/EdgePatrol/EdgePatrol.so`

**Cause**: getRobot function not exported correctly

**Solution**:
- Ensure `extern "C"` is used before getRobot function
- Check function signature matches: `RobotGame::Robot* getRobot(RobotGame::RobCtrl* robCtrl)`

---

### Behavioral Issues

**Issue**: Robot doesn't patrol edges

**Diagnosis**: Check patrol state machine in EdgePatrol.cpp

**Solution**:
- Verify robot reaches edge position correctly
- Check corner detection logic
- Ensure direction calculations use Trigonometry utilities

**Issue**: Robot shoots at walls or doesn't hit center

**Diagnosis**: Check center shooting logic

**Solution**:
- Verify center coordinates: (MAX_POS_X/2, MAX_POS_Y/2) = (4999, 4999)
- Check angle calculation: `Trigonometry::direction(getX(), getY(), centerX, centerY)`
- Ensure range is sufficient (6000 units)

**Issue**: Robot doesn't detect threats

**Diagnosis**: Check threat detection logic

**Solution**:
- Verify scan() is called each tick
- Check precision parameter: ±10 degrees
- Verify detection range comparison
- Confirm scan direction matches movement direction

---

## Integration Checklist

### Before Running

- [ ] Build `EdgePatrol.so` successfully
- [ ] Verify .so file exists in `Robots/EdgePatrol/` directory
- [ ] Test basic execution: `./RobotGame Robots/EdgePatrol/EdgePatrol.so`
- [ ] Confirm no runtime errors in terminal output

### After Running

- [ ] Robot appears in game window
- [ ] Robot moves along edges
- [ ] Robot shoots at center
- [ ] Robot survives in empty arena
- [ ] Threat detection works with multiple robots

---

## Next Steps

After verifying basic functionality:

1. **Adjust Tuning**: Modify `myTune` in EdgePatrol.cpp to experiment with different speed/range combinations
2. **Customize Parameters**: Edit detection range, edge distance, and corner threshold
3. **Add Logging**: Use `RobotGame::Logger::Log()` to debug behavior
4. **Test Edge Cases**: Spawn robot at center, test with obstacles, etc.

---

## Reference Files

- **Robot API**: `Robot.h`, `Robot.cpp`
- **Types & Constants**: `RobotTypes.h`
- **Trigonometry Utilities**: `Trigonometry.h`, `Trigonometry.cpp`
- **Sample Implementations**: `Robots/Runner/`, `Robots/Scanner/`
- **Main Entry Point**: `RobotGame.cpp` (shows plugin loading)
- **Scheduler**: `Scheduler.h`, `Scheduler.cpp` (shows robot lifecycle)

---

## Additional Resources

- **Specification**: [spec.md](./spec.md)
- **Implementation Plan**: [plan.md](./plan.md)
- **Research**: [research.md](./research.md)
- **Data Model**: [data-model.md](./data-model.md)

---

## Support

For issues or questions:
1. Check existing sample robots (Runner, Scanner) for reference
2. Review Robot API documentation in Robot.h
3. Check quickstart troubleshooting section
4. Verify build configuration matches project standards
