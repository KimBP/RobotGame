## 1. Analysis and Planning

- [x] 1.1 Review RobotGame.cpp and identify the main game loop structure
- [x] 1.2 Locate main() function and current command-line argument handling
- [x] 1.3 Identify where Scheduler::tick() is called in the game loop
- [x] 1.4 Determine frame/tick timing and where to insert step mode pauses

## 2. Command-Line Argument Parsing

- [x] 2.1 Add global or local variable to store step mode flag
- [x] 2.2 Modify main() to parse command-line arguments
- [x] 2.3 Detect `--step` flag in argv and set step mode enabled
- [x] 2.4 Add simple help message documentation for --step flag

## 3. Game Loop Modification

- [x] 3.1 Locate the main game loop in RobotGame.cpp
- [x] 3.2 Add conditional check for step_mode_enabled after each game tick
- [x] 3.3 Implement pause mechanism: call step mode pause/wait function when enabled
- [x] 3.4 Ensure game rendering occurs normally in step mode (no visual differences)
- [x] 3.5 Verify normal mode execution is unaffected when step mode is disabled

## 4. Console Input and Output Implementation

- [x] 4.1 Create a function to handle step mode pause (waits for Enter key)
- [x] 4.2 Implement console prompt output (e.g., "Press Enter to advance to next tick")
- [x] 4.3 Use std::cin or std::getline to read user input
- [x] 4.4 Add std::cout flush to ensure prompts are visible
- [x] 4.5 Handle EOF and other edge cases gracefully

## 5. Visual Feedback and Status Reporting

- [x] 5.1 Display tick count or iteration number after each advance in step mode
- [x] 5.2 Display initial status message when step mode is enabled at startup
- [x] 5.3 Display status message after step mode is active
- [x] 5.4 Ensure all console output is sufficiently clear for debugging purposes

## 6. Testing and Validation

- [x] 6.1 Test command-line parsing: verify --step flag is recognized
- [x] 6.2 Test normal mode: verify application runs without --step flag
- [x] 6.3 Manual test step mode: verify Enter key advances ticks
- [x] 6.4 Verify step mode does not affect game logic (robots move the same way)
- [x] 6.5 Verify normal mode performance is not impacted by step mode code
- [x] 6.6 Test multiple sequential steps to ensure each press advances exactly one tick
- [x] 6.7 Test that rendering works correctly in step mode

*Notes*:
- resolved responsiveness issue by unlocking scheduler mutex before waiting; viewer thread remains active so window stays responsive.
- preserved frame visibility by having viewer continue rendering and updating animations while scheduler is paused, but skipping Scheduler::end() call.

## 7. Documentation

- [x] 7.1 Add --step flag documentation to README or help output
- [x] 7.2 Add code comments explaining step mode conditional logic
- [x] 7.3 Document any behavioral differences or limitations of step mode
