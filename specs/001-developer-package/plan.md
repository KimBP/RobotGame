# Implementation Plan: Developer Package Creator

**Branch**: `001-developer-package` | **Date**: 2025-01-12 | **Spec**: [spec.md](spec.md)
**Input**: Feature specification from `/specs/001-developer-package/spec.md`

**Note**: This template is filled in by the `/speckit.plan` command. See `.specify/templates/commands/plan.md` for the execution workflow.

## Summary

Create a shell script `scripts/create-dev-kit.sh` that generates a self-contained `tar.gz` development package. The script must include robot-arena binary, shared libraries (librunner.so, libscanner.so), sample robot source code (Runner, Scanner), API headers (Robot.h), documentation (README with build instructions and robot development rules), and a template robot file (MyRobot.cpp). The script must build missing binaries and use only relative paths with repository files (no heredocs).

## Technical Context

**Language/Version**: Shell Scripting (Bash), C++17 (for robots)  
**Primary Dependencies**: CMake build system, g++ compiler, tar utility  
**Storage**: File-based package distribution (tar.gz archive)  
**Testing**: Package extraction and build validation  
**Target Platform**: Linux development environments  
**Project Type**: Build automation script with plugin package generation  
**Performance Goals**: Package creation <5 minutes, package size <50MB  
**Constraints**: No heredocs in scripts, only repository files, all paths relative  
**Scale/Scope**: Single shell script, 3 binary files, 2 sample robots, 1 template

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

- ✅ Plugin Architecture: Package includes robot plugins with shared library interface and getRobot() factory
- ✅ Real-time Events: Package preserves Scheduler-based robot event system via robot-arena binary
- ✅ Hardware Abstraction: Package includes RobCtrl interface through Robot.h header and existing robot implementations
- ✅ Simulation Integrity: Package maintains deterministic physics and behaviors through included robot-arena
- ✅ Visual Observability: Package provides complete visualization capability via robot-arena binary
- ✅ Scripts must not use 'heredocs': create-dev-kit.sh copies repository files only, no inline generation
- ✅ Files must not be duplicated: Package copies existing files for distribution, no source duplication

**Post-Design Validation**: All constitution principles are upheld. The package creation script maintains the core architecture while enabling external development without repository access.

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
scripts/
└── create-dev-kit.sh           # Main package creation script

docs/                           # New directory for documentation
└── dev-kit/
    ├── README.md              # Developer kit instructions
    ├── MyRobot.cpp            # Robot template
    └── ROBOT_RULES.md         # Robot development rules

build/                          # Existing build directory
├── robot-arena                # Main executable
├── Robots/
│   ├── Runner/librunner.so   # Runner robot library  
│   └── Scanner/libscanner.so # Scanner robot library

Robots/                         # Existing robot source code
├── Runner/
│   ├── Runner.cpp
│   ├── Runner.h
│   └── CMakeLists.txt
└── Scanner/
    ├── Scanner.cpp
    ├── Scanner.h
    └── CMakeLists.txt

Robot.h                         # Main API header (root level)
CMakeLists.txt                  # Main build configuration
```

**Structure Decision**: Adding new `scripts/` directory for automation script and `docs/dev-kit/` for documentation templates. Package will use existing build artifacts and robot source code structure.

## Complexity Tracking

> **Fill ONLY if Constitution Check has violations that must be justified**

| Violation | Why Needed | Simpler Alternative Rejected Because |
|-----------|------------|-------------------------------------|
| [e.g., 4th project] | [current need] | [why 3 projects insufficient] |
| [e.g., Repository pattern] | [specific problem] | [why direct DB access insufficient] |
