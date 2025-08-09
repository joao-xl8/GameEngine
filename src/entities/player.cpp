#include "player.hpp"
#include "../game_engine.hpp"
#include "../components.hpp"
#include <iostream>

Player::Player(GameEngine* game, EntityManager* entityManager, const Vec2& tileSize, int playerScale, int gameScale)
    : Character(game, entityManager, "Player", tileSize), m_playerScale(playerScale), m_gameScale(gameScale) {
}

void Player::init(const Vec2& position) {
    // Create the entity first
    createEntity("Player");
    
    std::cout << "Initializing Player at position: " << position.x << ", " << position.y << std::endl;
    
    setupComponents(position);
    setupAnimations();
    setupCamera(position);
    setupSound();
    
    std::cout << "Player initialization complete" << std::endl;
}

void Player::setupComponents(const Vec2& position) {
    // Add transform component
    addComponent<CTransform>(std::make_shared<CTransform>(position));
    
    // Add sprite component with player texture
    auto& playerTexture = m_game->getAssets().getTexture("Player");
    addComponent<CSprite>(std::make_shared<CSprite>("Player", playerTexture));
    
    // Set up sprite sheet frame (32x32 pixel frames based on the sprite sheet)
    auto sprite = getComponent<CSprite>();
    if (sprite) {
        sprite->sprite.setTextureRect(sf::IntRect(0, 0, m_playerScale, m_playerScale)); // First frame
    }
    
    // Add grid movement component
    auto gridMovement = std::make_shared<CGridMovement>(m_gameScale, 3.0f, true); // Scale-based grid, 3 moves/sec, smooth
    gridMovement->snapToGrid(position); // Snap player to nearest grid position
    addComponent<CGridMovement>(gridMovement);
    
    // Add bounding box
    addComponent<CBoundingBox>(std::make_shared<CBoundingBox>(Vec2{static_cast<float>(m_gameScale), static_cast<float>(m_gameScale)}));
    
    // Add input component
    addComponent<CInput>(std::make_shared<CInput>());
}

void Player::setupAnimations() {
    // Add animation component with flexible animation definitions
    auto animationComponent = std::make_shared<CAnimation>(Vec2{static_cast<float>(m_playerScale), static_cast<float>(m_playerScale)});

    // Define animations - using the same texture with different rows
    animationComponent->addAnimation("idle", "Player", 6, 0.2f, false, 0);        // Row 0, 6 frames, slow
    animationComponent->addAnimation("walk_down", "Player", 6, 0.15f, false, 3);  // Row 3, 6 frames
    animationComponent->addAnimation("walk_up", "Player", 6, 0.15f, false, 2);    // Row 2, 6 frames
    animationComponent->addAnimation("walk_right", "Player", 6, 0.15f, false, 1); // Row 1, 6 frames
    animationComponent->addAnimation("walk_left", "Player", 6, 0.15f, true, 1);   // Row 1, flipped

    animationComponent->play("idle"); // Start with idle animation
    addComponent<CAnimation>(animationComponent);
}

void Player::setupCamera(const Vec2& position) {
    // Add camera component with 1-tile dead zone
    Vec2 deadZoneSize = {static_cast<float>(m_gameScale), static_cast<float>(m_gameScale)}; // 1 tile dead zone
    auto cameraComponent = std::make_shared<CCamera>(position, deadZoneSize, 3.0f); // Follow speed of 3.0
    
    // Ensure camera starts exactly at player position (centered)
    cameraComponent->setPosition(position);  // This sets both position and targetPosition
    
    addComponent<CCamera>(cameraComponent);
    
    // Initialize game view to center player in middle of screen
    sf::View& gameView = m_game->getGameView();
    gameView.setCenter(position.x, position.y);
    
    std::cout << "Camera initialized at position: " << position.x << ", " << position.y << " (player centered)" << std::endl;
}

void Player::setupSound() {
    // Add sound component
    auto soundComponent = std::make_shared<CSound>();
    // Load player-specific sounds
    soundComponent->addSound("footstep", "assets/sounds/tap.wav");
    soundComponent->addSound("hurt", "assets/sounds/hurt.wav");
    soundComponent->addSound("jump", "assets/sounds/jump.wav");
    addComponent<CSound>(soundComponent);
}

void Player::update(float deltaTime) {
    // Player-specific update logic can go here
    // For now, most update logic is handled by the scene systems
}

bool Player::canMoveTo(const Vec2& position) const {
    // This would check for collisions at the given position
    // Implementation depends on how collision checking is done in the scene
    return true; // Placeholder
}

void Player::snapToGrid(const Vec2& position) {
    if (hasComponent<CGridMovement>()) {
        auto gridMovement = getComponent<CGridMovement>();
        if (gridMovement) {
            gridMovement->snapToGrid(position);
        }
    }
}
