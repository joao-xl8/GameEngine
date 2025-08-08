#include "scene_viewport_config.hpp"
#include "scene_menu.hpp"
#include "scene_options.hpp"
#include "../game_engine.hpp"
#include <sstream>
#include <iomanip>

Scene_ScreenConfig::Scene_ScreenConfig(GameEngine* game) : Scene(game)
{
    // Initialize resolution options
    m_resolutions = {
        {800, 600},
        {1024, 768},
        {1280, 720},
        {1280, 1024},
        {1366, 768},
        {1600, 900},
        {1920, 1080}
    };
    
    // Initialize scaling mode options with user-friendly names
    m_scalingModes = {
        "Fill Screen",
        "Keep Aspect Ratio",
        "Stretch to Fit"
    };
    
    // Initialize menu options with user-friendly labels
    m_menuOptions = {
        "Screen Resolution",
        "Display Mode", 
        "Zoom Level",
        "Fullscreen Toggle",
        "Apply Changes",
        "Reset to Default",
        "Back to Options"
    };
    
    // Get current configuration
    ViewportConfig& config = m_game->getViewportConfig();
    
    // Find current resolution
    for (size_t i = 0; i < m_resolutions.size(); i++) {
        if (m_resolutions[i].first == config.windowWidth && 
            m_resolutions[i].second == config.windowHeight) {
            m_selectedResolution = i;
            break;
        }
    }
    
    // Set current scaling mode
    m_selectedScalingMode = static_cast<size_t>(config.scalingMode);
    m_zoomFactor = config.zoomFactor;
}

void Scene_ScreenConfig::init()
{
    // Register standardized input actions
    // Movement controls (WASD + Arrow keys)
    registerAction(sf::Keyboard::W, "UP");
    registerAction(sf::Keyboard::Up, "UP");
    registerAction(sf::Keyboard::S, "DOWN");
    registerAction(sf::Keyboard::Down, "DOWN");
    registerAction(sf::Keyboard::A, "LEFT");
    registerAction(sf::Keyboard::Left, "LEFT");
    registerAction(sf::Keyboard::D, "RIGHT");
    registerAction(sf::Keyboard::Right, "RIGHT");
    
    // Confirm actions (Space/Enter)
    registerAction(sf::Keyboard::Space, "CONFIRM");
    registerAction(sf::Keyboard::Enter, "CONFIRM");
    
    // Cancel actions (Backspace/C/Escape)
    registerAction(sf::Keyboard::Backspace, "CANCEL");
    registerAction(sf::Keyboard::C, "CANCEL");
    registerAction(sf::Keyboard::Escape, "BACK");
    
    setupUI();
}

void Scene_ScreenConfig::setupUI()
{
    // Title
    m_titleText.setFont(m_game->getAssets().getFont("ShareTech"));
    m_titleText.setCharacterSize(34);  // Increased from 32 to 34
    m_titleText.setFillColor(sf::Color::White);
    m_titleText.setString("Screen Settings");
    
    // Instructions
    m_instructionsText.setFont(m_game->getAssets().getFont("ShareTech"));
    m_instructionsText.setCharacterSize(18);  // Increased from 16 to 18
    m_instructionsText.setFillColor(sf::Color(200, 200, 200));
    m_instructionsText.setString("Fullscreen mode uses the configured resolution above");
    
    // Current config text
    m_currentConfigText.setFont(m_game->getAssets().getFont("ShareTech"));
    m_currentConfigText.setCharacterSize(16);  // Increased from 14 to 16
    m_currentConfigText.setFillColor(sf::Color(150, 150, 255));
    
    updateOptionTexts();
}

void Scene_ScreenConfig::updateOptionTexts()
{
    m_optionTexts.clear();
    
    for (size_t i = 0; i < m_menuOptions.size(); i++) {
        sf::Text optionText;
        optionText.setFont(m_game->getAssets().getFont("ShareTech"));
        optionText.setCharacterSize(22);  // Increased from 20 to 22
        
        std::string displayText = m_menuOptions[i];
        
        // Add current values for configurable options
        if (i == 0) { // Screen Resolution
            displayText += ": " + std::to_string(m_resolutions[m_selectedResolution].first) + 
                          "x" + std::to_string(m_resolutions[m_selectedResolution].second);
        } else if (i == 1) { // Display Mode
            displayText += ": " + m_scalingModes[m_selectedScalingMode];
        } else if (i == 2) { // Zoom Level
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(1) << m_zoomFactor;
            displayText += ": " + oss.str() + "x";
        } else if (i == 3) { // Fullscreen Toggle
            bool isFullscreen = m_game->isFullscreen();
            displayText += ": " + std::string(isFullscreen ? "ON" : "OFF") + " (Press to toggle)";
        }
        
        // Highlight selected option
        if (i == m_selectedOption) {
            optionText.setFillColor(sf::Color::Yellow);
            displayText = "> " + displayText + " <";
        } else {
            optionText.setFillColor(sf::Color::White);
        }
        
        optionText.setString(displayText);
        m_optionTexts.push_back(optionText);
    }
    
    // Update current config display
    m_currentConfigText.setString(getCurrentConfigString());
}

std::string Scene_ScreenConfig::getCurrentConfigString()
{
    ViewportConfig& config = m_game->getViewportConfig();
    
    std::ostringstream oss;
    oss << "Current Settings: " << config.windowWidth << "x" << config.windowHeight;
    oss << " | " << m_scalingModes[static_cast<size_t>(config.scalingMode)];
    oss << " | " << std::fixed << std::setprecision(1) << config.zoomFactor << "x zoom";
    
    return oss.str();
}

void Scene_ScreenConfig::update()
{
    updateOptionTexts();
    
    // Use absolute positioning based on game view
    sf::View gameView = m_game->getGameView();
    sf::Vector2f viewSize = gameView.getSize();
    sf::Vector2f viewCenter = gameView.getCenter();
    
    // Calculate positions relative to view bounds
    float topEdge = viewCenter.y - viewSize.y / 2;
    float bottomEdge = viewCenter.y + viewSize.y / 2;
    
    // Title positioning - top 15% of screen
    sf::FloatRect titleBounds = m_titleText.getLocalBounds();
    float titleY = topEdge + viewSize.y * 0.15f;
    m_titleText.setPosition(
        viewCenter.x - titleBounds.width / 2, 
        titleY
    );
    
    // Instructions positioning - below title
    sf::FloatRect instrBounds = m_instructionsText.getLocalBounds();
    float instrY = topEdge + viewSize.y * 0.25f;
    m_instructionsText.setPosition(
        viewCenter.x - instrBounds.width / 2, 
        instrY
    );
    
    // Menu options positioning - center area
    float startY = topEdge + viewSize.y * 0.4f;
    float spacing = viewSize.y * 0.08f; // 8% of view height for spacing
    
    for (size_t i = 0; i < m_optionTexts.size(); i++) {
        sf::FloatRect bounds = m_optionTexts[i].getLocalBounds();
        float optionY = startY + i * spacing;
        m_optionTexts[i].setPosition(
            viewCenter.x - bounds.width / 2, 
            optionY
        );
    }
    
    // Current config positioning - bottom area
    sf::FloatRect configBounds = m_currentConfigText.getLocalBounds();
    float configY = bottomEdge - viewSize.y * 0.15f;
    m_currentConfigText.setPosition(
        viewCenter.x - configBounds.width / 2, 
        configY
    );
    
    // Call render method
    sRender();
}

void Scene_ScreenConfig::sDoAction(const Action& action)
{
    if (action.getType() == "START") {
        if (action.getName() == "UP") {
            if (m_selectedOption > 0) {
                m_selectedOption--;
            } else {
                m_selectedOption = m_menuOptions.size() - 1;
            }
        } else if (action.getName() == "DOWN") {
            m_selectedOption = (m_selectedOption + 1) % m_menuOptions.size();
        } else if (action.getName() == "LEFT") {
            // Adjust current option value
            if (m_selectedOption == 0) { // Resolution
                if (m_selectedResolution > 0) {
                    m_selectedResolution--;
                }
            } else if (m_selectedOption == 1) { // Scaling Mode
                if (m_selectedScalingMode > 0) {
                    m_selectedScalingMode--;
                }
            } else if (m_selectedOption == 2) { // Zoom Factor
                m_zoomFactor = std::max(0.1f, m_zoomFactor - 0.1f);
            }
        } else if (action.getName() == "RIGHT") {
            // Adjust current option value
            if (m_selectedOption == 0) { // Resolution
                if (m_selectedResolution < m_resolutions.size() - 1) {
                    m_selectedResolution++;
                }
            } else if (m_selectedOption == 1) { // Scaling Mode
                if (m_selectedScalingMode < m_scalingModes.size() - 1) {
                    m_selectedScalingMode++;
                }
            } else if (m_selectedOption == 2) { // Zoom Factor
                m_zoomFactor = std::min(2.0f, m_zoomFactor + 0.1f);
            }
        } else if (action.getName() == "CONFIRM") {
            if (m_selectedOption == 3) { // Fullscreen Toggle
                try {
                    m_game->toggleFullscreen(m_game->window());
                } catch (const std::exception& e) {
                    // Ignore fullscreen toggle errors
                }
            } else if (m_selectedOption == 4) { // Apply Changes
                applyConfiguration();
            } else if (m_selectedOption == 5) { // Reset to Default
                ViewportConfig defaultConfig;
                m_game->setViewportConfig(defaultConfig);
                
                // Update local values
                m_selectedResolution = 0;
                m_selectedScalingMode = static_cast<size_t>(defaultConfig.scalingMode);
                m_zoomFactor = defaultConfig.zoomFactor;
            } else if (m_selectedOption == 6) { // Back to Options
                m_game->changeScene("Options", std::make_shared<Scene_Options>(m_game));
            }
        } else if (action.getName() == "BACK" || action.getName() == "CANCEL") {
            m_game->changeScene("Options", std::make_shared<Scene_Options>(m_game));
        }
    }
}

void Scene_ScreenConfig::applyConfiguration()
{
    ViewportConfig config = m_game->getViewportConfig();
    
    // Update configuration
    config.windowWidth = m_resolutions[m_selectedResolution].first;
    config.windowHeight = m_resolutions[m_selectedResolution].second;
    config.scalingMode = static_cast<ViewportConfig::ScalingMode>(m_selectedScalingMode);
    config.zoomFactor = m_zoomFactor;
    
    // Apply to game engine
    m_game->setViewportConfig(config);
    
    std::cout << "Applied viewport configuration: " 
              << config.windowWidth << "x" << config.windowHeight
              << " | Mode: " << m_selectedScalingMode 
              << " | Zoom: " << m_zoomFactor << std::endl;
}

void Scene_ScreenConfig::sRender()
{
    // Draw a full-screen background
    sf::RectangleShape background;
    background.setSize(sf::Vector2f(2000, 2000)); // Large enough to cover any screen
    background.setPosition(-1000, -1000); // Position it to cover everything
    background.setFillColor(sf::Color(30, 30, 50));
    m_game->window().draw(background);
    
    // Draw UI elements
    m_game->window().draw(m_titleText);
    m_game->window().draw(m_instructionsText);
    
    for (const auto& optionText : m_optionTexts) {
        m_game->window().draw(optionText);
    }
    
    m_game->window().draw(m_currentConfigText);
    
    // Draw command overlay (always on top)
    renderCommandOverlay();
}

void Scene_ScreenConfig::onEnd()
{
    std::cout << "Screen configuration scene ended" << std::endl;
}
