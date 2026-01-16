# Implementation Plan: Edge Navigation Robot

**Branch**: `001-edge-robot` | **Date**: 2026-01-12 | **Spec**: [link](spec.md)
**Input**: Feature specification from `/specs/001-edge-robot/spec.md`

**Note**: This template is filled in by the `/speckit.plan` command. See `.specify/templates/commands/plan.md` for the execution workflow.

## Summary

Create a new sample robot that demonstrates advanced navigation and combat tactics by moving along arena edges while avoiding obstacles. The robot will continuously shoot toward the arena center unless it detects enemy robots within ±10 degrees of its heading, at which point it will prioritize threat engagement.

## Technical Context

**Language/Version**: C++17  
**Primary Dependencies**: SDL2, SDL2_ttf, SDL2_gfx, SDL2_image  
**Storage**: Files (in-memory robot state)  
**Testing**: Manual testing in game arena  
**Target Platform**: Linux executable  
**Project Type**: Single C++ project with modular robot samples  
**Performance Goals**: Real-time robot control (60+ ticks/second)  
**Constraints**: Memory < 100MB, CPU usage < 10% per robot  
**Scale/Scope**: Single robot implementation with ~300 lines of code

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

✅ **PHASE 0 PASSED**: Implementation follows established robot sample patterns and maintains project structure. No constitutional violations identified.

✅ **PHASE 1 RE-CHECK PASSED**: Design phase completed successfully. All contracts and data models align with project architecture. Build integration follows existing patterns.

## Project Structure

### Documentation (this feature)

```text
specs/[###-feature]/
├── plan.md              # This file (/speckit.plan command output)
├── research.md          # Phase 0 output (/speckit.plan command)
├── data-model.md        # Phase 1 output (/speckit.plan command)
├── quickstart.md        # Phase 1 output (/speckit.plan command)
├── contracts/           # Phase 1 output (/speckit.plan command)
└── tasks.md             # Phase 2 output (/speckit.tasks command - NOT created by /speckit.plan)
```

### Source Code (repository root)

```text
Robots/
├── Runner/
│   ├── Runner.h
│   └── Runner.cpp
├── Scanner/
│   ├── Scanner.h
│   └── Scanner.cpp
└── EdgeRobot/                    # NEW
    ├── EdgeRobot.h               # NEW
    └── EdgeRobot.cpp            # NEW

# Existing core files (unchanged)
Robot.h
Robot.cpp
RobotTypes.h
RobCtrl.h
RobCtrl.cpp
[...other core game files...]
```

**Structure Decision**: Following established sample robot pattern. New EdgeRobot directory placed parallel to existing Runner and Scanner samples in Robots/ directory. This maintains project consistency and follows the existing modular architecture.

## Complexity Tracking

> **Fill ONLY if Constitution Check has violations that must be justified**

| Violation | Why Needed | Simpler Alternative Rejected Because |
|-----------|------------|-------------------------------------|
| [e.g., 4th project] | [current need] | [why 3 projects insufficient] |
| [e.g., Repository pattern] | [specific problem] | [why direct DB access insufficient] |
