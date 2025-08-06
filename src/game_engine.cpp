#include "game_engine.hpp"
#include "scene_menu.hpp"
#include "scene_play.hpp"
#include <iostream>
#include <exception>
#include <algorithm>

void GameEngine::init()
{
    try {
        // Create window with minimum size enforcement (480x640 minimum)
        int windowWidth = std::max(800, 480);   // Ensure minimum width of 480
        int windowHeight = std::max(600, 640);  // Ensure minimum height of 640
        
        m_window.create(sf::VideoMode(windowWidth, windowHeight), "Game Engine", sf::Style::Default);
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

        // Initialize viewport system with safety checks
        try {
            // Set up game view to be centered with zoom factor
            float scaledWidth = static_cast<float>(m_gameWidth) * m_zoomFactor;
            float scaledHeight = static_cast<float>(m_gameHeight) * m_zoomFactor;
            
            m_gameView.setSize(scaledWidth, scaledHeight);
            m_gameView.setCenter(scaledWidth / 2.0f, scaledHeight / 2.0f);
            
            // Initialize letterbox rectangles with default values first
            m_letterboxTop.setSize(sf::Vector2f(0.0f, 0.0f));
            m_letterboxTop.setPosition(0.0f, 0.0f);
            m_letterboxTop.setFillColor(sf::Color::Black);
            
            m_letterboxBottom.setSize(sf::Vector2f(0.0f, 0.0f));
            m_letterboxBottom.setPosition(0.0f, 0.0f);
            m_letterboxBottom.setFillColor(sf::Color::Black);
            
            m_letterboxLeft.setSize(sf::Vector2f(0.0f, 0.0f));
            m_letterboxLeft.setPosition(0.0f, 0.0f);
            m_letterboxLeft.setFillColor(sf::Color::Black);
            
            m_letterboxRight.setSize(sf::Vector2f(0.0f, 0.0f));
            m_letterboxRight.setPosition(0.0f, 0.0f);
            m_letterboxRight.setFillColor(sf::Color::Black);
            
            // Now calculate viewport safely
            calculateViewport();
        } catch (const std::exception& e) {
            std::cerr << "Failed to initialize viewport system: " << e.what() << std::endl;
            // Continue without viewport system
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
            // m_scenes["Play"] = std::make_shared<Scene_Play>(this, "metadata/level1.txt");

            if (m_scenes[m_currentScene]) {
                m_scenes[m_currentScene]->init();
            } else {
                std::cerr << "Failed to create initial scene" << std::endl;
                m_running = false;
            }
        } catch (const std::exception& e) {
            std::cerr << "Failed to create scenes: " << e.what() << std::endl;
            m_running = false;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Critical error in GameEngine::init(): " << e.what() << std::endl;
        m_running = false;
    } catch (...) {
        std::cerr << "Unknown critical error in GameEngine::init()" << std::endl;
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
    if (windowWidth <= 0.0f || windowHeight <= 0.0f || m_gameWidth <= 0 || m_gameHeight <= 0) {
        return;
    }
    
    // Calculate game aspect ratio and window aspect ratio
    float gameAspect = static_cast<float>(m_gameWidth) / static_cast<float>(m_gameHeight);
    float windowAspect = windowWidth / windowHeight;
    
    float viewportWidth, viewportHeight;
    float viewportX = 0.0f, viewportY = 0.0f;
    
    if (windowAspect > gameAspect) {
        // Window is wider than game - add pillarboxes (left/right black bars)
        // Game view height fills the window, width is calculated to maintain aspect ratio
        viewportHeight = windowHeight;
        viewportWidth = windowHeight * gameAspect;
        
        // Center horizontally
        viewportX = (windowWidth - viewportWidth) / 2.0f;
        viewportY = 0.0f;
        
        // Set up pillarbox rectangles (left and right black bars)
        m_letterboxLeft.setSize(sf::Vector2f(viewportX, windowHeight));
        m_letterboxLeft.setPosition(0.0f, 0.0f);
        
        m_letterboxRight.setSize(sf::Vector2f(viewportX, windowHeight));
        m_letterboxRight.setPosition(viewportX + viewportWidth, 0.0f);
        
        // Clear letterboxes (top/bottom) - set them to zero size but position them properly
        m_letterboxTop.setSize(sf::Vector2f(0.0f, 0.0f));
        m_letterboxTop.setPosition(0.0f, 0.0f);
        m_letterboxBottom.setSize(sf::Vector2f(0.0f, 0.0f));
        m_letterboxBottom.setPosition(0.0f, 0.0f);
        
    } else {
        // Window is taller than game - add letterboxes (top/bottom black bars)
        // Game view width fills the window, height is calculated to maintain aspect ratio
        viewportWidth = windowWidth;
        viewportHeight = windowWidth / gameAspect;
        
        // Center vertically
        viewportX = 0.0f;
        viewportY = (windowHeight - viewportHeight) / 2.0f;
        
        // Set up letterbox rectangles (top and bottom black bars)
        m_letterboxTop.setSize(sf::Vector2f(windowWidth, viewportY));
        m_letterboxTop.setPosition(0.0f, 0.0f);
        
        m_letterboxBottom.setSize(sf::Vector2f(windowWidth, viewportY));
        m_letterboxBottom.setPosition(0.0f, viewportY + viewportHeight);
        
        // Clear pillarboxes (left/right) - set them to zero size but position them properly
        m_letterboxLeft.setSize(sf::Vector2f(0.0f, 0.0f));
        m_letterboxLeft.setPosition(0.0f, 0.0f);
        m_letterboxRight.setSize(sf::Vector2f(0.0f, 0.0f));
        m_letterboxRight.setPosition(0.0f, 0.0f);
    }
    
    // Set the viewport for the game view (normalized coordinates 0.0 to 1.0)
    // This ensures the game view is perfectly centered in the calculated area
    sf::FloatRect viewport(
        viewportX / windowWidth,        // Left edge (normalized)
        viewportY / windowHeight,       // Top edge (normalized)
        viewportWidth / windowWidth,    // Width (normalized)
        viewportHeight / windowHeight   // Height (normalized)
    );
    
    m_gameView.setViewport(viewport);
    
    // Debug output to help troubleshoot
    std::cout << "Window: " << windowWidth << "x" << windowHeight 
              << " | Viewport: " << viewportX << "," << viewportY 
              << " " << viewportWidth << "x" << viewportHeight
              << " | Normalized: " << viewport.left << "," << viewport.top 
              << " " << viewport.width << "x" << viewport.height << std::endl;
}

void GameEngine::update()
{
    sf::Clock delta_clock;
    while (m_window.isOpen() && m_running)
    {
        try {
            ImGui::SFML::Update(m_window, delta_clock.restart());
            sUserInput();
            
            // Clear the entire window with black first
            m_window.clear(sf::Color::Black);
            
            // Reset to default view for drawing letterboxes (this ensures they cover the entire window)
            m_window.setView(m_window.getDefaultView());
            
            // Draw letterbox/pillarbox bars FIRST (in window coordinates)
            try {
                m_window.draw(m_letterboxTop);
                m_window.draw(m_letterboxBottom);
                m_window.draw(m_letterboxLeft);
                m_window.draw(m_letterboxRight);
            } catch (const std::exception& e) {
                // If letterbox drawing fails, continue without them
            }
            
            // Now set the game view for rendering game content
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
    m_zoomFactor = std::max(0.1f, std::min(zoomFactor, 2.0f));
    
    float scaledWidth = static_cast<float>(m_gameWidth) * m_zoomFactor;
    float scaledHeight = static_cast<float>(m_gameHeight) * m_zoomFactor;
    
    m_gameView.setSize(scaledWidth, scaledHeight);
    
    // Recalculate viewport to maintain centering
    calculateViewport();
}
