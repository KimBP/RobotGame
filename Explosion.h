#pragma once

#include <cstdint>
#include "Vector2.h"

// Forward declaration to avoid including SDL headers in this header
struct SDL_Renderer;

namespace ExplosionConstants {
    constexpr uint32_t MAX_SIMULTANEOUS_EXPLOSIONS = 50;
    constexpr uint32_t PARTICLES_PER_EXPLOSION = 20;
    constexpr uint32_t ANIMATION_DURATION_MS = 1000;
    constexpr float MAX_EXPLOSION_RADIUS = 80.0f;
    constexpr float GRAVITY = 200.0f; // pixels per second^2
}

struct Particle {
    Vector2 position;
    Vector2 velocity;
    float life; // 1.0 = fully visible, 0.0 = invisible
    uint32_t color; // RGBA format
    
    Particle() : position(0, 0), velocity(0, 0), life(0.0f), color(0xFFFFFFFF) {}
    
    Particle(const Vector2& pos, const Vector2& vel, uint32_t col)
        : position(pos), velocity(vel), life(1.0f), color(col) {}
    
    void update(float deltaTime) {
        position = position + velocity * deltaTime;
        velocity.y += ExplosionConstants::GRAVITY * deltaTime;
        life -= deltaTime / (ExplosionConstants::ANIMATION_DURATION_MS / 1000.0f);
        if (life < 0.0f) life = 0.0f;
    }
};

struct Explosion {
    uint32_t explosionId;
    Vector2 position;
    float currentRadius;
    float maxRadius;
    uint32_t startTime;
    bool active;
    Particle particles[ExplosionConstants::PARTICLES_PER_EXPLOSION];
    
    Explosion() : explosionId(0), position(0, 0), currentRadius(0.0f),
                  maxRadius(ExplosionConstants::MAX_EXPLOSION_RADIUS),
                  startTime(0), active(false) {}
    
    Explosion(uint32_t id, const Vector2& pos, uint32_t time)
        : explosionId(id), position(pos), currentRadius(0.0f),
          maxRadius(ExplosionConstants::MAX_EXPLOSION_RADIUS),
          startTime(time), active(true) {
        
        // Initialize particles
        for (uint32_t i = 0; i < ExplosionConstants::PARTICLES_PER_EXPLOSION; ++i) {
            float angle = (2.0f * 3.14159f * i) / ExplosionConstants::PARTICLES_PER_EXPLOSION;
            float speed = 50.0f + (rand() % 100); // 50-150 pixels per second
            Vector2 velocity(std::cos(angle) * speed, std::sin(angle) * speed);
            
            // Orange to red color variation
            uint32_t color = 0xFF000000 | // Alpha 0xFF
                           ((0xFF - (rand() % 50)) << 16) | // Red: 205-255
                           ((0x80 + (rand() % 80)) << 8) | // Green: 128-207
                            (rand() % 50); // Blue: 0-49
            
            particles[i] = Particle(pos, velocity, color);
        }
    }
    
    void update(uint32_t currentTime, float deltaTime) {
        if (!active) return;
        
        uint32_t elapsed = currentTime - startTime;
        if (elapsed >= ExplosionConstants::ANIMATION_DURATION_MS) {
            active = false;
            return;
        }
        
        // Update explosion radius (expand for first half, then fade)
        float progress = static_cast<float>(elapsed) / ExplosionConstants::ANIMATION_DURATION_MS;
        if (progress < 0.5f) {
            currentRadius = maxRadius * (progress * 2.0f);
        } else {
            currentRadius = maxRadius;
        }
        
        // Update particles
        for (uint32_t i = 0; i < ExplosionConstants::PARTICLES_PER_EXPLOSION; ++i) {
            particles[i].update(deltaTime);
        }
    }
    
    float getAlpha(uint32_t currentTime) const {
        if (!active) return 0.0f;
        uint32_t elapsed = currentTime - startTime;
        float progress = static_cast<float>(elapsed) / ExplosionConstants::ANIMATION_DURATION_MS;
        return 1.0f - progress; // Fade out over animation duration
    }
};

// Global explosion system functions
uint32_t createExplosion(const Vector2& position);
void updateExplosions();
void renderExplosions(SDL_Renderer* renderer);
bool isComplete(uint32_t explosionId);
uint32_t getActiveExplosionCount();
void clearAllExplosions();
const Explosion* getExplosion(uint32_t explosionId);