#include "Explosion.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <cstdlib>
#include <algorithm>

static Explosion explosions[ExplosionConstants::MAX_SIMULTANEOUS_EXPLOSIONS];
static uint32_t nextExplosionId = 1;

uint32_t createExplosion(const Vector2& position) {
    uint32_t currentTime = SDL_GetTicks();
    
    // Find available slot
    for (uint32_t i = 0; i < ExplosionConstants::MAX_SIMULTANEOUS_EXPLOSIONS; ++i) {
        if (!explosions[i].active) {
            explosions[i] = Explosion(nextExplosionId++, position, currentTime);
            return explosions[i].explosionId;
        }
    }
    
    // No available slots, replace oldest active explosion
    uint32_t oldestTime = currentTime;
    uint32_t oldestIndex = 0;
    for (uint32_t i = 0; i < ExplosionConstants::MAX_SIMULTANEOUS_EXPLOSIONS; ++i) {
        if (explosions[i].active && explosions[i].startTime < oldestTime) {
            oldestTime = explosions[i].startTime;
            oldestIndex = i;
        }
    }
    
    explosions[oldestIndex] = Explosion(nextExplosionId++, position, currentTime);
    return explosions[oldestIndex].explosionId;
}

void updateExplosions() {
    uint32_t currentTime = SDL_GetTicks();
    float deltaTime = 1.0f / 60.0f; // Assume 60 FPS for consistent physics
    
    for (uint32_t i = 0; i < ExplosionConstants::MAX_SIMULTANEOUS_EXPLOSIONS; ++i) {
        explosions[i].update(currentTime, deltaTime);
    }
}

void renderExplosions(SDL_Renderer* renderer) {
    uint32_t currentTime = SDL_GetTicks();
    
    for (uint32_t i = 0; i < ExplosionConstants::MAX_SIMULTANEOUS_EXPLOSIONS; ++i) {
        const Explosion& explosion = explosions[i];
        
        if (!explosion.active) continue;
        
        float alpha = explosion.getAlpha(currentTime);
        if (alpha <= 0.0f) continue;
        
        int alphaInt = static_cast<int>(alpha * 255);
        
        // Render multiple layered circles for explosion effect
        // Outer layer - bright yellow
        if (explosion.currentRadius > 0) {
            filledCircleColor(renderer, 
                            static_cast<int16_t>(explosion.position.x),
                            static_cast<int16_t>(explosion.position.y),
                            static_cast<int16_t>(explosion.currentRadius),
                            (255 << 24) | (255 << 16) | (200 << 8) | 0); // Yellow with alpha
            
            // Middle layer - orange
            float middleRadius = explosion.currentRadius * 0.7f;
            if (middleRadius > 0) {
                filledCircleColor(renderer,
                                static_cast<int16_t>(explosion.position.x),
                                static_cast<int16_t>(explosion.position.y),
                                static_cast<int16_t>(middleRadius),
                                (alphaInt << 24) | (255 << 16) | (140 << 8) | 0); // Orange
            }
            
            // Inner layer - red/white core
            float coreRadius = explosion.currentRadius * 0.3f;
            if (coreRadius > 0) {
                int coreAlpha = std::min(255, alphaInt + 50); // Brighter core
                filledCircleColor(renderer,
                                static_cast<int16_t>(explosion.position.x),
                                static_cast<int16_t>(explosion.position.y),
                                static_cast<int16_t>(coreRadius),
                                (coreAlpha << 24) | (255 << 16) | (255 << 8) | 200); // White-red core
            }
        }
        
        // Render particles
        for (uint32_t p = 0; p < ExplosionConstants::PARTICLES_PER_EXPLOSION; ++p) {
            const Particle& particle = explosion.particles[p];
            
            if (particle.life <= 0.0f) continue;
            
            int particleAlpha = static_cast<int>(particle.life * alpha * 255);
            if (particleAlpha <= 0) continue;
            
            // Extract RGB from particle color and apply alpha
            uint32_t r = (particle.color >> 16) & 0xFF;
            uint32_t g = (particle.color >> 8) & 0xFF;
            uint32_t b = particle.color & 0xFF;
            
            uint32_t particleColor = (particleAlpha << 24) | (r << 16) | (g << 8) | b;
            
            // Render particle as small pixel or small circle
            int16_t px = static_cast<int16_t>(particle.position.x);
            int16_t py = static_cast<int16_t>(particle.position.y);
            
            // Use pixelColor for single pixel particles (more efficient)
            pixelColor(renderer, px, py, particleColor);
            
            // For slightly larger particles, draw small circles
            if (particle.life > 0.5f) {
                // Brighter, larger particles when fresh
                filledCircleColor(renderer, px, py, 1, particleColor);
            }
        }
    }
}

bool isComplete(uint32_t explosionId) {
    for (uint32_t i = 0; i < ExplosionConstants::MAX_SIMULTANEOUS_EXPLOSIONS; ++i) {
        if (explosions[i].explosionId == explosionId) {
            return !explosions[i].active;
        }
    }
    return true; // Not found, assume complete
}

uint32_t getActiveExplosionCount() {
    uint32_t count = 0;
    for (uint32_t i = 0; i < ExplosionConstants::MAX_SIMULTANEOUS_EXPLOSIONS; ++i) {
        if (explosions[i].active) {
            count++;
        }
    }
    return count;
}

void clearAllExplosions() {
    for (uint32_t i = 0; i < ExplosionConstants::MAX_SIMULTANEOUS_EXPLOSIONS; ++i) {
        explosions[i].active = false;
    }
    nextExplosionId = 1;
}

const Explosion* getExplosion(uint32_t explosionId) {
    for (uint32_t i = 0; i < ExplosionConstants::MAX_SIMULTANEOUS_EXPLOSIONS; ++i) {
        if (explosions[i].explosionId == explosionId && explosions[i].active) {
            return &explosions[i];
        }
    }
    return nullptr;
}