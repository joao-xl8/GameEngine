#include "scene_play.hpp"
#include "scene_menu.hpp"
#include "game_engine.hpp"
#include "components.hpp"
#include <sstream>
void Scene_Play::init(const std::string &levelPath)
{
    registerAction(sf::Keyboard::P, "PAUSE");
    registerAction(sf::Keyboard::Escape, "PAUSE");
    registerAction(sf::Keyboard::T, "TOGGLE_TEXTURE");
    registerAction(sf::Keyboard::C, "TOGGLE_COLLISION");
    registerAction(sf::Keyboard::G, "TOGGLE_GRID");

    m_tileText.setCharacterSize(16);
    m_tileText.setFont(m_game->getAssets().getFont("ShareTech"));
    m_tileText.setFillColor(sf::Color::White);

    // load level
    // Tile Ground 0 0
    // Tile Ground 1 0
    // Tile Ground 2 0
    // Tile Ground 3 0
    // Dec Bushing 0 1
    std::printf("Loading level: %s\n", levelPath.c_str());
    std::ifstream file(levelPath);
    if (!file.is_open())
    {
        std::cerr << "Failed to open level file: " << levelPath << std::endl;
        return;
    }
    std::string line;
    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string type;
        std::string spriteName;
        int x, y;
        ss >> type >> spriteName >> x >> y;
        if (type == "Tile")
        {
            auto e = m_entityManager.addEntity(type);
            e->addComponent<CTransform>(std::make_shared<CTransform>(Vec2{x * m_tileSize.x, y * m_tileSize.y}));
            e->addComponent<CSprite>(std::make_shared<CSprite>(spriteName, m_game->getAssets().getTexture(spriteName)));
            e->addComponent<CBoundingBox>(std::make_shared<CBoundingBox>(m_tileSize));
        }
        else if (type == "Dec")
        {
            auto e = m_entityManager.addEntity(type);
            e->addComponent<CTransform>(std::make_shared<CTransform>(Vec2{x * m_tileSize.x, y * m_tileSize.y}));
            e->addComponent<CSprite>(std::make_shared<CSprite>(spriteName, m_game->getAssets().getTexture(spriteName)));
        }
    }
    file.close();
    std::printf("Level loaded\n");
}

void Scene_Play::init()
{
    init(m_levelPath);
}

void Scene_Play::onEnd()
{
}

void Scene_Play::sAnimation()
{
    for (auto &entity : m_entityManager.getEntities())
    {
        // if (entity->hasComponent(ComponentType::ANIMATION))
        // {
        //     auto animation = entity->getComponent<Animation>(ComponentType::ANIMATION);
        //     animation->update();
        // }
    }
}

void Scene_Play::sCollision()
{
}

void Scene_Play::sEnemySpawner()
{
}

void Scene_Play::sMovement()
{
    for (auto &entity : m_entityManager.getEntities())
    {
        // if (entity->hasComponent(ComponentType::MOVEMENT))
        // {
        //     auto movement = entity->getComponent<Movement>(ComponentType::MOVEMENT);
        //     movement->update();
        // }
    }
}

void Scene_Play::sRender()
{
    m_game->window().clear(sf::Color::Cyan);
    if(m_drawTextures){
        for (auto &entity : m_entityManager.getEntities())
        {
            if (entity->hasComponent<CSprite>() && entity->hasComponent<CTransform>())
            {
                auto sprite = entity->getComponent<CSprite>();
                auto transform = entity->getComponent<CTransform>();
                // Calculate the bottom-left coordinate of the sprite
                float posX = transform->pos.x;
                float posY = m_game->window().getSize().y - transform->pos.y - sprite->sprite.getGlobalBounds().height; // Adjust posY
                sprite->sprite.setPosition(posX, posY);
                m_game->window().draw(sprite->sprite);
            }
        }
    }
    if (m_drawGrid)
    {
        // Calculate the bottom-left corner of the grid
        float gridBottom = m_game->window().getSize().y;

        for (size_t i = 0; i < 30; i++)
        {
            for (size_t j = 0; j < 30; j++)
            {
                float posX = j * m_tileSize.x;
                float posY = gridBottom - (i + 1) * m_tileSize.y; // Adjust the Y position

                m_tileText.setPosition(posX, posY);
                m_tileText.setString("(" + std::to_string(j) + ", " + std::to_string(i) + ")");
                m_game->window().draw(m_tileText);

                // draw the transparent square with white border for the tile
                sf::RectangleShape rect;
                rect.setSize({m_tileSize.x, m_tileSize.y});
                rect.setPosition(posX, posY);
                rect.setFillColor(sf::Color::Transparent);
                rect.setOutlineColor(sf::Color::White);
                rect.setOutlineThickness(1);
                m_game->window().draw(rect);
            }
        }
    }
    if(m_drawCollision){
        for (auto &entity : m_entityManager.getEntities())
        {
            if (entity->hasComponent<CBoundingBox>() && entity->hasComponent<CTransform>())
            {
                auto boundingBox = entity->getComponent<CBoundingBox>();
                auto transform = entity->getComponent<CTransform>();
                float posX = transform->pos.x;
                float posY = m_game->window().getSize().y - transform->pos.y - boundingBox->size.y; // Adjust posY
                sf::RectangleShape rect;
                rect.setSize({boundingBox->size.x, boundingBox->size.y});
                rect.setPosition(posX, posY);
                rect.setFillColor(sf::Color::Transparent);
                rect.setOutlineColor(sf::Color::Red);
                rect.setOutlineThickness(1);
                m_game->window().draw(rect);
            }
        }
    }
}

void Scene_Play::sDoAction(const Action &action)
{
    if (action.getType() == "START")
    {
        std::printf("Start action: %s\n", action.getName().c_str());
        if (action.getName() == "PAUSE")
        {
            m_game->changeScene("Menu", std::make_shared<Scene_Menu>(m_game));
        }
        else if (action.getName() == "TOGGLE_TEXTURE")
        {
            m_drawTextures = !m_drawTextures;
        }
        else if (action.getName() == "TOGGLE_COLLISION")
        {
            m_drawCollision = !m_drawCollision;
        }
        else if (action.getName() == "TOGGLE_GRID")
        {
            m_drawGrid = !m_drawGrid;
        }
    }
    else if (action.getType() == "END")
    {
        std::printf("End action: %s\n", action.getName().c_str());
    }
}

void Scene_Play::sDebug()
{
    // if (m_showGrid)
    // {
    //     // drawGrid();
    // }
}

Vec2 Scene_Play::gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity)
{
    if (entity->hasComponent<CTransform>() && entity->hasComponent<CBoundingBox>())
    {
        Vec2 pos = entity->getComponent<CTransform>()->pos;
        Vec2 scale = entity->getComponent<CTransform>()->scale;
        Vec2 size = entity->getComponent<CBoundingBox>()->size;
        return Vec2{gridX * m_tileSize.x + m_tileSize.x / 2 - size.x * scale.x / 2 + pos.x, gridY * m_tileSize.y + m_tileSize.y / 2 - size.y * scale.y / 2 + pos.y};
    }

    return Vec2{0, 0};
}

Scene_Play::Scene_Play(GameEngine *game, const std::string &levelPath)
    : Scene(game), m_levelPath(levelPath)
{}

void Scene_Play::update()
{
    m_entityManager.update();
    sMovement();
    sCollision();
    sEnemySpawner();
    sAnimation();
    sRender();
    sDebug();
}
