# Research: Robot Colors and Dead State Visualization

**Feature**: 001-robot-colors-dead-state  
**Date**: 2026-01-22  
**Status**: Complete

## Research Questions

### 1. How are robot colors currently implemented?

**Decision**: Use existing color array with index-based assignment

**Rationale**: 
- Colors already stored in `Viewer::colors[]` array (Viewer.cpp:19-24)
- RobPos struct already has `unsigned int color` field (Viewer.h:28)
- Color assignment happens at `_RobotShow()` using `colors[id]` (Viewer.cpp:120)
- Format is 32-bit RGBA (0xRRGGBBAA) compatible with SDL2_gfx

**Alternatives considered**:
- Random color generation: Rejected - inconsistent across sessions
- Config file colors: Rejected - adds complexity, existing array sufficient

**Current colors** (Viewer.cpp:19-24):
```cpp
unsigned int Viewer::colors[] = {
    0xFFF000FF,  // Yellow (Robot 0)
    0xFF0F00FF,  // Red (Robot 1)
    0xFF00F0FF,  // Cyan (Robot 2)
    0xFF000FFF   // Blue (Robot 3)
};
```

### 2. How should dead robots be preserved in the arena?

**Decision**: Add `isDead` boolean to RobPos; modify cleanup to set flag instead of erasing

**Rationale**:
- Current `cleanupRobotTextures()` (Viewer.cpp:192-217) erases robots from map
- Adding a flag preserves position/color data for rendering
- Minimal change to existing data structures
- Allows different rendering path for dead vs alive robots

**Alternatives considered**:
- Separate dead robots collection: Rejected - duplicates data, more complex
- New DeadRobot struct: Rejected - RobPos already has all needed fields

**Implementation approach**:
1. Add `bool isDead = false;` to RobPos struct
2. Create `Viewer::markRobotDead(int id)` that sets flag, destroys textures
3. Replace `cleanupRobotTextures()` call in Scheduler with `markRobotDead()`
4. Modify `PrintRobot()` to render dead robots at 50% opacity

### 3. How should shell colors be implemented?

**Decision**: Add `ownerColor` field to AnimatedShell; propagate from firing robot

**Rationale**:
- ShellPool currently renders all shells white (ShellPool.cpp:142,149)
- Need to track which robot fired each shell
- Color should match robot for visual correlation

**Implementation approach**:
1. Add `uint32_t ownerColor;` to AnimatedShell struct (ShellPool.h)
2. Modify `createShell()` to accept color parameter
3. Modify `_CannonShow()` to pass robot color to shell creation
4. Update `renderShells()` to use `ownerColor` instead of hardcoded white

**Alternatives considered**:
- Store robot ID instead of color: Rejected - requires lookup, color is more direct
- Colored trails: Consider for future - current implementation uses alpha fade only

### 4. How should the scoreboard display robot colors?

**Decision**: Existing implementation already uses robot colors; add color indicator box and skull symbol

**Rationale**:
- `PrintRobotStatus()` (Viewer.cpp:530-578) already sets render color from `robots[id].color`
- Name text already rendered in robot's color
- Need to add: color indicator box, skull symbol for dead robots

**Implementation approach**:
1. Add colored filled rectangle (10x10px) before robot name
2. For dead robots: render skull character "☠" (Unicode U+2620) using TTF
3. Reduce text opacity for dead robots (set alpha to 128)

**Alternatives considered**:
- Load skull PNG: Rejected - TTF rendering more consistent, no asset dependencies
- Strikethrough text: Rejected - harder to implement, skull is clearer

### 5. How to render the skull symbol?

**Decision**: Use Unicode skull character (U+2620 "☠") rendered via SDL2_ttf

**Rationale**:
- Game already uses SDL2_ttf for text rendering
- Unicode skull is widely supported in fonts
- No additional asset files needed
- Consistent with existing text rendering approach

**Implementation approach**:
1. Render "☠" character using existing font with `TTF_RenderUTF8_Blended()`
2. Cache as SDL_Texture for performance
3. Display adjacent to robot name when `isDead == true`

**Alternatives considered**:
- Draw skull with SDL2_gfx primitives: Rejected - complex, harder to maintain
- Load skull image file: Rejected - adds asset dependency

### 6. What opacity level for dead robots?

**Decision**: 50% opacity (alpha = 128) for both arena and scoreboard

**Rationale**:
- 50% provides clear visual distinction while maintaining color identity
- Consistent with spec assumption (spec.md line 129)
- Easy to implement by modifying alpha channel

**Implementation approach**:
- Arena: Modify color alpha from 0xFF to 0x80 before rendering
- Scoreboard: Set SDL_SetRenderDrawColor alpha to 128

## Dependencies & Integration

### Files to Modify

| File | Modification | Complexity |
|------|--------------|------------|
| Viewer.h | Add `isDead` to RobPos, skull texture member | Low |
| Viewer.cpp | markRobotDead(), PrintRobot() opacity, PrintRobotStatus() skull/box | Medium |
| ShellPool.h | Add `ownerColor` to AnimatedShell | Low |
| ShellPool.cpp | Use ownerColor in renderShells() | Low |
| Scheduler.cpp | Replace cleanupRobotTextures() with markRobotDead() | Low |

### Rendering Order Considerations

Dead robots should render on a lower layer than living robots to avoid visual obstruction:
- Option A: Render dead robots first in the robot layer loop
- Option B: Create separate dead robot layer (more complex)

**Decision**: Option A - sort robots by alive status before rendering, dead first.

## Best Practices Applied

1. **Minimal data structure changes**: Single boolean flag, single color field
2. **Preserve existing patterns**: Follow existing color/rendering conventions
3. **Cache rendered textures**: Skull texture created once, reused
4. **Thread safety**: No new thread interactions; Viewer operations remain on main thread
5. **Determinism**: Visual-only changes; no impact on game physics or timing
