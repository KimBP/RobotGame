## ADDED Requirements

### Requirement: Command-line step mode enablement
The executable SHALL accept a `--step` command-line argument that enables step mode execution.

#### Scenario: Step mode enabled with --step flag
- **WHEN** the application is invoked with `./robot-game --step`
- **THEN** the game initializes in step mode

#### Scenario: Normal mode when flag is absent
- **WHEN** the application is invoked without the `--step` flag
- **THEN** the game runs in normal (continuous) mode

### Requirement: Step mode pauses after each tick
When step mode is enabled, the game SHALL pause execution after each game tick and wait for user input (Enter key) before advancing to the next tick.

#### Scenario: Game pauses after tick execution
- **WHEN** step mode is enabled and a game tick completes
- **THEN** the game loop pauses and displays a prompt to the console

#### Scenario: User advances to next tick via Enter key
- **WHEN** a user presses Enter in the console
- **THEN** the game advances to the next tick

#### Scenario: Multiple sequential steps
- **WHEN** a user presses Enter multiple times in quick succession
- **THEN** each press advances the game by exactly one tick

### Requirement: Step mode user feedback
The system SHALL provide clear console output indicating step mode is active and waiting for user input.

#### Scenario: Console prompt displayed
- **WHEN** step mode is enabled and waiting for input
- **THEN** the console displays a prompt (e.g., "Press Enter to advance to next tick" or similar clear message)

#### Scenario: Feedback after advancement
- **WHEN** a tick has been executed in step mode
- **THEN** the console displays confirmation or status information (e.g., tick number or robot state summary)

### Requirement: Step mode does not affect game logic
The system SHALL execute game logic identically in step mode and normal mode; step mode only controls pacing, not behavior.

#### Scenario: Robot behavior unchanged
- **WHEN** a sequence of ticks is executed in step mode
- **THEN** the same tick sequence in normal mode produces identical game state and robot positions

#### Scenario: Tick duration timing unchanged
- **WHEN** tick execution occurs in step mode
- **THEN** individual tick execution time is not affected by step mode; only the wait-between-ticks is added

### Requirement: Step mode is mutually exclusive with normal execution
The system SHALL not allow or require simultaneous operation of step mode and continuous execution.

#### Scenario: No mode toggling during runtime
- **WHEN** the game is running in step mode
- **THEN** there is no interactive mechanism to disable step mode without restarting the application

#### Scenario: Mode is determined at startup
- **WHEN** the game process starts
- **THEN** the execution mode (step vs. continuous) is determined by command-line arguments and does not change until application restart
