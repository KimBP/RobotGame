---

description: "Task list for Edge Navigation Robot implementation"
---

# Tasks: Edge Navigation Robot

**Input**: Design documents from `/specs/001-edge-robot/`
**Prerequisites**: plan.md (required), spec.md (required for user stories), research.md, data-model.md, contracts/

**Tests**: Manual testing specified - no automated test tasks included

**Organization**: Tasks are grouped by user story to enable independent implementation and testing of each story.

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: Which user story this task belongs to (e.g., US1, US2, US3)
- Include exact file paths in descriptions

## Path Conventions

- **Robot samples**: `Robots/EdgeRobot/` (following Runner/Scanner pattern)
- **CMake files**: `CMakeLists.txt` in main directory and each robot subdirectory
- **Core game files**: Repository root (unchanged)
- **Documentation**: `specs/001-edge-robot/`

---

## Phase 1: Setup (Shared Infrastructure)

**Purpose**: Project initialization and basic structure

- [X] T001 Create Robots/EdgeRobot directory structure per implementation plan
- [X] T002 [P] Create EdgeRobot.h header file with class declaration
- [X] T003 [P] Create EdgeRobot.cpp implementation file with basic structure

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: Core infrastructure that MUST be complete before ANY user story can be implemented

**⚠️ CRITICAL**: No user story work can begin until this phase is complete

- [X] T004 Implement EdgeRobot constructor and destructor in Robots/EdgeRobot/EdgeRobot.cpp
- [X] T005 [P] Implement required virtual methods (init, run, name) in Robots/EdgeRobot/EdgeRobot.cpp
- [X] T006 [P] Add robot tuning parameters (myTune, myName) in Robots/EdgeRobot/EdgeRobot.cpp
- [X] T007 Implement getRobot factory function for dynamic loading in Robots/EdgeRobot/EdgeRobot.cpp
- [X] T008 Add navigation state enumeration and member variables in Robots/EdgeRobot/EdgeRobot.h
- [X] T009 Add helper method declarations for navigation and combat in Robots/EdgeRobot/EdgeRobot.h

**Checkpoint**: Foundation ready - user story implementation can now begin

---

## Phase 3: User Story 1 - Edge Navigation Robot (Priority: P1) 🎯 MVP

**Goal**: Create a sample robot that demonstrates advanced navigation and combat tactics

**Independent Test**: Deploy robot in arena with obstacles and other robots, observe edge navigation and shooting behavior

### Implementation for User Story 1

- [X] T010 [US1] Implement calculateAngleToCenter helper method in Robots/EdgeRobot/EdgeRobot.cpp
- [X] T011 [US1] Implement detectThreatInCone helper method in Robots/EdgeRobot/EdgeRobot.cpp
- [X] T012 [US1] Implement navigateEdges helper method in Robots/EdgeRobot/EdgeRobot.cpp
- [X] T013 [US1] Implement engageThreat helper method in Robots/EdgeRobot/EdgeRobot.cpp
- [X] T014 [US1] Implement avoidObstacle helper method in Robots/EdgeRobot/EdgeRobot.cpp
- [X] T015 [US1] Implement main run() method with navigation logic and threat detection in Robots/EdgeRobot/EdgeRobot.cpp
- [X] T016 [US1] Add edge distance calculations and boundary checking in Robots/EdgeRobot/EdgeRobot.cpp
- [X] T017 [US1] Implement state transition logic between navigation modes in Robots/EdgeRobot/EdgeRobot.cpp
- [X] T018 [US1] Add center shooting logic when no threats detected in Robots/EdgeRobot/EdgeRobot.cpp
- [X] T019 [US1] Add obstacle avoidance while maintaining edge proximity in Robots/EdgeRobot/EdgeRobot.cpp

**Checkpoint**: At this point, User Story 1 should be fully functional and testable independently

---

## Phase 4: Polish & Cross-Cutting Concerns

**Purpose**: Improvements that affect multiple user stories

- [X] T020 [P] Review and optimize navigation algorithm performance in Robots/EdgeRobot/EdgeRobot.cpp
- [X] T021 [P] Add defensive programming and error handling in Robots/EdgeRobot/EdgeRobot.cpp
- [X] T022 [P] Add inline documentation for key algorithms in Robots/EdgeRobot/EdgeRobot.h
- [X] T023 Update quickstart.md with CMake build integration notes in specs/001-edge-robot/quickstart.md
- [X] T024 Validate robot behavior meets success criteria in specs/001-edge-robot/checklists/requirements.md
- [X] T025 Create CMakeLists.txt for EdgeRobot following Runner/Scanner pattern in Robots/EdgeRobot/CMakeLists.txt
- [X] T026 Add EdgeRobot subdirectory to main CMakeLists.txt in CMakeLists.txt
- [X] T027 Update all documentation to reflect CMake-based build system in plan.md, quickstart.md, AGENTS.md

---

## Phase 5: CMake Build Verification

**Purpose**: Verify CMake build configuration works correctly

- [X] T028 Test CMake build configuration for EdgeRobot

✅ **CMake Build Verification PASSED**
- EdgeRobot compiles successfully as shared library (libedgeRobot.so)
- getRobot() function properly exported for dynamic loading
- Follows established pattern with Runner and Scanner
- All robot libraries build consistently with CMake

**CMake Build Test Commands**:
```bash
# Test clean build
rm -rf build/
cmake -DCMAKE_BUILD_TYPE=Release -B build -S .
cmake --build build

# Verify EdgeRobot library created
ls -la build/Robots/EdgeRobot/libedgeRobot.so

# Verify main executable links properly
ldd build/robot-arena | grep edgeRobot
```

---

## Dependencies & Execution Order

### Phase Dependencies

- **Setup (Phase 1)**: No dependencies - can start immediately
- **Foundational (Phase 2)**: Depends on Setup completion - BLOCKS all user stories
- **User Stories (Phase 3+)**: All depend on Foundational phase completion
  - User stories can then proceed in parallel (if staffed)
  - Or sequentially in priority order (P1 → P2 → P3)
- **Polish (Final Phase)**: Depends on all desired user stories being complete

### User Story Dependencies

- **User Story 1 (P1)**: Can start after Foundational (Phase 2) - No dependencies on other stories

### Within Each User Story

- Helper methods before main run() implementation
- Core implementation before integration
- Story complete before moving to next priority

### Parallel Opportunities

- All Setup tasks marked [P] can run in parallel
- All Foundational tasks marked [P] can run in parallel (within Phase 2)
- Helper method tasks within User Story 1 marked [P] can run in parallel

---

## Parallel Example: User Story 1

```bash
# Launch all helper methods for User Story 1 together:
Task: "Implement calculateAngleToCenter helper method in Robots/EdgeRobot/EdgeRobot.cpp"
Task: "Implement detectThreatInCone helper method in Robots/EdgeRobot/EdgeRobot.cpp"
Task: "Implement navigateEdges helper method in Robots/EdgeRobot/EdgeRobot.cpp"
Task: "Implement engageThreat helper method in Robots/EdgeRobot/EdgeRobot.cpp"
Task: "Implement avoidObstacle helper method in Robots/EdgeRobot/EdgeRobot.cpp"

# Launch optimization and documentation tasks together:
Task: "Review and optimize navigation algorithm performance in Robots/EdgeRobot/EdgeRobot.cpp"
Task: "Add inline documentation for key algorithms in Robots/EdgeRobot/EdgeRobot.h"
Task: "Update quickstart.md with any implementation-specific notes in specs/001-edge-robot/quickstart.md"
```

---

## Implementation Strategy

### MVP First (User Story 1 Only)

1. Complete Phase 1: Setup
2. Complete Phase 2: Foundational (CRITICAL - blocks all stories)
3. Complete Phase 3: User Story 1
4. **STOP and VALIDATE**: Test User Story 1 independently in game arena
5. Demo edge navigation and combat behavior

### Incremental Delivery

1. Complete Setup + Foundational → Foundation ready
2. Add User Story 1 → Test independently → Demo (MVP!)
3. Polish and optimize performance
4. Final validation against success criteria

### Parallel Team Strategy

With multiple developers:

1. Team completes Setup + Foundational together
2. Once Foundational is done:
   - Developer A: Edge navigation algorithms
   - Developer B: Threat detection and combat logic
   - Developer C: State management and helper methods
3. Integration and testing of complete robot behavior

---

## Notes

- [P] tasks = different files/methods, no dependencies
- [Story] label maps task to specific user story for traceability
- Each user story should be independently completable and testable
- Manual testing specified - focus on observable robot behavior in arena
- Stop at any checkpoint to validate story independently
- Avoid: vague tasks, same file conflicts, cross-story dependencies that break independence