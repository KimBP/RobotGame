## Why

Robot colors shown in the arena do not match the colors shown in the scoreboard for the same robots. The first and last robots have their colors swapped, while the middle robot appears correct. This makes it impossible to visually correlate scoreboard entries with arena robots.

## What Changes

- Fix `Viewer::PrintRobot()` to extract and pass RGBA color components explicitly rather than passing the packed `uint32_t` value to `filledCircleColor()`.
- Replace the `filledCircleColor()` call with `filledCircleRGBA()` using the same bit-shift component extraction already used everywhere else in the codebase.

## Capabilities

### New Capabilities

- `robot-color-consistency`: Arena robot circles and scoreboard color indicators use the same color for each robot.

### Modified Capabilities

<!-- No existing spec-level requirements are changing. -->

## Impact

- `Viewer.cpp`: `PrintRobot()` function only — a one-line change in the arena rendering path.
- No API, protocol, or data model changes.
- No change to the `colors[]` array values or color assignment logic.
- The dead-robot 50% alpha behavior is preserved.
