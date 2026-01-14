# Viewer API Contract

## Class: Viewer

### Enhanced Renderer Creation

#### Constructor Modifications
VSync-enabled renderer creation with graceful fallback.

**New Behavior**:
```cpp
Viewer::Viewer() : goDie(false) {
    // ... existing SDL initialization ...
    
    // ENHANCED: VSync-enabled renderer with fallback
    gRenderer = createRendererWithVSyncFallback(gWindow);
    if (!gRenderer) {
        Logger::LogError("Failed to create any renderer");
        goDie = true;
        return;
    }
    
    // ... existing font initialization ...
}
```

**Private Helper Method**:
```cpp
SDL_Renderer* Viewer::createRendererWithVSyncFallback(SDL_Window* window) {
    // Try VSync + accelerated
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    if (!renderer) {
        // Fallback: accelerated without VSync
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    }
    
    if (!renderer) {
        // Final fallback: software renderer
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    }
    
    return renderer;
}
```

### Shell Animation System

#### New Data Members

```cpp
class Viewer {
private:
    // NEW: Shell animation pool
    ShellPool shellPool;
    
    // NEW: Explosion system
    std::vector<Explosion> explosions;
    
    // NEW: Render layers
    SDL_Texture* terrainLayer;
    SDL_Texture* robotLayer;  
    SDL_Texture* explosionLayer;
    
    // Existing members preserved...
};
```

#### Modified Methods

##### `Runner()` - Enhanced Render Pipeline
```cpp
void Viewer::Runner() {
    // NEW: Initialize render layers
    initializeRenderLayers();
    
    while(!goDie) {
        eventProcess.lock();
        
        // Process events (existing logic)
        while (!evQueue.empty()) {
            RobEvent* ev = evQueue.dequeue();
            ev->execute();
            delete ev;
        }
        
        // NEW: Update animation systems
        shellPool.updateAll();
        updateExplosions();
        
        // NEW: Render with layers
        renderFrameWithLayers();
        
        // ... existing SDL event handling ...
        Scheduler::end();
    }
}
```

##### `_CannonShow()` - Event Processing
```cpp
void Viewer::_CannonShow(struct ShellPos shell) {
    // REPLACED: shells.push_back(shell);
    // NEW: Create animated shell
    Vector2 start(shell.x1, shell.y1);
    Vector2 end(shell.x2, shell.y2);
    Vector2 velocity = (end - start).normalized() * SHELL_SPEED;
    
    uint32_t shellId = shellPool.createShell(start, velocity);
    
    if (shell.blasted) {
        createExplosion(end.x, end.y);
    }
}
```

##### `PrintShell()` - Removed
**Status**: **DEPRECATED** - Replaced by shell pool rendering system

### Explosion System

#### New Methods

##### `createExplosion(int x, int y)`
Creates animated explosion at specified position.

**Parameters**:
- `x` (int): Explosion X coordinate  
- `y` (int): Explosion Y coordinate

**Behavior**:
```cpp
void Viewer::createExplosion(int x, int y) {
    Explosion exp;
    exp.position = Vector2(x, y);
    exp.currentRadius = 0;
    exp.maxRadius = 60;
    exp.startTime = SDL_GetTicks();
    exp.active = true;
    
    // Create debris particles
    for (int i = 0; i < 20; i++) {
        Particle p;
        float angle = (M_PI * 2 * i) / 20;
        float speed = 2.0f + (rand() % 3);
        p.velocity = Vector2(cos(angle) * speed, sin(angle) * speed);
        p.position = Vector2(x, y);
        p.life = 1.0f;
        exp.particles.push_back(p);
    }
    
    explosions.push_back(exp);
}
```

### Render Layer System

#### New Methods

##### `initializeRenderLayers()`
Creates texture targets for layered rendering.

**Behavior**:
```cpp
void Viewer::initializeRenderLayers() {
    int screenWidth = ARENA_WIDTH + STATUS_WIDTH;
    int screenHeight = std::max(ARENA_HEIGHT, STATUS_HEIGHT);
    
    terrainLayer = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET, screenWidth, screenHeight);
    robotLayer = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET, screenWidth, screenHeight);
    explosionLayer = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET, screenWidth, screenHeight);
        
    // Set blend modes for transparency
    SDL_SetTextureBlendMode(robotLayer, SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(explosionLayer, SDL_BLENDMODE_BLEND);
}
```

##### `renderFrameWithLayers()`
Composites layered textures in correct order.

**Behavior**:
```cpp
void Viewer::renderFrameWithLayers() {
    // Clear all layers
    clearAllLayers();
    
    // Render to each layer
    renderTerrainLayer();
    renderRobotLayer();
    renderExplosionLayer();
    
    // Composite to screen
    SDL_SetRenderTarget(gRenderer, NULL);
    SDL_RenderClear(gRenderer);
    
    SDL_RenderCopy(gRenderer, terrainLayer, NULL, NULL);
    SDL_RenderCopy(gRenderer, robotLayer, NULL, NULL);
    SDL_RenderCopy(gRenderer, explosionLayer, NULL, NULL);
    
    SDL_RenderPresent(gRenderer);
}
```

## Performance Specifications

### Memory Usage
- **Shell Pool**: 100 shells × 20 trail points = ~3.2KB
- **Explosion System**: 50 explosions × 20 particles = ~4.8KB  
- **Render Layers**: 3 × 1400×1000 × 4 bytes = ~16.8MB VRAM

### Update Frequency
- Shell trail updates: Every frame (target 60 FPS)
- Explosion animations: Every frame
- Layer compositing: Every frame
- Layer clearing: Every frame

### Rendering Order
1. **Terrain Layer** (bottom): Arena background, static elements
2. **Robot Layer** (middle): Robots, status panel, shell trails  
3. **Explosion Layer** (top): Blast effects, particles

## Error Handling

### Resource Management
- **Layer Creation Failure**: Fallback to immediate rendering mode
- **Texture Allocation Failure**: Reduce layer resolution or disable
- **Memory Pool Exhaustion**: Reuse oldest resources, log warning

### Runtime Errors
- **VSync Unavailable**: Continue with frame rate capping
- **SDL Context Loss**: Recreate all layers and pools
- **Performance Degradation**: Dynamically reduce effect quality

## Backward Compatibility

### Preserved Behavior
- All existing public method signatures unchanged
- Robot plugin interface unchanged
- Event processing architecture preserved
- Status panel rendering unchanged

### Migration Notes
- Existing `_CannonShow()` calls now create animated shells
- `PrintShell()` method removed but no external dependencies
- Render loop enhanced but maintains same frame timing
- VSync enhancement is transparent to existing code