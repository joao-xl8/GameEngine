#include <iostream>
#include <memory>
#include <fstream>
#include <vector>
#include <string>

#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "imgui-SFML.h"

#include "entity_manager.hpp"

const unsigned int INIT_WINDOW_WIDTH = 400;
const unsigned int INIT_WINDOW_HEIGHT = 400;

enum Options
{
    Window,
    Font,
    Player,
    Enemy,
    Bullet,
    Invalid
};
struct PlayerConfig { float SX, SY, SR, CR; int FR, FG, FB, OR, OG, OB, OT, V; };
struct EnemyConfig { float  SR, CR, SF; int FR, FG, FB, OR, OG, OB, OT, RV1, RV2, RS1, RS2, L; };
struct BulletConfig { float SX, SY, SR, CR; int FR, FG, FB, OR, OG, OB, OT, V, L; };

class Game 
{
    sf::RenderWindow m_window;
    EntityManager m_entities;
    int m_currentFrame = 0;
    bool m_paused = false;
    bool m_running = true;
    sf::Text m_text;
    std::shared_ptr<sf::Font> font;
    int font_size;
    sf::Color font_color;
    sf::Color bg_color;
    int window_width;
    int window_height;
    int frame_rate;
    bool full_screen;

    PlayerConfig m_playerConfig;
    EnemyConfig m_enemyConfig;
    BulletConfig m_bulletConfig;

    int m_score = 0;
    int m_lastEnemySpawnTime = 0;
    int m_lastSpecialTime = -500;
    std::chrono::seconds m_enemySpawnInterval = std::chrono::seconds(2);
    std::chrono::seconds m_specialCooldown = std::chrono::seconds(5);

    std::shared_ptr<Entity> m_player;

    void loadConfigFromFile(const std::string &filename);
    void setPaused(bool paused);

    void sMovement(EntityVec &entities);
    void sCollisions(EntityVec &entities);
    void getOutOfBounce(auto& e, float min_bound, float max_bound_width, float max_bound, int window_width, int window_height);
    void sRender(EntityVec &entities, sf::RenderWindow& win);
    void sUserInput(); 
    void sLifespan(EntityVec &entities);
    void sEnemySpawner();
    void sSmallEnemySpawner();
    void updateUI();
    void spawnPlayer();
    void spawnEnemy();
    void spawnSmallEnemies(std::shared_ptr<Entity> enemy);
    void spawnBullet(std::shared_ptr<Entity> player, const Vec2 &target);
    void spawnSpecialBullet(std::shared_ptr<Entity> player, const Vec2 &target);

    friend class InputManager;
public:
    void init();
    Game() : window_width{400}, window_height{400} {
        loadConfigFromFile("config.txt");
    };
    ~Game(){
        ImGui::SFML::Shutdown();
    };
    void update();
};
