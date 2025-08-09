#pragma once
#include <memory>
#include <string>
#include <stack>
#include <SFML/Graphics.hpp>
#include "components/engine_components.hpp"
#include "imgui-SFML.h"
#include "assets.hpp"

class Scene;

// Viewport configuration structure for easy management
struct ViewportConfig {
    // Resolution settings
    int windowWidth = 800;
    int windowHeight = 640;
    
    // Game view settings
    int gameWidth = 600;
    int gameHeight = 800;
    float zoomFactor = 0.9f;
    
    // Scaling mode
    enum ScalingMode {
        STRETCH,        // Stretch to fill window (no aspect ratio preservation)
        MAINTAIN_ASPECT, // Maintain aspect ratio with letterboxing
        FILL_WINDOW     // Fill entire window (may crop content)
    } scalingMode = FILL_WINDOW;
    
    // Background color (for letterboxing if used)
    sf::Color backgroundColor = sf::Color::Black;
};

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
    
    // Scene stack for proper scene return handling
    std::stack<std::string> m_sceneStack;
    
    // Timing
    sf::Clock m_deltaClock;
    float m_deltaTime = 0.0f;
    
    // Viewport configuration
    ViewportConfig m_viewportConfig;
    sf::View m_gameView;      // The game viewport
    
    // Sound settings
    float m_masterVolume = 1.0f;
    float m_musicVolume = 0.8f;
    float m_effectsVolume = 1.0f;
    bool m_soundEnabled = true;
    
    void calculateViewport(); // Calculate viewport based on configuration
    void applyViewportConfig(); // Apply current viewport configuration
    void loadSoundSettings(); // Load sound settings from file

    void init();
    std::shared_ptr<Scene> currentScene();
    void run();
    void sUserInput();

public:
    sf::RenderWindow &window();
    sf::View &getGameView() { return m_gameView; }  // Add method to access game view
    std::shared_ptr<CSound> getGlobalSoundManager() { return m_globalSoundManager; }  // Access global sound manager
    void setZoom(float zoomFactor);  // Add method to control zoom level
    float getDeltaTime() const { return m_deltaTime; }  // Get delta time for animations and timing
    
    // Sound settings getters
    float getMasterVolume() const { return m_masterVolume; }
    float getMusicVolume() const { return m_musicVolume; }
    float getEffectsVolume() const { return m_effectsVolume; }
    bool isSoundEnabled() const { return m_soundEnabled; }
    
    // Sound settings setter
    void updateSoundSettings(float master, float music, float effects, bool enabled);
    
    // Viewport configuration methods
    ViewportConfig& getViewportConfig() { return m_viewportConfig; }
    void setViewportConfig(const ViewportConfig& config);
    void setResolution(int width, int height);
    void setGameViewSize(int width, int height);
    void setScalingMode(ViewportConfig::ScalingMode mode);
    void updateViewport(); // Public method to update viewport
    void toggleFullscreen(); // Toggle fullscreen mode
    bool isFullscreen() const { return m_fullscreen; } // Get fullscreen state
    
    // Configuration file methods
    void loadScreenConfig();
    void saveScreenConfig();
    void saveScreenConfig(const ViewportConfig& config);
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
    void pushScene(const std::string &sceneName, std::shared_ptr<Scene> scene); // Push new scene, keeping current on stack
    void popScene(); // Return to previous scene
    void quit();
    void update();
};
