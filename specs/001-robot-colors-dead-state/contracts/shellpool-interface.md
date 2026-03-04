# ShellPool Interface Contract

**Feature**: 001-robot-colors-dead-state  
**Date**: 2026-01-22

## Overview

This document defines the internal C++ interface changes for the ShellPool class to support shell coloring based on firing robot.

## Modified Methods

### ShellPool::createShell (modified)

**Current Signature**:
```cpp
void createShell(int x, int y, Vector2 velocity);
```

**New Signature**:
```cpp
void createShell(int x, int y, Vector2 velocity, uint32_t ownerColor);
```

**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| x | int | Starting X position |
| y | int | Starting Y position |
| velocity | Vector2 | Shell velocity vector |
| **ownerColor** | **uint32_t** | **NEW: RGBA color of firing robot** |

**Behavior**:
1. Create AnimatedShell as before
2. Store `ownerColor` in shell struct
3. Initialize trail as before

**Default Color**: If backward compatibility needed, provide overload:
```cpp
void createShell(int x, int y, Vector2 velocity) {
    createShell(x, y, velocity, 0xFFFFFFFF);  // Default white
}
```

---

### ShellPool::renderShells (modified)

**Current Signature**:
```cpp
void renderShells(SDL_Renderer* renderer, float scale) const;
```

**Current Behavior** (lines 126-151):
- Trail rendered with hardcoded white: `(0xFF << 24) | (alpha << 16) | (alpha << 8) | alpha`
- Shell head rendered with hardcoded white: `0xFFFFFFFF`

**Modified Behavior**:
1. Extract RGB from `shell->ownerColor`
2. Apply trail alpha fade to owner color:
   ```cpp
   uint8_t r = (shell->ownerColor >> 24) & 0xFF;
   uint8_t g = (shell->ownerColor >> 16) & 0xFF;
   uint8_t b = (shell->ownerColor >> 8) & 0xFF;
   uint8_t a = static_cast<uint8_t>(255.0f * alpha);
   uint32_t trailColor = (r << 24) | (g << 16) | (b << 8) | a;
   ```
3. Render shell head with full opacity owner color:
   ```cpp
   filledCircleColor(renderer, shellX, shellY, 3, shell->ownerColor);
   ```

---

## Struct Modification

### AnimatedShell (ShellPool.h)

**New Field**:
```cpp
struct AnimatedShell {
    // ... existing fields ...
    uint32_t ownerColor;  // NEW: Color of firing robot (RGBA format)
    // ... rest of fields ...
};
```

**Field Position**: After `creationTime`, before `trail` array for cache locality.

---

## Caller Updates Required

### Viewer::_CannonShow

**Location**: Viewer.cpp (approx. line 86-107)

**Current Call**:
```cpp
shellPool->createShell(x, y, velocity);
```

**Modified Call**:
```cpp
shellPool->createShell(x, y, velocity, robots[id].color);
```

**Note**: Must ensure robot exists in map before accessing color.
