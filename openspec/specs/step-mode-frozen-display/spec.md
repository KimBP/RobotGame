## Purpose

Defines the behaviour of the viewer when the game runs in `--step` mode. The viewer SHALL keep the last rendered frame visible on screen while the scheduler is blocked waiting for the user to press Enter, so that robot positions and the scoreboard remain readable between ticks.

## Requirements

### Requirement: Frozen display during step pause
While the scheduler is paused waiting for user input in `--step` mode, the viewer SHALL display the most-recently-known game state (robot positions, shells, scoreboard) on screen without blanking the arena between ticks.

#### Scenario: Robots visible while paused
- **WHEN** `--step` mode is active and the scheduler is blocked on `std::getline` waiting for Enter
- **THEN** robots, cannon shells, and the scoreboard SHALL remain visible on screen and SHALL reflect the positions from the last completed tick

#### Scenario: No visual artefacts between ticks
- **WHEN** the viewer thread is blocked in `eventCv.wait()` between ticks
- **THEN** the SDL window SHALL display the last presented frame unchanged (no blank arena, no missing robots)

### Requirement: Single frame present per tick
The viewer SHALL compose the full frame (arena and scoreboard) into the back-buffer and call `SDL_RenderPresent` exactly once per tick, so that the presented frame contains both arena and status content.

#### Scenario: Arena and scoreboard both visible after present
- **WHEN** a tick completes and the viewer calls `SDL_RenderPresent`
- **THEN** the presented frame SHALL include both robot positions in the arena area and scoreboard data in the status area

### Requirement: Status area cleared without wiping arena
When clearing the status (scoreboard) area before redrawing it, the viewer SHALL clear only the status area pixels and SHALL NOT affect the arena area.

#### Scenario: Arena preserved during status clear
- **WHEN** the viewer clears the status area before drawing scoreboard data
- **THEN** the arena area of the back-buffer SHALL be unmodified by the clear operation

### Requirement: Resume after Enter
After the user presses Enter and the scheduler advances to the next tick, the viewer SHALL render and display the updated game state.

#### Scenario: Seamless resume
- **WHEN** the user presses Enter in `--step` mode and the scheduler posts the next tick's events
- **THEN** the viewer SHALL wake from `eventCv.wait()`, process the events, render the updated frame, and present it
