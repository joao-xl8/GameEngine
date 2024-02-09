#include "game_engine.hpp"
#include "scene_menu.hpp"
#include "scene_play.hpp"

void GameEngine::init()
{
    m_window.create(sf::VideoMode(800, 600), "Game Engine");
    if (ImGui::SFML::Init(m_window)) {
        m_running = true;
    } else {
        // Handle initialization failure
    }
    m_currentScene = "Play";

    assets.loadAssets("metadata/assets.txt");

    m_scenes["Menu"] = std::make_shared<Scene_Menu>(this);
    m_scenes["Play"] = std::make_shared<Scene_Play>(this, "metadata/level1.txt");

}

void GameEngine::update()
{
    sf::Clock delta_clock;
    while (m_window.isOpen() && m_running)
    {

        ImGui::SFML::Update(m_window, delta_clock.restart());
        sUserInput();
        // std::printf("Current scene: %s\n", m_currentScene.c_str());
        // std::printf("Current scene: %d\n",  m_scenes[m_currentScene]->width()); 
        m_scenes[m_currentScene]->update();

        ImGui::SFML::Render(m_window);
        m_window.display();
    }
}

void GameEngine::sUserInput()
{
    sf::Event event;
    while (m_window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            m_window.close();
        }
        if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased)
        {
            if(currentScene()->getActionMap().find(event.key.code) == currentScene()->getActionMap().end())
                continue;
            
            const std::string &actionType = event.type == sf::Event::KeyPressed ? "START" : "END";

            currentScene()->sDoAction(Action(currentScene()->getActionMap()[event.key.code], actionType));
        }
    }
}

std::shared_ptr<Scene> GameEngine::currentScene()
{
    return m_scenes[m_currentScene];
}

void GameEngine::run()
{
    while (m_window.isOpen() && m_running)
    {
        update();
    }
}

void GameEngine::changeScene(const std::string& sceneName, std::shared_ptr<Scene> scene, bool endCurrentScene)
{
    if (endCurrentScene)
    {
        m_scenes[m_currentScene]->onEnd();
    }
    m_currentScene = sceneName;
    m_scenes[m_currentScene] = scene;
    m_scenes[m_currentScene]->init();
}

void GameEngine::quit()
{
    m_running = false;
}

Assets &GameEngine::getAssets()
{
    return assets;
}

sf::RenderWindow &GameEngine::window()
{
    return m_window;
}
