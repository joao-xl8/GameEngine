#pragma once
#include "../components/engine_components.hpp"
#include "scene.hpp"

// Forward declaration for dialogue scene
class Scene_Dialogue;

class Scene_Play : public Scene
{
    struct PlayerConfig { 
        float X, Y, CX, CY, SPEED, MAXSPEED, JUMP, GRAVITY;
        std::string WEAPON;
     };
protected:
    std::string m_levelPath;
    std::shared_ptr<Entity> m_player;
    std::shared_ptr<CSound> m_soundManager;  // Global sound manager for music and global sounds
    PlayerConfig m_playerConfig;
    bool m_drawTextures = true;
    bool m_drawCollision = false;
    bool m_drawGrid = false;
    const int m_gameScale = 64; // Configurable scale for all game elements (tiles, player, grid)
    const int m_playerScale = 64; // Now matches game scale for consistency
    const Vec2 m_tileSize = {static_cast<float>(m_gameScale), static_cast<float>(m_gameScale)};
    
    // Grid movement timing control
    float m_changeGridSleep = 0.5f; // Delay in seconds after each grid movement (prevents multiple moves)
    float m_gridMoveTimer = 0.0f;   // Current timer for grid movement cooldown
    
    // Dialogue interaction system
    std::shared_ptr<Entity> m_nearbyNPC = nullptr;  // NPC the player can interact with
    float m_interactionRange = 80.0f;  // Distance for NPC interaction (slightly more than one tile)
    sf::Text m_interactionPrompt;      // "Press E to talk" text
    bool m_showInteractionPrompt = false;
    
    sf::Text m_tileText;
    sf::Clock m_deltaClock;
    float m_deltaTime = 0.0f;

    void init(const std::string &levelPath);
    void init();
    void onEnd();
    void sAnimation();
    void sCamera();
    void sCollision();
    void sEnemySpawner();
    void sMovement();
    void sRender();
    void sDoAction(const Action& action);
    void sDebug();
    void spawnPlayer();
    
    // Dialogue interaction methods
    void sInteraction();  // Check for nearby NPCs and handle interaction prompts
    void startDialogue(std::shared_ptr<Entity> npc);  // Start dialogue with an NPC
    std::string getNPCDialogueFile(const std::string& npcName);  // Get dialogue file for NPC
    
    bool isColliding(const Vec2& pos1, const Vec2& size1, const Vec2& pos2, const Vec2& size2);
    bool wouldCollideAtPosition(const Vec2& position, const Vec2& size);
    Vec2 gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity);
public:
    Scene_Play(GameEngine* game, const std::string& levelPath);
    void update();
};
