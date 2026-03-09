## ADDED Requirements

### Requirement: Robot viewer ID is assigned at spawn and never changes
Each robot SHALL be assigned a numeric viewer ID when it is added to the game. This ID SHALL remain constant for the robot's entire lifetime, regardless of other robots dying or being removed.

#### Scenario: ID is stable after another robot dies
- **WHEN** robot B (ID 1) dies and is removed from the game
- **THEN** robot C (ID 2) SHALL continue to be identified and rendered with ID 2 in subsequent ticks

#### Scenario: Colors remain correct after a death
- **WHEN** any robot dies during a game with 3 or more robots
- **THEN** all surviving robots SHALL continue to display the same color in the arena that they had before the death occurred

#### Scenario: Scoreboard and arena colors remain in sync after a death
- **WHEN** a robot dies
- **THEN** the arena circle color for each surviving robot SHALL match the color indicator shown for that robot in the scoreboard

#### Scenario: Dead robot ghost persists at correct position
- **WHEN** a robot dies
- **THEN** its ghost (50% alpha circle) SHALL remain visible at its last known position and SHALL NOT be replaced by a surviving robot's circle
