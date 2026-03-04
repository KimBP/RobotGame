# Data Model: Robot Colors and Dead State Visualization

**Feature**: 001-robot-colors-dead-state  
**Date**: 2026-01-22

## Entity Changes

### RobPos (Viewer.h)

**Current structure**:
```cpp
struct RobPos {
    int x;
    int y;
    unsigned int color;
    int energy;
    int armor;
    SDL_Texture* nameTexture;
    SDL_Texture* energyTexture;
    SDL_Texture* armorTexture;
};
```

**Modified structure**:
```cpp
struct RobPos {
    int x;
    int y;
    unsigned int color;
    int energy;
    int armor;
    bool isDead;                    // NEW: Track dead state for rendering
    SDL_Texture* nameTexture;
    SDL_Texture* energyTexture;
    SDL_Texture* armorTexture;
};
```

| Field | Type | Default | Description |
|-------|------|---------|-------------|
| x | int | 0 | X position in arena coordinates |
| y | int | 0 | Y position in arena coordinates |
| color | unsigned int | colors[id] | RGBA color (0xRRGGBBAA format) |
| energy | int | 0 | Current energy level |
| armor | int | 0 | Current armor level |
| **isDead** | **bool** | **false** | **NEW: True when robot has died** |
| nameTexture | SDL_Texture* | nullptr | Cached name text texture |
| energyTexture | SDL_Texture* | nullptr | Cached energy text texture |
| armorTexture | SDL_Texture* | nullptr | Cached armor text texture |

**State Transitions**:
- `isDead`: false → true (when armor reaches 0, irreversible within match)

### AnimatedShell (ShellPool.h)

**Current structure**:
```cpp
struct AnimatedShell {
    uint32_t shellId;
    Vector2 position;
    Vector2 velocity;
    bool active;
    uint32_t creationTime;
    std::array<TrailPoint, 20> trail;
    uint32_t trailCount;
    uint32_t lastTrailUpdate;
};
```

**Modified structure**:
```cpp
struct AnimatedShell {
    uint32_t shellId;
    Vector2 position;
    Vector2 velocity;
    bool active;
    uint32_t creationTime;
    uint32_t ownerColor;            // NEW: Color of firing robot
    std::array<TrailPoint, 20> trail;
    uint32_t trailCount;
    uint32_t lastTrailUpdate;
};
```

| Field | Type | Default | Description |
|-------|------|---------|-------------|
| shellId | uint32_t | auto | Unique shell identifier |
| position | Vector2 | (0,0) | Current shell position |
| velocity | Vector2 | (0,0) | Shell velocity vector |
| active | bool | true | Whether shell is active |
| creationTime | uint32_t | tick | When shell was created |
| **ownerColor** | **uint32_t** | **0xFFFFFFFF** | **NEW: RGBA color from firing robot** |
| trail | array<TrailPoint,20> | {} | Trail points for rendering |
| trailCount | uint32_t | 0 | Number of active trail points |
| lastTrailUpdate | uint32_t | 0 | Last trail update tick |

### Viewer Class (Viewer.h)

**New class member**:
```cpp
class Viewer {
    // ... existing members ...
    static SDL_Texture* skullTexture;  // NEW: Cached skull symbol texture
};
```

| Member | Type | Scope | Description |
|--------|------|-------|-------------|
| **skullTexture** | **SDL_Texture*** | **static** | **NEW: Cached "☠" texture for scoreboard** |

## Color Palette

**Existing** (Viewer.cpp):
```cpp
unsigned int Viewer::colors[] = {
    0xFFF000FF,  // Yellow - Robot 0
    0xFF0F00FF,  // Red - Robot 1  
    0xFF00F0FF,  // Cyan - Robot 2
    0xFF000FFF   // Blue - Robot 3
};
```

**Color format**: 0xRRGGBBAA (Red, Green, Blue, Alpha)

| Index | Color | Hex Value | Usage |
|-------|-------|-----------|-------|
| 0 | Yellow | 0xFFF000FF | First robot |
| 1 | Red | 0xFF0F00FF | Second robot |
| 2 | Cyan | 0xFF00F0FF | Third robot |
| 3 | Blue | 0xFF000FFF | Fourth robot |

**Dead state color modification**:
- Alpha reduced from 0xFF to 0x80 (50% opacity)
- Example: Yellow dead = 0xFFF00080

## Relationships

```
┌─────────────────┐
│     Viewer      │
├─────────────────┤
│ colors[4]       │──────┐
│ robots<id,RobPos>      │
│ skullTexture    │      │
└─────────────────┘      │
        │                │
        │ contains       │ assigns
        ▼                │
┌─────────────────┐      │
│     RobPos      │◄─────┘
├─────────────────┤
│ x, y            │
│ color           │──────┐
│ isDead          │      │
│ energy, armor   │      │ propagates to
│ textures        │      │
└─────────────────┘      │
                         │
                         ▼
┌─────────────────┐    ┌─────────────────┐
│   ShellPool     │    │  AnimatedShell  │
├─────────────────┤    ├─────────────────┤
│ activeShells[]  │───►│ ownerColor      │◄── robot color
│ createShell()   │    │ position, vel   │
│ renderShells()  │    │ trail[]         │
└─────────────────┘    └─────────────────┘
```

## Validation Rules

| Entity | Field | Rule |
|--------|-------|------|
| RobPos | color | Must be from colors[] array (index = robot ID % 4) |
| RobPos | isDead | Once true, cannot revert to false |
| RobPos | isDead | Textures should be destroyed when set to true |
| AnimatedShell | ownerColor | Must be valid RGBA (alpha > 0) |
| Viewer | skullTexture | Must be initialized before first dead robot |

## Data Flow

### Robot Color Assignment
```
Robot loaded → Scheduler::addRobot()
            → Viewer::_RobotShow(id, x, y)
            → robots[id].color = colors[id % maxRobots]
```

### Shell Color Propagation
```
Robot fires → Scheduler creates CannonShotEvent
           → Viewer::_CannonShow(id, x, y, heading)
           → shellPool->createShell(x, y, vel, robots[id].color)
           → shell.ownerColor = color
```

### Dead State Transition
```
Robot armor = 0 → RobCtrl::tick() returns false
              → Scheduler::tickEnd() detects death
              → Viewer::markRobotDead(id)
              → robots[id].isDead = true
              → Destroy textures (keep position/color)
```

### Scoreboard Dead Indication
```
PrintRobotStatus(id) → check robots[id].isDead
                     → if dead: render skull, reduce alpha
                     → render name in robot color (with alpha)
                     → render color indicator box
```
