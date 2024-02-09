#pragma once
#include <iostream>
#include <memory>
#include <fstream>
#include <vector>
#include <string>

#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "imgui-SFML.h"

#include "entity.hpp"
#include "assets.hpp"
#include "action.hpp"

class Scene;

class GameEngine 
{
protected:
    sf::RenderWindow m_window;
    std::map<std::string, std::shared_ptr<Scene>> m_scenes;
    Assets assets;
    std::string m_currentScene;
    bool m_running = true;

    void init();
    std::shared_ptr<Scene> currentScene();
    void run();
    void sUserInput();
public:
    sf::RenderWindow&  window();
    GameEngine() {
        // ImGui::SFML::Init(m_window);
        init();
    };
    ~GameEngine(){
        ImGui::SFML::Shutdown();
    };

    Assets& getAssets();
    void changeScene(const std::string& sceneName, std::shared_ptr<Scene> scene, bool endCurrentScene = true);
    void quit();
    void update();
};
