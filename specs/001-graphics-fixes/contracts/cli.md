# Command Line Interface Contract

## Usage Syntax

```bash
RobotGame [options] robot1.so [robot2.so ...]
```

## Options

| Option | Short | Parameter | Default | Range | Description |
|--------|-------|-----------|---------|-------|-------------|
| `--delay` | `-d` | `<milliseconds>` | 16 | 1-1000 | Battle delay between ticks |
| `--help` | `-h` | - | - | - | Show usage information |

## Examples

```bash
# Default 60 FPS timing
./RobotGame Scanner.so Runner.so

# Slow motion (20 FPS)
./RobotGame --delay 50 EdgeRobot.so Scanner.so

# Very slow analysis (10 FPS)  
./RobotGame -d 100 Runner.so

# Show help
./RobotGame --help
```

## Error Handling

### Invalid Delay Values
```bash
./RobotGame --delay 1500 robot.so
# Output: Error: Delay must be between 1 and 1000 milliseconds
# Exit code: 1
```

### Missing Delay Parameter
```bash
./RobotGame --delay robot.so
# Output: Error: --delay requires a millisecond value
# Exit code: 1
```

### Unknown Options
```bash
./RobotGame --unknown robot.so
# Output: Error: Unknown option: --unknown
# Exit code: 1
```

## Success Behavior

- Valid arguments: Game starts with specified timing
- No arguments: Game starts with default timing
- Help argument: Usage shown, program exits with code 0

## Integration Requirements

- Timing configuration passed to Scheduler::setBattleDelay()
- Help display should include robot plugin requirement
- All parsing errors should go to stderr
- Success messages to stdout only