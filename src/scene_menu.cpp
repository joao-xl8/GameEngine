#include "scene_menu.hpp"
#include "scene_play.hpp"
#include "game_engine.hpp"

void Scene_Menu::init()
{
    registerAction(sf::Keyboard::W, "UP");
    registerAction(sf::Keyboard::S, "DOWN");
    registerAction(sf::Keyboard::Return, "SELECT");
    registerAction(sf::Keyboard::Escape, "QUIT");

    m_menuText.setFont(m_game->getAssets().getFont("ShareTech"));
    m_menuText.setFillColor(sf::Color::White);
    m_menuStrings.push_back("Start");
    m_menuStrings.push_back("Exit");
}

void Scene_Menu::sRender()
{
    m_game->window().clear(sf::Color::Black);
    m_menuText.setString("Menu");
    m_menuText.setCharacterSize(24);
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
            if (m_menuIndex > 0)
            {
                m_menuIndex--;
            }
            else
            {
                m_menuIndex = m_menuStrings.size() - 1;
            }
        } else if(action.getName() == "DOWN")
        {
            m_menuIndex = (m_menuIndex + 1) % m_menuStrings.size();
        }   
        else if (action.getName() == "SELECT")
        {
            if (m_menuStrings[m_menuIndex] == "Exit")
            {
                m_game->quit();
            } else {
                m_game->changeScene("Play", std::make_shared<Scene_Play>(m_game, "metadata/level1.txt"));
            }
        } else if (action.getName() == "QUIT")
        {
            m_game->quit();
        }
    }
}

Scene_Menu::Scene_Menu(GameEngine *game)
    : Scene(game) {}


void Scene_Menu::update()
{
    // std::printf("Scene_Menu::update()\n");
    sRender();
}

void Scene_Menu::onEnd()
{
    m_currentScene = -1;
}
