# Research Findings: Graphics Fixes Implementation

**Date**: 2026-01-13  
**Feature**: Graphics Fixes (001-graphics-fixes)  
**Status**: Complete

## SDL2 VSync Implementation

### Decision
Enable VSync using `SDL_RENDERER_PRESENTVSYNC` flag with graceful fallback to software renderer if hardware acceleration fails.

### Rationale
- Eliminates screen tearing by synchronizing with monitor refresh rate
- Provides consistent frame timing for smooth animation
- Hardware support is widespread but fallback ensures compatibility
- Minimal performance overhead compared to visual improvement

### Implementation Strategy
```cpp
SDL_Renderer* createRendererWithVSyncFallback(SDL_Window* window) {
    // Try accelerated with VSync first
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    if (!renderer) {
        // Try accelerated without VSync
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer) {
            // Final fallback to software
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
        }
    }
    return renderer;
}
```

### Alternatives Considered
- Manual frame rate capping: Less reliable, still can have tearing
- Triple buffering: More complex, VSync is simpler for this use case

## Command-Line Argument Parsing

### Decision
Implement lightweight argument parsing without external dependencies using direct string processing.

### Rationale
- Zero external dependencies maintains simple build system
- Minimal code addition for just 2-3 options
- Clear error messages without complexity
- Fits existing codebase style

### Implementation Strategy
```cpp
int main(int argc, char** argv) {
    int battleDelayMs = 16; // Default ~60 FPS
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            // Show usage and return
        }
        else if ((arg == "--delay" || arg == "-d") && i + 1 < argc) {
            // Parse and validate delay (1-1000ms range)
        }
    }
    // Pass delay to scheduler
}
```

### Alternatives Considered
- cxxopts library: Feature-rich but adds external dependency
- getopt enhancement: Complex for mixed-style arguments
- CLI11: Overkill for simple use case

## Shell Animation State Management

### Decision
Implement object pooling with trail history buffer to maintain shell motion continuity.

### Rationale
- Eliminates `shells.clear()` that destroys motion trails
- Object pooling prevents memory allocation overhead
- Trail buffer provides smooth visual continuity
- Scalable to handle many simultaneous shells

### Implementation Strategy
```cpp
class AnimatedShell {
    Vector2 position;
    std::vector<TrailPoint> trail; // History buffer
    uint32_t shellId;
    bool active;
    
    void update() {
        trail.push_back({position, SDL_GetTicks()});
        // Limit trail length and age out old points
    }
};

class ShellPool {
    std::vector<AnimatedShell> pool; // Fixed-size pool
    std::queue<size_t> availableIndices;
    std::unordered_map<uint32_t, size_t> activeShells;
};
```

### Alternatives Considered
- Ring buffer approach: Simpler but less flexible for complex animations
- naive vector persistence: Memory inefficient with constant growth
- State machine per shell: Overkill for simple trail rendering

## Blast Rendering and Layering

### Decision
Implement texture-based layering system with animated explosion effects.

### Rationale
- Fixes overlap issues by proper render order
- Animated explosions improve visual quality
- Layer system provides clean separation of concerns
- Particle system adds professional polish

### Implementation Strategy
```cpp
// Layer textures for proper compositing
SDL_Texture* terrainLayer, *robotLayer, *explosionLayer;

// Explosion with particles
struct Explosion {
    Vector2 position;
    float radius, maxRadius;
    std::vector<Particle> particles;
    Uint32 startTime;
    bool active;
};

void renderFrame() {
    // Clear and render each layer
    renderToLayer(terrainLayer);
    renderToLayer(robotLayer);
    renderToLayer(explosionLayer);
    
    // Composite in correct order
    SDL_RenderCopy(renderer, terrainLayer, NULL, NULL);
    SDL_RenderCopy(renderer, robotLayer, NULL, NULL);
    SDL_RenderCopy(renderer, explosionLayer, NULL, NULL);
}
```

### Alternatives Considered
- Z-order rendering: Complex to maintain with mixed primitive types
- Immediate mode rendering: No persistent state for animations
- Single texture with draw order: Difficult to manage overlapping effects

## Testing Strategy

### Decision
Implement unit testing using existing C++ testing framework (to be identified).

### Rationale
- Ensures VSync fallback logic works correctly
- Validates command-line parsing edge cases
- Tests shell animation state lifecycle
- Verifies explosion layering and cleanup

### Testing Approach
- Mock SDL2 renderer for testing VSync creation paths
- Parameterized tests for delay validation ranges
- Visual regression tests for shell trail continuity
- Performance tests for multiple simultaneous explosions

## Integration Points

### Scheduler Integration
- Add configurable delay parameter to Scheduler class
- Modify tickEnd() to use configurable timing
- Ensure thread safety for timing configuration

### Viewer Integration  
- Replace shells.clear() with shellPool.updateAll()
- Add layer texture creation and management
- Implement explosion animation system
- Update render pipeline for proper layering

### Build System
- No new dependencies required
- Maintain CMake configuration as-is
- Ensure header-only additions work with existing build

## Performance Considerations

### Memory Usage
- Object pool: Fixed ~100 shells * ~20 trail points = 2000 positions
- Layer textures: 3 * 1000x1000 * 4 bytes = 12MB video memory
- Explosion particles: 20 particles per explosion, max 50 explosions

### CPU Impact
- Shell trail updates: O(n) where n = active shells
- Explosion animations: O(m*p) where m = explosions, p = particles per explosion
- Layer compositing: 3 texture copies per frame (negligible)

### Expected Results
- Smooth 30-60 FPS rendering
- No screen tearing with VSync
- Continuous shell motion trails
- Professional explosion effects

## Risk Assessment

### Low Risk
- VSync implementation: Standard SDL2 feature
- Command-line parsing: Simple string processing
- Object pooling: Well-understood pattern

### Medium Risk  
- Layer system integration: Requires careful render state management
- Shell state synchronization: Must match existing event system

### Mitigation Strategies
- Incremental implementation with fallback options
- Thorough testing of layer creation and cleanup
- Preserve existing event system architecture