#include "scene_menu.hpp"
#include "scene_options.hpp"
#include "scene_play.hpp"
#include "game_engine.hpp"

void Scene_Options::init()
{
    registerAction(sf::Keyboard::W, "UP");
    registerAction(sf::Keyboard::S, "DOWN");
    registerAction(sf::Keyboard::Up, "UP");
    registerAction(sf::Keyboard::Down, "DOWN");
    registerAction(sf::Keyboard::Return, "SELECT");
    registerAction(sf::Keyboard::Escape, "QUIT");

    m_menuText.setFont(m_game->getAssets().getFont("ShareTech"));
    m_menuText.setFillColor(sf::Color::White);
    m_menuStrings.push_back("Full Screen");
    m_menuStrings.push_back("Back");
}

void Scene_Options::sRender()
{
    m_game->window().clear(sf::Color::Black);
    m_menuText.setString("Options");
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

void Scene_Options::sDoAction(const Action &action)
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
        }
        else if (action.getName() == "DOWN")
        {
            m_menuIndex = (m_menuIndex + 1) % m_menuStrings.size();
        }
        else if (action.getName() == "SELECT")
        {
            if (m_menuStrings[m_menuIndex] == "Back")
            {
                m_game->changeScene("Menu", std::make_shared<Scene_Menu>(m_game));
            }
            else if (m_menuStrings[m_menuIndex] == "Full Screen")
            {
                m_game->toggleFullscreen(m_game->window());
            }
        }
        else if (action.getName() == "QUIT")
        {
            m_game->changeScene("Menu", std::make_shared<Scene_Menu>(m_game));
        }
    }
}

Scene_Options::Scene_Options(GameEngine *game)
    : Scene(game) {}

void Scene_Options::update()
{
    sRender();
}

void Scene_Options::onEnd()
{
    m_currentScene = -1;
}
