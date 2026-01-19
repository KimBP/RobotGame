# Quickstart Guide: Robot Game Bug Fixes and Arena Border

**Feature**: Fix Robot Game Bugs and Add Arena Border  
**Date**: 2026-01-14  
**Purpose**: Quick setup and testing instructions for the bug fixes

## Overview

This implementation fixes three critical issues in the RobotGame:
1. **Robot Death Bug** - Robots can't die when armor reaches 0
2. **Rendering Errors** - Logger shows "Error rendering name" and "Error rendering energy"
3. **Arena Border** - Add visual separation between arena and scoreboard

## Prerequisites

- Linux system with SDL2 development libraries
- CMake 3.28+ 
- C++17 compatible compiler
- Font: FreeSansBold.ttf (usually in `/usr/share/fonts/truetype/freefont/`)

## Build Instructions

```bash
# Configure build
cmake -DCMAKE_BUILD_TYPE=Debug -B build -S .

# Build all components
cmake --build build

# Run the game
./build/robot-arena
```

## Testing the Fixes

### 1. Robot Death Mechanism Test

**Expected Behavior**: Robots should die when armor reaches 0 and be removed from arena.

**Test Steps**:
1. Start the game with multiple robots
2. Wait for combat to occur
3. Observe robots taking damage
4. Verify robots disappear when armor reaches 0
5. Check logger confirms robot elimination

**Verification**:
- No robots remain with 0 armor
- Dead robots are removed from robots vector
- Game continues with remaining robots

### 2. Rendering Error Test

**Expected Behavior**: No "Error rendering name" or "Error rendering energy" messages in logger.

**Test Steps**:
1. Start the game
2. Monitor console/logger output
3. Verify robot names and energy values are visible
4. Check for absence of rendering error messages

**Verification**:
- All robot names display correctly
- Energy values show properly
- Clean logger output during gameplay

### 3. Arena Border Test

**Expected Behavior**: Clear visual border separating arena from scoreboard.

**Test Steps**:
1. Start the game
2. Observe the game window layout
3. Verify border around the robot arena area
4. Check that scoreboard is visually separate

**Verification**:
- Border clearly visible around 1000x1000 pixel arena
- No interference with robot movement or visibility
- Consistent border appearance throughout gameplay

## Debugging

### Enabling Debug Output

```cpp
// Add to main.cpp or relevant source
Logger::SetLogLevel(LogLevel::Debug);
```

### Common Issues

**Font Not Found**:
```bash
# Install required fonts
sudo apt-get install fonts-freefont-ttf
```

**SDL2 Initialization Errors**:
```bash
# Install SDL2 development packages
sudo apt-get install libsdl2-dev libsdl2-ttf-dev libsdl2-gfx-dev libsdl2-image-dev
```

### Key Files to Monitor

- `RobCtrl.cpp` - Robot armor and death logic
- `Viewer.cpp` - Rendering and border display
- `RobotDataEvent.cpp` - Texture management
- Logger output - Error messages and status

## Performance Considerations

- Border rendering should add <1ms per frame
- Texture caching prevents performance degradation
- Robot cleanup prevents memory leaks

## Rollback Plan

If issues arise, you can:
1. Comment out border rendering in `renderFrameWithLayers()`
2. Revert armor logic changes in `RobCtrl::tick()`
3. Restore original texture handling in `PrintRobotStatus()`

## Success Criteria

✅ Robots die properly when armor reaches 0  
✅ No rendering error messages in logger  
✅ Visual border clearly separates arena from scoreboard  
✅ Game maintains 60 FPS performance  
✅ All existing functionality preserved

## Support

For issues during implementation:
1. Check CMake build output for compilation errors
2. Review SDL2 error messages in console
3. Verify font file path and permissions
4. Test with debug logging enabled