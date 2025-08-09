#pragma once

#include "../vec2.hpp"
#include "base_component.hpp"
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <cstdio>
#include <map>
#include <vector>
#include <string>
#include <functional>
#include <algorithm>

// Core Engine Components - These are engine-level systems

// Layer-based rendering component for 5-layer world system
// Rendering order: 0 -> 1 -> 2 -> 3 -> 4
class CLayer : public Component {
public:
    enum LayerType {
        GROUND = 0,      // Layer 0: Ground tiles - collision optional
        DECORATION_1 = 1, // Layer 1: First decoration layer - collision optional
        DECORATION_2 = 2, // Layer 2: Second decoration layer - collision optional  
        DECORATION_3 = 3, // Layer 3: Third decoration layer - collision optional
        ENTITY = 4       // Layer 4: Entity layer - collision optional
    };
    
    LayerType layer;
    int subLayer;  // For fine-grained ordering within the same layer (0 = bottom, higher = top)
    
    CLayer(LayerType l, int sub = 0) : layer(l), subLayer(sub) {}
    ~CLayer() {}
    
    // Helper methods
    static const char* getLayerName(LayerType layer) {
        switch(layer) {
            case GROUND: return "Ground";
            case DECORATION_1: return "Decoration_1";
            case DECORATION_2: return "Decoration_2";
            case DECORATION_3: return "Decoration_3";
            case ENTITY: return "Entity";
            default: return "Unknown";
        }
    }
    
    // Get total rendering order for sorting (layer * 100 + subLayer)
    int getRenderOrder() const {
        return static_cast<int>(layer) * 100 + subLayer;
    }
};

// Multi-cell tile component for assets that occupy multiple grid cells
class CMultiCell : public Component {
public:
    int width;   // Width in grid cells
    int height;  // Height in grid cells
    int originX; // Origin cell X (top-left corner)
    int originY; // Origin cell Y (top-left corner)
    float rotation; // Rotation in degrees (0, 90, 180, 270)
    
    CMultiCell(int w = 1, int h = 1, int ox = 0, int oy = 0, float rot = 0.0f) 
        : width(w), height(h), originX(ox), originY(oy), rotation(rot) {}
    
    // Check if a grid position is occupied by this multi-cell asset
    bool occupiesCell(int x, int y) const {
        // Apply rotation to check occupied cells
        int rotatedWidth = width;
        int rotatedHeight = height;
        
        // For 90° and 270° rotations, swap width and height
        if (rotation == 90.0f || rotation == 270.0f) {
            rotatedWidth = height;
            rotatedHeight = width;
        }
        
        return (x >= originX && x < originX + rotatedWidth && 
                y >= originY && y < originY + rotatedHeight);
    }
    
    // Get all cells occupied by this asset
    std::vector<std::pair<int, int>> getOccupiedCells() const {
        std::vector<std::pair<int, int>> cells;
        
        int rotatedWidth = width;
        int rotatedHeight = height;
        
        // For 90° and 270° rotations, swap width and height
        if (rotation == 90.0f || rotation == 270.0f) {
            rotatedWidth = height;
            rotatedHeight = width;
        }
        
        for (int x = originX; x < originX + rotatedWidth; x++) {
            for (int y = originY; y < originY + rotatedHeight; y++) {
                cells.push_back({x, y});
            }
        }
        
        return cells;
    }
};

// Collision component - independent of layer
class CCollision : public Component {
public:
    bool hasCollision;
    Vec2 collisionSize;  // Custom collision size (defaults to sprite size)
    Vec2 collisionOffset; // Offset from sprite position
    
    CCollision(bool collision = true, Vec2 size = Vec2(0, 0), Vec2 offset = Vec2(0, 0))
        : hasCollision(collision), collisionSize(size), collisionOffset(offset) {}
    
    // Check if collision is enabled
    bool isCollidable() const {
        return hasCollision;
    }
    
    // Toggle collision
    void toggleCollision() {
        hasCollision = !hasCollision;
    }
};

// Save/Load System Component
class CSave : public Component
{
public:
    bool enabled = true;
    std::string savePointName = "";
    std::string displayText = "Save Game";
    
    CSave() = default;
    CSave(const std::string& name, const std::string& text = "Save Game")
        : savePointName(name), displayText(text) {}
};

// Script Tile component for interactive tiles that trigger actions
class CScriptTile : public Component {
public:
    enum TriggerType {
        ON_ENTER,    // Triggered when player enters the tile
        ON_EXIT,     // Triggered when player exits the tile
        ON_INTERACT  // Triggered when player presses interact key on tile
    };
    
    std::string scriptName;     // Name/ID of the script to execute
    TriggerType triggerType;    // When this tile should trigger
    bool isTriggered;          // Track if tile has been triggered (for one-time events)
    bool repeatable;           // Can this tile be triggered multiple times?
    std::string parameters;    // Optional parameters for the script
    
    CScriptTile(const std::string& script, TriggerType trigger = ON_ENTER, bool repeat = true)
        : scriptName(script), triggerType(trigger), isTriggered(false), repeatable(repeat) {}
    ~CScriptTile() {}
    
    // Reset trigger state (useful for repeatable tiles)
    void reset() {
        isTriggered = false;
    }
    
    // Check if tile can be triggered
    bool canTrigger() const {
        return repeatable || !isTriggered;
    }
    
    // Mark tile as triggered
    void trigger() {
        isTriggered = true;
    }
};

class CTransform : public Component {
public:
  Vec2 pos = {0, 0};
  Vec2 velocity = {0, 0};
  Vec2 scale = {1, 1};
  double angle = 0;

  CTransform(const Vec2 &p) : pos{p} {};
  ~CTransform() {};
};

class CSprite : public Component {
public:
  sf::Sprite sprite;
  std::string name;

  CSprite(const std::string &n, const sf::Texture &texture)
      : sprite{texture}, name{n} {};
  ~CSprite() {};
};

class CAnimation : public Component {
public:
  std::string currentAnimation = "idle";
  std::string previousAnimation = "idle";
  int currentFrame = 0;
  float frameTime = 0.1f;    // time per frame in seconds
  float frameTimer = 0.0f;   // current time accumulator
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

  ~CAnimation() {};

  // Add an animation definition
  void addAnimation(const std::string &name, const std::string &textureName,
                    int frameCount, float frameTime, bool flipX = false,
                    int row = 0, bool repeat = true) {
    (void)textureName; // Reserved for future use with multiple textures per
                       // entity
    animations[name] = {row, flipX, frameCount, frameTime, repeat};
  }

  // Play a specific animation
  void play(const std::string &animationName) {
    if (animations.find(animationName) != animations.end() &&
        currentAnimation != animationName) {
      previousAnimation = currentAnimation;
      currentAnimation = animationName;
      currentFrame = 0;
      frameTimer = 0.0f;

      // Update frame settings from animation data
      auto &animData = animations[animationName];
      frameTime = animData.frameTime;
      repeat = animData.repeat;
    }
  }

  // Update animation with delta time
  void update(float deltaTime, sf::Sprite &sprite) {
    if (animations.find(currentAnimation) == animations.end())
      return;

    frameTimer += deltaTime;

    // Check if it's time to advance to next frame
    if (frameTimer >= frameTime) {
      frameTimer = 0.0f;
      currentFrame++;

      auto &animData = animations[currentAnimation];

      // Loop animation if repeat is true
      if (currentFrame >= animData.frameCount) {
        if (animData.repeat) {
          currentFrame = 0;
        } else {
          currentFrame = animData.frameCount - 1;
        }
      }
    }

    // Update sprite texture rectangle
    updateSpriteFrame(sprite);
  }

  // Update the sprite's texture rectangle based on current animation
  void updateSpriteFrame(sf::Sprite &sprite) {
    if (animations.find(currentAnimation) == animations.end())
      return;

    auto &animData = animations[currentAnimation];

    int frameX = currentFrame * frameSize.x;
    int frameY = animData.row * frameSize.y;

    // Set texture rectangle with flipping support
    if (animData.flipX) {
      sprite.setTextureRect(
          sf::IntRect(frameX + frameSize.x, frameY, -frameSize.x, frameSize.y));
    } else {
      sprite.setTextureRect(
          sf::IntRect(frameX, frameY, frameSize.x, frameSize.y));
    }
  }

  // Check if animation has finished (for non-repeating animations)
  bool hasFinished() const {
    if (animations.find(currentAnimation) == animations.end())
      return false;
    auto &animData = animations.at(currentAnimation);
    return !animData.repeat && currentFrame >= animData.frameCount - 1;
  }

  // Get current animation name
  const std::string &getCurrentAnimation() const { return currentAnimation; }
};

class CBoundingBox : public Component {
public:
  Vec2 size;

public:
  CBoundingBox(const Vec2 &s) : size{s} {};
  ~CBoundingBox() {};
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
  ~CInput() {};

  // Reset press flags (call this after processing input)
  void resetPressFlags() {
    upPressed = false;
    downPressed = false;
    leftPressed = false;
    rightPressed = false;
  }
};

class CSound : public Component {
public:
  std::map<std::string, sf::Sound> sounds;
  std::map<std::string, sf::SoundBuffer> soundBuffers;
  std::map<std::string, sf::Music *> music; // For background music (streaming)

  CSound() {};
  ~CSound() {
    // Clean up music pointers
    for (auto &pair : music) {
      delete pair.second;
    }
  };

  // Add a sound effect (loaded into memory)
  bool addSound(const std::string &name, const std::string &filename) {
    sf::SoundBuffer buffer;
    if (buffer.loadFromFile(filename)) {
      soundBuffers[name] = buffer;
      sounds[name].setBuffer(soundBuffers[name]);
      return true;
    }
    std::printf("Failed to load sound: %s\n", filename.c_str());
    return false;
  }

  // Add background music (streamed from file)
  bool addMusic(const std::string &name, const std::string &filename) {
    sf::Music *musicPtr = new sf::Music();
    if (musicPtr->openFromFile(filename)) {
      music[name] = musicPtr;
      return true;
    }
    std::printf("Failed to load music: %s\n", filename.c_str());
    delete musicPtr;
    return false;
  }

  // Play a sound effect
  void playSound(const std::string &name, float volume = 100.0f) {
    if (sounds.find(name) != sounds.end()) {
      sounds[name].setVolume(volume);
      sounds[name].play();
    }
  }

  // Play background music
  void playMusic(const std::string &name, bool loop = true,
                 float volume = 50.0f) {
    if (music.find(name) != music.end()) {
      music[name]->setVolume(volume);
      music[name]->setLoop(loop);
      music[name]->play();
    }
  }

  // Stop a sound effect
  void stopSound(const std::string &name) {
    if (sounds.find(name) != sounds.end()) {
      sounds[name].stop();
    }
  }

  // Stop background music
  void stopMusic(const std::string &name) {
    if (music.find(name) != music.end()) {
      music[name]->stop();
    }
  }

  // Check if music is playing
  bool isMusicPlaying(const std::string &name) {
    if (music.find(name) != music.end()) {
      return music[name]->getStatus() == sf::Music::Playing;
    }
    return false;
  }
};

class CCamera : public Component {
public:
  Vec2 position = {0.0f, 0.0f}; // Current camera position (center of view)
  Vec2 targetPosition = {0.0f, 0.0f}; // Target position to follow
  Vec2 deadZone = {64.0f, 64.0f};     // Dead zone size (1 tile = 64x64)
  float followSpeed = 5.0f;           // How fast camera catches up (multiplier)
  bool isFollowing = false;           // Whether camera is actively following

  CCamera() {};
  CCamera(const Vec2 &pos, const Vec2 &deadZoneSize, float speed)
      : position(pos), targetPosition(pos), deadZone(deadZoneSize),
        followSpeed(speed) {};
  ~CCamera() {};

  // Update camera to follow a target position with dead zone
  void followTarget(const Vec2 &targetPos, float deltaTime) {
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

    // Check vertical movement - Using SFML/Level coordinate system
    // Player UP (W key) = -Y (towards top of screen), Player DOWN (S key) = +Y (towards bottom of screen)
    if (offset.y > deadZone.y / 2.0f) {
      // Player is too far down (positive Y), move camera down
      newTargetPos.y = targetPos.y - deadZone.y / 2.0f;
      needsUpdate = true;
    } else if (offset.y < -deadZone.y / 2.0f) {
      // Player is too far up (negative Y), move camera up
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
        float moveDistance =
            followSpeed * 100.0f * deltaTime; // Base speed of 100 pixels/second

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
  void setPosition(const Vec2 &pos) {
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
        position.x - halfWidth,  // left
        position.x + halfWidth,  // right
        position.y - halfHeight, // top
        position.y + halfHeight  // bottom
    };
  }
};

// Grid-based movement component for tile-based games
class CGridMovement : public Component {
public:
    float gridSize = 64.0f;
    float moveSpeed = 4.0f;
    bool smoothMovement = true;
    bool canMove = true;
    
    // Grid position (in grid coordinates)
    Vec2 gridPos;
    
    // Target position for smooth movement
    Vec2 targetPosition;
    Vec2 previousPosition;
    bool isMoving = false;
    float moveProgress = 0.0f;
    
    CGridMovement(float grid = 64.0f, float speed = 4.0f, bool smooth = true)
        : gridSize(grid), moveSpeed(speed), smoothMovement(smooth) {}
    
    void snapToGrid(const Vec2& worldPos) {
        gridPos.x = std::round(worldPos.x / gridSize);
        gridPos.y = std::round(worldPos.y / gridSize);
        targetPosition = Vec2(gridPos.x * gridSize, gridPos.y * gridSize);
        previousPosition = targetPosition;
        isMoving = false;
        moveProgress = 0.0f;
    }
    
    Vec2 getWorldPosition() const {
        if (smoothMovement && isMoving) {
            // Interpolate between previous and target position
            float t = moveProgress;
            return Vec2(
                previousPosition.x + (targetPosition.x - previousPosition.x) * t,
                previousPosition.y + (targetPosition.y - previousPosition.y) * t
            );
        }
        return targetPosition;
    }
    
    bool tryMove(int deltaX, int deltaY) {
        if (!canMove || isMoving) return false;
        
        Vec2 newGridPos = Vec2(gridPos.x + deltaX, gridPos.y + deltaY);
        Vec2 newWorldPos = Vec2(newGridPos.x * gridSize, newGridPos.y * gridSize);
        
        // Update positions
        previousPosition = targetPosition;
        gridPos = newGridPos;
        targetPosition = newWorldPos;
        
        if (smoothMovement) {
            isMoving = true;
            moveProgress = 0.0f;
        }
        
        return true;
    }
    
    void update(float deltaTime) {
        if (smoothMovement && isMoving) {
            moveProgress += moveSpeed * deltaTime;
            if (moveProgress >= 1.0f) {
                moveProgress = 1.0f;
                isMoving = false;
            }
        }
    }
    
    // Method for collision-aware movement
    bool startMoveWithCollisionCheck(const Vec2& direction, const Vec2& /*currentPos*/, const Vec2& size, std::function<bool(Vec2, Vec2)> collisionCheck) {
        if (!canMove || isMoving) return false;
        
        int deltaX = static_cast<int>(direction.x);
        int deltaY = static_cast<int>(direction.y);
        
        Vec2 newGridPos = Vec2(gridPos.x + deltaX, gridPos.y + deltaY);
        Vec2 newWorldPos = Vec2(newGridPos.x * gridSize, newGridPos.y * gridSize);
        
        // Check collision at new position
        if (collisionCheck && collisionCheck(newWorldPos, size)) {
            return false; // Movement blocked by collision
        }
        
        // Update positions
        previousPosition = targetPosition;
        gridPos = newGridPos;
        targetPosition = newWorldPos;
        
        if (smoothMovement) {
            isMoving = true;
            moveProgress = 0.0f;
        }
        
        return true;
    }
    
    // Update movement and return current world position
    Vec2 updateMovement(float deltaTime, const Vec2& /*currentTransformPos*/) {
        update(deltaTime);
        return getWorldPosition();
    }
};
