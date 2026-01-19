# Component Contracts: Robot Game Bug Fixes

**Feature**: Fix Robot Game Bugs and Add Arena Border  
**Date**: 2026-01-14  
**Type**: Internal Component Contracts

## Robot Death System Contract

### Interface: RobCtrl

**Method**: `bool tick(int deltaTime, bool& changed)`

**Preconditions**:
- Robot is alive (`currArmor > 0`)
- deltaTime is positive integer milliseconds

**Postconditions**:
- If armor reaches 0: returns false (robot dead)
- If armor > 0: returns true (robot alive)
- Energy refilled only if robot remains alive
- Robot state marked as dead when armor <= 0

**Contract**:
```cpp
// Guarantees:
// 1. Armor never goes below 0
// 2. Energy only refilled when armor > 0
// 3. Death state properly set when armor == 0
bool tick(int deltaTime, bool& changed);
```

**Validation**:
- Armor calculation: `newArmor = max(0, currentArmor - damage)`
- Death check: `return currArmor > 0;`

## Rendering System Contract

### Interface: Viewer

**Method**: `void PrintRobotStatus()`

**Preconditions**:
- SDL renderer is initialized
- Font is loaded successfully
- Robot data is valid

**Postconditions**:
- Robot names displayed without error
- Energy values displayed without error
- No error messages in logger
- Textures created and rendered successfully

**Contract**:
```cpp
// Guarantees:
// 1. All textures are valid before rendering
// 2. Error handling prevents crashes
// 3. Logger only used for non-critical issues
void PrintRobotStatus();
```

**Error Handling**:
- Font loading validation
- Texture creation validation
- Safe fallback for missing textures

## Arena Border Contract

### Interface: Viewer

**Method**: `void renderFrameWithLayers()`

**Preconditions**:
- Arena viewport is set
- SDL_gfx is initialized
- Renderer is active

**Postconditions**:
- Border drawn around arena area
- No interference with robot rendering
- Consistent appearance across frames

**Contract**:
```cpp
// Guarantees:
// 1. Border drawn at correct coordinates
// 2. Performance impact < 1ms per frame
// 3. Visual consistency maintained
void renderFrameWithLayers();
```

**Border Specifications**:
- Rectangle around: {STATUS_WIDTH, 0, ARENA_WIDTH, ARENA_HEIGHT}
- Color: High contrast (e.g., white or bright color)
- Thickness: 1-2 pixels
- Style: Solid line

## Data Integrity Contract

### Robot State Consistency

**Invariant**: Robot state must remain consistent across all operations

**Rules**:
1. `0 <= armor <= maxArmor`
2. `0 <= energy <= maxEnergy`
3. `alive == (armor > 0)`
4. Texture validity matches alive state

**Validation Points**:
- After damage application
- After energy refill
- Before rendering
- After death cleanup

## Performance Contract

### Frame Rate Guarantee

**Target**: 60 FPS (16.67ms per frame)

**Budget Allocation**:
- Robot logic: < 5ms
- Terrain rendering: < 3ms
- Robot rendering: < 6ms
- UI rendering: < 1ms
- Border rendering: < 1ms
- Buffer: < 0.67ms

**Validation**:
- Frame timing measurements
- Performance profiling
- Budget adherence verification

## Error Recovery Contract

### Graceful Degradation

**Font Loading Failure**:
- Log warning
- Use fallback rendering (simple shapes/colors)
- Continue game functionality

**Texture Creation Failure**:
- Log specific error
- Skip problematic element
- Continue with other elements

**Rendering Failure**:
- Log SDL error
- Attempt recovery or safe fallback
- Maintain game state integrity

## Integration Contract

### Robot Plugin Compatibility

**Guarantees**:
- No changes to Robot interface
- Existing plugins continue to work
- New behavior is backward compatible
- Plugin loading/unloading unaffected

**Validation**:
- Test with all existing robot types
- Verify plugin loading sequence
- Confirm game loop compatibility

## Testing Contract

### Coverage Requirements

**Unit Tests** (when implemented):
- Armor calculation edge cases
- Death condition validation
- Texture creation scenarios
- Border rendering accuracy

**Integration Tests**:
- Complete robot lifecycle
- Multi-robot combat scenarios
- Rendering pipeline validation
- Performance under load

**Manual Tests**:
- Gameplay observation
- Visual verification
- Error condition testing
- Long-running stability