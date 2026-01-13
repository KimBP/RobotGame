# Tasks: Fix Undefined Symbols in Robot Shared Libraries

**Input**: Design documents from `/specs/001-fix-undefined-symbols/`
**Prerequisites**: plan.md, spec.md, research.md, data-model.md, contracts/, quickstart.md
**Tests**: Manual testing via game execution (no automated tests requested in spec)
**Organization**: Tasks are grouped by user story to enable independent implementation and testing of each story.

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: Which user story this task belongs to (US1, US2)
- Include exact file paths in descriptions

## Path Conventions

- **Single project**: Repository root with Robots/ subdirectories for plugins
- **Project structure**: Each robot has its own directory in Robots/

---

## Phase 1: Setup (Already Complete)

**Purpose**: Project initialization and branch setup

**Status**: ✅ COMPLETED during `/speckit.plan` command

- [x] T001 Create feature branch 001-fix-undefined-symbols
- [x] T002 Generate research.md with all unknowns resolved
- [x] T003 Create data-model.md with robot plugin entities
- [x] T004 Create contracts/robot-plugin-api.md with complete API documentation
- [x] T005 Create quickstart.md with standalone building guide
- [x] T006 Update AGENTS.md with C++11 and RobotGame framework

**Checkpoint**: Setup phase complete - ready for implementation

---

## Phase 2: Foundational (No Blocking Tasks)

**Purpose**: Core infrastructure prerequisites

**Analysis**: This feature fixes existing code rather than creating new infrastructure. No blocking foundational tasks are needed. The robot plugin system, build configuration, and API headers already exist. Implementation can proceed directly to User Story 1.

**Checkpoint**: No foundational blockers - ready for User Story 1

---

## Phase 3: User Story 1 - Robot Plugin Loads Successfully (Priority: P1) 🎯 MVP

**Goal**: Fix EdgePatrol robot plugin undefined symbol error so it loads successfully without errors into RobotGame.

**Independent Test**: Execute game with EdgePatrol plugin and verify it loads without undefined symbol errors and executes in game simulation.

### Implementation for User Story 1

- [x] T007 [US1] Implement name() method inline in Robots/EdgePatrol/EdgePatrol.h at line 23
- [x] T008 [US1] Rebuild EdgePatrol robot plugin using cmake --build build
- [x] T009 [US1] Verify EdgePatrol library has no undefined symbols using nm -D build/Robots/EdgePatrol/libedgepatrol.so | grep " U "
- [x] T010 [US1] Load EdgePatrol plugin by running ./robot-arena build/Robots/EdgePatrol/libedgepatrol.so
- [x] T011 [US1] Verify EdgePatrol initializes successfully and executes in game simulation

**Checkpoint**: EdgePatrol loads without undefined symbol errors and participates in game

---

## Phase 4: User Story 2 - Robot Plugins Follow Consistent Pattern (Priority: P2)

**Goal**: Verify Runner and Scanner robots follow correct pattern, and document implementation approach for future robots.

**Independent Test**: Review code structure of Runner and Scanner to confirm they implement all virtual methods correctly and follow the inline header pattern.

### Implementation for User Story 2

- [x] T012 [P] [US2] Verify Runner implements name() inline in Robots/Runner/Runner.h at line 23
- [x] T013 [P] [US2] Verify Scanner implements name() inline in Robots/Scanner/Scanner.h at line 23
- [x] T014 [P] [US2] Rebuild Runner robot plugin using cmake --build build
- [x] T015 [P] [US2] Rebuild Scanner robot plugin using cmake --build build
- [x] T016 [P] [US2] Verify Runner library has no undefined symbols using nm -D build/Robots/Runner/librunner.so | grep " U "
- [x] T017 [P] [US2] Verify Scanner library has no undefined symbols using nm -D build/Robots/Scanner/libscanner.so | grep " U "
- [x] T018 [US2] Test all three robot plugins load successfully by running ./robot-arena build/Robots/EdgePatrol/libedgepatrol.so build/Robots/Runner/librunner.so build/Robots/Scanner/libscanner.so

**Checkpoint**: All robot plugins load without undefined symbol errors and follow consistent pattern

---

- [x] T018 [US2] Test all three robot plugins load successfully by running ./robot-arena build/Robots/EdgePatrol/libedgepatrol.so build/Robots/Runner/librunner.so build/Robots/Scanner/libscanner.so

---

## Phase 5: Polish & Documentation

**Purpose**: Final verification and documentation updates

- [x] T019 [P] Confirm all three robot plugins load and run correctly in game simulation
- [x] T020 [P] Update AGENTS.md with guidance on inline virtual method implementation pattern
- [x] T021 [P] Update quickstart.md if any additional findings during implementation
- [x] T022 [P] Create summary documentation in specs/001-fix-undefined-symbols/IMPLEMENTATION_NOTES.md

**Checkpoint**: Feature complete with all robots loading successfully

---

## Dependencies & Execution Order

### Phase Dependencies

- **Setup (Phase 1)**: ✅ COMPLETE - no dependencies, started immediately
- **Foundational (Phase 2)**: No blocking tasks - implementation can proceed directly
- **User Story 1 (Phase 3)**: Can start immediately - no dependencies on other stories
- **User Story 2 (Phase 4)**: Can start after User Story 1 complete - needs EdgePatrol fixed first
- **Polish (Phase 5)**: Depends on User Stories 1 AND 2 being complete

### User Story Dependencies

- **User Story 1 (P1)**: Can start immediately - no dependencies, fixes EdgePatrol specifically
- **User Story 2 (P2)**: Depends on User Story 1 - needs to verify all three robots work after EdgePatrol is fixed
