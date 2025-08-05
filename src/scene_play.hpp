#pragma once
#include "scene.hpp"

class Scene_Play : public Scene
{
    struct PlayerConfig { 
        float X, Y, CX, CY, SPEED, MAXSPEED, JUMP, GRAVITY;
        std::string WEAPON;
     };
protected:
    std::string m_levelPath;
    std::shared_ptr<Entity> m_player;
    PlayerConfig m_playerConfig;
    bool m_drawTextures = true;
    bool m_drawCollision = false;
    bool m_drawGrid = false;
    const int m_gameScale = 64; // Configurable scale for all game elements (tiles, player, grid)
    const int m_playerScale = 32;
    const Vec2 m_tileSize = {static_cast<float>(m_gameScale), static_cast<float>(m_gameScale)};
    sf::Text m_tileText;
    sf::Clock m_deltaClock;
    float m_deltaTime = 0.0f;

    void init(const std::string &levelPath);
    void init();
    void onEnd();
    void sAnimation();
    void sCollision();
    void sEnemySpawner();
    void sMovement();
    void sRender();
    void sDoAction(const Action& action);
    void sDebug();
    void spawnPlayer();
    bool isColliding(const Vec2& pos1, const Vec2& size1, const Vec2& pos2, const Vec2& size2);
    bool wouldCollideAtPosition(const Vec2& position, const Vec2& size);
    Vec2 gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity);
public:
    Scene_Play(GameEngine* game, const std::string& levelPath);
    void update();
};
