#pragma once
#include <memory>
#include <string>
#include <SFML/Graphics.hpp>
#include "components/engine_components.hpp"
#include "imgui-SFML.h"
#include "assets.hpp"

class Scene;

class GameEngine
{
protected:
    sf::RenderWindow m_window;
    std::map<std::string, std::shared_ptr<Scene>> m_scenes;
    Assets assets;
    std::shared_ptr<CSound> m_globalSoundManager;  // Global sound manager for persistent music
    std::string m_currentScene;
    bool m_running = true;
    bool m_fullscreen = false;
    
    // Viewport system for aspect ratio preservation
    int m_gameWidth = 600;   // Native game resolution width
    int m_gameHeight = 800;   // Native game resolution height
    float m_zoomFactor = 0.9f;  // Default 2x zoom (0.5 = half view size = 2x zoom)
    sf::View m_gameView;      // The game viewport
    sf::RectangleShape m_letterboxTop, m_letterboxBottom, m_letterboxLeft, m_letterboxRight; // Black bars
    
    void calculateViewport(); // Calculate viewport based on window size

    void init();
    std::shared_ptr<Scene> currentScene();
    void run();
    void sUserInput();

public:
    sf::RenderWindow &window();
    sf::View &getGameView() { return m_gameView; }  // Add method to access game view
    std::shared_ptr<CSound> getGlobalSoundManager() { return m_globalSoundManager; }  // Access global sound manager
    void setZoom(float zoomFactor);  // Add method to control zoom level
    GameEngine()
    {
        init();
    };
    ~GameEngine()
    {
        ImGui::SFML::Shutdown();
    };
    void toggleFullscreen(sf::RenderWindow& currentWindow);

    Assets &getAssets();
    void changeScene(const std::string &sceneName, std::shared_ptr<Scene> scene, bool endCurrentScene = true);
    void quit();
    void update();
};
