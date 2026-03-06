## Context

The RobotGame's main game loop currently runs continuously in RobotGame.cpp, executing ticks as fast as possible. For debugging and development, developers need the ability to pause after each tick and manually advance by pressing Enter. This requires modifications to command-line parsing and the main game loop execution flow.

**Current State:**
- Main game loop uses the Scheduler to tick robots
- No step-mode awareness in the loop
- Command-line arguments not currently parsed in main

**Constraints:**
- Must work with existing SDL2-based renderer and game architecture
- Cannot block SDL event polling for extended periods (would freeze rendering)
- Step mode is for development/debugging, not production gameplay

## Goals / Non-Goals

**Goals:**
- Parse `--step` command-line argument
- Support step-mode execution where Enter keypress advances the game by one tick
- Display clear console feedback about step mode status
- Maintain backward compatibility with normal (continuous) execution

**Non-Goals:**
- Persist step-mode recordings
- Network/remote debugging features
- UI overlays or graphics changes for step mode
- Frame buffering or replay functionality

## Decisions

### 1. Command-Line Argument Parsing
**Decision**: Parse `--step` in main() before game initialization.
**Rationale**: Simple, clear, and doesn't require changes to other initialization code.
**Implementation**: Check command-line args to set a `stepModeEnabled` flag.

### 2. Step Mode Control Variable
**Decision**: Use a simple boolean flag passed to the game loop or stored in a shared location.
**Rationale**: Minimal changes to existing code; alternative would be to refactor game loop into a class (more complex).

### 3. Input Handling during Step Mode
**Decision**: Use standard input (std::cin or std::getline) to wait for Enter key.
**Rationale**: No new dependencies, works cross-platform, debugging-friendly. Step mode is development-only.
**Alternative Considered**: Threading for non-blocking input—rejected due to complexity and potential race conditions with game state updates.

### 4. Game Loop Modification
**Decision**: After each tick, if step mode is enabled, prompt user and wait for Enter.
**Rationale**: Minimal invasiveness; the tick execution remains unchanged, only the wait-between-ticks behavior differs.
**Placement**: In RobotGame.cpp main loop, after scheduler.tick() and rendering.

### 5. Console Output
**Decision**: Use std::cout with flush to ensure prompts appear immediately.
**Rationale**: Ensures user sees the prompt even if stdout is buffered.

## Risks / Trade-offs

**Risk**: Console input blocking might cause frame drops.
→ **Mitigation**: Step mode is for development; document that it's not for production. Accept this as expected behavior.

**Risk**: Multiple threads accessing step state (if threading is used elsewhere).
→ **Mitigation**: Keep step control in main thread only; avoid shared state.

**Risk**: SDL event queue filling up during stdin wait.
→ **Mitigation**: SDL continues processing events from the queue; occasional frame drops acceptable in step mode.

## Migration Plan

1. No breaking changes to existing APIs or game state
2. Step mode is opt-in via command-line flag
3. Default behavior (without `--step`) remains unchanged
4. No database, file, or configuration changes required

**Rollback**: Simply don't pass `--step` flag; binary remains fully backward compatible.

## Open Questions

- Should step mode be toggleable at runtime (e.g., press 'S' to toggle), or only via startup argument?
  - **Current Plan**: Startup argument only for simplicity. Can be enhanced later.

- Should there be a visual indicator in the SDL window that step mode is active?
  - **Current Plan**: Console messages only. Can add visual indicator in future iteration.
