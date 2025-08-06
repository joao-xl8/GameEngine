#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <map>
#include <functional>
#include <cmath>
#include "vec2.hpp"

class Component {
public:
    virtual ~Component() = default;
};

class CTransform : public Component {
public:
    Vec2 pos = {0, 0};
    Vec2 velocity = {0, 0};
    Vec2 scale = {1, 1};
    double angle = 0;

    CTransform(const Vec2& p) : pos{p} {};
    ~CTransform(){};
};

class CBoundingBox : public Component {
public:
    Vec2 size;
    public:
    CBoundingBox(const Vec2 &s) : size{s} {};
    ~CBoundingBox(){};
};

class CSprite : public Component {
public:
    sf::Sprite sprite;
    std::string name;

    CSprite(const std::string& n,const sf::Texture &texture) : sprite{texture}, name{n} {};
    ~CSprite(){};
};

class CInput : public Component {
public:
    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;
    
    // Track key press events (not held keys)
    bool upPressed = false;
    bool downPressed = false;
    bool leftPressed = false;
    bool rightPressed = false;
    
    CInput() {};
    ~CInput(){};
    
    // Reset press flags (call this after processing input)
    void resetPressFlags() {
        upPressed = false;
        downPressed = false;
        leftPressed = false;
        rightPressed = false;
    }
};

class CAnimation : public Component {
public:
    std::string currentAnimation = "idle";
    std::string previousAnimation = "idle";
    int currentFrame = 0;
    float frameTime = 0.1f; // time per frame in seconds
    float frameTimer = 0.0f; // current time accumulator
    Vec2 frameSize = {32, 32}; // size of each frame in sprite sheet
    bool repeat = true;
    
    // Animation definitions - maps animation name to row and flip settings
    struct AnimationData {
        int row;
        bool flipX;
        int frameCount;
        float frameTime;
        bool repeat;
    };
    std::map<std::string, AnimationData> animations;
    
    CAnimation(Vec2 size) : frameSize(size) {};
    
    ~CAnimation(){};
    
    // Add an animation definition
    void addAnimation(const std::string& name, const std::string& textureName, int frameCount, float frameTime, bool flipX = false, int row = 0, bool repeat = true) {
        (void)textureName; // Reserved for future use with multiple textures per entity
        animations[name] = {row, flipX, frameCount, frameTime, repeat};
    }
    
    // Play a specific animation
    void play(const std::string& animationName) 
    {
        if (animations.find(animationName) != animations.end() && currentAnimation != animationName)
        {
            previousAnimation = currentAnimation;
            currentAnimation = animationName;
            currentFrame = 0;
            frameTimer = 0.0f;
            
            // Update frame settings from animation data
            auto& animData = animations[animationName];
            frameTime = animData.frameTime;
            repeat = animData.repeat;
        }
    }
    
    // Update animation with delta time
    void update(float deltaTime, sf::Sprite& sprite)
    {
        if (animations.find(currentAnimation) == animations.end()) return;
        
        frameTimer += deltaTime;
        
        // Check if it's time to advance to next frame
        if (frameTimer >= frameTime)
        {
            frameTimer = 0.0f;
            currentFrame++;
            
            auto& animData = animations[currentAnimation];
            
            // Loop animation if repeat is true
            if (currentFrame >= animData.frameCount)
            {
                if (animData.repeat)
                {
                    currentFrame = 0;
                }
                else
                {
                    currentFrame = animData.frameCount - 1;
                }
            }
        }
        
        // Update sprite texture rectangle
        updateSpriteFrame(sprite);
    }
    
    // Update the sprite's texture rectangle based on current animation
    void updateSpriteFrame(sf::Sprite& sprite)
    {
        if (animations.find(currentAnimation) == animations.end()) return;
        
        auto& animData = animations[currentAnimation];
        
        int frameX = currentFrame * frameSize.x;
        int frameY = animData.row * frameSize.y;
        
        // Set texture rectangle with flipping support
        if (animData.flipX)
        {
            sprite.setTextureRect(sf::IntRect(
                frameX + frameSize.x, frameY, 
                -frameSize.x, frameSize.y));
        }
        else
        {
            sprite.setTextureRect(sf::IntRect(frameX, frameY, 
                frameSize.x, frameSize.y));
        }
    }
    
    // Check if animation has finished (for non-repeating animations)
    bool hasFinished() const
    {
        if (animations.find(currentAnimation) == animations.end()) return false;
        auto& animData = animations.at(currentAnimation);
        return !animData.repeat && currentFrame >= animData.frameCount - 1;
    }
    
    // Get current animation name
    const std::string& getCurrentAnimation() const
    {
        return currentAnimation;
    }
};

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

class CSound : public Component {
public:
    std::map<std::string, sf::Sound> sounds;
    std::map<std::string, sf::SoundBuffer> soundBuffers;
    
    CSound() {};
    ~CSound(){};
    
    void addSound(const std::string& name, const std::string& filename)
    {
        sf::SoundBuffer buffer;
        if (buffer.loadFromFile(filename))
        {
            soundBuffers[name] = buffer;
            sounds[name].setBuffer(soundBuffers[name]);
        }
    }
    
    void playSound(const std::string& name)
    {
        if (sounds.find(name) != sounds.end())
        {
            sounds[name].play();
        }
    }
    
    void stopSound(const std::string& name)
    {
        if (sounds.find(name) != sounds.end())
        {
            sounds[name].stop();
        }
    }
};

class CCamera : public Component {
public:
    Vec2 position = {0.0f, 0.0f};        // Current camera position (center of view)
    Vec2 targetPosition = {0.0f, 0.0f};  // Target position to follow
    Vec2 deadZone = {64.0f, 64.0f};      // Dead zone size (1 tile = 64x64)
    float followSpeed = 5.0f;            // How fast camera catches up (multiplier)
    bool isFollowing = false;            // Whether camera is actively following
    
    CCamera() {};
    CCamera(const Vec2& pos, const Vec2& deadZoneSize, float speed) 
        : position(pos), targetPosition(pos), deadZone(deadZoneSize), followSpeed(speed) {};
    ~CCamera(){};
    
    // Update camera to follow a target position with dead zone
    void followTarget(const Vec2& targetPos, float deltaTime) {
        // Calculate the offset from camera center to target
        Vec2 offset = targetPos - position;
        
        // Check if target is outside the dead zone
        bool needsUpdate = false;
        Vec2 newTargetPos = position;
        
        // Check horizontal movement
        if (offset.x > deadZone.x / 2.0f) {
            // Target is too far right, move camera right
            newTargetPos.x = targetPos.x - deadZone.x / 2.0f;
            needsUpdate = true;
        } else if (offset.x < -deadZone.x / 2.0f) {
            // Target is too far left, move camera left
            newTargetPos.x = targetPos.x + deadZone.x / 2.0f;
            needsUpdate = true;
        }
        
        // Check vertical movement - Using same coordinate system as player movement
        // Player UP = +Y, Player DOWN = -Y, Camera follows in same coordinate space
        if (offset.y > deadZone.y / 2.0f) {
            // Player is too far up (positive Y), move camera up
            newTargetPos.y = targetPos.y - deadZone.y / 2.0f;
            needsUpdate = true;
        } else if (offset.y < -deadZone.y / 2.0f) {
            // Player is too far down (negative Y), move camera down  
            newTargetPos.y = targetPos.y + deadZone.y / 2.0f;
            needsUpdate = true;
        }
        
        if (needsUpdate) {
            targetPosition = newTargetPos;
            isFollowing = true;
        }
        
        // Smoothly move camera towards target position
        if (isFollowing) {
            Vec2 direction = targetPosition - position;
            float distance = direction.length();
            
            if (distance < 1.0f) {
                // Close enough, snap to target
                position = targetPosition;
                isFollowing = false;
            } else {
                // Move towards target with smooth interpolation
                Vec2 normalizedDir = direction.normalized();
                float moveDistance = followSpeed * 100.0f * deltaTime; // Base speed of 100 pixels/second
                
                // Don't overshoot
                if (moveDistance >= distance) {
                    position = targetPosition;
                    isFollowing = false;
                } else {
                    position = position + (normalizedDir * moveDistance);
                }
            }
        }
    }
    
    // Set camera position immediately (no smooth movement)
    void setPosition(const Vec2& pos) {
        position = pos;
        targetPosition = pos;
        isFollowing = false;
    }
    
    // Get the camera's view bounds (useful for culling)
    struct ViewBounds {
        float left, right, top, bottom;
    };
    
    ViewBounds getViewBounds(float viewWidth, float viewHeight) const {
        float halfWidth = viewWidth / 2.0f;
        float halfHeight = viewHeight / 2.0f;
        
        return ViewBounds{
            position.x - halfWidth,   // left
            position.x + halfWidth,   // right
            position.y - halfHeight,  // top
            position.y + halfHeight   // bottom
        };
    }
};