## Context

The game uses SDL2_gfx's `filledCircleColor()` in `Viewer::PrintRobot()` to draw robot circles in the arena. The function's API accepts a packed `uint32_t` color argument and — on little-endian x86/x64 hardware — extracts the R, G, B, A components from bytes 0–3 (LSB→MSB), treating byte 0 (LSB) as R and byte 3 (MSB) as A.

Everywhere else in the codebase (the scoreboard color box, the text texture creation in `_RobotDataShow`, the name rendering), color components are extracted with right-bit-shifts that treat the most significant byte as R:

```cpp
r = (color >> 24) & 0xFF;
g = (color >> 16) & 0xFF;
b = (color >>  8) & 0xFF;
a = (color >>  0) & 0xFF;
```

The `colors[]` array values were authored with this MSB=R convention:

```cpp
unsigned int Viewer::colors[] = {
    0xFFF000FF,  // Yellow  (R=FF, G=F0, B=00, A=FF)
    0xFF0F00FF,  // Red     (R=FF, G=0F, B=00, A=FF)
    0xFF00F0FF,  // Purple  (R=FF, G=00, B=F0, A=FF)
    0xFF000FFF   // Cyan    (R=FF, G=00, B=0F, A=FF)
};
```

When `filledCircleColor()` receives `0xFFF000FF` on a little-endian machine, it reads byte 0 (value `0xFF`) as R, byte 1 (`0x00`) as G, byte 2 (`0xF0`) as B — yielding purple, not yellow. The scoreboard correctly shows Yellow for the same color value, causing the visible mismatch.

## Goals / Non-Goals

**Goals:**
- Arena robot circles display the same color as the scoreboard entry for that robot.
- Dead robots continue to render at 50% alpha.
- No changes to `colors[]` values, color assignment logic, or scoreboard rendering.

**Non-Goals:**
- Changing the color palette or adding more colors.
- Platform-specific color endianness abstraction.
- Modifying any SDL color handling outside of `PrintRobot()`.

## Decisions

### Replace `filledCircleColor` with `filledCircleRGBA`

`SDL2_gfx` provides both:
- `filledCircleColor(renderer, x, y, r, uint32_t color)` — byte-order-dependent on the calling platform.
- `filledCircleRGBA(renderer, x, y, r, R, G, B, A)` — explicit components, unambiguous.

**Decision**: Use `filledCircleRGBA` with the same component extraction used everywhere else.

```cpp
void Viewer::PrintRobot(int id) {
    unsigned int c = robots[id].color;
    Uint8 r = (c >> 24) & 0xFF;
    Uint8 g = (c >> 16) & 0xFF;
    Uint8 b = (c >>  8) & 0xFF;
    Uint8 a = robots[id].isDead ? 0x80 : (c & 0xFF);
    filledCircleRGBA(gRenderer, robots[id].x, robots[id].y, ROBOT_RADIUS, r, g, b, a);
}
```

**Alternative considered**: Byte-swap `colors[]` values to match what `filledCircleColor` expects on little-endian. Rejected — it would require understanding all call sites that read `robots[id].color` and would make the color literal values non-intuitive.

**Alternative considered**: Wrap in a helper that converts from MSB-R encoding to platform byte order. Rejected — over-engineering for a one-site fix.

## Risks / Trade-offs

- **Risk**: On a big-endian platform `filledCircleColor` and the scoreboard would have agreed, so this fix would break big-endian builds → **Mitigation**: The project only targets x86/x64 Linux; no big-endian support exists or is planned.
- **No migration required**: Pure visual fix, no data or API change.
