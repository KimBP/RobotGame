## Context

The game supports a `--step` flag that pauses execution after each tick so the user can inspect the arena frame-by-frame. The viewer thread (`Viewer::Runner`) and the scheduler thread communicate via a mutex (`schedulerMtx`), an event queue (`evQueue`), a condition variable (`eventCv`), and a boolean flag (`stepPaused`).

When the scheduler finishes a tick it:
1. Posts position/data events to `evQueue`.
2. Calls `Viewer::tick()` to wake the viewer (notifies `eventCv`).
3. Blocks on `schedulerMtx.lock()` waiting for the viewer to call `Scheduler::end()`.

The viewer renders the frame, calls `Scheduler::end()`, then loops back to wait:
```cpp
eventCv.wait(lock, [&]{ return goDie || !evQueue.empty(); });
```

After that, the scheduler calls `pauseForStepMode()`, which:
1. Sets `stepPaused = true`.
2. Calls `Viewer::notifyEvent()` (notifies `eventCv`).
3. Unlocks `schedulerMtx` and blocks on `std::getline(std::cin, ...)`.

**The bug**: By the time `notifyEvent()` fires, the viewer is already back in `eventCv.wait()`. The predicate `goDie || !evQueue.empty()` is false (queue is empty), so the viewer immediately goes back to sleep — the notification is lost. The viewer never transitions to its paused-render loop, and the SDL window shows a blank or stale frame (artifacts disappear; only the scoreboard, drawn last, persists).

## Goals / Non-Goals

**Goals:**
- While the scheduler is paused waiting for Enter, the viewer SHALL continuously render the frozen game state (robots, shells, scoreboard) so the user can inspect it.
- The fix SHALL be minimal: touch only the wait predicate (and the sleep interval if appropriate), leaving all other rendering and synchronization logic unchanged.
- Behavior outside `--step` mode SHALL be identical to before.

**Non-Goals:**
- Not replacing the overall scheduler/viewer synchronization model.
- Not adding keyboard input to SDL (Enter is still read from stdin).
- Not changing frame rate, rendering layers, or visual styling.

## Decisions

### Decision 1: Add `isStepPaused()` to the condition-variable wait predicate

**Choice**: Extend the wait predicate to `goDie || !evQueue.empty() || Scheduler::getScheduler().isStepPaused()`.

**Rationale**: `pauseForStepMode()` already sets `stepPaused = true` _before_ calling `notifyEvent()`, and `notifyEvent()` acquires `eventMutex` before calling `notify_one()`. Because `eventCv.wait()` atomically releases `eventMutex` while sleeping, the write to `stepPaused` is visible to the viewer when the predicate is re-evaluated after the notify. Adding `isStepPaused()` to the predicate is therefore both correct (happens-before via `eventMutex`) and sufficient — no additional synchronisation primitives are needed.

**Alternatives considered**:
- Posting a dedicated "pause" event to `evQueue`: eliminates the predicate change but adds a new event type and couples the pause protocol to the event system unnecessarily.
- Using a separate condition variable for the pause signal: correct but over-engineered for a single flag.

### Decision 2: Keep the existing 10 ms paused-render sleep

The existing `std::this_thread::sleep_for(10ms)` in the paused branch is retained unchanged. It yields the CPU and keeps the paused refresh at ~100 fps, which is sufficient to show a frozen frame without busy-waiting.

## Risks / Trade-offs

- [Risk] `stepPaused` is a plain `bool`, not `std::atomic<bool>`. Visibility is guaranteed through `eventMutex` (the same lock used by `eventCv.wait()`), but if `isStepPaused()` is called outside that lock elsewhere it could theoretically race. → **Mitigation**: All reads of `stepPaused` in `Viewer::Runner()` happen either inside the `eventCv.wait()` predicate (under the lock) or after the wait at the top/bottom of the loop — no new unsynchronised reads are introduced.
- [Trade-off] Rendering a frozen frame every 10 ms consumes a small amount of CPU/GPU while waiting for Enter. Acceptable because `--step` is a developer/debug tool, not a production run mode.

## Migration Plan

Single-file change in `Viewer.cpp`. No build-system changes, no new dependencies. Revert by reverting the one-line predicate change.
