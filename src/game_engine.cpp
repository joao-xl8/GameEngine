#include "game_engine.hpp"
#include "scenes/scene_menu.hpp"
#include <iostream>
#include <exception>
#include <algorithm>
#include <fstream>
#include <sstream>

void GameEngine::init()
{
    try {
        // Load screen configuration first
        loadScreenConfig();
        
        // Create window with size from loaded viewport config
        m_window.create(sf::VideoMode(m_viewportConfig.windowWidth, m_viewportConfig.windowHeight), "Game Engine", sf::Style::Default);
        if (!m_window.isOpen()) {
            std::cerr << "Failed to create SFML window" << std::endl;
            m_running = false;
            return;
        }

        // Initialize ImGui with error checking
        if (ImGui::SFML::Init(m_window)) {
            m_running = true;
        } else {
            std::cerr << "Failed to initialize ImGui" << std::endl;
            m_running = false;
            return;
        }

        m_currentScene = "Menu";

        // Initialize viewport system
        calculateViewport();

        // Initialize global sound manager
        try {
            m_globalSoundManager = std::make_shared<CSound>();
            
            // Load background music
            m_globalSoundManager->addMusic("background", "assets/music/time_for_adventure.mp3");
            
            // Load global sound effects
            m_globalSoundManager->addSound("menu_select", "assets/sounds/tap.wav");
            m_globalSoundManager->addSound("menu_confirm", "assets/sounds/jump.wav");
            
            // Start background music immediately
            m_globalSoundManager->playMusic("background", true, 25.0f); // Loop, 25% volume
            
            std::printf("Global sound system initialized with background music\n");
        } catch (const std::exception& e) {
            std::printf("Warning: Could not initialize global sound system: %s\n", e.what());
            // Create a dummy sound manager to prevent crashes
            m_globalSoundManager = std::make_shared<CSound>();
        }

        // Load assets with error checking
        try {
            assets.loadAssets("metadata/assets.txt");
        } catch (const std::exception& e) {
            std::cerr << "Failed to load assets: " << e.what() << std::endl;
            // Continue with default assets
        }

        // Create scenes with error checking
        try {
            m_scenes["Menu"] = std::make_shared<Scene_Menu>(this);
            currentScene()->init();
        } catch (const std::exception& e) {
            std::cerr << "Failed to initialize scenes: " << e.what() << std::endl;
            m_running = false;
        }

    } catch (const std::exception& e) {
        std::cerr << "Critical error during GameEngine initialization: " << e.what() << std::endl;
        m_running = false;
    }
}

void GameEngine::calculateViewport()
{
    // Safety check: ensure window is valid
    if (!m_window.isOpen()) {
        return;
    }
    
    // Get current window size
    sf::Vector2u windowSize = m_window.getSize();
    float windowWidth = static_cast<float>(windowSize.x);
    float windowHeight = static_cast<float>(windowSize.y);
    
    // Safety check: prevent division by zero
    if (windowWidth <= 0.0f || windowHeight <= 0.0f || m_viewportConfig.gameWidth <= 0 || m_viewportConfig.gameHeight <= 0) {
        return;
    }
    
    // Update viewport config window size to match actual window
    m_viewportConfig.windowWidth = static_cast<int>(windowWidth);
    m_viewportConfig.windowHeight = static_cast<int>(windowHeight);
    
    // Calculate scaled game view size
    float scaledWidth = static_cast<float>(m_viewportConfig.gameWidth) * m_viewportConfig.zoomFactor;
    float scaledHeight = static_cast<float>(m_viewportConfig.gameHeight) * m_viewportConfig.zoomFactor;
    
    // Set up game view based on scaling mode
    switch (m_viewportConfig.scalingMode) {
        case ViewportConfig::STRETCH:
            // Stretch to fill entire window (may distort aspect ratio)
            m_gameView.setSize(scaledWidth, scaledHeight);
            m_gameView.setCenter(scaledWidth / 2.0f, scaledHeight / 2.0f);
            m_gameView.setViewport(sf::FloatRect(0.0f, 0.0f, 1.0f, 1.0f));
            break;
            
        case ViewportConfig::MAINTAIN_ASPECT:
            // Maintain aspect ratio with letterboxing (original behavior)
            {
                float gameAspect = static_cast<float>(m_viewportConfig.gameWidth) / static_cast<float>(m_viewportConfig.gameHeight);
                float windowAspect = windowWidth / windowHeight;
                
                float viewportWidth, viewportHeight;
                float viewportX = 0.0f, viewportY = 0.0f;
                
                if (windowAspect > gameAspect) {
                    // Window is wider - add pillarboxes
                    viewportHeight = windowHeight;
                    viewportWidth = windowHeight * gameAspect;
                    viewportX = (windowWidth - viewportWidth) / 2.0f;
                } else {
                    // Window is taller - add letterboxes
                    viewportWidth = windowWidth;
                    viewportHeight = windowWidth / gameAspect;
                    viewportY = (windowHeight - viewportHeight) / 2.0f;
                }
                
                sf::FloatRect viewport(
                    viewportX / windowWidth,
                    viewportY / windowHeight,
                    viewportWidth / windowWidth,
                    viewportHeight / windowHeight
                );
                
                m_gameView.setSize(scaledWidth, scaledHeight);
                m_gameView.setCenter(scaledWidth / 2.0f, scaledHeight / 2.0f);
                m_gameView.setViewport(viewport);
            }
            break;
            
        case ViewportConfig::FILL_WINDOW:
        default:
            // Fill entire window (no letterboxing, may crop content)
            // Scale the game view to match window aspect ratio
            float windowAspect = windowWidth / windowHeight;
            float gameAspect = static_cast<float>(m_viewportConfig.gameWidth) / static_cast<float>(m_viewportConfig.gameHeight);
            
            if (windowAspect > gameAspect) {
                // Window is wider - expand game view width
                float adjustedWidth = scaledHeight * windowAspect;
                m_gameView.setSize(adjustedWidth, scaledHeight);
            } else {
                // Window is taller - expand game view height  
                float adjustedHeight = scaledWidth / windowAspect;
                m_gameView.setSize(scaledWidth, adjustedHeight);
            }
            
            m_gameView.setCenter(scaledWidth / 2.0f, scaledHeight / 2.0f);
            m_gameView.setViewport(sf::FloatRect(0.0f, 0.0f, 1.0f, 1.0f));
            break;
    }
    
    // Debug output
    sf::FloatRect viewport = m_gameView.getViewport();
    sf::Vector2f viewSize = m_gameView.getSize();
    std::cout << "Window: " << windowWidth << "x" << windowHeight 
              << " | Game View: " << viewSize.x << "x" << viewSize.y
              << " | Viewport: " << viewport.left << "," << viewport.top 
              << " " << viewport.width << "x" << viewport.height 
              << " | Mode: " << m_viewportConfig.scalingMode << std::endl;
}

void GameEngine::update()
{
    while (m_window.isOpen() && m_running)
    {
        try {
            // Update delta time
            sf::Time deltaTimeObj = m_deltaClock.restart();
            m_deltaTime = deltaTimeObj.asSeconds();
            
            ImGui::SFML::Update(m_window, deltaTimeObj);
            sUserInput();
            
            // Clear the entire window with background color
            m_window.clear(m_viewportConfig.backgroundColor);
            
            // Set the game view for rendering game content
            try {
                m_window.setView(m_gameView);
            } catch (const std::exception& e) {
                // If view setting fails, use default view
                m_window.setView(m_window.getDefaultView());
            }
            
            // Update and render the current scene (with safety checks)
            if (m_scenes.find(m_currentScene) != m_scenes.end() && m_scenes[m_currentScene]) {
                try {
                    m_scenes[m_currentScene]->update();
                } catch (const std::exception& e) {
                    std::cerr << "Error updating scene: " << e.what() << std::endl;
                    // Continue running but skip this frame
                }
            }
            
            // Reset to default view for ImGui
            m_window.setView(m_window.getDefaultView());

            ImGui::SFML::Render(m_window);
            m_window.display();
            
        } catch (const std::exception& e) {
            std::cerr << "Error in main loop: " << e.what() << std::endl;
            // Continue running but skip this frame
        } catch (...) {
            std::cerr << "Unknown error in main loop" << std::endl;
            // Continue running but skip this frame
        }
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
        
        // Handle window resize events
        if (event.type == sf::Event::Resized)
        {
            // Enforce minimum window size (480x640)
            sf::Vector2u currentSize = m_window.getSize();
            sf::Vector2u newSize = currentSize;
            bool needsResize = false;
            
            if (currentSize.x < 480) {
                newSize.x = 480;
                needsResize = true;
            }
            if (currentSize.y < 640) {
                newSize.y = 640;
                needsResize = true;
            }
            
            // If window is too small, resize it to minimum
            if (needsResize) {
                m_window.setSize(newSize);
            }
            
            // Recalculate viewport when window is resized
            calculateViewport();
        }
        
        if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased)
        {
            // Handle global fullscreen toggle (F11)
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F11) {
                toggleFullscreen();
                continue;
            }
            
            if (currentScene()->getActionMap().find(event.key.code) == currentScene()->getActionMap().end())
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

void GameEngine::changeScene(const std::string &sceneName, std::shared_ptr<Scene> scene, bool endCurrentScene)
{
    if (endCurrentScene)
    {
        m_scenes[m_currentScene]->onEnd();
    }
    m_currentScene = sceneName;
    m_scenes[m_currentScene] = scene;
    scene->init();
}

void GameEngine::quit()
{
    m_running = false;
}

void GameEngine::toggleFullscreen(sf::RenderWindow &currentWindow)
{
    try {
        m_fullscreen = !m_fullscreen;
        
        sf::VideoMode mode;
        if (m_fullscreen) {
            // Use fullscreen mode
            mode = sf::VideoMode::getFullscreenModes()[0];
        } else {
            // Use windowed mode with minimum size enforcement
            int windowWidth = std::max(800, 480);   // Ensure minimum width of 480
            int windowHeight = std::max(600, 640);  // Ensure minimum height of 640
            mode = sf::VideoMode(windowWidth, windowHeight);
        }
        
        currentWindow.close();
        currentWindow.create(mode, "Game Engine", m_fullscreen ? sf::Style::Fullscreen : sf::Style::Default);
        
        // Recalculate viewport after fullscreen toggle
        calculateViewport();
    } catch (const std::exception& e) {
        // If fullscreen toggle fails, ensure we have a valid window with minimum size
        if (!currentWindow.isOpen()) {
            int windowWidth = std::max(800, 480);   // Ensure minimum width of 480
            int windowHeight = std::max(600, 640);  // Ensure minimum height of 640
            currentWindow.create(sf::VideoMode(windowWidth, windowHeight), "Game Engine", sf::Style::Default);
            m_fullscreen = false;
            calculateViewport();
        }
    }
}

Assets &GameEngine::getAssets()
{
    return assets;
}

sf::RenderWindow &GameEngine::window()
{
    return m_window;
}

void GameEngine::setZoom(float zoomFactor)
{
    // Clamp zoom factor to reasonable values
    m_viewportConfig.zoomFactor = std::max(0.1f, std::min(zoomFactor, 2.0f));
    calculateViewport();
}

// Viewport configuration methods
void GameEngine::setViewportConfig(const ViewportConfig& config)
{
    m_viewportConfig = config;
    
    // Save configuration to file
    saveScreenConfig(config);
    
    // Recreate window if size changed
    if (m_window.isOpen()) {
        sf::Vector2u currentSize = m_window.getSize();
        if (currentSize.x != static_cast<unsigned int>(config.windowWidth) || 
            currentSize.y != static_cast<unsigned int>(config.windowHeight)) {
            
            m_window.create(sf::VideoMode(config.windowWidth, config.windowHeight), "Game Engine");
            m_window.setFramerateLimit(60);
        }
    }
    
    calculateViewport();
}

void GameEngine::setResolution(int width, int height)
{
    m_viewportConfig.windowWidth = width;
    m_viewportConfig.windowHeight = height;
    
    if (m_window.isOpen()) {
        m_window.create(sf::VideoMode(width, height), "Game Engine");
        m_window.setFramerateLimit(60);
        calculateViewport();
    }
}

void GameEngine::setGameViewSize(int width, int height)
{
    m_viewportConfig.gameWidth = width;
    m_viewportConfig.gameHeight = height;
    calculateViewport();
}

void GameEngine::setScalingMode(ViewportConfig::ScalingMode mode)
{
    m_viewportConfig.scalingMode = mode;
    calculateViewport();
}

void GameEngine::updateViewport()
{
    calculateViewport();
}

void GameEngine::loadScreenConfig()
{
    std::ifstream configFile("metadata/screen_config.txt");
    if (!configFile.is_open()) {
        std::cout << "Screen config file not found, using defaults" << std::endl;
        return;
    }
    
    std::string line;
    while (std::getline(configFile, line)) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        std::istringstream iss(line);
        std::string setting;
        if (!(iss >> setting)) {
            continue;
        }
        
        if (setting == "WINDOW_WIDTH") {
            int value;
            if (iss >> value) {
                m_viewportConfig.windowWidth = value;
            }
        } else if (setting == "WINDOW_HEIGHT") {
            int value;
            if (iss >> value) {
                m_viewportConfig.windowHeight = value;
            }
        } else if (setting == "GAME_WIDTH") {
            int value;
            if (iss >> value) {
                m_viewportConfig.gameWidth = value;
            }
        } else if (setting == "GAME_HEIGHT") {
            int value;
            if (iss >> value) {
                m_viewportConfig.gameHeight = value;
            }
        } else if (setting == "ZOOM_FACTOR") {
            float value;
            if (iss >> value) {
                m_viewportConfig.zoomFactor = value;
            }
        } else if (setting == "SCALING_MODE") {
            int value;
            if (iss >> value) {
                m_viewportConfig.scalingMode = static_cast<ViewportConfig::ScalingMode>(value);
            }
        } else if (setting == "BACKGROUND_COLOR_R") {
            int value;
            if (iss >> value) {
                m_viewportConfig.backgroundColor.r = static_cast<sf::Uint8>(value);
            }
        } else if (setting == "BACKGROUND_COLOR_G") {
            int value;
            if (iss >> value) {
                m_viewportConfig.backgroundColor.g = static_cast<sf::Uint8>(value);
            }
        } else if (setting == "BACKGROUND_COLOR_B") {
            int value;
            if (iss >> value) {
                m_viewportConfig.backgroundColor.b = static_cast<sf::Uint8>(value);
            }
        }
    }
    
    configFile.close();
    std::cout << "Screen configuration loaded: " << m_viewportConfig.windowWidth << "x" << m_viewportConfig.windowHeight 
              << ", mode: " << m_viewportConfig.scalingMode << ", zoom: " << m_viewportConfig.zoomFactor << std::endl;
}

void GameEngine::saveScreenConfig()
{
    saveScreenConfig(m_viewportConfig);
}

void GameEngine::saveScreenConfig(const ViewportConfig& config)
{
    std::ofstream configFile("metadata/screen_config.txt");
    if (!configFile.is_open()) {
        std::cerr << "Failed to save screen configuration file" << std::endl;
        return;
    }
    
    configFile << "# Screen Configuration File\n";
    configFile << "# This file stores display and viewport settings for the game\n";
    configFile << "# Format: SETTING_NAME VALUE\n\n";
    
    configFile << "# Window resolution\n";
    configFile << "WINDOW_WIDTH " << config.windowWidth << "\n";
    configFile << "WINDOW_HEIGHT " << config.windowHeight << "\n\n";
    
    configFile << "# Game view settings\n";
    configFile << "GAME_WIDTH " << config.gameWidth << "\n";
    configFile << "GAME_HEIGHT " << config.gameHeight << "\n";
    configFile << "ZOOM_FACTOR " << config.zoomFactor << "\n\n";
    
    configFile << "# Scaling mode: 0=STRETCH, 1=MAINTAIN_ASPECT, 2=FILL_WINDOW\n";
    configFile << "SCALING_MODE " << static_cast<int>(config.scalingMode) << "\n\n";
    
    configFile << "# Background color (RGB values 0-255)\n";
    configFile << "BACKGROUND_COLOR_R " << static_cast<int>(config.backgroundColor.r) << "\n";
    configFile << "BACKGROUND_COLOR_G " << static_cast<int>(config.backgroundColor.g) << "\n";
    configFile << "BACKGROUND_COLOR_B " << static_cast<int>(config.backgroundColor.b) << "\n";
    
    configFile.close();
    std::cout << "Screen configuration saved: " << config.windowWidth << "x" << config.windowHeight 
              << ", mode: " << config.scalingMode << ", zoom: " << config.zoomFactor << std::endl;
}

void GameEngine::toggleFullscreen()
{
    m_fullscreen = !m_fullscreen;
    
    if (m_fullscreen) {
        // Get desktop resolution for fullscreen
        sf::VideoMode desktopMode = sf::VideoMode::getDesktopMode();
        m_window.create(desktopMode, "Game Engine", sf::Style::Fullscreen);
    } else {
        // Return to windowed mode with configured resolution
        m_window.create(sf::VideoMode(m_viewportConfig.windowWidth, m_viewportConfig.windowHeight), 
                       "Game Engine", sf::Style::Default);
    }
    
    m_window.setFramerateLimit(60);
    calculateViewport();
    
    std::cout << "Toggled to " << (m_fullscreen ? "fullscreen" : "windowed") << " mode" << std::endl;
}
