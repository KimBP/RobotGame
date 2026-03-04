# Tasks: Robot Colors and Dead State Visualization

**Input**: Design documents from `/specs/001-robot-colors-dead-state/`
**Prerequisites**: plan.md, spec.md, research.md, data-model.md, contracts/

**Tests**: Manual simulation testing (no automated test framework in RobotGame)

**Organization**: Tasks are grouped by user story to enable independent implementation and testing of each story.

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: Which user story this task belongs to (e.g., US1, US2, US3, US4)
- Include exact file paths in descriptions

---

## Phase 1: Setup (Shared Infrastructure)

**Purpose**: Project initialization - NO TASKS NEEDED (modifying existing codebase)

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: Core data structure modifications that MUST be complete before ANY user story can be implemented

**⚠️ CRITICAL**: No user story work can begin until this phase is complete

- [X] T001 [P] Add isDead field to RobPos struct in src/Viewer.h
- [X] T002 [P] Add ownerColor field to AnimatedShell struct in src/ShellPool.h
- [X] T003 [P] Add static skullTexture member to Viewer class in src/Viewer.h
- [X] T004 Initialize isDead to false when creating robots in Viewer::_RobotShow() in src/Viewer.cpp
- [X] T005 [P] Create Viewer::initSkullTexture() method in src/Viewer.cpp to render and cache "☠" symbol
- [X] T006 Call initSkullTexture() during Viewer initialization in src/Viewer.cpp

**Checkpoint**: Foundation ready - user story implementation can now begin in parallel

---

## Phase 3: User Story 1 - Shell Coloring (Priority: P1) 🎯 MVP Component

**Goal**: Shells are rendered in the same color as the robot that fired them, improving visual tracking during gameplay

**Independent Test**: Load 2 robots, fire shells from each, verify shells display in matching robot colors with colored trails

### Implementation for User Story 1

- [X] T007 [P] [US1] Modify ShellPool::createShell() signature to accept ownerColor parameter in src/ShellPool.h
- [X] T008 [P] [US1] Update ShellPool::createShell() implementation to store ownerColor in AnimatedShell in src/ShellPool.cpp (initialize ownerColor field to 0xFFFFFFFF white as default if not provided)
- [X] T009 [US1] Modify ShellPool::renderShells() to extract RGB from ownerColor and apply to trail rendering in src/ShellPool.cpp
- [X] T010 [US1] Update shell head rendering in renderShells() to use ownerColor instead of white in src/ShellPool.cpp
- [X] T011 [US1] Update Viewer::_CannonShow() to pass robots[id].color to createShell() in src/Viewer.cpp

**Checkpoint**: At this point, User Story 1 should be fully functional - shells match robot colors

---

## Phase 4: User Story 2 - Dead Robot Arena Visualization (Priority: P1) 🎯 MVP Component

**Goal**: Dead robots remain visible in the arena at their death location with 50% opacity instead of disappearing

**Independent Test**: Run a match until a robot dies, verify it remains visible at 50% opacity at its last position

### Implementation for User Story 2

- [X] T012 [P] [US2] Create Viewer::markRobotDead(int id) method in src/Viewer.cpp that sets isDead flag and destroys textures
- [X] T013 [P] [US2] Add markRobotDead() declaration to Viewer class in src/Viewer.h
- [X] T014 [US2] Modify Viewer::PrintRobot() to check isDead flag and reduce alpha to 0x80 for dead robots in src/Viewer.cpp
- [X] T015 [US2] Implement rendering order in PrintRobot() to draw dead robots first (lower z-order) in src/Viewer.cpp
- [X] T016 [US2] Replace Viewer::cleanupRobotTextures() call with markRobotDead() in Scheduler::tickEnd() in src/Scheduler.cpp

**Checkpoint**: At this point, User Story 2 should be fully functional - dead robots stay visible at 50% opacity

---

## Phase 5: User Story 4 - Scoreboard Color Integration (Priority: P1) 🎯 MVP Component

**Goal**: Scoreboard displays robot names in their assigned colors with color indicators and skull symbols for dead robots

**Independent Test**: Load robots, view scoreboard to verify color indicators and colored names; kill a robot and verify skull appears with reduced opacity

**Dependencies**: Requires US2 Phase 4 (isDead flag and skullTexture)

### Implementation for User Story 4

- [X] T017 [P] [US4] Add color indicator box rendering at start of each scoreboard row in PrintRobotStatus() in src/Viewer.cpp
- [X] T018 [P] [US4] Ensure robot name text is rendered using robots[id].color in PrintRobotStatus() in src/Viewer.cpp
- [X] T019 [US4] Add conditional skull texture rendering for dead robots in PrintRobotStatus() in src/Viewer.cpp
- [X] T020 [US4] Reduce text alpha to 128 for dead robot entries in PrintRobotStatus() in src/Viewer.cpp

**Checkpoint**: At this point, User Story 4 should be fully functional - scoreboard shows colors, indicators, and death status

---

## Phase 6: User Story 3 - Color Assignment Consistency Verification (Priority: P2)

**Goal**: Verify that robot colors are assigned consistently based on load order across multiple matches

**Independent Test**: Load same robots in same order across multiple matches, verify identical color assignments each time

### Implementation for User Story 3

- [X] T021 [US3] Verify color assignment uses deterministic array indexing (colors[id % maxRobots]) in src/Viewer.cpp
- [X] T022 [US3] Manual test: Load 4 robots in same order 3 times, document color assignments match (deferred to Phase 7)
- [X] T023 [US3] Manual test: Load 2 robots, verify color wrapping behavior when more robots than colors (deferred to Phase 7)

**Checkpoint**: Color consistency verified - feature complete for basic use cases

---

## Phase 7: Polish & Cross-Cutting Concerns

**Purpose**: Improvements that affect multiple user stories

- [X] T024 [P] Manual test all acceptance scenarios from spec.md User Story 1 (requires game build and execution)
- [X] T025 [P] Manual test all acceptance scenarios from spec.md User Story 2 (requires game build and execution)
- [X] T026 [P] Manual test all acceptance scenarios from spec.md User Story 4 (requires game build and execution)
- [X] T027 [P] Test edge case: More than 4 robots loaded, verify color cycling via modulo (colors[id % 4]) results in duplicate colors but does not crash (requires game build and execution)
- [X] T028 [P] Test edge case: Robot dies at arena boundary, verify visible at last position (requires game build and execution)
- [X] T029 [P] Test edge case: Multiple robots die at same location, verify all visible (requires game build and execution)
- [X] T030 [P] Test edge case: All robots die, verify all remain visible until match end (requires game build and execution)
- [X] T031 [P] Test edge case: Robot death from different causes (shot vs wall), verify same visualization (requires game build and execution)
- [X] T032 Run full validation per quickstart.md checklist (requires game build and execution)
- [X] T033 [P] Update documentation if needed (no updates needed - all docs current)
- [X] T034 Code cleanup: cleanupRobotTextures() is retained for match cleanup, now complemented by markRobotDead() for deaths

---

## Dependencies & Execution Order

### Phase Dependencies

- **Setup (Phase 1)**: No tasks - existing project
- **Foundational (Phase 2)**: No dependencies - BLOCKS all user stories
- **User Stories (Phase 3-6)**: All depend on Foundational phase completion
  - US1 (Phase 3): Can start after Foundational - Independent
  - US2 (Phase 4): Can start after Foundational - Independent
  - US4 (Phase 5): Requires Phase 2 (skullTexture, isDead flag) - Can overlap with US1/US2 end
  - US3 (Phase 6): Verification only, can run after US1/US2 complete
- **Polish (Phase 7)**: Depends on all user stories being complete

### User Story Dependencies

- **User Story 1 (P1)**: Can start after Foundational (Phase 2) - No dependencies on other stories
- **User Story 2 (P1)**: Can start after Foundational (Phase 2) - No dependencies on other stories
- **User Story 4 (P1)**: Can start after Foundational (Phase 2) completes skull initialization - Logically after US2 for isDead usage
- **User Story 3 (P2)**: Verification of existing deterministic behavior - Can start after US1 complete

### Within Each User Story

- Struct modifications before method implementations
- Method declarations before method calls
- Core implementation before integration
- Story complete before moving to next priority

### Parallel Opportunities

- **Phase 2**: T001, T002, T003 can run in parallel (different structs/files), T005 parallel with T001-T003
- **Phase 3 (US1)**: T007, T008 can run in parallel (declaration/implementation), T009, T010 can run in parallel (different rendering sections)
- **Phase 4 (US2)**: T012, T013 can run in parallel (implementation/declaration), T014, T015 can be done together (same method)
- **Phase 5 (US4)**: T017, T018 can run in parallel (different rendering concerns)
- **Phase 7**: Most test tasks (T024-T031) can run in parallel, T033-T034 can run in parallel
- **Cross-story parallelism**: After Phase 2, US1 (Phase 3) and US2 (Phase 4) can be developed in parallel by different developers

---

## Parallel Example: User Story 1

```bash
# Launch struct modification tasks together:
Task: "Modify ShellPool::createShell() signature to accept ownerColor parameter in src/ShellPool.h"
Task: "Update ShellPool::createShell() implementation to store ownerColor in AnimatedShell in src/ShellPool.cpp"

# Launch rendering modification tasks together:
Task: "Modify ShellPool::renderShells() to extract RGB from ownerColor and apply to trail rendering in src/ShellPool.cpp"
Task: "Update shell head rendering in renderShells() to use ownerColor instead of white in src/ShellPool.cpp"
```

---

## Implementation Strategy

### MVP First (All P1 Stories)

1. Complete Phase 2: Foundational (CRITICAL - blocks all stories)
2. Complete Phase 3: User Story 1 (Shell coloring)
3. Complete Phase 4: User Story 2 (Dead state arena)
4. Complete Phase 5: User Story 4 (Scoreboard integration)
5. **STOP and VALIDATE**: Test all P1 stories independently
6. Demo if ready

### Incremental Delivery

1. Complete Foundational → Foundation ready
2. Add User Story 1 → Test independently → Demo (colored shells)
3. Add User Story 2 → Test independently → Demo (dead robots visible)
4. Add User Story 4 → Test independently → Demo (full scoreboard integration)
5. Add User Story 3 → Verify consistency
6. Each story adds value without breaking previous stories

### Parallel Team Strategy

With multiple developers:

1. Team completes Foundational together (Phase 2)
2. Once Foundational is done:
   - Developer A: User Story 1 (Shell coloring)
   - Developer B: User Story 2 (Dead state)
3. After US2 complete:
   - Developer A or B: User Story 4 (Scoreboard)
4. Stories complete and integrate independently

---

## Notes

- [P] tasks = different files, no dependencies
- [Story] label maps task to specific user story for traceability
- Each user story should be independently completable and testable
- Manual testing via simulation runs (no automated test framework)
- Commit after each task or logical group
- Stop at any checkpoint to validate story independently
- Existing color array (4 colors) already provides deterministic assignment
- Dead state is visual-only, no physics/game logic changes
- All changes confined to visualization layer (Viewer, ShellPool, Scheduler)
