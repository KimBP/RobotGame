# RobotGame

A robot battle simulation game built with C++ and SDL2.

## Prerequisites

```bash
sudo apt install libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev libsdl2-gfx-dev libsdl2-2.0-0 -y
```

## Building

```bash
mkdir build && cd build
cmake ..
make
```

## Running

```bash
./robot-arena [options] robot1.so [robot2.so ...]
```

### Options

- `-d, --delay <ms>`: Battle delay in milliseconds (1-1000, default: 16)
- `--step`: Enable step mode (pause after each tick, press Enter to advance)
- `-h, --help`: Show help message

### Step Mode

When `--step` is specified, the game pauses after each game tick and waits for the user to press Enter in the console to advance to the next tick. This is useful for debugging robot behavior and observing game state changes frame-by-frame.

Example:
```bash
./robot-arena --step ./Robots/Scanner/libscanner.so ./Robots/Runner/librunner.so
```

In step mode, you'll see prompts like:
```
[Step Mode] Tick 1 - Press Enter to advance to next tick:
```

Press Enter to advance to the next tick.

**Note**: Step mode is intended for development and debugging. It does not affect game logic - robots behave identically in step mode and normal mode.
