# Research Document: Robot Game Bug Fixes and Arena Border

**Feature**: Fix Robot Game Bugs and Add Arena Border  
**Date**: 2026-01-14  
**Research for**: Implementation plan Phase 0

## Technical Decisions

### Robot Death Mechanism Fix

**Decision**: The armor death mechanism exists but may have logic errors in damage calculation or armor reduction.

**Research Findings**:
- Death logic is implemented in `RobCtrl.cpp` and `Scheduler.cpp`
- Robots are removed when `getArmor() == 0` 
- Damage calculation uses distance-based system (8, 4, 2, 1 damage based on range)
- Energy refill only occurs if `currArmor > 0`

**Potential Issue Locations**:
1. `RobCtrl::shotBlasted()` - damage application logic
2. `RobCtrl::tick()` - armor/energy update cycle
3. Damage calculation may not properly reduce armor to 0
4. Float/integer conversion issues in damage calculation

### Rendering Error Fixes

**Decision**: Rendering errors are caused by texture creation failures or invalid texture references.

**Research Findings**:
- Errors occur in `PrintRobotStatus()` method at lines 421, 429, 437
- Uses SDL2_ttf for text rendering with texture caching
- Textures stored in `RobotDataEvent` array
- Error handling exists but textures may be null/invalid

**Root Causes**:
1. Font loading issues (`FreeSansBold.ttf` path may be incorrect)
2. Texture creation failures not properly handled
3. Texture cleanup/reuse problems
4. Invalid texture coordinates or dimensions

### Arena Border Addition

**Decision**: Add border rendering using SDL2_gfx primitives in the main rendering pipeline.

**Research Findings**:
- Arena viewport: 1000x1000 pixels, offset by 400px from left (status area)
- Rendering pipeline in `renderFrameWithLayers()` method
- SDL2_gfx library available for drawing primitives
- Border should be drawn after arena clearing but before robot rendering

**Implementation Approach**:
1. Use `rectangleColor()` or `boxColor()` from SDL2_gfx
2. Draw border around arena viewport coordinates (1000x1000 area starting at STATUS_WIDTH, 0)
3. Use dark blue color for visibility (RGB: 0, 0, 139, 255)
4. Ensure border doesn't interfere with robot movement

## Technology Stack Confirmation

**Language/Version**: C++17  
**Primary Dependencies**: SDL2, SDL2_ttf, SDL2_gfx, SDL2_image  
**Storage**: In-memory state (no persistent storage)  
**Testing**: Manual testing + logger output (no formal framework)  
**Target Platform**: Linux  
**Project Type**: Single C++ application with modular robot plugins  
**Performance Goals**: 60 FPS real-time gameplay  
**Constraints**: <16ms frame time, 1000x1000 pixel arena  
**Scale/Scope**: Single game instance with multiple robot plugins

## Architecture Considerations

**Modular Design**: 
- Core engine (`robot-engine`) handles game logic
- Rendering (`Viewer.cpp`) handles display
- Robot plugins loaded dynamically
- Clear separation of concerns

**Error Handling**:
- Logger system for debugging
- SDL2 error checking present
- Need to improve texture validation

## Implementation Strategy

1. **Phase 0 (Research)**: ✅ Complete - identify specific bug locations
2. **Phase 1 (Design)**: Create detailed fix plans for each issue
3. **Phase 2 (Implementation)**: Apply fixes with testing

## Risks and Mitigations

**Risk 1**: Armor calculation may involve floating-point precision
- **Mitigation**: Review all armor arithmetic operations

**Risk 2**: Font path may be system-dependent
- **Mitigation**: Add font loading validation

**Risk 3**: Border rendering may impact performance
- **Mitigation**: Profile border drawing, optimize if needed

## Dependencies

**External**: SDL2 libraries (already available)
**Internal**: 
- `RobCtrl.cpp/h` - robot logic
- `Viewer.cpp/h` - rendering system
- `Scheduler.cpp` - game loop management
- `RobotDataEvent.cpp` - texture management

All research complete, ready for Phase 1 design.