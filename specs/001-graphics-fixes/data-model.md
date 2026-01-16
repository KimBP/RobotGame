# Data Model: Graphics Fixes

**Date**: 2026-01-13  
**Feature**: Graphics Fixes (001-graphics-fixes)

## Core Entities

### BattleTimingConfig
Represents configurable timing control for battle animation speed.

**Fields**:
- `delayMs` (int): Milliseconds between game ticks (1-1000, default: 16)
- `useVSync` (bool): Whether VSync is enabled (default: true)
- `vsyncFallback` (bool): Whether to use frame rate capping if VSync fails (default: true)

**Validation Rules**:
- delayMs must be between 1 and 1000 milliseconds
- useVSync can only be disabled if vsyncFallback is enabled
- Changes take effect at next tick boundary

**State Transitions**:
```
[Default] --delay specified--> [Custom Timing]
[Default] --vsync fails--> [Fallback Mode]
[Fallback Mode] --vsync available--> [Default]
```

### AnimatedShell
Enhanced shell representation with motion trail history.

**Fields**:
- `shellId` (uint32_t): Unique identifier for the shell
- `position` (Vector2): Current x,y coordinates
- `velocity` (Vector2): Movement direction and speed
- `active` (bool): Whether shell is currently in flight
- `creationTime` (uint32_t): Timestamp when shell was created
- `trail` (TrailPoint[]): Array of position history points

**TrailPoint Structure**:
- `position` (Vector2): Position at this trail point
- `timestamp` (uint32_t): When this position was recorded
- `alpha` (float): Transparency for fading effect (0.0-1.0)

**Validation Rules**:
- Maximum trail length: 20 points
- Trail point lifetime: 500ms
- Maximum active shells: 100
- Position bounds: 0-1000 for both x and y

### Explosion
Animated explosion effect with particle system.

**Fields**:
- `explosionId` (uint32_t): Unique identifier
- `position` (Vector2): Center of explosion
- `currentRadius` (float): Current explosion radius
- `maxRadius` (float): Maximum explosion radius
- `startTime` (uint32_t): Animation start timestamp
- `active` (bool): Whether explosion is animating
- `particles` (Particle[]): Array of debris particles

**Particle Structure**:
- `position` (Vector2): Current particle position
- `velocity` (Vector2): Particle movement direction and speed
- `life` (float): Particle lifetime (1.0 = fully visible, 0.0 = invisible)
- `color` (uint32_t): Particle color (RGBA)

**Validation Rules**:
- Maximum simultaneous explosions: 50
- Particles per explosion: 20
- Animation duration: 1000ms
- Max explosion radius: 80 pixels

### RenderLayer
Texture-based rendering layer for proper visual compositing.

**Fields**:
- `layerType` (enum): TERRAIN, ROBOT, EXPLOSION
- `texture` (SDL_Texture*): Render target texture
- `width`, `height` (int): Layer dimensions
- `blendMode` (SDL_BlendMode): Transparency blending mode

**Validation Rules**:
- All layers must have same dimensions (1000x1000 + status area)
- Texture format: RGBA8888
- All layers must use SDL_BLENDMODE_BLEND except terrain

## Relationships

```
BattleTimingConfig 1..1 Scheduler
AnimatedShell 0..* Viewer
Explosion 0..* Viewer
RenderLayer 3 Viewer

AnimatedShell 0..* TrailPoint
Explosion 0..* Particle
```

## Data Flow

```
[CannonShell::tick()] --create--> [AnimatedShell]
[AnimatedShell.update()] --add--> [TrailPoint]
[AnimatedShell] --collision--> [Explosion]
[Explosion.update()] --update--> [Particle]
[Viewer.render()] --composite--> [RenderLayer[]]
```

## Performance Constraints

### Memory Limits
- Shell pool: 100 shells × 20 trail points = 2000 positions
- Explosion pool: 50 explosions × 20 particles = 1000 particles  
- Layer textures: 3 × 1400×1000 × 4 bytes = ~16.8MB VRAM

### Update Frequency
- Shell trail updates: Every frame (60 FPS target)
- Explosion animations: Every frame
- Layer compositing: Every frame
- Timing configuration: Tick boundary only

### State Persistence
- No disk persistence required (all in-memory)
- State resets between battles
- Configuration only lasts for current session

## Integration Points

### Existing Classes
- **Scheduler**: Add BattleTimingConfig member and timing logic
- **Viewer**: Add AnimatedShell pool, Explosion system, RenderLayer management
- **CannonShell**: Post creation events instead of direct viewer updates
- **RobotGame**: Add command-line parsing for BattleTimingConfig initialization

### New Dependencies
- No external libraries required
- Uses existing SDL2, SDL2_gfx primitives
- Standard C++17 features only

## Error Handling

### Validation Errors
- Invalid delay values: Use default, log warning
- Shell pool exhaustion: Reuse oldest shell
- Layer creation failure: Fallback to immediate rendering
- Texture allocation failure: Reduce quality or disable effects

### Runtime Errors
- VSync failure detected: Enable frame rate capping
- Memory allocation failure: Reduce effect quality dynamically
- Renderer context loss: Recreate layers on recovery