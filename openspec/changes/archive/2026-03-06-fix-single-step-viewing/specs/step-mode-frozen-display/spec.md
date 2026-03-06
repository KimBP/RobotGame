## ADDED Requirements

### Requirement: Viewer wakes on step-pause notification
When the scheduler enters step-pause state (`stepPaused = true`) and notifies the viewer's condition variable, the viewer thread SHALL exit its event-wait and proceed to render a frame, even when the event queue is empty.

#### Scenario: Notification received with empty queue
- **WHEN** the scheduler calls `pauseForStepMode()` (setting `stepPaused = true` and notifying `eventCv`) while the viewer is blocked in `eventCv.wait()` with an empty queue
- **THEN** the viewer thread SHALL exit the wait within one notify/wakeup cycle and proceed to the render path

### Requirement: Frozen display during step pause
While the scheduler is paused waiting for user input in `--step` mode, the viewer SHALL continuously render the most-recently-known game state (robot positions, shells, scoreboard) at a refresh interval of at most 100 ms.

#### Scenario: Robots visible while paused
- **WHEN** `--step` mode is active and the scheduler is blocked on `std::getline` waiting for Enter
- **THEN** robots, cannon shells, and the scoreboard SHALL remain visible on screen and SHALL reflect the positions from the last completed tick

#### Scenario: No rendering outside step mode
- **WHEN** `--step` mode is NOT active
- **THEN** the viewer rendering loop behavior SHALL be identical to the pre-fix behavior (event-driven only, no continuous re-render)

### Requirement: Resume after Enter
After the user presses Enter and the scheduler unpauses (`stepPaused = false`), the viewer SHALL resume normal event-driven rendering for the next tick without requiring any additional user action.

#### Scenario: Seamless resume
- **WHEN** the user presses Enter in `--step` mode, causing `stepPaused` to become `false` and `notifyEvent()` to fire
- **THEN** the viewer SHALL return to waiting for scheduler tick events and SHALL render the next tick's frame when events arrive
