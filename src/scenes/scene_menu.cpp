#include "scene_menu.hpp"
#include "scene_play.hpp"
#include "scene_options.hpp"
#include "scene_map_editor.hpp"
#include "scene_loading.hpp"
#include "scene_viewport_config.hpp"
#include "../game_engine.hpp"
#include <exception>

void Scene_Menu::init()
{
    registerAction(sf::Keyboard::W, "UP");
    registerAction(sf::Keyboard::S, "DOWN");
    registerAction(sf::Keyboard::Up, "UP");
    registerAction(sf::Keyboard::Down, "DOWN");
    registerAction(sf::Keyboard::Return, "SELECT");
    registerAction(sf::Keyboard::Escape, "QUIT");

    m_menuText.setFont(m_game->getAssets().getFont("ShareTech"));
    m_menuText.setFillColor(sf::Color::White);
    m_menuStrings.push_back("Start Game");
    m_menuStrings.push_back("Map Editor");
    m_menuStrings.push_back("Options");
    m_menuStrings.push_back("Exit");
    
    // Background music is handled by global sound manager - already playing
    std::printf("Scene_Menu initialized (background music continues from global manager)\n");
}

void Scene_Menu::sRender()
{
    // Get game view for proper positioning
    sf::View gameView = m_game->getGameView();
    sf::Vector2f viewSize = gameView.getSize();
    sf::Vector2f viewCenter = gameView.getCenter();
    
    // Draw background
    sf::RectangleShape background;
    background.setSize(viewSize);
    background.setPosition(viewCenter.x - viewSize.x / 2, viewCenter.y - viewSize.y / 2);
    background.setFillColor(sf::Color(20, 20, 40));
    m_game->window().draw(background);
    
    // Draw title
    m_menuText.setString("GameEngine");
    m_menuText.setCharacterSize(32);
    m_menuText.setFillColor(sf::Color::White);
    
    sf::FloatRect titleBounds = m_menuText.getLocalBounds();
    m_menuText.setPosition(
        viewCenter.x - titleBounds.width / 2, 
        viewCenter.y - viewSize.y * 0.3f
    );
    m_game->window().draw(m_menuText);
    
    // Draw menu options
    float startY = viewCenter.y - viewSize.y * 0.1f;
    float spacing = viewSize.y * 0.06f;
    
    for (size_t i = 0; i < m_menuStrings.size(); i++)
    {
        m_menuText.setString(m_menuStrings[i]);
        m_menuText.setCharacterSize(20);
        
        if (i == m_menuIndex)
        {
            m_menuText.setFillColor(sf::Color::Yellow);
            m_menuText.setString("> " + m_menuStrings[i] + " <");
        }
        else
        {
            m_menuText.setFillColor(sf::Color::White);
        }
        
        sf::FloatRect bounds = m_menuText.getLocalBounds();
        m_menuText.setPosition(
            viewCenter.x - bounds.width / 2, 
            startY + i * spacing
        );
        m_game->window().draw(m_menuText);
    }
}

void Scene_Menu::sDoAction(const Action &action)
{
    if (action.getType() == "START")
    {
        if (action.getName() == "UP")
        {
            // Play menu navigation sound
            if (auto globalSound = m_game->getGlobalSoundManager()) {
                globalSound->playSound("menu_select", 60.0f);
            }
            
            if (m_menuIndex > 0)
            {
                m_menuIndex--;
            }
            else
            {
                m_menuIndex = m_menuStrings.size() - 1;
            }
        }
        else if (action.getName() == "DOWN")
        {
            // Play menu navigation sound
            if (auto globalSound = m_game->getGlobalSoundManager()) {
                globalSound->playSound("menu_select", 60.0f);
            }
            
            m_menuIndex = (m_menuIndex + 1) % m_menuStrings.size();
        }
        else if (action.getName() == "SELECT")
        {
            // Play menu confirm sound
            if (auto globalSound = m_game->getGlobalSoundManager()) {
                globalSound->playSound("menu_confirm", 80.0f);
            }
            
            // Add bounds checking to prevent crashes
            if (m_menuIndex >= 0 && m_menuIndex < m_menuStrings.size())
            {
                if (m_menuStrings[m_menuIndex] == "Exit")
                {
                    m_game->quit();
                }
                else if (m_menuStrings[m_menuIndex] == "Map Editor")
                {
                    // Use loading screen for Map Editor transition
                    try {
                        Scene_Loading::loadMapEditorScene(m_game);
                    } catch (const std::exception& e) {
                        // If scene creation fails, just quit to prevent crash
                        m_game->quit();
                    }
                }
                else if (m_menuStrings[m_menuIndex] == "Options")
                {
                    // Use try-catch to handle potential memory allocation failures
                    try {
                        m_game->changeScene("Options", std::make_shared<Scene_Options>(m_game));
                    } catch (const std::exception& e) {
                        // If scene creation fails, just quit to prevent crash
                        m_game->quit();
                    }
                }
                else
                {
                    // Use loading screen for Play scene transition
                    try {
                        Scene_Loading::loadPlayScene(m_game, "metadata/levels/level1.txt");
                    } catch (const std::exception& e) {
                        // If scene creation fails, just quit
                        m_game->quit();
                    }
                }
            }
        }
        else if (action.getName() == "QUIT")
        {
            m_game->quit();
        }
    }
}

Scene_Menu::Scene_Menu(GameEngine *game)
    : Scene(game) {}

void Scene_Menu::update()
{
    sRender();
}

void Scene_Menu::onEnd()
{
    m_currentScene = -1;
}
