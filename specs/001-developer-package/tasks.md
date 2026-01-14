---

description: "Task list for Developer Package Creator feature implementation"
---

# Tasks: Developer Package Creator

**Input**: Design documents from `/specs/001-developer-package/`
**Prerequisites**: plan.md (required), spec.md (required for user stories), research.md, data-model.md, contracts/

**Tests**: Manual package extraction and build validation (per specification requirements)

**Organization**: Tasks are grouped by user story to enable independent implementation and testing of each story.

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: Which user story this task belongs to (e.g., US1, US2, US3)
- Include exact file paths in descriptions

## Path Conventions

- **Main Script**: `scripts/create-dev-kit.sh` (primary deliverable)
- **Documentation**: `docs/dev-kit/` for package templates
- **Binaries**: `build/` for robot-arena and .so files
- **Robot Sources**: `Robots/Runner/` and `Robots/Scanner/` for examples
- **Package Output**: Root directory (tar.gz file)

## Phase 1: Setup (Shared Infrastructure)

**Purpose**: Project initialization and basic structure

- [x] T001 Create scripts directory for package creation automation
- [x] T002 Create docs/dev-kit directory for package documentation templates
- [x] T003 [P] Set up script execution permissions and basic error handling

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: Core infrastructure that MUST be complete before ANY user story can be implemented

**⚠️ CRITICAL**: No user story work can begin until this phase is complete

- [x] T004 Create main script skeleton in scripts/create-dev-kit.sh with command line argument parsing
- [x] T005 Implement build status detection function for checking binary artifacts in scripts/create-dev-kit.sh
- [x] T006 Implement CMake build execution function in scripts/create-dev-kit.sh
- [x] T007 [P] Implement file collection function for gathering package contents in scripts/create-dev-kit.sh
- [x] T008 Implement tar.gz archive creation function in scripts/create-dev-kit.sh
- [x] T009 Implement package validation function in scripts/create-dev-kit.sh
- [x] T010 [P] Create error handling and logging utilities in scripts/create-dev-kit.sh
- [x] T011 Add script version information and help text in scripts/create-dev-kit.sh

**Checkpoint**: Foundation ready - user story implementation can now begin in parallel

---

## Phase 3: User Story 1 - Complete Developer Package Creation (Priority: P1) 🎯 MVP

**Goal**: Create a distributable package containing all required headers, libraries, documentation, and example code for immediate robot development

**Independent Test**: Execute script and verify generated package contains all components needed for a new developer to compile and run a simple robot example

### Testing for User Story 1 (Manual Validation)

- [x] T012 [P] [US1] Manual package extraction test to verify all required files are present
- [x] T013 [P] [US1] Manual build test of Runner robot from extracted package sources
- [x] T014 [P] [US1] Manual execution test of robot-arena binary from extracted package

### Implementation for User Story 1

- [x] T015 [US1] Implement binary artifact collection (robot-arena, librunner.so, libscanner.so) in scripts/create-dev-kit.sh
- [x] T016 [US1] Implement source code collection (Runner, Scanner robots) in scripts/create-dev-kit.sh
- [x] T017 [US1] Implement header file collection (Robot.h) in scripts/create-dev-kit.sh
- [x] T018 [US1] Implement basic documentation template collection in scripts/create-dev-kit.sh
- [x] T019 [US1] Add package size validation (<50MB) in scripts/create-dev-kit.sh
- [x] T020 [US1] Add build time tracking (<5 minutes) in scripts/create-dev-kit.sh
- [x] T021 [US1] Configure relative path handling for portability in scripts/create-dev-kit.sh
- [x] T022 [US1] Integrate all functions into main script workflow in scripts/create-dev-kit.sh

**Checkpoint**: At this point, User Story 1 should be fully functional and testable independently

---

## Phase 4: User Story 2 - Robot Testing Environment Setup (Priority: P2)

**Goal**: Provide a simplified test harness that validates robot behavior and basic functionality in isolation

**Independent Test**: Build package, create a test robot, and execute test environment to verify robot can run and produce expected outputs

### Testing for User Story 2 (Manual Validation)

- [x] T023 [P] [US2] Manual test of isolated robot execution using robot-arena with single robot
- [x] T024 [P] [US2] Manual validation of robot event logging and output capture

### Implementation for User Story 2

- [x] T025 [US2] Create MyRobot.cpp template with basic robot structure in docs/dev-kit/MyRobot.cpp
- [x] T026 [US2] Create robot build instructions documentation in docs/dev-kit/ROBOT_RULES.md
- [x] T027 [US2] Enhance package collection to include robot template in scripts/create-dev-kit.sh
- [x] T028 [US2] Add test environment documentation to package in scripts/create-dev-kit.sh
- [x] T029 [US2] Integrate template and rules collection into main workflow in scripts/create-dev-kit.sh

**Checkpoint**: At this point, User Stories 1 AND 2 should both work independently

---

## Phase 5: User Story 3 - Documentation and Examples (Priority: P3)

**Goal**: Provide comprehensive documentation and working examples for new developers to understand robot system and follow best practices

**Independent Test**: Review generated documentation for completeness and verify all example robots compile and run as described

### Testing for User Story 3 (Manual Validation)

- [x] T030 [P] [US3] Manual review of README.md for completeness and clarity
- [x] T031 [P] [US3] Manual verification of all example robot builds from documentation instructions
- [x] T032 [P] [US3] Manual test of 30-minute onboarding claim using package contents

### Implementation for User Story 3

- [x] T033 [US3] Create comprehensive README.md with quick start guide in docs/dev-kit/README.md
- [x] T034 [US3] Create CMakeLists.txt templates for robot examples in docs/dev-kit/
- [x] T035 [US3] Enhance MyRobot.cpp template with detailed comments and examples in docs/dev-kit/MyRobot.cpp
- [x] T036 [US3] Create detailed robot development rules in docs/dev-kit/ROBOT_RULES.md
- [x] T037 [US3] Add comprehensive documentation collection to package script in scripts/create-dev-kit.sh
- [x] T038 [US3] Add documentation validation checks in scripts/create-dev-kit.sh

**Checkpoint**: All user stories should now be independently functional

---

## Phase 6: Polish & Cross-Cutting Concerns

**Purpose**: Improvements that affect multiple user stories

- [x] T039 [P] Add comprehensive error messages with resolution suggestions to scripts/create-dev-kit.sh
- [x] T040 [P] Add script exit codes and status reporting in scripts/create-dev-kit.sh
- [x] T041 [P] Optimize package creation performance and disk usage in scripts/create-dev-kit.sh
- [x] T042 Add script option parsing (--force, --output, --verbose, --help) to scripts/create-dev-kit.sh
- [x] T043 Create package validation test script in scripts/test-dev-kit.sh
- [x] T044 Add script version management and compatibility checking in scripts/create-dev-kit.sh

- [x] T039 [P] Add comprehensive error messages with resolution suggestions to scripts/create-dev-kit.sh
- [x] T040 Add script exit codes and status reporting in scripts/create-dev-kit.sh
- [x] T041 [P] Optimize package creation performance and disk usage in scripts/create-dev-kit.sh
- [x] T042 Add script option parsing (--force, --output, --verbose, --help) in scripts/create-dev-kit.sh
- [x] T043 Create package validation test script in scripts/test-dev-kit.sh
- [x] T044 Add script version management and compatibility checking in scripts/create-dev-kit.sh

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
- **User Story 2 (P2)**: Can start after Foundational (Phase 2) - May integrate with US1 but should be independently testable
- **User Story 3 (P3)**: Can start after Foundational (Phase 2) - May integrate with US1/US2 but should be independently testable

### Within Each User Story

- Manual tests MUST be created and PASS before implementation (validation approach)
- Core functionality before documentation enhancements
- Script functions before main workflow integration
- Story complete before moving to next priority

### Parallel Opportunities

- All Setup tasks marked [P] can run in parallel
- All Foundational tasks marked [P] can run in parallel (within Phase 2)
- Once Foundational phase completes, all user stories can start in parallel (if team capacity allows)
- All manual tests for a user story marked [P] can run in parallel
- Documentation tasks marked [P] can run in parallel

---

## Parallel Example: User Story 1

```bash
# Launch all manual tests for User Story 1 together:
Task: "Manual package extraction test to verify all required files are present"
Task: "Manual build test of Runner robot from extracted package sources"
Task: "Manual execution test of robot-arena binary from extracted package"

# Launch all core implementation tasks together:
Task: "Implement binary artifact collection (robot-arena, librunner.so, libscanner.so) in scripts/create-dev-kit.sh"
Task: "Implement source code collection (Runner, Scanner robots) in scripts/create-dev-kit.sh"
Task: "Implement header file collection (Robot.h) in scripts/create-dev-kit.sh"
Task: "Implement basic documentation template collection in scripts/create-dev-kit.sh"
Task: "Add package size validation (<50MB) in scripts/create-dev-kit.sh"
Task: "Add build time tracking (<5 minutes) in scripts/create-dev-kit.sh"
Task: "Configure relative path handling for portability in scripts/create-dev-kit.sh"
Task: "Integrate all functions into main script workflow in scripts/create-dev-kit.sh"
```

---

## Implementation Strategy

### MVP First (User Story 1 Only)

1. Complete Phase 1: Setup
2. Complete Phase 2: Foundational (CRITICAL - blocks all stories)
3. Complete Phase 3: User Story 1
4. **STOP and VALIDATE**: Test User Story 1 independently
5. Demo package creation and extraction

### Incremental Delivery

1. Complete Setup + Foundational → Foundation ready
2. Add User Story 1 → Test independently → Deploy/Demo (MVP!)
3. Add User Story 2 → Test independently → Deploy/Demo
4. Add User Story 3 → Test independently → Deploy/Demo
5. Each story adds value without breaking previous stories

### Parallel Team Strategy

With multiple developers:

1. Team completes Setup + Foundational together
2. Once Foundational is done:
   - Developer A: User Story 1
   - Developer B: User Story 2
   - Developer C: User Story 3
3. Stories complete and integrate independently

---

## Notes

- [P] tasks = different files, no dependencies
- [Story] label maps task to specific user story for traceability
- Each user story should be independently completable and testable
- Manual validation approach (no automated test framework)
- Constitution compliance: no heredocs, only repository files, relative paths
- Success criteria: <5min build, <50MB package, 30min onboarding
- Verify package extraction and build from any location