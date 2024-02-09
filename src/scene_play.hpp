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
    const Vec2 m_tileSize = {64, 64};
    sf::Text m_tileText;

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
    Vec2 gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity);
public:
    Scene_Play(GameEngine* game, const std::string& levelPath);
    void update();
};