# Graphics Fixes Implementation Quickstart

## Overview

This feature fixes four critical graphical issues in RobotGame:
1. **VSync** - Eliminates screen tearing
2. **Configurable Timing** - User-controllable battle speed  
3. **Shell Motion Continuity** - Smooth projectile trails
4. **Explosion Layering** - Proper blast effect rendering

## Files to Modify

### Core Implementation Files

| File | Purpose | Key Changes |
|------|---------|-------------|
| `RobotGame.cpp` | Command-line parsing | Add `--delay` option processing |
| `Scheduler.cpp` | Battle timing control | Add configurable delay in `tickEnd()` |
| `Scheduler.h` | Scheduler interface | Add timing configuration methods |
| `Viewer.cpp` | Rendering engine | Complete renderer overhaul with layers |
| `Viewer.h` | Viewer interface | Add new rendering system declarations |

### New Supporting Files

| File | Purpose |
|------|---------|
| `ShellPool.h/.cpp` | Object pool for animated shells |
| `Explosion.h/.cpp` | Animated explosion system |
| `Vector2.h` | Simple 2D vector math utility |

## Implementation Steps

### Step 1: Command-Line Arguments (RobotGame.cpp)

**Before**:
```cpp
int c;
while ( ( c = ::getopt(argc, argv, "" ) ) != -1 ) {
    // Empty argument processing
}
```

**After**:
```cpp
int battleDelayMs = 16; // Default ~60 FPS

for (int i = 1; i < argc; i++) {
    std::string arg = argv[i];
    
    if (arg == "--help" || arg == "-h") {
        std::cout << "Usage: " << argv[0] << " [options] robot1.so [robot2.so ...]\n"
                  << "Options:\n"
                  << "  -d, --delay <ms>    Battle delay (1-1000ms, default: 16)\n"
                  << "  -h, --help          Show this help\n";
        return 0;
    }
    else if ((arg == "--delay" || arg == "-d") && i + 1 < argc) {
        try {
            battleDelayMs = std::stoi(argv[++i]);
            if (battleDelayMs < 1 || battleDelayMs > 1000) {
                std::cerr << "Error: Delay must be between 1 and 1000ms\n";
                return 1;
            }
        } catch (...) {
            std::cerr << "Error: Invalid delay value\n";
            return 1;
        }
    }
}

// Pass to scheduler after creation
Scheduler& scheduler = Scheduler::getScheduler();
scheduler.setBattleDelay(battleDelayMs);
```

### Step 2: Scheduler Timing (Scheduler.h/.cpp)

**Add to Scheduler.h**:
```cpp
private:
    int battleDelayMs = 16; // Default ~60 FPS

public:
    void setBattleDelay(int delayMs);
    int getBattleDelay() const;
```

**Add to Scheduler.cpp**:
```cpp
void Scheduler::setBattleDelay(int delayMs) {
    if (delayMs >= 1 && delayMs <= 1000) {
        battleDelayMs = delayMs;
    } else {
        Logger::LogError("Invalid battle delay, using default");
        battleDelayMs = 16;
    }
}

int Scheduler::getBattleDelay() const {
    return battleDelayMs;
}

// In tickEnd() method, add before Viewer::tick():
if (battleDelayMs > 0) {
    std::this_thread::sleep_for(std::chrono::milliseconds(battleDelayMs));
}
```

### Step 3: VSync Renderer (Viewer.cpp)

**Replace renderer creation**:
```cpp
// In Viewer constructor:
gRenderer = createRendererWithVSyncFallback(gWindow);

// Add new private method:
SDL_Renderer* Viewer::createRendererWithVSyncFallback(SDL_Window* window) {
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    if (!renderer) {
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        Logger::LogWarning("VSync not available, using accelerated renderer");
    }
    
    if (!renderer) {
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
        Logger::LogWarning("Hardware acceleration failed, using software renderer");
    }
    
    return renderer;
}
```

### Step 4: Shell Animation System

**Create Vector2.h**:
```cpp
struct Vector2 {
    float x, y;
    
    Vector2() : x(0), y(0) {}
    Vector2(float x, float y) : x(x), y(y) {}
    
    Vector2 operator+(const Vector2& other) const { return Vector2(x + other.x, y + other.y); }
    Vector2 operator-(const Vector2& other) const { return Vector2(x - other.x, y - other.y); }
    Vector2 operator*(float scalar) const { return Vector2(x * scalar, y * scalar); }
};
```

**Create ShellPool.h**:
```cpp
#pragma once
#include "Vector2.h"
#include <vector>
#include <cstdint>

class ShellPool {
public:
    struct TrailPoint {
        Vector2 position;
        uint32_t timestamp;
        float alpha;
    };
    
    struct AnimatedShell {
        Vector2 position;
        std::vector<TrailPoint> trail;
        uint32_t shellId;
        bool active;
    };
    
    uint32_t createShell(const Vector2& pos, const Vector2& velocity);
    void updateAll();
    void renderShells(SDL_Renderer* renderer);
    void destroyShell(uint32_t shellId);
    
private:
    static constexpr int MAX_SHELLS = 100;
    std::vector<AnimatedShell> pool;
    uint32_t nextId = 1;
};
```

### Step 5: Explosion System

**Create Explosion.h**:
```cpp
#pragma once
#include "Vector2.h"
#include <vector>

struct Explosion {
    Vector2 position;
    float currentRadius, maxRadius;
    uint32_t startTime;
    bool active;
    
    struct Particle {
        Vector2 position, velocity;
        float life;
    };
    
    std::vector<Particle> particles;
};

class ExplosionSystem {
public:
    void createExplosion(int x, int y);
    void updateAll();
    void renderExplosions(SDL_Renderer* renderer);
    
private:
    std::vector<Explosion> explosions;
};
```

### Step 6: Integration in Viewer

**Add to Viewer.h private members**:
```cpp
ShellPool shellPool;
ExplosionSystem explosionSystem;
SDL_Texture* terrainLayer;
SDL_Texture* robotLayer;
SDL_Texture* explosionLayer;
```

**Modify Viewer.cpp _CannonShow()**:
```cpp
void Viewer::_CannonShow(struct ShellPos shell) {
    Vector2 start(shell.x1, shell.y1);
    Vector2 end(shell.x2, shell.y2);
    Vector2 velocity = (end - start) * 0.1f; // Scale velocity
    
    uint32_t shellId = shellPool.createShell(start, velocity);
    
    if (shell.blasted) {
        explosionSystem.createExplosion(shell.x2, shell.y2);
    }
}
```

**Replace shells.clear() in Runner()**:
```cpp
// OLD: shells.clear();
// NEW: Update animation systems
shellPool.updateAll();
explosionSystem.updateAll();

// Replace PrintShell() calls with:
shellPool.renderShells(gRenderer);
explosionSystem.renderExplosions(gRenderer);
```

## Testing Steps

### 1. Build and Test VSync
```bash
cmake --build build
./build/RobotGame --help  # Should show usage
./build/RobotGame Scanner.so Runner.so  # Should run without tearing
```

### 2. Test Timing Control
```bash
# Default speed
./build/RobotGame Scanner.so Runner.so

# Slow motion
./build/RobotGame --delay 100 Scanner.so Runner.so

# Very fast
./build/RobotGame -d 5 Scanner.so Runner.so
```

### 3. Verify Visual Improvements
- Shells should show smooth trails
- Explosions should animate with particles
- No screen tearing during rapid movement
- Multiple explosions should layer correctly

## Performance Targets

- **Memory**: <20MB additional VRAM for layers
- **CPU**: <5% overhead for animation systems  
- **Frame Rate**: Stable 30-60 FPS based on delay setting
- **Visual Quality**: Smooth trails, layered explosions, no tearing

## Troubleshooting

### VSync Issues
If screen tearing persists:
1. Check SDL_RENDERER_PRESENTVSYNC support
2. Verify monitor refresh rate
3. Try different SDL drivers (SDL_VIDEODRIVER environment variable)

### Performance Problems
If frame rate drops:
1. Reduce trail length (MAX_TRAIL_POINTS)
2. Lower particle count per explosion
3. Disable layer system as fallback

### Memory Issues
If memory usage is high:
1. Reduce pool sizes (MAX_SHELLS, MAX_EXPLOSIONS)
2. Lower texture resolution for layers
3. Implement more aggressive cleanup

## Backward Compatibility

All changes are backward compatible:
- Existing robot plugins work unchanged
- Default behavior preserves current timing
- No changes to public APIs
- Gradual deployment possible