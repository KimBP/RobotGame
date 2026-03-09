## 1. Fix Arena Color Rendering

- [x] 1.1 In `Viewer::PrintRobot()`, replace `filledCircleColor(gRenderer, robots[id].x, robots[id].y, ROBOT_RADIUS, color)` with `filledCircleRGBA()` using explicit component extraction via right-bit-shifts (same pattern as the scoreboard and `_RobotDataShow`)
- [x] 1.2 Remove the intermediate `color` variable and dead-robot alpha mutation — inline the alpha selection directly into the `filledCircleRGBA` call

## 2. Verification

- [x] 2.1 Build the project (`cmake --build build`) and confirm no compile errors
- [x] 2.2 Run the game with 3 robots and visually verify that the arena robot colors match the scoreboard color indicators (Yellow↔Yellow, Red↔Red, Purple↔Purple)
- [x] 2.3 Verify dead robots still appear at 50% alpha in the arena (same hue, half opacity)
