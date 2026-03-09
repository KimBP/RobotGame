## ADDED Requirements

### Requirement: Arena and scoreboard use the same color per robot
Each robot SHALL be displayed with the same color in the arena (filled circle) and in the scoreboard (color indicator box and name text).

#### Scenario: Colors match on game start
- **WHEN** the game starts with multiple robots
- **THEN** the filled circle drawn for each robot in the arena SHALL use the same RGB values as the color indicator shown for that robot in the scoreboard

#### Scenario: Dead robot color is consistent
- **WHEN** a robot dies and is shown as a ghost (50% alpha)
- **THEN** the hue of the arena ghost circle SHALL match the hue of the corresponding scoreboard entry at 50% alpha

#### Scenario: Color identity with 3 robots
- **WHEN** three robots are loaded (e.g., A, B, C assigned colors Yellow, Red, Purple respectively)
- **THEN** robot A SHALL appear yellow in the arena, robot B SHALL appear red, and robot C SHALL appear purple — matching the scoreboard labels exactly
