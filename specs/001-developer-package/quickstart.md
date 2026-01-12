# Developer Kit Quick Start

## Overview

This quick start guide helps you get up and running with robot development in under 30 minutes. The developer kit contains everything you need to build, test, and deploy robot behaviors for the RobotGame system.

## Package Contents

```
robot-dev-kit/
├── README.md              # This file
├── Robot.h               # Robot API header
├── MyRobot.cpp           # Robot template (your starting point)
├── ROBOT_RULES.md        # Robot development rules
├── bin/
│   └── robot-arena      # Main game executable
├── lib/
│   ├── librunner.so     # Runner robot library
│   └── libscanner.so    # Scanner robot library
└── robots/
    ├── Runner/
    │   ├── Runner.cpp
    │   ├── Runner.h
    │   └── CMakeLists.txt
    └── Scanner/
        ├── Scanner.cpp
        ├── Scanner.h
        └── CMakeLists.txt
```

## 30-Minute Quick Start

### Step 1: Extract and Verify (5 minutes)

1. Extract the package anywhere:
   ```bash
   tar -xzf robot-dev-kit.tar.gz
   cd robot-dev-kit
   ```

2. Verify the contents:
   ```bash
   ls -la
   # Should see README.md, Robot.h, MyRobot.cpp, ROBOT_RULES.md, bin/, lib/, robots/
   ```

3. Test the main executable:
   ```bash
   ./bin/robot-arena --help
   # Should show usage information
   ```

### Step 2: Build Your First Robot (10 minutes)

1. Copy the template:
   ```bash
   cp MyRobot.cpp MyFirstRobot.cpp
   ```

2. Edit `MyFirstRobot.cpp` with your favorite text editor:
   ```cpp
   #include "Robot.h"
   
   class MyFirstRobot : public Robot {
   public:
       void init() override {
           // Your initialization code here
           setName("MyFirstRobot");
       }
       
       void run() override {
           // Your robot behavior here
           // For now, let's just scan for enemies
           scan();
       }
   };
   
   // Factory function (required)
   extern "C" Robot* getRobot() {
       return new MyFirstRobot();
   }
   ```

3. Create a build directory:
   ```bash
   mkdir build
   cd build
   ```

4. Configure with CMake:
   ```bash
   cmake .. -DCMAKE_PREFIX_PATH=..
   ```

5. Build your robot:
   ```bash
   cmake --build .
   ```

### Step 3: Test Your Robot (10 minutes)

1. Run the game with sample robots:
   ```bash
   cd ..  # Back to robot-dev-kit root
   ./bin/robot-arena --robots lib/librunner.so lib/libscanner.so
   ```

2. Run with your robot (once built):
   ```bash
   ./bin/robot-arena --robots lib/librunner.so build/libMyFirstRobot.so
   ```

3. Observe the behavior and iterate on your robot logic.

### Step 4: Learn from Examples (5 minutes)

1. Study the Runner robot:
   ```bash
   cat robots/Runner/Runner.cpp
   ```

2. Study the Scanner robot:
   ```bash
   cat robots/Scanner/Scanner.cpp
   ```

3. Read the Robot API:
   ```bash
   cat Robot.h
   ```

## Building the Example Robots

### Runner Robot

```bash
cd robots/Runner
mkdir build
cd build
cmake .. -DCMAKE_PREFIX_PATH=../..
cmake --build .
```

### Scanner Robot

```bash
cd ../../Scanner
mkdir build
cd build
cmake .. -DCMAKE_PREFIX_PATH=../../..
cmake --build .
```

## Robot Development Workflow

1. **Copy Template**: Start from `MyRobot.cpp`
2. **Implement Logic**: Add your robot behavior in the `run()` method
3. **Build**: Use CMake to compile your robot as a shared library
4. **Test**: Run with `robot-arena` to test behavior
5. **Iterate**: Refine your robot logic based on observations

## Common Build Commands

```bash
# Build in parallel (faster)
cmake --build . --parallel $(nproc)

# Clean build
rm -rf build
mkdir build
cd build
cmake .. -DCMAKE_PREFIX_PATH=..
cmake --build .

# Debug build
cmake .. -DCMAKE_PREFIX_PATH=.. -DCMAKE_BUILD_TYPE=Debug
cmake --build .
```

## Troubleshooting

### Build Errors

**Problem**: CMake cannot find Robot.h
**Solution**: Ensure you're using `-DCMAKE_PREFIX_PATH=..` from your build directory

**Problem**: Linker errors
**Solution**: Verify you're linking against the base Robot class

### Runtime Errors

**Problem**: robot-arena cannot find your robot library
**Solution**: Ensure your library is in a location accessible to robot-arena

**Problem**: Robot doesn't appear in game
**Solution**: Verify your `getRobot()` function returns a valid Robot instance

## Next Steps

1. Read `ROBOT_RULES.md` for detailed development guidelines
2. Study the `Robot.h` header for complete API reference
3. Experiment with the example robots
4. Join the community forum for questions and discussions

## Need Help?

- Check `ROBOT_RULES.md` for detailed guidelines
- Review the example robots for patterns
- Examine the Robot.h header for API documentation
- Test with different robot combinations to understand interactions