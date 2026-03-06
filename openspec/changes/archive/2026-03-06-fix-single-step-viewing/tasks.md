## 1. Fix Viewer Wait Predicate

- [x] 1.1 In `Viewer.cpp`, locate the `eventCv.wait()` call inside `Viewer::Runner()` (the `if (!isPaused)` branch) and extend its predicate from `goDie || !evQueue.empty()` to `goDie || !evQueue.empty() || Scheduler::getScheduler().isStepPaused()`

## 2. Verify Correct Behavior

- [x] 2.1 Build the project (`cmake --build build`) and confirm it compiles without errors or warnings
- [x] 2.2 Run the game with `--step` and verify robots remain visible on screen while waiting for Enter between ticks
- [x] 2.3 Confirm that pressing Enter advances to the next tick and the display updates correctly
- [x] 2.4 Run without `--step` and confirm normal (continuous) game behavior is unchanged
