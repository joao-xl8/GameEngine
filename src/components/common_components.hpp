#pragma once

#include "base_component.hpp"
#include "../vec2.hpp"
#include <functional>
#include <SFML/Graphics.hpp>

// Common Components - Shared between engine and game logic

class CGridMovement : public Component {
public:
    int gridSize = 32;           // Size of each grid cell
    Vec2 gridPos = {0, 0};       // Current grid position (in grid coordinates)
    Vec2 targetPos = {0, 0};     // Target world position
    Vec2 startPos = {0, 0};      // Starting position for interpolation
    float moveSpeed = 4.0f;      // Grid moves per second
    float moveTimer = 0.0f;      // Current movement timer
    float moveDuration = 0.0f;   // Time to complete one grid move
    bool isMoving = false;       // Is currently moving between grid positions
    bool smoothMovement = true;  // Enable smooth interpolation between grid positions
    
    CGridMovement(int size = 32, float speed = 4.0f, bool smooth = true) 
        : gridSize(size), moveSpeed(speed), smoothMovement(smooth) 
    {
        moveDuration = 1.0f / moveSpeed;
    }
    
    ~CGridMovement() {}
    
    // Convert grid coordinates to world coordinates
    Vec2 gridToWorld(Vec2 gridCoords) const {
        return Vec2{gridCoords.x * gridSize, gridCoords.y * gridSize};
    }
    
    // Convert world coordinates to grid coordinates
    Vec2 worldToGrid(Vec2 worldCoords) const {
        return Vec2{
            std::floor(worldCoords.x / gridSize),
            std::floor(worldCoords.y / gridSize)
        };
    }
    
    // Start moving to a new grid position
    bool startMove(Vec2 direction, Vec2 currentWorldPos) {
        if (isMoving) return false; // Already moving
        
        Vec2 newGridPos = gridPos + direction;
        Vec2 newWorldPos = gridToWorld(newGridPos);
        
        gridPos = newGridPos;
        startPos = currentWorldPos;
        targetPos = newWorldPos;
        
        if (smoothMovement) {
            isMoving = true;
            moveTimer = 0.0f;
        }
        
        return true;
    }
    
    // Start moving with collision checking
    bool startMoveWithCollisionCheck(Vec2 direction, Vec2 currentWorldPos, Vec2 entitySize, 
                                   std::function<bool(Vec2, Vec2)> collisionCheck) {
        if (isMoving) return false; // Already moving
        
        Vec2 newGridPos = gridPos + direction;
        Vec2 newWorldPos = gridToWorld(newGridPos);
        
        // Check if the new position would cause a collision
        if (collisionCheck && collisionCheck(newWorldPos, entitySize)) {
            return false; // Movement blocked by collision
        }
        
        gridPos = newGridPos;
        startPos = currentWorldPos;
        targetPos = newWorldPos;
        
        if (smoothMovement) {
            isMoving = true;
            moveTimer = 0.0f;
        }
        
        return true;
    }
    
    // Update movement interpolation
    Vec2 updateMovement(float deltaTime, Vec2 currentPos) {
        if (!isMoving || !smoothMovement) {
            return targetPos;
        }
        
        (void)currentPos; // Suppress unused parameter warning
        
        moveTimer += deltaTime;
        float progress = moveTimer / moveDuration;
        
        if (progress >= 1.0f) {
            isMoving = false;
            moveTimer = 0.0f;
            return targetPos;
        }
        
        // Smooth interpolation (you can change this for different easing)
        return Vec2{
            startPos.x + (targetPos.x - startPos.x) * progress,
            startPos.y + (targetPos.y - startPos.y) * progress
        };
    }
    
    // Snap to grid (useful for initialization)
    void snapToGrid(Vec2 worldPos) {
        gridPos = worldToGrid(worldPos);
        targetPos = gridToWorld(gridPos);
        isMoving = false;
        moveTimer = 0.0f;
    }
};

class CLifespan : public Component {
public:
    int lifespan = 0;
    int frameCreated = 0;
    bool has = false;

    CLifespan() {}
    CLifespan(int duration, int frame) : lifespan(duration), frameCreated(frame), has(true) {}
};

class CScore : public Component {
public:
    int score = 0;
    CScore() {}
    CScore(int s) : score(s) {}
};

class CState : public Component {
public:
    std::string state = "default";
    CState() {}
    CState(const std::string& s) : state(s) {}
};

// Text rendering component
class CText : public Component {
public:
    sf::Text text;
    bool has = false;
    
    CText() {}
    CText(const std::string& str, sf::Font& font, int size = 24) : has(true) {
        text.setString(str);
        text.setFont(font);
        text.setCharacterSize(size);
        text.setFillColor(sf::Color::White);
    }
    
    void setString(const std::string& str) { text.setString(str); }
    void setPosition(float x, float y) { text.setPosition(x, y); }
    void setColor(const sf::Color& color) { text.setFillColor(color); }
};

// Health component (can be used by both engine and game)
class CHealth : public Component {
public:
    int max = 100;
    int current = 100;
    bool has = false;
    
    CHealth() {}
    CHealth(int m) : max(m), current(m), has(true) {}
    CHealth(int m, int c) : max(m), current(c), has(true) {}
    
    void takeDamage(int damage) {
        current = std::max(0, current - damage);
    }
    
    void heal(int amount) {
        current = std::min(max, current + amount);
    }
    
    bool isAlive() const { return current > 0; }
    float getPercentage() const { return static_cast<float>(current) / max; }
};

// Timer component for various timing needs
class CTimer : public Component {
public:
    float duration = 0.0f;
    float elapsed = 0.0f;
    bool active = false;
    bool repeat = false;
    std::function<void()> onComplete;
    
    CTimer() {}
    CTimer(float dur, bool rep = false) : duration(dur), elapsed(0.0f), active(true), repeat(rep) {}
    
    void start(float dur) {
        duration = dur;
        elapsed = 0.0f;
        active = true;
    }
    
    void update(float deltaTime) {
        if (!active) return;
        
        elapsed += deltaTime;
        if (elapsed >= duration) {
            if (onComplete) onComplete();
            
            if (repeat) {
                elapsed = 0.0f;
            } else {
                active = false;
            }
        }
    }
    
    void stop() { active = false; }
    void reset() { elapsed = 0.0f; }
    bool isComplete() const { return !active && elapsed >= duration; }
    float getProgress() const { return elapsed / duration; }
};
