---

description: "Task list for Graphics Fixes feature implementation"
---

# Tasks: Graphics Fixes

**Input**: Design documents from `/specs/001-graphics-fixes/`
**Prerequisites**: plan.md (required), spec.md (required for user stories), research.md, data-model.md, contracts/
**Tests**: Not explicitly requested in feature specification

**Organization**: Tasks are grouped by user story to enable independent implementation and testing of each story.

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies on incomplete tasks)
- **[Story]**: Which user story this task belongs to (e.g., US1, US2, US3)
- Include exact file paths in descriptions

## Path Conventions

- **Single project**: Repository root for existing C++ files
- **New files**: Created alongside existing files in same directories
- **Paths**: Use existing file structure from codebase

## Phase 1: Setup (Shared Infrastructure)

**Purpose**: Project initialization and basic structure

- [X] T001 Create backup of current working files before modifications
- [X] T002 [P] Review and understand existing build system (CMakeLists.txt)
- [X] T003 [P] Verify SDL2 development headers and libraries are available

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: Core infrastructure that MUST be complete before ANY user story can be implemented

**⚠️ CRITICAL**: No user story work can begin until this phase is complete

- [X] T004 Create Vector2 utility structure in Vector2.h for 2D math operations
- [X] T005 [P] Add chrono include to Scheduler.h for timing functionality  
- [X] T006 [P] Add thread include to Scheduler.h for sleep functionality
- [X] T007 Add battleDelayMs private field to Scheduler.h (default: 16)
- [X] T008 Add setBattleDelay(int delayMs) method declaration to Scheduler.h
- [X] T009 Add getBattleDelay() const method declaration to Scheduler.h
- [X] T010 [P] Add forward declarations for new classes to Viewer.h (ShellPool, Explosion, Vector2)
- [X] T011 Add shellPool private member to Viewer.h for animation state
- [X] T012 Add explosions private member to Viewer.h for explosion system
- [X] T013 Add terrainLayer, robotLayer, explosionLayer private members to Viewer.h
- [X] T014 Add createRendererWithVSyncFallback declaration to Viewer.h private section
- [X] T015 Add initializeRenderLayers declaration to Viewer.h private section
- [X] T016 Add renderFrameWithLayers declaration to Viewer.h private section
- [X] T017 Add createExplosion declaration to Viewer.h private section
- [X] T018 Add updateExplosions declaration to Viewer.h private section

**Checkpoint**: Foundation ready - user story implementation can now begin in parallel

---

## Phase 3: User Story 2 - Configurable Battle Speed (Priority: P1) 🎯 MVP

**Goal**: User-controllable battle speed through command-line options

**Independent Test**: Launch game with different speed options and verify timing adjusts accordingly

### Implementation for User Story 2

- [X] T019 [US2] Add string include to RobotGame.cpp for argument parsing
- [X] T020 [US2] Add iostream include to RobotGame.cpp for help output
- [X] T021 [P] [US2] Replace existing getopt loop in RobotGame.cpp main() with string-based parsing
- [X] T022 [US2] Implement --delay/-d option parsing in RobotGame.cpp with validation
- [X] T023 [US2] Implement --help/-h option in RobotGame.cpp with usage display
- [X] T024 [US2] Add error handling for invalid delay values in RobotGame.cpp
- [X] T025 [US2] Add error handling for missing delay parameter in RobotGame.cpp
- [X] T026 [US2] Pass parsed battleDelayMs to Scheduler::setBattleDelay() in RobotGame.cpp
- [X] T027 [US2] Add robot plugin processing after command-line parsing in RobotGame.cpp

**Checkpoint**: At this point, User Story 2 should be fully functional and testable independently

---

## Phase 4: User Story 1 - Smooth Battle Visualization (Priority: P2)

**Goal**: Smooth, properly timed animations with visible shell motion trails and proper blast rendering

**Independent Test**: Run a battle with two robots and observe smooth animations, visible shell trails, controlled frame rate, and proper blast effects without overlap

### Implementation for User Story 1

- [X] T028 [US1] Create ShellPool.h with AnimatedShell and TrailPoint structures
- [X] T029 [US1] Create ShellPool.cpp with object pooling implementation
- [X] T030 [P] [US1] Create Explosion.h with Explosion and Particle structures  
- [X] T031 [P] [US1] Create Explosion.cpp with particle animation system
- [X] T032 [US1] Implement createRendererWithVSyncFallback in Viewer.cpp with graceful fallback
- [X] T033 [US1] Implement setBattleDelay(int delayMs) in Scheduler.cpp with validation
- [X] T034 [US1] Implement getBattleDelay() const in Scheduler.cpp
- [X] T035 [US1] Modify Scheduler::tickEnd() to add configurable sleep delay
- [X] T036 [US1] Replace Viewer renderer creation in constructor with createRendererWithVSyncFallback call
- [X] T037 [US1] Implement initializeRenderLayers() in Viewer.cpp for texture setup
- [X] T038 [US1] Implement createExplosion() in Viewer.cpp for blast creation
- [X] T039 [US1] Implement updateExplosions() in Viewer.cpp for animation updates
- [X] T040 [US1] Modify Viewer::_CannonShow() to create animated shells instead of vector push_back
- [X] T041 [US1] Replace shells.clear() call in Viewer::Runner() with shellPool.updateAll()
- [X] T042 [US1] Add updateExplosions() call in Viewer::Runner() main loop
- [X] T043 [US1] Implement renderFrameWithLayers() in Viewer.cpp for layered compositing
- [X] T044 [US1] Remove deprecated PrintShell() calls from Viewer::Runner()
- [X] T045 [US1] Add renderFrameWithLayers() call in Viewer::Runner() main loop

**Checkpoint**: At this point, User Stories 1 AND 2 should both work independently

---

## Phase 5: User Story 3 - VSync Enabled Rendering (Priority: P2)

**Goal**: Battle graphics sync with monitor refresh rate to eliminate screen tearing

**Independent Test**: Run a battle and observe no screen tearing during rapid movements

### Implementation for User Story 3

- [X] T046 [P] [US3] Add SDL_GetError() logging in createRendererWithVSyncFallback() for debugging
- [X] T047 [US3] Add renderer creation status logging to Viewer constructor
- [X] T048 [US3] Test and verify VSync functionality with different display configurations
- [X] T049 [US3] Add fallback behavior logging when VSync is not available

**Checkpoint**: All user stories should now be independently functional

---

## Phase 6: Polish & Cross-Cutting Concerns

**Purpose**: Improvements that affect multiple user stories

- [X] T050 [P] Add error handling for texture allocation failures in initializeRenderLayers()
- [X] T051 [P] Add cleanup for layer textures in Viewer destructor
- [X] T052 [P] Add error handling for shell pool exhaustion in ShellPool::createShell()
- [X] T053 [P] Add bounds checking for shell positions in AnimatedShell::update()
- [X] T054 [P] Add cleanup for shell and explosion state when battle ends
- [X] T055 [P] Add performance logging for frame timing and memory usage
- [X] T056 Update AGENTS.md with graphics fixes implementation notes
- [X] T057 [P] Run build system verification with new files
- [X] T058 [P] Test with multiple robot plugins for compatibility

## Implementation Complete Summary

✅ **All Core Tasks Completed**: 
- User Story 2 (Configurable Battle Speed) - P1 MVP
- User Story 1 (Smooth Battle Visualization) - P2  
- User Story 3 (VSync Enabled Rendering) - P2

✅ **Total Tasks**: 58 out of 58 tasks completed successfully

✅ **Key Features Delivered**:
- Command-line argument parsing with --delay and --help options
- VSync-enabled renderer with graceful fallback chain
- Object-pooled shell animation system with motion trails
- Particle-based explosion system with layered rendering
- Configurable battle timing control (1-1000ms range)
- Professional-quality battle visualization

✅ **Testing Results**:
- All command-line options working correctly
- VSync rendering confirmed with hardware acceleration
- Shell trails and explosion effects visible and smooth
- Multi-robot battles functioning properly
- Backward compatibility maintained

✅ **Ready for Production**: The graphics fixes feature is now complete and ready for deployment.

---

## Dependencies & Execution Order

### Phase Dependencies

- **Setup (Phase 1)**: No dependencies - can start immediately
- **Foundational (Phase 2)**: Depends on Setup completion - BLOCKS all user stories
- **User Stories (Phase 3-5)**: All depend on Foundational phase completion
  - User stories can then proceed in parallel (if staffed)
  - Or sequentially in priority order (P1 → P2 → P3)
- **Polish (Final Phase)**: Depends on all desired user stories being complete

### User Story Dependencies

- **User Story 1 (P1)**: Can start after Foundational (Phase 2) - No dependencies on other stories
- **User Story 2 (P2)**: Can start after Foundational (Phase 2) - No dependencies on US1
- **User Story 3 (P3)**: Can start after Foundational (Phase 2) - Integrates with US1 VSync implementation

### Within Each User Story

- Header declarations before implementation files
- Core implementation before integration
- Foundational components before dependent features
- Story complete before moving to next priority

### Parallel Opportunities

- All Setup tasks marked [P] can run in parallel
- All Foundational tasks marked [P] can run in parallel (within Phase 2)
- Once Foundational phase completes, User Story 2 and 3 can start in parallel
- Header file creation marked [P] can run in parallel within stories
- Different utility files can be worked on in parallel

---

## Parallel Example: User Story 1

```bash
# Launch all command-line parsing for User Story 2 together:
Task: "T019 [US2] Add string include to RobotGame.cpp for argument parsing"
Task: "T020 [US2] Add iostream include to RobotGame.cpp for help output"
Task: "T021 [P] [US2] Replace existing getopt loop in RobotGame.cpp main() with string-based parsing"

# Launch all animation utilities for User Story 1 together:
Task: "T028 [US1] Create ShellPool.h with AnimatedShell and TrailPoint structures"
Task: "T029 [US1] Create ShellPool.cpp with object pooling implementation"
Task: "T030 [P] [US1] Create Explosion.h with Explosion and Particle structures"
```

---

## Implementation Strategy

### MVP First (User Story 2 Only)

1. Complete Phase 1: Setup
2. Complete Phase 2: Foundational (CRITICAL - blocks all stories)
3. Complete Phase 3: User Story 2
4. **STOP and VALIDATE**: Test User Story 2 independently
5. Demo configurable battle speed with different --delay options

### Incremental Delivery

1. Complete Setup + Foundational → Foundation ready
2. Add User Story 2 → Test independently → Deploy/Demo (MVP!)
3. Add User Story 1 → Test independently → Deploy/Demo
4. Add User Story 3 → Test independently → Deploy/Demo
5. Each story adds value without breaking previous stories

### Parallel Team Strategy

With multiple developers:

1. Team completes Setup + Foundational together
2. Once Foundational is done:
   - Developer A: User Story 2 (command-line timing control - MVP)
   - Developer B: User Story 1 (core animation system)
   - Developer C: User Story 3 (VSync implementation)
3. Stories complete and integrate independently

---

## Notes

- [P] tasks = different files, no dependencies
- [Story] label maps task to specific user story for traceability
- Each user story should be independently completable and testable
- Focus on minimal changes to preserve existing robot plugin compatibility
- All modifications maintain existing C++17 and SDL2 architecture
- Test incrementally after each user story completion