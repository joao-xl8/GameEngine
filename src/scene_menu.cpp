#include "scene_menu.hpp"
#include "scene_play.hpp"
#include "scene_options.hpp"
#include "game_engine.hpp"
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
    m_menuStrings.push_back("Options");
    m_menuStrings.push_back("Exit");
    
    // Background music is handled by global sound manager - already playing
    std::printf("Scene_Menu initialized (background music continues from global manager)\n");
}

void Scene_Menu::sRender()
{
    // GameEngine now handles window clearing - just draw menu elements
    m_menuText.setString("Menu");
    m_menuText.setCharacterSize(24);
    m_menuText.setFillColor(sf::Color::White);
    m_menuText.setPosition(100, 100);
    m_game->window().draw(m_menuText);
    for (int i = 0; i < m_menuStrings.size(); i++)
    {
        m_menuText.setString(m_menuStrings[i]);
        m_menuText.setCharacterSize(16);
        m_menuText.setPosition(100, 150 + i * 20);
        if (i == m_menuIndex)
        {
            m_menuText.setFillColor(sf::Color::Red);
        }
        else
        {
            m_menuText.setFillColor(sf::Color::White);
        }
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
                    // Use try-catch for Play scene creation
                    try {
                        m_game->changeScene("Play", std::make_shared<Scene_Play>(m_game, "metadata/level1.txt"));
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
