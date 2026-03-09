## 1. Add Stable ID to RobCtrl

- [x] 1.1 In `RobCtrl.h`, add `int viewerId = -1;` as a private member and `int getId() const { return viewerId; }` as a public accessor
- [x] 1.2 In `Scheduler::addRobot()` (`Scheduler.cpp`), assign `robCtrl->viewerId = inst.robCnt;` immediately before `inst.robots.push_back(robCtrl)` (before the `++inst.robCnt` increment)

## 2. Use Stable ID in Event Posting

- [x] 2.1 In `Scheduler::tickEnd()`, remove the local `int id = 0` counter from the first event loop (pos events) and replace `id` with `robCtrl->getId()`; remove the `id++` increment
- [x] 2.2 In `Scheduler::tickEnd()`, remove the local `int id = 0` counter from the second event loop (data events) and replace `id` with `robCtrl->getId()`; remove the `id++` increment
- [x] 2.3 In `Scheduler::run()`, remove the local `int id = 0` counter from the step-mode initial-render loop and replace `id` with `rob->getId()`; remove the `id++` increment (note: this loop uses range-for over `robots`, so use `rob->getId()`)

## 3. Verification

- [x] 3.1 Build the project (`cmake --build build`) and confirm no compile errors
- [x] 3.2 Run the game with 3 robots and let one die; verify surviving robots keep their original colors in the arena (no color shifts)
- [x] 3.3 Verify the dead robot's ghost remains visible at its last position with 50% alpha and is not overwritten by a surviving robot
