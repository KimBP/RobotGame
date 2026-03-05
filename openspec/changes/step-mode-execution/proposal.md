## Why

Debugging robot behavior and understanding game state changes requires the ability to pause execution and inspect the game state between ticks. Currently, the game runs continuously at maximum speed, making it difficult for developers to observe robot movements, collisions, and state transitions frame-by-frame. Adding a step mode enables frame-by-frame execution for development and debugging.

## What Changes

- Add command-line argument `--step` to enable step mode
- When step mode is enabled, the game loop pauses after each tick and waits for the user to press Enter to advance to the next tick
- Console output provides clear indication that the game is in step mode and waiting for input
- Step mode is mutually exclusive with normal (continuous) execution mode

## Capabilities

### New Capabilities
- `step-mode`: Enables frame-by-frame execution of game ticks controlled by console user input (Enter key)

### Modified Capabilities
<!-- No existing capabilities require specification changes -->

## Impact

**Affected Code:**
- RobotGame.cpp: Main game loop and initialization code
- Command-line argument parsing in main()

**APIs:**
- No public API changes; feature is console-facing only

**Dependencies:**
- No new dependencies (uses existing SDL2 and console I/O)

**Systems:**
- Game scheduler/tick system affected
- Console input handling required
