## Context

The Scheduler maintains robots as `std::vector<RobCtrl*>`. When a robot's armor reaches zero, it is erased from this vector after the frame renders:

```cpp
robots.erase(
    std::remove_if(robots.begin(), robots.end(),
        [](RobCtrl* rob) { return rob->getArmor() == 0; }),
    robots.end()
);
```

Each tick, `tickEnd()` posts viewer events using a bare counter that increments with vector iteration:

```cpp
int id = 0;
robCtrl = iterateRobots(0);
while (0 != robCtrl) {
    Viewer::PostEvent(new RobotPosEvent(id, ...));
    id++;
    robCtrl = iterateRobots(robCtrl);
}
```

After a robot is erased, the vector compacts and every robot at a higher index shifts down by one. The next tick they broadcast with shifted IDs, corrupting the Viewer's color and dead-state mapping.

The Viewer's `robots` map (`std::map<int, RobPos>`) keyed by viewer ID never "forgets" an entry — `_RobotShow` only initialises color and dead-state on **first appearance** (`if (!robots.count(id))`). Stable IDs exploit this correctly: a dead robot's slot stays in the map forever with `isDead=true`, and its ID is never reused.

## Goals / Non-Goals

**Goals:**
- Each robot receives a numeric ID at spawn time that never changes.
- Viewer color, dead-state, and name texture remain permanently bound to the correct robot.
- The step-mode initial-render loop uses the same stable IDs.

**Non-Goals:**
- Changing `Scheduler::robots` from `vector` to `map` (not needed; stable IDs don't require map-ordering).
- Exposing robot IDs to robot `.so` implementations.
- Persisting IDs across game restarts.

## Decisions

### Add `viewerId` to `RobCtrl`

`RobCtrl` is the per-robot controller owned by the Scheduler. Adding a single `int viewerId` field is the narrowest possible change:

```cpp
// RobCtrl.h
int viewerId = -1;
int getId() const { return viewerId; }
```

```cpp
// Scheduler.cpp — addRobot()
robCtrl->viewerId = inst.robCnt;   // assigned before robCnt increment
inst.robots.push_back(robCtrl);
++inst.robCnt;
```

`robCnt` is already incremented in `addRobot()` and starts at 0, giving robots IDs 0, 1, 2, … in load order — exactly the indices the Viewer currently expects for colors and scoreboard slots.

**Alternative considered**: a separate `static int nextId` counter inside `addRobot()`. Equivalent; using the existing `robCnt` avoids adding another variable.

**Alternative considered**: change `Scheduler::robots` to `std::map<int, RobCtrl*>`. Fixes the shifting problem from the container side but requires rewriting `iterateRobots()` and all loop logic. Over-engineered for this problem.

### Replace loop counter in `tickEnd()` and `run()`

Both event-posting loops in `tickEnd()` and the step-mode loop in `run()` use a local `int id = 0` counter. Replace each with `robCtrl->getId()` and remove the counter:

```cpp
// Before
int id = 0;
while (0 != robCtrl) {
    Viewer::PostEvent(new RobotPosEvent(id, ...));
    id++;
    robCtrl = iterateRobots(robCtrl);
}

// After
while (0 != robCtrl) {
    Viewer::PostEvent(new RobotPosEvent(robCtrl->getId(), ...));
    robCtrl = iterateRobots(robCtrl);
}
```

## Risks / Trade-offs

- **Risk**: `robCnt` is incremented after `push_back`, so the ID assigned is `inst.robCnt` *before* the increment — must be careful to assign before incrementing. → **Mitigation**: Assign `robCtrl->viewerId = inst.robCnt` immediately before `push_back`, then `++inst.robCnt` last. This is the current order in `addRobot()`.
- **Risk**: `iterateRobots()` uses a `static` iterator, making it non-reentrant. This is a pre-existing issue; this change doesn't worsen it.
- **No migration required**: pure internal change, no saved state.
