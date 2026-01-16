#pragma once

#include "Vector2.h"
#include <cstdint>
#include <array>
#include <vector>
#include <SDL2/SDL.h>

namespace RobotGame {

struct TrailPoint {
    Vector2 position;
    uint32_t timestamp;
    float alpha;
    
    TrailPoint() : position(0, 0), timestamp(0), alpha(1.0f) {}
    TrailPoint(const Vector2& pos, uint32_t time, float a = 1.0f) 
        : position(pos), timestamp(time), alpha(a) {}
};

struct AnimatedShell {
    uint32_t shellId;
    Vector2 position;
    Vector2 velocity;
    bool active;
    uint32_t creationTime;
    std::array<TrailPoint, 20> trail;
    uint32_t trailIndex;
    uint32_t trailCount;
    
    AnimatedShell() : shellId(0), position(0, 0), velocity(0, 0), 
                      active(false), creationTime(0), trailIndex(0), trailCount(0) {
        trail.fill(TrailPoint());
    }
    
    void addTrailPoint(const Vector2& pos, uint32_t currentTime) {
        if (trailCount < 20) {
            trail[trailIndex] = TrailPoint(pos, currentTime, 1.0f);
            trailCount++;
        } else {
            trail[trailIndex] = TrailPoint(pos, currentTime, 1.0f);
        }
        trailIndex = (trailIndex + 1) % 20;
    }
    
    void updateTrailAlphas(uint32_t currentTime) {
        const uint32_t trailLifetime = 500;
        for (uint32_t i = 0; i < trailCount && i < 20; ++i) {
            uint32_t age = currentTime - trail[i].timestamp;
            if (age >= trailLifetime) {
                trail[i].alpha = 0.0f;
            } else {
                trail[i].alpha = 1.0f - (static_cast<float>(age) / trailLifetime);
            }
        }
    }
    
    bool isPositionValid() const {
        return position.x >= 0.0f && position.x <= 1000.0f &&
               position.y >= 0.0f && position.y <= 1000.0f;
    }
};

class ShellPool {
public:
    static constexpr uint32_t MAX_SHELLS = 100;
    static constexpr uint32_t MAX_TRAIL_LENGTH = 20;
    static constexpr uint32_t TRAIL_POINT_LIFETIME = 500;
    static constexpr float MIN_POSITION = 0.0f;
    static constexpr float MAX_POSITION = 1000.0f;
    
    ShellPool();
    
    uint32_t createShell(const Vector2& position, const Vector2& velocity, uint32_t currentTime);
    bool destroyShell(uint32_t shellId);
    AnimatedShell* getShell(uint32_t shellId);
    void updateShells(uint32_t currentTime);
    void clearInactiveShells();
    void renderShells(SDL_Renderer* renderer, float scale = 1.0f) const;
    
    const std::vector<AnimatedShell*>& getActiveShells() const { return activeShells; }
    uint32_t getActiveShellCount() const { return static_cast<uint32_t>(activeShells.size()); }
    uint32_t getTotalShellCount() const { return MAX_SHELLS; }
    
private:
    std::array<AnimatedShell, MAX_SHELLS> shells;
    std::vector<AnimatedShell*> activeShells;
    std::vector<uint32_t> freeIds;
    uint32_t nextShellId;
    
    uint32_t getNextFreeId();
    bool isIdInUse(uint32_t shellId) const;
    void addToActiveList(AnimatedShell* shell);
    void removeFromActiveList(AnimatedShell* shell);
};

} // namespace RobotGame