## Why

When running the game with `--step`, the viewer thread blocks indefinitely in its event-wait loop while the scheduler waits for the user to press Enter, so the last rendered frame is never displayed — robots and shells disappear from the screen between ticks. Users must press Enter rapidly to produce enough frames to see game state, which defeats the purpose of single-step inspection.

## What Changes

- Fix the viewer's condition-variable wait predicate so that the "step paused" signal reliably wakes the viewer thread.
- While the scheduler is blocked waiting for Enter, the viewer renders the frozen last-known game state continuously at a low refresh rate (≈100 ms), keeping robots, shells, and the scoreboard visible.

## Capabilities

### New Capabilities

- `step-mode-frozen-display`: Viewer remains active and displays a frozen frame of the current game state (robots, shells, scoreboard) while the scheduler is paused waiting for user input in `--step` mode.

### Modified Capabilities

<!-- No existing spec-level capability requirements are changing. -->

## Impact

- **Viewer.cpp** – `Viewer::Runner()`: the `eventCv.wait()` predicate gains an `isStepPaused()` condition so a pause notification reliably exits the wait.
- No API changes, no new dependencies, no behavioral change outside `--step` mode.
