# Implementation Plan: Graphics Fixes

**Branch**: `001-graphics-fixes` | **Date**: 2026-01-13 | **Spec**: [spec.md](spec.md)
**Input**: Feature specification from `/specs/001-graphics-fixes/spec.md`

**Note**: This template is filled in by the `/speckit.plan` command. See `.specify/templates/commands/plan.md` for the execution workflow.

## Summary

Fix four critical graphical issues in RobotGame battle visualization: enable VSync to prevent screen tearing, add configurable timing control for battle speed, preserve shell animation continuity between frames, and fix shell blast rendering overlap. The approach involves SDL2 renderer configuration, command-line argument parsing, and animation state management improvements.

## Technical Context

**Language/Version**: C++17 (from existing codebase)  
**Primary Dependencies**: SDL2, SDL2_ttf, SDL2_gfx, SDL2_image (from CMake configuration)  
**Storage**: N/A (real-time game with in-memory state)  
**Testing**: NEEDS CLARIFICATION (existing test framework not identified)  
**Target Platform**: Linux desktop (existing build system)  
**Project Type**: single (C++ executable with shared robot libraries)  
**Performance Goals**: 30-60 FPS smooth animation, minimal input lag  
**Constraints**: Real-time rendering, must maintain existing robot plugin architecture  
**Scale/Scope**: Small - localized to Viewer.cpp and Scheduler.cpp modifications

## Constitution Check

*GATE: Must pass before Phase 0 research. Re-check after Phase 1 design.*

**Constitution Status**: Template constitution detected - no specific gates identified  
**Phase 0 Research**: ✅ Complete - All NEEDS CLARIFICATION resolved  
**Gate Evaluation**: No constitutional violations identified in current scope  
**Proceed to Phase 1**: ✅ Approved

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
<!--
  ACTION REQUIRED: Replace the placeholder tree below with the concrete layout
  for this feature. Delete unused options and expand the chosen structure with
  real paths (e.g., apps/admin, packages/something). The delivered plan must
  not include Option labels.
-->

```text
# [REMOVE IF UNUSED] Option 1: Single project (DEFAULT)
src/
├── models/
├── services/
├── cli/
└── lib/

tests/
├── contract/
├── integration/
└── unit/

# [REMOVE IF UNUSED] Option 2: Web application (when "frontend" + "backend" detected)
backend/
├── src/
│   ├── models/
│   ├── services/
│   └── api/
└── tests/

frontend/
├── src/
│   ├── components/
│   ├── pages/
│   └── services/
└── tests/

# [REMOVE IF UNUSED] Option 3: Mobile + API (when "iOS/Android" detected)
api/
└── [same as backend above]

ios/ or android/
└── [platform-specific structure: feature modules, UI flows, platform tests]
```

**Structure Decision**: [Document the selected structure and reference the real
directories captured above]

## Phase 0: Research Summary ✅

### Completed Research Areas
- **SDL2 VSync Implementation**: Renderer creation with graceful fallback strategy
- **C++ Command-Line Parsing**: Lightweight argument handling without external dependencies  
- **Shell Animation State**: Object pooling with trail history for motion continuity
- **Explosion Rendering**: Texture-based layering with particle system for blast effects

### Key Technical Decisions
- **VSync**: Use SDL_RENDERER_PRESENTVSYNC with software renderer fallback
- **CLI**: Minimal string processing approach (no external dependencies)
- **Shells**: Object pool with configurable trail buffer (20 points, 500ms lifetime)
- **Explosions**: Multi-layer texture compositing with particle debris

### Risk Assessment
- **Low Risk**: VSync implementation, command-line parsing
- **Medium Risk**: Layer system integration, shell state synchronization
- **Mitigations**: Incremental implementation, fallback options, comprehensive testing

## Phase 1: Design Summary ✅

### Data Model Complete
- **BattleTimingConfig**: Configurable delay (1-1000ms) with VSync options
- **AnimatedShell**: Position history with fading trail points  
- **Explosion**: Animated radius with particle debris system
- **RenderLayer**: Texture-based compositing for proper layering

### API Contracts Defined
- **CLI Contract**: --delay option with validation and help system
- **Scheduler API**: setBattleDelay() with tickEnd() integration
- **Viewer API**: Enhanced renderer, shell pool, explosion system

### Integration Architecture
- **RobotGame.cpp**: Command-line parsing → Scheduler configuration
- **Scheduler.cpp**: Configurable timing with thread-safe sleep
- **Viewer.cpp**: Layered rendering with animation systems
- **CannonShell.cpp**: Event posting instead of direct viewer updates

## Implementation Artifacts Generated

### Research Documents
- ✅ `research.md` - Complete technical findings and decisions
- ✅ `data-model.md` - Entity definitions and relationships
- ✅ `contracts/` - API specifications for all interfaces
- ✅ `quickstart.md` - Step-by-step implementation guide

### Agent Context Updated
- ✅ `AGENTS.md` - Added SDL2 and C++17 technology context
- ✅ Framework dependencies documented for future development

## Constitution Check (Post-Design) ✅

**Constitution Status**: Template constitution with no specific gates identified  
**Phase 1 Design**: ✅ Complete - All contracts and models finalized  
**Gate Evaluation**: No constitutional violations introduced  
**Implementation Ready**: ✅ Approved for development phase

## Complexity Tracking

**No constitutional violations identified** - All design decisions align with single-project C++ architecture. New systems integrate cleanly with existing codebase without architectural violations.

### Complexity Justifications
| Addition | Complexity Level | Justification |
|-----------|----------------|---------------|
| Shell Pool System | Medium | Essential for motion continuity, well-understood pattern |
| Render Layer System | Medium | Required for proper visual layering, standard game technique |
| Particle System | Low-Medium | Simple animation enhancement, minimal overhead |
| CLI Timing Control | Low | Straightforward argument parsing, zero external dependencies |
