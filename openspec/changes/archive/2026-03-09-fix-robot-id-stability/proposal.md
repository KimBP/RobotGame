## Why

When a robot dies and is removed from the Scheduler's internal vector, all subsequent robots shift to lower indices. The Scheduler assigns viewer IDs by iteration position each tick, so surviving robots receive new IDs and are rendered with the wrong color in the arena. For example, with robots A(0), B(1), C(2): when B dies, C becomes index 1 and is subsequently rendered with B's color.

## What Changes

- Add a stable `viewerId` field to `RobCtrl`, assigned once at construction time in `addRobot()`.
- Expose it via `RobCtrl::getId()`.
- Replace the per-tick loop counter with `robCtrl->getId()` in all `tickEnd()` event postings.

## Capabilities

### New Capabilities

- `stable-robot-identity`: Each robot retains a fixed viewer ID for its entire lifetime, independent of other robots dying or being removed.

### Modified Capabilities

<!-- No existing spec-level requirements are changing. -->

## Impact

- `RobCtrl.h`: add `int viewerId` field and `getId()` accessor.
- `RobCtrl.cpp`: initialise `viewerId` to `-1` in constructor.
- `Scheduler.cpp`: set `robCtrl->viewerId` in `addRobot()`; use `robCtrl->getId()` in `tickEnd()` and `run()` (step-mode initial render loop).
- No changes to Viewer, the robots map, or any robot `.so` implementations.
