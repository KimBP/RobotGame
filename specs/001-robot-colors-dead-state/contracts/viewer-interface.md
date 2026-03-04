# Viewer Interface Contract

**Feature**: 001-robot-colors-dead-state  
**Date**: 2026-01-22

## Overview

This document defines the internal C++ interface changes for the Viewer class to support robot colors and dead state visualization.

## New Methods

### Viewer::markRobotDead

**Signature**:
```cpp
static void markRobotDead(int id);
```

**Purpose**: Mark a robot as dead without removing it from the display.

**Parameters**:
| Name | Type | Description |
|------|------|-------------|
| id | int | Robot identifier (0 to maxRobots-1) |

**Behavior**:
1. Verify robot exists in `robots` map
2. Set `robots[id].isDead = true`
3. Destroy cached textures (nameTexture, energyTexture, armorTexture)
4. Preserve position and color for continued rendering

**Preconditions**:
- Robot ID must exist in robots map
- Robot must not already be marked dead (idempotent call is safe)

**Postconditions**:
- `robots[id].isDead == true`
- Textures destroyed (pointers set to nullptr)
- Robot remains in robots map for rendering

**Thread Safety**: Must be called from main thread only.

---

### Viewer::initSkullTexture

**Signature**:
```cpp
static void initSkullTexture();
```

**Purpose**: Initialize the cached skull symbol texture for dead robot indication.

**Parameters**: None

**Behavior**:
1. Render "☠" (U+2620) using existing font via TTF_RenderUTF8_Blended()
2. Create SDL_Texture from rendered surface
3. Store in static `skullTexture` member

**Preconditions**:
- Font must be loaded
- Renderer must be initialized

**Postconditions**:
- `skullTexture` is valid SDL_Texture* or nullptr on failure

**Thread Safety**: Must be called from main thread during initialization.

---

## Modified Methods

### Viewer::PrintRobot (modified)

**Current Signature**:
```cpp
void PrintRobot(int id);
```

**Modified Behavior**:
1. Check `robots[id].isDead`
2. If dead: modify color alpha to 0x80 (50% opacity)
3. Render using `filledCircleColor()` with (possibly modified) color

**Rendering Order**:
- Dead robots rendered first (lower z-order)
- Living robots rendered second (on top)

---

### Viewer::PrintRobotStatus (modified)

**Current Signature**:
```cpp
void PrintRobotStatus(int id);
```

**Modified Behavior**:
1. Render color indicator box (10x10 filled rectangle) at start of row
2. Check `robots[id].isDead`
3. If dead:
   - Render skull texture after color indicator
   - Set text alpha to 128 (50% opacity)
4. Render robot name in robot color (with alpha)
5. Render energy/armor bars as before

---

## Method Deprecation

### Viewer::cleanupRobotTextures

**Status**: Deprecated for death handling

**Reason**: Should no longer be called when robot dies. Use `markRobotDead()` instead.

**Retained Use**: May still be called for match cleanup (when all robots should be removed).

---

## Static Member Addition

### Viewer::skullTexture

**Declaration**:
```cpp
static SDL_Texture* skullTexture;
```

**Initialization**: nullptr, set by `initSkullTexture()`

**Cleanup**: Destroyed in Viewer destructor or cleanup method
