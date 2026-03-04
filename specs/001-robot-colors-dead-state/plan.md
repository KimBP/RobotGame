# Implementation Plan: Robot Colors and Dead State Visualization

**Branch**: `001-robot-colors-dead-state` | **Date**: 2026-01-22 | **Spec**: [spec.md](./spec.md)
**Input**: Feature specification from `/specs/001-robot-colors-dead-state/spec.md`

## Summary

Implement visual differentiation for robots through unique color assignment, extend color usage to shells and scoreboard, and add dead robot visualization (50% opacity in arena, reduced opacity + skull symbol in scoreboard). The existing color array and RobPos struct provide foundation; modifications focus on preventing dead robot removal and propagating colors to shells/scoreboard.

## Technical Context

**Language/Version**: C++17 (MANDATORY)  
**Primary Dependencies**: SDL2, SDL2_image, SDL2_ttf, SDL2_gfx (MANDATORY)  
**Storage**: In-memory game state only (no persistent storage)  
**Testing**: Manual testing through simulation runs  
**Target Platform**: Linux (MANDATORY for SDL2 compatibility)  
**Project Type**: Single executable with plugin architecture  
**Performance Goals**: 60 fps visualization, deterministic physics, <1ms tick processing  
**Constraints**: Thread-safe event system, plugin symbol visibility hidden, real-time scheduling  
**Scale/Scope**: Single arena simulation, 2-8 robot plugins, <10000 LOC core framework

**Feature-Specific Technical Details**:
- Color format: RGBA 32-bit (0xRRGGBBAA) used by SDL2_gfx
- Existing color array: 4 colors at Viewer.cpp:19-24
- Robot rendering: filledCircleColor() at Viewer.cpp:506-512
- Shell rendering: Currently hardcoded white at ShellPool.cpp:126-151
- Death handling: cleanupRobotTextures() removes robots at Viewer.cpp:192-217
- Scoreboard: PrintRobotStatus() at Viewer.cpp:530-578 already uses robot colors

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

| Principle | Status | Rationale |
|-----------|--------|-----------|
| Plugin Architecture | PASS | Feature modifies Viewer only; no changes to robot plugin interface |
| Real-time Events | PASS | Visual changes only; no new timing paths; uses existing RobotPosEvent |
| Hardware Abstraction | PASS | No robot code changes; RobCtrl interface unchanged |
| Simulation Integrity | PASS | Visual-only; no physics changes; dead state is display property |
| Visual Observability | PASS | Enhances observability with colors and dead state visualization |

**Gate Result**: PASS - No violations. Feature is purely visual enhancement.

### Post-Design Re-Check (Phase 1 Complete)

| Principle | Status | Post-Design Rationale |
|-----------|--------|----------------------|
| Plugin Architecture | PASS | No changes to robot plugin interface. All modifications in Viewer/ShellPool. |
| Real-time Events | PASS | Uses existing RobotPosEvent. markRobotDead() is synchronous from Scheduler. |
| Hardware Abstraction | PASS | RobCtrl interface unchanged. All changes in visualization layer. |
| Simulation Integrity | PASS | Visual only. Physics unchanged. isDead flag doesn't affect game logic. |
| Visual Observability | PASS | Enhanced: distinct robot colors, dead robots visible, scoreboard shows status. |

**Post-Design Gate Result**: PASS - Design complies with all constitution principles.

## Project Structure

### Documentation (this feature)

```text
specs/001-robot-colors-dead-state/
├── plan.md              # This file
├── research.md          # Phase 0 output
├── data-model.md        # Phase 1 output
├── quickstart.md        # Phase 1 output
├── contracts/           # Phase 1 output (N/A - no API)
└── tasks.md             # Phase 2 output
```

### Source Code (repository root)

```text
# Existing structure - files to modify
src/
├── Viewer.h             # Add isDead flag to RobPos, skull texture
├── Viewer.cpp           # Modify rendering, prevent dead robot removal
├── ShellPool.h          # Add ownerColor to AnimatedShell
├── ShellPool.cpp        # Use owner color for shell rendering
└── Scheduler.cpp        # Replace cleanup with markRobotDead()
```

**Structure Decision**: No new files or directories. Modifications to existing Viewer, ShellPool, and Scheduler components only.

## Complexity Tracking

> No violations. Feature is a visual enhancement that fits within existing architecture.

| Item | Complexity | Justification |
|------|------------|---------------|
| Dead robot state | Low | Single boolean flag in existing struct |
| Shell coloring | Low | Add color field and propagate existing robot color |
| Skull symbol | Medium | Requires TTF rendering or texture loading |
