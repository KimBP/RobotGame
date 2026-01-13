# Implementation Plan: Edge Patrol Robot

**Branch**: `001-edge-patrol-robot` | **Date**: 2025-01-12 | **Spec**: [spec.md](./spec.md)
**Input**: Feature specification from `/specs/001-edge-patrol-robot/spec.md`

**Note**: This template is filled in by the `/speckit.plan` command. See `.specify/templates/commands/plan.md` for the execution workflow.

## Summary

Create a new sample robot that patrols the arena edges while shooting toward the center, with adaptive threat detection within ±10 degrees of movement direction. The robot will be implemented as a C++ shared library (.so file) that follows the existing RobotGame architecture, using the Robot base class API for movement, scanning, and shooting. The robot will use the Trigonometry utility for calculations and be loaded dynamically by the game engine.

## Technical Context

**Language/Version**: C++11 (based on existing .cproject configuration)
**Primary Dependencies**: RobotGame framework (Robot, RobCtrl, Trigonometry classes), POSIX threading (pthread), SDL2 libraries (for game visualization)
**Storage**: N/A (in-memory game state)
**Testing**: Manual testing via game execution and observation
**Target Platform**: Linux (Linux GCC toolchain)
**Project Type**: Single project (C++ desktop application with plugin architecture)
**Performance Goals**: Robot execution must complete within game tick timeframe (typically ~16ms for 60fps)
**Constraints**: Robot code runs in separate thread, must complete quickly to not block game loop; must not exceed max 3 shots in air simultaneously; cannon fire costs 200 energy
**Scale/Scope**: Single robot implementation as a sample plugin; arena size 9999x9999 units

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

**Constitution Status**: Constitution file exists but contains only template placeholders. No active constraints to validate.

## Project Structure

### Documentation (this feature)

```text
specs/001-edge-patrol-robot/
├── plan.md              # This file (/speckit.plan command output)
├── research.md          # Phase 0 output (/speckit.plan command)
├── data-model.md        # Phase 1 output (/speckit.plan command)
├── quickstart.md        # Phase 1 output (/speckit.plan command)
└── checklists/          # Quality checklists
    └── requirements.md  # Spec quality checklist
```

### Source Code (repository root)

```text
# Robot plugin structure (DEFAULT)
Robots/
├── Runner/              # Existing sample robot
│   ├── Runner.h
│   └── Runner.cpp
├── Scanner/             # Existing sample robot
│   ├── Scanner.h
│   └── Scanner.cpp
└── EdgePatrol/          # New sample robot (to be created)
    ├── EdgePatrol.h
    └── EdgePatrol.cpp
```

**Structure Decision**: The new EdgePatrol robot will follow the established pattern of sample robots (Runner, Scanner) in the Robots/ directory. Each robot is a self-contained C++ class in its own subdirectory with header and implementation files. Robots are compiled as shared libraries and loaded dynamically via dlopen at runtime.

## Complexity Tracking

> **Fill ONLY if Constitution Check has violations that must be justified**

No constitution violations exist to track.
