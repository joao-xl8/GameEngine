#pragma once
#include "character.hpp"

class Player : public Character {
private:
    int m_playerScale;
    int m_gameScale;
    
public:
    Player(GameEngine* game, EntityManager* entityManager, const Vec2& tileSize, int playerScale = 32, int gameScale = 64);
    
    // Override base class methods
    void init(const Vec2& position) override;
    void update(float deltaTime) override;
    
    // Player-specific methods
    void setupAnimations();
    void setupComponents(const Vec2& position);
    void setupCamera(const Vec2& position);
    void setupSound();
    
    // Movement helpers
    bool canMoveTo(const Vec2& position) const;
    void snapToGrid(const Vec2& position);
};
