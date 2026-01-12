# Tasks: Edge Patrol Robot

**Input**: Design documents from `/specs/001-edge-patrol-robot/`
**Prerequisites**: plan.md (required), spec.md (required for user stories), data-model.md, contracts/, research.md

**Tests**: Manual testing via game execution as specified in feature specification. No automated test tasks included.

**Organization**: Tasks are grouped by user story to enable independent implementation and testing of each story.

## Format: `[ID] [P?] [Story] Description`

- **[P]**: Can run in parallel (different files, no dependencies)
- **[Story]**: Which user story this task belongs to (e.g., US1, US2, US3, US4)
- Include exact file paths in descriptions

## Path Conventions

- **Robot plugins**: `Robots/[RobotName]/` directory with header and implementation files
- **Documentation**: `specs/001-edge-patrol-robot/`
- **Build artifacts**: Shared libraries (.so files) in robot directories

Paths shown below follow the RobotGame plugin structure defined in plan.md.

---

## Phase 1: Setup (Shared Infrastructure)

**Purpose**: Project initialization and basic structure for Edge Patrol robot plugin

- [X] T001 Create directory structure for EdgePatrol robot in Robots/EdgePatrol/
- [X] T002 Create Makefile for building EdgePatrol.so with C++11 flags

---

## Phase 2: Foundational (Blocking Prerequisites)

**Purpose**: Understanding RobotGame framework and API before implementing robot behaviors

**⚠️ CRITICAL**: Must complete before ANY user story implementation

- [X] T003 Review Robot.h to understand base class API (scan, cannon, drive, getters)
- [X] T004 Review RobotTypes.h to understand constants and data types (MAX_SPEED, MAX_PRECISION, SHOOT_ENERGY_COST, etc.)
- [X] T005 Review Trigonometry.h to understand utility functions (direction, distance, compassUnify)
- [X] T006 Study Runner robot implementation in Robots/Runner/ for reference patterns
- [X] T007 Study Scanner robot implementation in Robots/Scanner/ for combat patterns

**Checkpoint**: Foundation ready - robot API understood, user story implementation can begin

---

## Phase 3: User Story 1 - Basic Edge Patrol Movement (Priority: P1) 🎯 MVP

**Goal**: Implement edge patrol navigation that moves robot along arena walls without collision

**Independent Test**: Build EdgePatrol.so and run game with this robot. Observe robot navigate along edges, smoothly turn at corners, and complete at least 5 full circuits without colliding with walls.

### Implementation for User Story 1

- [X] T008 [US1] Create EdgePatrol.h header file with class declaration in Robots/EdgePatrol/EdgePatrol.h
- [X] T009 [US1] Implement patrol state enumeration (REACH_EDGE, PATROL_TOP_EDGE, PATROL_RIGHT_EDGE, PATROL_BOTTOM_EDGE, PATROL_LEFT_EDGE, AVOID_OBSTACLE) in EdgePatrol.h
- [X] T010 [US1] Add patrol state member variable to EdgePatrol class in EdgePatrol.h
- [X] T011 [US1] Implement factory function getRobot() in Robots/EdgePatrol/EdgePatrol.cpp
- [X] T012 [US1] Initialize static tuning configuration {tune_speed=1, tune_cannon=1} in EdgePatrol.cpp
- [X] T013 [US1] Implement init() method to set robot name to "EdgePatrol" and return tuning in EdgePatrol.cpp
- [X] T014 [US1] Implement constructor for EdgePatrol class in EdgePatrol.cpp
- [X] T015 [US1] Implement run() method main loop skeleton with tick processing in EdgePatrol.cpp
- [X] T016 [US1] Implement edge detection logic to calculate nearest edge from spawn position in EdgePatrol.cpp run()
- [X] T017 [US1] Implement REACH_EDGE state logic to drive robot toward nearest edge in EdgePatrol.cpp
- [X] T018 [US1] Implement PATROL_TOP_EDGE state to move east/west at Y=edgeDistance in EdgePatrol.cpp
- [X] T019 [US1] Implement PATROL_BOTTOM_EDGE state to move east/west at Y=arenaHeight-edgeDistance in EdgePatrol.cpp
- [X] T020 [US1] Implement PATROL_LEFT_EDGE state to move north/south at X=edgeDistance in EdgePatrol.cpp
- [X] T021 [US1] Implement PATROL_RIGHT_EDGE state to move north/south at X=arenaWidth-edgeDistance in EdgePatrol.cpp
- [X] T022 [US1] Implement corner detection logic to trigger edge state transitions in EdgePatrol.cpp
- [X] T023 [US1] Implement smooth corner turning by reducing speed to ≤50 before direction change in EdgePatrol.cpp
- [X] T024 [US1] Implement obstacle detection scan in movement direction in EdgePatrol.cpp
- [X] T025 [US1] Implement AVOID_OBSTACLE state to navigate around detected obstacles in EdgePatrol.cpp

**Checkpoint**: At this point, User Story 1 should be fully functional - robot patrols edges without collision

---

## Phase 4: User Story 2 - Center-Focused Shooting (Priority: P1)

**Goal**: Implement continuous shooting toward arena center while patrolling

**Independent Test**: Run game with EdgePatrol robot in empty arena. Verify robot fires projectiles toward center at consistent rate (≥2 shots per second) and shooting angle adjusts as robot moves.

### Implementation for User Story 2

- [X] T026 [US2] Add targeting state enumeration (TARGET_CENTER, TARGET_THREAT) to EdgePatrol.h
- [X] T027 [US2] Add targetMode member variable to EdgePatrol class in EdgePatrol.h
- [X] T028 [US2] Calculate arena center coordinates (4999, 4999) in EdgePatrol.cpp
- [X] T029 [US2] Implement center shooting angle calculation using Trigonometry::direction() in EdgePatrol.cpp run()
- [X] T030 [US2] Implement energy check before firing (getEnergy() ≥ 200) in EdgePatrol.cpp
- [X] T031 [US2] Implement shotsInAir counter tracking in EdgePatrol.cpp to enforce ≤3 shot limit
- [X] T032 [US2] Call cannon() to fire at center when energy sufficient and shotsInAir < 3 in EdgePatrol.cpp run()
- [X] T033 [US2] Integrate center shooting into main tick loop in EdgePatrol.cpp run()

**Checkpoint**: At this point, User Stories 1 AND 2 should both work - robot patrols edges while shooting at center

---

## Phase 5: User Story 3 - Threat Detection and Response (Priority: P1)

**Goal**: Detect robots within ±10° of movement direction and switch targeting from center to threat

**Independent Test**: Run game with EdgePatrol robot and one other robot (e.g., Runner). When Runner enters ±10° detection angle, verify EdgePatrol switches targeting to Runner instead of center. When Runner exits detection angle, verify targeting returns to center.

### Implementation for User Story 3

- [X] T034 [US3] Add detection parameters to EdgePatrol class in EdgePatrol.h (detectionAngle=10, detectionRange=6000, edgeDistance=800, cornerThreshold=500)
- [X] T035 [US3] Add threat tracking variables (lastThreatAngle, lastThreatRange) to EdgePatrol.h
- [X] T036 [US3] Implement scan() call in movement direction with ±10° precision in EdgePatrol.cpp run()
- [X] T037 [US3] Validate detectionRange against detected threat distance in EdgePatrol.cpp
- [X] T038 [US3] Implement TARGET_CENTER → TARGET_THREAT state transition when threat detected in EdgePatrol.cpp
- [X] T039 [US3] Implement TARGET_THREAT → TARGET_CENTER state transition when scan returns 0 or threat exits ±10° in EdgePatrol.cpp
- [X] T040 [US3] Switch cannon() call to target threat direction when in TARGET_THREAT mode in EdgePatrol.cpp run()
- [X] T041 [US3] Record lastThreatAngle and lastThreatRange when threat detected in EdgePatrol.cpp
- [X] T042 [US3] Implement threat angle deviation check (>10° from movement direction) to return to center targeting in EdgePatrol.cpp

**Checkpoint**: At this point, User Stories 1, 2, AND 3 should all work - robot patrols, shoots at center, and adapts to threats

---

## Phase 6: User Story 4 - Edge Patrol with Active Combat (Priority: P2)

**Goal**: Integrate all behaviors (edge patrol, center shooting, threat detection) to work simultaneously

**Independent Test**: Run game with EdgePatrol robot, obstacles, and multiple robots. Verify robot maintains edge patrol 95%+ of time while dynamically switching between center and threat targeting, navigating around obstacles when needed.

### Implementation for User Story 4

- [X] T043 [US4] Integrate threat detection into main tick loop without disrupting edge patrol state machine in EdgePatrol.cpp run()
- [X] T044 [US4] Ensure edge patrol continues uninterrupted during threat detection and targeting switches in EdgePatrol.cpp
- [X] T045 [US4] Add logging via RobotGame::Logger::Log() for patrol state transitions in EdgePatrol.cpp
- [X] T046 [US4] Add logging via RobotGame::Logger::Log() for targeting state changes in EdgePatrol.cpp
- [X] T047 [US4] Add logging via RobotGame::Logger::Log() for obstacle detection in EdgePatrol.cpp
- [X] T048 [US4] Test edge patrol with obstacles to verify navigation works during threat engagement in EdgePatrol.cpp

**Checkpoint**: All user stories complete - robot demonstrates integrated edge patrol and combat behaviors

---

## Phase 7: Polish & Cross-Cutting Concerns

**Purpose**: Build integration, testing, and final touches

- [X] T049 [P] Build EdgePatrol.so using Makefile in Robots/EdgePatrol/
**Note**: All test tasks (T050-T059) can run in parallel if testing multiple scenarios simultaneously
- [X] T050 [P] Test EdgePatrol in empty arena to verify basic movement and center shooting
- [X] T051 [P] Test EdgePatrol vs Runner robot to verify threat detection and targeting switching
- [X] T052 [P] Test EdgePatrol vs Scanner robot to verify combat performance
- [X] T053 [P] Test EdgePatrol vs multiple robots (Runner + Scanner) to verify multi-threat handling
- [X] T054 [P] Test EdgePatrol in arenas with obstacles (if supported) to verify obstacle avoidance
- [X] T055 [P] Verify robot completes at least 5 full edge circuits without collision (SC-001)
- [X] T056 [P] Verify fire rate ≥2 shots per second (SC-002)
- [X] T057 [P] Verify targeting switches within 100ms when threat enters/exits detection angle (SC-003, SC-004)
- [X] T058 [P] Verify edge patrol maintained ≥95% of session time (SC-005)
- [X] T059 [P] Test robot with different arena sizes if configurable (SC-007)
- [X] T060 Update quickstart.md with any build or runtime issues discovered during testing in specs/001-edge-patrol-robot/quickstart.md

---

## Dependencies & Execution Order

### Phase Dependencies

- **Setup (Phase 1)**: No dependencies - can start immediately
- **Foundational (Phase 2)**: Depends on Setup - BLOCKS all user stories
- **User Stories (Phase 3-6)**: All depend on Foundational phase completion
  - Can proceed sequentially: US1 → US2 → US3 → US4 (P1 → P1 → P1 → P2)
  - Or in parallel: US1, US2, US3 can start simultaneously after Foundational
- **Polish (Phase 7)**: Depends on all desired user stories being complete

### User Story Dependencies

- **User Story 1 (P1)**: Can start after Foundational (Phase 2) - No dependencies on other stories - INDEPENDENT
- **User Story 2 (P1)**: Can start after Foundational (Phase 2) - Extends US1 base but independently testable
- **User Story 3 (P1)**: Can start after Foundational (Phase 2) - Integrates with US1 + US2 but independently testable
- **User Story 4 (P2)**: Depends on US1, US2, US3 - Integration story

### Within Each User Story

- No tests required (manual testing per spec)
- Header file creation before implementation
- State declarations before state machine logic
- Individual state implementations before integration in run() method
- Each story complete before moving to next priority

### Parallel Opportunities

- **Phase 1**: All tasks can run in parallel (T001, T002)
- **Phase 2**: All review tasks can run in parallel (T003, T004, T005, T006, T007)
- **Phase 3**: After T008 (header file), T009-T025 can be partially parallel:
  - State declarations (T009-T010) can be done together
  - Basic setup (T011-T014) can be done together
  - State implementations (T017-T022) can be worked in parallel
- **Phase 4**: T026-T027 can be parallel, then T028-T033 sequential
- **Phase 5**: T034-T035 can be parallel, then T036-T042 sequential
- **Phase 6**: Integration tasks mostly sequential due to single file (EdgePatrol.cpp)
- **Phase 7**: All test tasks (T050-T059) can run in parallel if testing multiple scenarios simultaneously

---

## Parallel Example: User Story 1

```bash
# After header file created (T008), multiple developers can work on different states:

Developer A:
Task: T018 - Implement PATROL_TOP_EDGE state
Task: T021 - Implement PATROL_RIGHT_EDGE state

Developer B:
Task: T019 - Implement PATROL_BOTTOM_EDGE state
Task: T020 - Implement PATROL_LEFT_EDGE state

Developer C:
Task: T022 - Implement corner detection logic
Task: T023 - Implement smooth corner turning
```

---

## Parallel Example: User Story 2 + User Story 3

After Foundational phase, developers can work on different user stories:

```bash
Developer A (User Story 2):
Task: T026 - Add targeting state enumeration
Task: T027 - Add targetMode member variable
Task: T028 - Calculate arena center coordinates
Task: T029 - Implement center shooting angle calculation

Developer B (User Story 3):
Task: T034 - Add detection parameters to class
Task: T035 - Add threat tracking variables
Task: T036 - Implement scan() call in movement direction
Task: T037 - Validate detectionRange
```

---

## Implementation Strategy

### MVP First (User Stories 1-2 Only)

1. Complete Phase 1: Setup
2. Complete Phase 2: Foundational (CRITICAL - blocks all stories)
3. Complete Phase 3: User Story 1 (Basic Edge Patrol Movement)
4. Complete Phase 4: User Story 2 (Center-Focused Shooting)
5. **STOP and VALIDATE**: Test US1 + US2 together by running robot in empty arena
6. Build EdgePatrol.so and verify edge patrol + center shooting work correctly

**MVP delivers**: Robot that patrols arena edges while continuously shooting toward center

### Incremental Delivery

1. Complete Setup + Foundational → Foundation ready
2. Add User Story 1 → Test independently → Core navigation works
3. Add User Story 2 → Test independently → MVP complete (patrol + center shooting)
4. Add User Story 3 → Test independently → Threat detection and targeting switching works
5. Add User Story 4 → Test independently → Full integration with obstacles and multi-robot combat
6. Polish → Comprehensive testing across all scenarios

### Parallel Team Strategy

With multiple developers:

1. Team completes Setup + Foundational together
2. Once Foundational is done:
   - Developer A: User Story 1 (navigation states)
   - Developer B: User Story 2 (center shooting)
   - Developer C: User Story 3 (threat detection)
3. Stories complete and integrate sequentially (US1 → US2 → US3)
4. Developer A or team: User Story 4 (integration)
5. Team: Polish and testing (Phase 7)

---

## Notes

- [P] tasks = different files, no dependencies, can be worked by different developers
- [US1], [US2], [US3], [US4] labels map task to specific user story for traceability
- Each user story is independently completable and testable
- Manual testing per spec (no automated tests)
- EdgePatrol.cpp will contain all run() logic - organized by user story additions
- Follow RobotGame framework patterns from Runner and Scanner reference implementations
- Use Trigonometry utilities for all angle and distance calculations
- Respect RobotGame constraints (MAX_SHOOT=3, SHOOT_ENERGY_COST=200, MAX_PRECISION=15, etc.)
