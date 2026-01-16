#include "ShellPool.h"
#include <algorithm>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

namespace RobotGame {

ShellPool::ShellPool() : nextShellId(1) {
    freeIds.reserve(MAX_SHELLS);
    activeShells.reserve(MAX_SHELLS);
    
    for (uint32_t i = 0; i < MAX_SHELLS; ++i) {
        freeIds.push_back(i);
    }
}

uint32_t ShellPool::createShell(const Vector2& position, const Vector2& velocity, uint32_t currentTime) {
    if (freeIds.empty()) {
        return 0;
    }
    
    uint32_t poolIndex = freeIds.back();
    freeIds.pop_back();
    
    AnimatedShell& shell = shells[poolIndex];
    shell.shellId = nextShellId++;
    shell.position = position;
    shell.velocity = velocity;
    shell.active = true;
    shell.creationTime = currentTime;
    shell.trailIndex = 0;
    shell.trailCount = 0;
    shell.trail.fill(TrailPoint());
    
    shell.addTrailPoint(position, currentTime);
    
    addToActiveList(&shell);
    
    return shell.shellId;
}

bool ShellPool::destroyShell(uint32_t shellId) {
    AnimatedShell* shell = getShell(shellId);
    if (!shell || !shell->active) {
        return false;
    }
    
    uint32_t poolIndex = shell - &shells[0];
    
    shell->active = false;
    removeFromActiveList(shell);
    
    freeIds.push_back(poolIndex);
    
    return true;
}

AnimatedShell* ShellPool::getShell(uint32_t shellId) {
    for (auto* shell : activeShells) {
        if (shell->shellId == shellId) {
            return shell;
        }
    }
    return nullptr;
}

void ShellPool::updateShells(uint32_t currentTime) {
    std::vector<AnimatedShell*> toDestroy;
    
    for (auto* shell : activeShells) {
        if (!shell->active) continue;
        
        shell->position = shell->position + shell->velocity;
        
        if (!shell->isPositionValid()) {
            toDestroy.push_back(shell);
            continue;
        }
        
        shell->addTrailPoint(shell->position, currentTime);
        shell->updateTrailAlphas(currentTime);
    }
    
    for (auto* shell : toDestroy) {
        destroyShell(shell->shellId);
    }
}

void ShellPool::clearInactiveShells() {
    activeShells.erase(
        std::remove_if(activeShells.begin(), activeShells.end(),
            [](AnimatedShell* shell) { return !shell->active; }),
        activeShells.end()
    );
}

uint32_t ShellPool::getNextFreeId() {
    if (freeIds.empty()) {
        return MAX_SHELLS;
    }
    return freeIds.back();
}

bool ShellPool::isIdInUse(uint32_t shellId) const {
    for (auto* shell : activeShells) {
        if (shell->shellId == shellId) {
            return true;
        }
    }
    return false;
}

void ShellPool::addToActiveList(AnimatedShell* shell) {
    if (std::find(activeShells.begin(), activeShells.end(), shell) == activeShells.end()) {
        activeShells.push_back(shell);
    }
}

void ShellPool::removeFromActiveList(AnimatedShell* shell) {
    auto it = std::find(activeShells.begin(), activeShells.end(), shell);
    if (it != activeShells.end()) {
        activeShells.erase(it);
    }
}

void ShellPool::renderShells(SDL_Renderer* renderer, float scale) const {
    for (const auto* shell : activeShells) {
        if (!shell->active || shell->trailCount < 2) continue;
        
        for (uint32_t i = 1; i < shell->trailCount; ++i) {
            const TrailPoint& prevPoint = shell->trail[(shell->trailIndex - i + 20) % 20];
            const TrailPoint& currPoint = shell->trail[(shell->trailIndex - i - 1 + 20) % 20];
            
            if (prevPoint.alpha <= 0.0f || currPoint.alpha <= 0.0f) continue;
            
            int x1 = static_cast<int>(prevPoint.position.x * scale);
            int y1 = static_cast<int>(prevPoint.position.y * scale);
            int x2 = static_cast<int>(currPoint.position.x * scale);
            int y2 = static_cast<int>(currPoint.position.y * scale);
            
            uint8_t alpha = static_cast<uint8_t>(255.0f * std::min(prevPoint.alpha, currPoint.alpha));
            uint32_t color = (0xFF << 24) | (alpha << 16) | (alpha << 8) | alpha;
            
            lineColor(renderer, x1, y1, x2, y2, color);
        }
        
        int shellX = static_cast<int>(shell->position.x * scale);
        int shellY = static_cast<int>(shell->position.y * scale);
        filledCircleColor(renderer, shellX, shellY, 3, 0xFFFFFFFF);
    }
}

} // namespace RobotGame