# Quickstart: Robot Colors and Dead State Visualization

**Feature**: 001-robot-colors-dead-state  
**Date**: 2026-01-22

## Overview

This feature enhances visual differentiation in the RobotGame arena by:
1. Assigning unique colors to each robot
2. Coloring shells to match the firing robot
3. Keeping dead robots visible at 50% opacity
4. Adding color indicators and skull symbols to the scoreboard

## Prerequisites

- C++17 compiler (g++)
- SDL2, SDL2_ttf, SDL2_gfx libraries installed
- Existing RobotGame build environment

## Implementation Order

### Step 1: Add isDead flag to RobPos (Viewer.h)

```cpp
struct RobPos {
    int x;
    int y;
    unsigned int color;
    int energy;
    int armor;
    bool isDead;  // ADD THIS LINE
    SDL_Texture* nameTexture;
    SDL_Texture* energyTexture;
    SDL_Texture* armorTexture;
};
```

Initialize in `_RobotShow()`:
```cpp
if (! robots.count(id)) {
    robots[id].color = colors[id];
    robots[id].isDead = false;  // ADD THIS LINE
    robots[id].nameTexture = 0;
}
```

### Step 2: Add ownerColor to AnimatedShell (ShellPool.h)

```cpp
struct AnimatedShell {
    uint32_t shellId;
    Vector2 position;
    Vector2 velocity;
    bool active;
    uint32_t creationTime;
    uint32_t ownerColor;  // ADD THIS LINE
    std::array<TrailPoint, 20> trail;
    // ...
};
```

### Step 3: Modify createShell() (ShellPool.cpp)

Update signature and implementation to accept and store color:
```cpp
void ShellPool::createShell(int x, int y, Vector2 velocity, uint32_t ownerColor) {
    // ... existing code ...
    shell->ownerColor = ownerColor;
    // ...
}
```

### Step 4: Modify renderShells() (ShellPool.cpp)

Replace hardcoded white with owner color:
```cpp
// Trail color with alpha
uint8_t r = (shell->ownerColor >> 24) & 0xFF;
uint8_t g = (shell->ownerColor >> 16) & 0xFF;
uint8_t b = (shell->ownerColor >> 8) & 0xFF;
uint8_t a = static_cast<uint8_t>(255.0f * alpha);
uint32_t trailColor = (r << 24) | (g << 16) | (b << 8) | a;
lineColor(renderer, x1, y1, x2, y2, trailColor);

// Shell head with full color
filledCircleColor(renderer, shellX, shellY, 3, shell->ownerColor);
```

### Step 5: Add markRobotDead() (Viewer.cpp)

```cpp
void Viewer::markRobotDead(int id) {
    auto it = viewer.robots.find(id);
    if (it != viewer.robots.end()) {
        it->second.isDead = true;
        // Destroy textures
        if (it->second.nameTexture) {
            SDL_DestroyTexture(it->second.nameTexture);
            it->second.nameTexture = nullptr;
        }
        // ... same for energy/armor textures
    }
}
```

### Step 6: Modify PrintRobot() for dead state (Viewer.cpp)

```cpp
void Viewer::PrintRobot(int id) {
    unsigned int color = robots[id].color;
    if (robots[id].isDead) {
        // Reduce alpha to 50%
        color = (color & 0xFFFFFF00) | 0x80;
    }
    filledCircleColor(gRenderer, robots[id].x, robots[id].y, ROBOT_RADIUS, color);
}
```

### Step 7: Update Scheduler to use markRobotDead() (Scheduler.cpp)

Replace:
```cpp
Viewer::cleanupRobotTextures(deadId);
```

With:
```cpp
Viewer::markRobotDead(deadId);
```

### Step 8: Add skull texture and scoreboard updates (Viewer.cpp)

Initialize skull texture:
```cpp
void Viewer::initSkullTexture() {
    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, "☠", {255, 255, 255, 255});
    skullTexture = SDL_CreateTextureFromSurface(gRenderer, surface);
    SDL_FreeSurface(surface);
}
```

Modify PrintRobotStatus():
```cpp
void Viewer::PrintRobotStatus(int id) {
    // Draw color indicator box
    SDL_Rect colorBox = {ARENA_WIDTH + 5, robStatusY + 5, 10, 10};
    SDL_SetRenderDrawColor(gRenderer, 
        (robots[id].color >> 24) & 0xFF,
        (robots[id].color >> 16) & 0xFF,
        (robots[id].color >> 8) & 0xFF,
        robots[id].isDead ? 128 : 255);
    SDL_RenderFillRect(gRenderer, &colorBox);
    
    // Draw skull if dead
    if (robots[id].isDead && skullTexture) {
        SDL_Rect skullRect = {ARENA_WIDTH + 20, robStatusY + 2, 16, 16};
        SDL_RenderCopy(gRenderer, skullTexture, nullptr, &skullRect);
    }
    // ... rest of status rendering with adjusted alpha if dead
}
```

## Testing

### Manual Test Cases

1. **Color Assignment**: Load 4 robots, verify each has distinct color
2. **Shell Colors**: Fire shells from each robot, verify colors match
3. **Dead State Arena**: Kill a robot, verify it stays visible at 50% opacity
4. **Dead State Scoreboard**: Kill a robot, verify skull appears and opacity reduces
5. **Multiple Deaths**: Kill multiple robots, verify all remain visible
6. **Death Causes**: Test death by shooting and wall collision, verify same visualization

## Build & Run

```bash
# Build
cmake -B build -S .
cmake --build build

# Run with robots
./build/RobotGame ./build/Robots/Runner/libRunner.so ./build/Robots/Scanner/libScanner.so
```

## Verification Checklist

- [ ] Each robot has unique color in arena
- [ ] Robot names in scoreboard match arena colors
- [ ] Color indicator boxes appear in scoreboard
- [ ] Shells are colored to match firing robot
- [ ] Dead robots remain visible at reduced opacity
- [ ] Skull symbol appears for dead robots in scoreboard
- [ ] Dead robot scoreboard entries have reduced opacity
