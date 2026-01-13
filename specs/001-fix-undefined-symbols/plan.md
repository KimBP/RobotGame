# Implementation Plan: Fix Undefined Symbols in Robot Shared Libraries

**Branch**: `001-fix-undefined-symbols` | **Date**: 2026-01-13 | **Spec**: [spec.md](spec.md)
**Input**: Feature specification from `/specs/001-fix-undefined-symbols/spec.md`

**Note**: This template is filled in by the `/speckit.plan` command. See `.specify/templates/commands/plan.md` for the execution workflow.

## Summary

Fix undefined symbol errors when loading robot plugins (shared libraries) to enable standalone robot development. The EdgePatrol robot plugin fails to load with undefined symbol `RobotGame::EdgePatrol::name() const`. The solution must support standalone robot building with only Robot.h and Logger.h as API files, without requiring access to full arena source code.

## Technical Context

**Language/Version**: C++11 (based on existing .cproject configuration)
**Primary Dependencies**: RobotGame framework (Robot, RobCtrl, Trigonometry classes), POSIX threading (pthread), SDL2 libraries (for game visualization)
**Storage**: N/A (in-memory game simulation)
**Testing**: Manual testing via game execution
**Target Platform**: Linux
**Project Type**: Single project with plugin architecture
**Performance Goals**: Load time <100ms per robot plugin
**Constraints**: Robots must compile standalone with only Robot.h and Logger.h API headers
**Scale/Scope**: Multiple robot plugins (EdgePatrol, Runner, Scanner), each as independent shared library

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

No constitution defined in project - no gates to check.

## Project Structure

### Documentation (this feature)

```text
specs/001-fix-undefined-symbols/
├── plan.md              # This file (/speckit.plan command output)
├── research.md          # Phase 0 output (/speckit.plan command)
├── data-model.md        # Phase 1 output (/speckit.plan command)
├── quickstart.md        # Phase 1 output (/speckit.plan command)
├── contracts/           # Phase 1 output (/speckit.plan command)
└── tasks.md             # Phase 2 output (/speckit.tasks command - NOT created by /speckit.plan)
```

### Source Code (repository root)

```text
# Existing robot plugin structure
Robots/
├── EdgePatrol/
│   ├── EdgePatrol.h
│   ├── EdgePatrol.cpp
│   └── CMakeLists.txt
├── Runner/
│   ├── Runner.h
│   ├── Runner.cpp
│   ├── .cproject
│   ├── .project
│   └── Makefile
└── Scanner/
    ├── Scanner.h
    ├── Scanner.cpp
    ├── .cproject
    ├── .project
    └── Makefile

# Core arena files
Robot.h           # API header for robot plugins
Robot.cpp         # Robot base class implementation
Logger.h          # API header for logging
Logger.cpp        # Logger implementation
RobotGame.cpp     # Main entry point with plugin loader
```

**Structure Decision**: Robot plugins follow existing pattern in Robots/ directory. Each robot is a self-contained C++ class in its own subdirectory with header and implementation files. Robots are compiled as shared libraries and loaded dynamically via dlopen at runtime. Key requirement: plugins must compile standalone with only Robot.h and Logger.h headers.

## Complexity Tracking

> **Fill ONLY if Constitution Check has violations that must be justified**

No constitution violations to justify.

---

## Phase 0: Research & Unknowns Resolution

### Unknowns to Resolve

1. **Plugin Architecture Pattern**: How to build C++ shared library plugins that can be compiled standalone without linking to main executable source code?

2. **Virtual Method Linkage**: How should virtual methods (like `name()`) be implemented in plugins to avoid undefined symbols when loaded via dlopen?

3. **API Header Isolation**: What must Robot.h and Logger.h provide to enable standalone robot compilation without access to arena implementation files?

4. **Symbol Visibility**: Which symbols need to be exported/imported for the plugin architecture to work correctly?

5. **Existing Working Pattern**: Analyze why Runner and Scanner plugins work correctly while EdgePatrol fails - what's the difference?

### Research Tasks

- **Task 1**: Investigate C++ shared library plugin architecture patterns for standalone plugin compilation
- **Task 2**: Analyze Runner and Scanner implementations to identify correct pattern for virtual method implementation
- **Task 3**: Review Robot.h and Logger.h headers to determine what APIs are exposed for standalone robot development
- **Task 4**: Research symbol visibility and linking requirements for dlopen-based plugin systems
- **Task 5**: Examine EdgePatrol implementation to identify specific cause of undefined symbol error

### Research Summary

**✅ COMPLETED**: All unknowns resolved (see [research.md](research.md))

**Key Findings**:
1. **Root Cause**: EdgePatrol's `name()` method declared but not implemented
2. **Working Pattern**: Runner/Scanner implement `name()` inline in headers (weak symbols)
3. **API Requirements**: Robot.h + RobotTypes.h + Logger.h + Trigonometry.h
4. **Symbol Resolution**: dlopen with RTLD_LAZY, no undefined symbols allowed
5. **Build Requirements**: -fPIC -shared flags, proper include paths

---

## Phase 1: Design & Contracts

### Data Model

**✅ COMPLETED**: [data-model.md](data-model.md) created

**Key Entities Defined**:
- Robot Plugin (shared library entity)
- Robot Base Class (abstract interface)
- Robot Instance (runtime state)
- API Headers (Robot.h, RobotTypes.h, Logger.h, Trigonometry.h)
- Symbol Table (T/W/U/V symbol types)

### API Contracts

**✅ COMPLETED**: [contracts/robot-plugin-api.md](contracts/robot-plugin-api.md) created

**Contract Covers**:
- Plugin requirements (factory function, virtual methods)
- Robot base class API (init, run, name, actions, queries)
- Logger API (logging functions)
- Trigonometry utilities (navigation helpers)
- Type definitions and constants
- Build configuration requirements
- Error handling and troubleshooting

### Quick Start Guide

**✅ COMPLETED**: [quickstart.md](quickstart.md) created

**Guide Includes**:
- Prerequisites and setup
- Step-by-step robot creation tutorial
- Build instructions (CMake and Make)
- Common pitfalls and solutions
- Testing procedures
- Debugging tips
- Complete working example

### Agent Context Update

**✅ COMPLETED**: Agent context updated with C++11 and RobotGame framework

**Changes**:
- Added language: C++11
- Added framework: RobotGame, pthread, SDL2
- Added database: N/A (in-memory)

---

## Post-Phase 1 Constitution Check

*GATE: Must pass before proceeding to Phase 2*

**Status**: ✅ PASSED

**Check Results**:
- No constitution defined in project
- No gates to verify
- All Phase 1 deliverables completed successfully
- Design aligns with standalone robot building requirements

---

## Next Steps (Phase 2: Implementation)

**Status**: Ready to proceed with `/speckit.tasks`

**Implementation Plan**:
1. Fix EdgePatrol by implementing `name()` inline in header
2. Verify Runner and Scanner follow correct pattern
3. Update AGENTS.md with any additional guidance
4. Create tests for all three robot plugins
5. Document final implementation
