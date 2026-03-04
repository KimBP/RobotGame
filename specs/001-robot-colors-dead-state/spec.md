# Feature Specification: Robot Colors and Dead State Visualization

**Feature Branch**: `001-robot-colors-dead-state`  
**Created**: 2026-01-22  
**Status**: Draft  
**Input**: User description: "The RobotGame arena must assign different colors to the different robots being loaded. Furthermore instead of totally removing a robot visually from the arena while it dies, it should be shown in a dead state. A dead state could e.g. be another symbol or are weaker color"

## User Scenarios & Testing *(mandatory)*

### User Story 1 - Distinguish Robots by Color (Priority: P1)

As a player watching the arena, I want each robot to be displayed in a unique, distinct color so that I can easily identify and track individual robots during gameplay.

**Why this priority**: This is the primary visual differentiation mechanism. Without distinct colors, players cannot identify which robot is which during fast-paced gameplay, making the game difficult to follow and analyze.

**Independent Test**: Can be fully tested by loading 2-4 robots into the arena and verifying each robot renders with a visually distinct color that persists throughout the match.

**Acceptance Scenarios**:

1. **Given** 2 robots are loaded into the arena, **When** the game starts, **Then** each robot is displayed with a different, visually distinct color.
2. **Given** 4 robots are loaded into the arena, **When** the game starts, **Then** all 4 robots have unique colors that are easily distinguishable from each other and the background.
3. **Given** a robot is loaded into the arena, **When** the robot moves around, **Then** its assigned color remains consistent throughout the entire match.
4. **Given** a robot fires shells, **When** shells are displayed, **Then** the shells are rendered in the same color as the robot that fired them.

---

### User Story 2 - Visualize Dead Robots (Priority: P1)

As a player, I want dead robots to remain visible in the arena in a clearly distinguishable "dead" state so that I can see where robots fell and understand the final battle state.

**Why this priority**: Currently robots simply disappear when they die, which is jarring and loses important match information. Showing dead robots provides better game feedback and post-match analysis capability.

**Independent Test**: Can be fully tested by running a match until a robot dies and verifying the dead robot remains visible with a distinct dead appearance.

**Acceptance Scenarios**:

1. **Given** a robot with armor reduced to 0, **When** the robot dies, **Then** the robot remains visible in the arena at its last position.
2. **Given** a robot has just died, **When** viewing the dead robot, **Then** it is clearly distinguishable from living robots through visual differences (faded color or different symbol).
3. **Given** multiple robots have died during a match, **When** viewing the arena, **Then** all dead robots remain visible at their death locations.
4. **Given** a dead robot is displayed, **When** the match continues, **Then** the dead robot does not obstruct or interfere with living robots' movements or visibility.

---

### User Story 3 - Color Assignment Consistency (Priority: P2)

As a player running multiple matches, I want robot colors to be assigned consistently based on the order robots are loaded, so I can develop familiarity with my robots across sessions.

**Why this priority**: Consistent color assignment enhances player experience but is secondary to having distinct colors in the first place.

**Independent Test**: Can be tested by loading the same robots in the same order across multiple matches and verifying color assignments are identical.

**Acceptance Scenarios**:

1. **Given** the same robots are loaded in the same order, **When** starting multiple matches, **Then** each robot receives the same color assignment.
2. **Given** a robot color palette exists, **When** more robots are loaded than colors available, **Then** colors are reused in a predictable pattern while maximizing visual distinction.

---

### User Story 4 - Scoreboard Color Integration (Priority: P1)

As a player viewing the scoreboard, I want each robot's entry to display in its assigned color so that I can quickly correlate scoreboard information with robots in the arena.

**Why this priority**: The scoreboard is a key information display. Without color correlation, players must rely on names alone to match scoreboard entries to arena robots, reducing usability.

**Independent Test**: Can be fully tested by loading robots and verifying the scoreboard displays each robot's name in its assigned color with a color indicator.

**Acceptance Scenarios**:

1. **Given** robots are loaded into the arena, **When** viewing the scoreboard, **Then** each robot's name text is displayed in its assigned color.
2. **Given** robots are loaded into the arena, **When** viewing the scoreboard, **Then** a color indicator (dot/box) appears next to each robot's name matching its assigned color.
3. **Given** a robot dies, **When** viewing the scoreboard, **Then** the robot's name is shown at reduced opacity with a skull symbol next to it.

---

### Edge Cases

- What happens when more than 4 robots are loaded? Colors cycle using existing modulo arithmetic (colors[id % 4]), resulting in multiple robots sharing the same color. This is acceptable for MVP; future enhancement could add color palette extension or color generation algorithm if >4 simultaneous robots become common.
- What happens when a robot dies exactly at the arena boundary? Dead robot should be displayed at its last valid position.
- What happens if two robots die at nearly the same location? Both dead robots should be visible, potentially with slight offset or overlapping display.
- What happens when all robots die? All robots remain visible in their dead state until the match ends.
- What happens when a robot dies from different causes (shot vs wall collision)? The same dead state visualization is shown regardless of death cause.

## Requirements *(mandatory)*

### Functional Requirements

- **FR-001**: System MUST assign a unique color to each robot when it is loaded into the arena.
- **FR-002**: System MUST ensure all robot colors are visually distinct from each other and from the arena background (black). Colors in the predefined palette provide sufficient visual distinction for manual identification (validated through existing 4-color array: Yellow 0xFFF000FF, Red 0xFF0F00FF, Cyan 0xFF00F0FF, Blue 0xFF000FFF).
- **FR-003**: System MUST maintain a robot's assigned color for the entire duration of the match.
- **FR-004**: System MUST display robots that fire shells with shells colored to match the firing robot.
- **FR-005**: System MUST keep dead robots visible in the arena at their death location instead of removing them.
- **FR-006**: System MUST display dead robots with a clearly distinguishable visual state (original color at 50% opacity via alpha channel reduction).
- **FR-007**: System MUST support at least 4 distinct robot colors for simultaneous robots.
- **FR-008**: System MUST render dead robots on a layer that does not obstruct living robots or active gameplay elements.
- **FR-009**: System MUST use the same color for a robot's status display elements (name, energy, armor text) as its body color.
- **FR-010**: System MUST display the robot's assigned color in the scoreboard by coloring the robot name text and showing a color indicator (dot/box) next to the name.
- **FR-011**: System MUST display the same dead state visualization regardless of death cause (shot by enemy, wall collision, or any other cause).
- **FR-012**: System MUST indicate dead robots in the scoreboard by reducing the name text opacity and displaying a skull symbol next to the name.

### Key Entities

- **Robot Visual State**: The display properties of a robot including position, color, and alive/dead status.
- **Color Palette**: A predefined set of visually distinct colors for robot assignment.
- **Dead Robot Marker**: A visual representation of a robot that has been destroyed, showing its final position.
- **Scoreboard Entry**: A row in the scoreboard displaying robot name (colored), color indicator, status (alive/dead with skull symbol), and statistics.

## Success Criteria *(mandatory)*

### Measurable Outcomes

- **SC-001**: Players can identify each robot within 1 second of viewing the arena by its distinct color.
- **SC-002**: Players can immediately distinguish living robots from dead robots at a glance.
- **SC-003**: 100% of dead robots remain visible in the arena until the match concludes.
- **SC-004**: Robot color assignments are deterministic - same load order produces same color assignments every time.
- **SC-005**: All 4 supported robot slots have visually distinct colors. Note: Colorblind accessibility testing is aspirational for future validation; current palette (Yellow, Red, Cyan, Blue) provides varied hues for basic distinction.
- **SC-006**: Dead robot visual state is distinguishable from living robots within 0.5 seconds of observation.

## Clarifications

### Session 2026-01-22

- Q: What scoreboard elements should reflect the robot's color? → A: Color the robot name text and add a color indicator (dot/box) next to the name.
- Q: Should death visualization differ based on death cause (shot vs wall collision)? → A: No, death visualization should be identical regardless of death cause.
- Q: How should the scoreboard indicate that a robot is dead? → A: Reduce opacity of name and add a skull symbol.

## Assumptions

- The existing color array (4 colors) is sufficient for typical usage (2-4 robots). If more than 4 robots are loaded, colors cycle via modulo arithmetic (colors[id % 4]), resulting in duplicate colors. Palette extension is considered a future enhancement if needed.
- The dead state will use the robot's original color at 50% opacity (alpha channel reduction only, no color desaturation), as this provides clear visual distinction while maintaining color identity.
- Dead robots will be rendered as the same circular shape but with reduced opacity, rather than a different symbol. This maintains visual consistency and is simpler to implement.
- Shell coloring to match firing robot is a natural extension that improves gameplay clarity.
- The layered rendering system already in place can accommodate a separate layer or rendering pass for dead robots.
