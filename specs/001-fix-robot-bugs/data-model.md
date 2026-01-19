# Data Model: Robot Game Bug Fixes and Arena Border

**Feature**: Fix Robot Game Bugs and Add Arena Border  
**Date**: 2026-01-14  
**Phase**: 1 - Design & Contracts

## Core Entities

### Robot

**Description**: Game entity that participates in combat with armor, energy, and visual representation.

**Attributes**:
- `id`: Unique identifier (integer)
- `name`: Display name for the robot (string)
- `position`: Current coordinates (x, y) in arena units
- `armor`: Current health value (0-100, integer)
- `maxArmor`: Maximum possible armor (100, constant)
- `energy`: Current energy for actions (0-100, integer)
- `maxEnergy`: Maximum energy (100, constant)
- `speed`: Current movement speed
- `alive`: Life status (boolean)
- `nameTexture`: Rendered name texture (SDL_Texture*)
- `energyTexture`: Rendered energy texture (SDL_Texture*)

**State Transitions**:
1. **Alive → Damaged**: `armor = max(0, armor - damage)`
2. **Damaged → Dead**: Triggered when `armor == 0`
3. **Dead → Removed**: Robot removed from game, textures cleaned up

**Validation Rules**:
- `armor` must be between 0 and `maxArmor`
- `energy` must be between 0 and `maxEnergy`
- `position` must be within arena bounds (0-10000 units)
- `nameTexture` and `energyTexture` must be valid SDL textures when robot is alive

### Arena

**Description**: Game area where robots move and interact, with visual boundaries.

**Attributes**:
- `width`: Arena width in pixels (1000)
- `height`: Arena height in pixels (1000)
- `offsetX`: Horizontal offset from window edge (400 pixels for status area)
- `offsetY`: Vertical offset from window edge (0)
- `borderVisible`: Whether border is currently displayed (boolean)
- `borderColor`: Dark blue RGBA color values for border (r, g, b, a)
- `borderThickness`: Width of border line in pixels

**Layout**:
```
Window Layout:
┌─────────────────────────────────────┐
│ [400px STATUS] │ [1000px ARENA]   │
│   Scoreboard   │   Robot Arena    │
│                │   + Border       │
└─────────────────────────────────────┘
      1000px height total
```

### TextRendering

**Description**: Management of text textures for robot information display.

**Attributes**:
- `fontPath`: Path to font file (`/usr/share/fonts/truetype/freefont/FreeSansBold.ttf`)
- `fontSize`: Font size for text rendering (integer)
- `font`: Loaded SDL font pointer (TTF_Font*)
- `textCache`: Cache mapping text strings to SDL textures
- `maxCacheSize`: Maximum number of cached textures

**Error States**:
- `fontLoadFailed`: Font file not found or invalid
- `textureCreateFailed`: Unable to create texture from text
- `textureInvalid`: Texture reference is null or corrupted

## Damage System

### DamageCalculation

**Description**: Calculates damage based on explosion distance and collision events.

**Damage by Distance**:
- Range 0-10 units: 8 damage
- Range 11-20 units: 4 damage
- Range 21-30 units: 2 damage
- Range 31-40 units: 1 damage
- Range >40 units: 0 damage

**Wall Collision Damage**: `damage = (speed * 2) / 10`
**Robot Collision Damage**: `damage = 2 * (speed1 + speed2) / 10`

### ArmorReduction

**Formula**: `newArmor = max(0, currentArmor - calculatedDamage)`

**Edge Cases**:
- **Overkill damage**: Damage exceeds current armor should result in exactly 0 armor
- **Simultaneous hits**: Multiple damage sources should accumulate properly
- **Float precision**: All armor values must be integers, round down fractional damage

## Rendering Pipeline

### ViewportManagement

**Arena Viewport**:
- Source rectangle: {0, 0, ARENA_WIDTH, ARENA_HEIGHT}
- Destination rectangle: {STATUS_WIDTH, 0, ARENA_WIDTH, ARENA_HEIGHT}

**Rendering Order**:
1. Clear arena background
2. Draw border (new feature)
3. Render terrain/obstacles
4. Render robots
5. Render effects (explosions)
6. Render UI overlays

### TextureLifecycle

**Creation**:
1. Load font from system path
2. Render text to surface using TTF_RenderText_Blended()
3. Convert surface to texture using SDL_CreateTextureFromSurface()
4. Store texture reference in robot data structure

**Cleanup**:
1. Destroy texture when robot dies
2. Remove from texture cache when no longer needed
3. Clean up font resources on shutdown

## Relationships

```
Robot (1) ──has──→ TextRendering (1)
Robot (*) ──exists in──→ Arena (1)
Arena (1) ──displays──→ Robot (*)
Robot (*) ──receives──→ DamageCalculation (1)
```

## Constraints

**Performance**:
- All rendering must complete within 16ms (60 FPS)
- Texture cache size limited by available GPU memory
- Border rendering should add minimal overhead (<1ms)

**Memory**:
- Robot textures must be cleaned up when robots die
- Font resources shared across all robots
- Texture cache should implement LRU eviction if needed

**Compatibility**:
- Must work with existing robot plugin system
- Cannot change robot interface contracts
- Must preserve existing game mechanics