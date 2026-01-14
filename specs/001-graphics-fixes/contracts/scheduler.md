# Scheduler API Contract

## Class: Scheduler

### New Methods

#### `setBattleDelay(int delayMs)`
Sets the configurable delay between game ticks.

**Parameters**:
- `delayMs` (int): Milliseconds to wait between ticks (1-1000)

**Returns**: `void`

**Preconditions**:
- `delayMs` must be between 1 and 1000
- Scheduler must not be currently running

**Postconditions**:
- `tickEnd()` method will wait for specified delay
- Default timing (16ms) used if not called

**Thread Safety**:
- Must be called before `run()` method
- Not thread-safe during battle execution

#### `getBattleDelay() const`
Gets the current battle delay setting.

**Returns**: `int` - Current delay in milliseconds

## Modified Methods

### `tickEnd()`
Enhanced with configurable timing control.

**Changes**:
- Added configurable delay at end of tick processing
- Maintains existing robot and shell update logic
- Preserves thread synchronization behavior

**Behavior**:
```cpp
void tickEnd() {
    // ... existing robot and shell updates ...
    
    // NEW: Configurable delay
    if (battleDelayMs > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(battleDelayMs));
    }
    
    Viewer::tick(tick);
    schedulerMtx.lock(); // Wait for frame update
    
    // ... existing logic continues ...
}
```

## Data Members

### New Fields

#### `battleDelayMs` (private)
Configurable delay between game ticks.

**Type**: `int`  
**Default**: `16` (~60 FPS)  
**Access**: Private, accessed via `setBattleDelay()` / `getBattleDelay()`

## Integration Points

### RobotGame.cpp Integration
```cpp
// In main() after argument parsing
Scheduler& scheduler = Scheduler::getScheduler();
scheduler.setBattleDelay(parsedDelayMs);
```

### Error Handling
- Invalid delay values: Use default (16ms) and log warning
- Delay changes during battle: Ignored, take effect next battle

## Performance Impact

### CPU Usage
- Sleep-based timing reduces CPU usage during delays
- No additional computational overhead
- Minimal memory impact (single int field)

### Timing Accuracy
- `std::this_thread::sleep_for` precision: ~1ms on Linux
- Actual frame rate may be slightly lower than target
- VSync will still provide final frame timing sync

## Backward Compatibility

### Existing Behavior
- Default delay of 16ms maintains current ~60 FPS behavior
- All existing method signatures unchanged
- Thread synchronization behavior preserved

### Migration Path
- No code changes required for existing robots
- Existing battle timing behavior preserved
- Command-line option provides opt-in customization