# Feature Specification: Graphics Fixes

**Feature Branch**: `001-graphics-fixes`  
**Created**: 2026-01-13  
**Status**: Draft  
**Input**: User description: "We want to fix the graphical findings found during review: 1: Viewer.cpp:207 Missing SDL_RENDERER_PRESENT_VSYNC flag, 2: Scheduler.cpp:163: No timing control between ticks, 3: Viewer.cpp:160: Shells vector cleared each frame, losing motion continuity, 4: Viewer.cpp:261-264: Shell blast rendering draws over everything. The timing control must be configurable as game option optionally given at the commandline when starting a battle"

## User Scenarios & Testing *(mandatory)*

<!--
  IMPORTANT: User stories should be PRIORITIZED as user journeys ordered by importance.
  Each user story/journey must be INDEPENDENTLY TESTABLE - meaning if you implement just ONE of them,
  you should still have a viable MVP (Minimum Viable Product) that delivers value.
  
  Assign priorities (P1, P2, P3, etc.) to each story, where P1 is the most critical.
  Think of each story as a standalone slice of functionality that can be:
  - Developed independently
  - Tested independently
  - Deployed independently
  - Demonstrated to users independently
-->

### User Story 1 - Smooth Battle Visualization (Priority: P1)

As a player watching a battle, I want to see smooth, properly timed animations so I can follow the action and understand what's happening during robot combat.

**Why this priority**: This addresses the core user experience issue where battles are too fast and visually chaotic, making it impossible to follow the gameplay.

**Independent Test**: Can be fully tested by running a battle with two robots and observing that animations are smooth, shell movements are visible, and the frame rate is controlled.

**Acceptance Scenarios**:

1. **Given** a battle is started with timing control enabled, **When** robots move and shoot, **Then** animations play at a controlled, viewable speed
2. **Given** shells are fired, **When** they travel across the arena, **Then** their motion trails are visible and smooth
3. **Given** a shell explodes, **When** the blast effect renders, **Then** it appears properly without drawing over other elements

---

### User Story 2 - Configurable Battle Speed (Priority: P2)

As a player, I want to configure the battle speed through command-line options so I can choose between fast-paced or slow-mo viewing based on my preferences.

**Why this priority**: Provides user control over the viewing experience, allowing both fast gameplay for testing and slow gameplay for analysis.

**Independent Test**: Can be fully tested by launching the game with different speed options and verifying the timing adjusts accordingly.

**Acceptance Scenarios**:

1. **Given** no speed option is provided, **When** the game starts, **Then** it uses default timing (e.g., 60 FPS)
2. **Given** a speed option is provided (e.g., "--delay 100"), **When** the game runs, **Then** each tick waits the specified milliseconds
3. **Given** an invalid speed option is provided, **When** the game starts, **Then** it displays an error and uses default timing

---

### User Story 3 - VSync Enabled Rendering (Priority: P2)

As a player, I want the battle graphics to sync with my monitor's refresh rate so I don't see screen tearing or visual artifacts during fast movements.

**Why this priority**: Eliminates screen tearing and provides a more professional visual experience.

**Independent Test**: Can be fully tested by running a battle and observing that no screen tearing occurs during rapid movements.

**Acceptance Scenarios**:

1. **Given** the game launches, **When** the SDL renderer is created, **Then** it uses the PRESENT_VSYNC flag
2. **Given** robots move rapidly across the screen, **When** rendering occurs, **Then** no horizontal tearing lines are visible
3. **Given** multiple shells are in flight, **When** they animate, **Then** their movement appears smooth without visual artifacts

---

[Add more user stories as needed, each with an assigned priority]

### Edge Cases

- What happens when the user specifies a very low delay value (e.g., 1ms)?
- How does the system handle when VSync is not supported by the display hardware?
- What happens when multiple shells explode in the same frame?

## Requirements *(mandatory)*

<!--
  ACTION REQUIRED: The content in this section represents placeholders.
  Fill them out with the right functional requirements.
-->

### Functional Requirements

- **FR-001**: System MUST enable SDL_RENDERER_PRESENT_VSYNC flag when creating the renderer
- **FR-002**: System MUST provide configurable timing control between game ticks via command-line option
- **FR-003**: System MUST preserve shell motion continuity between frames instead of clearing shells vector each frame
- **FR-004**: System MUST render shell blast effects without overlapping other game elements improperly
- **FR-005**: System MUST accept a command-line argument for specifying millisecond delay between ticks
- **FR-006**: System MUST use default timing when no command-line option is provided
- **FR-007**: System MUST validate command-line timing parameters and handle invalid input gracefully

### Key Entities *(include if feature involves data)*

- **Battle Timing Configuration**: Represents the delay between game ticks, including default value and user-specified override
- **Shell Animation State**: Tracks shell positions across multiple frames to maintain motion continuity
- **Render State**: Manages VSync-enabled renderer and frame presentation timing

## Success Criteria *(mandatory)*

<!--
  ACTION REQUIRED: Define measurable success criteria.
  These must be technology-agnostic and measurable.
-->

### Measurable Outcomes

- **SC-001**: Battle animations run at a controlled, viewable speed (typically 30-60 FPS)
- **SC-002**: Shell projectiles maintain visible motion trails across frames
- **SC-003**: Screen tearing is eliminated through VSync synchronization
- **SC-004**: Users can specify battle timing via command-line with millisecond precision
- **SC-005**: Blast effects render properly without visual overlap issues
- **SC-006**: System handles timing configuration errors gracefully without crashing
