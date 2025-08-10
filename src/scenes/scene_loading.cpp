#include "scene_loading.hpp"
#include "scene_play.hpp"
#include "scene_menu.hpp"
#include "scene_map_editor.hpp"
#include "../game_engine.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <thread>
#include <chrono>

Scene_Loading::Scene_Loading(GameEngine* game, const std::string& nextSceneName,
                           std::function<std::shared_ptr<Scene>()> sceneFactory,
                           const std::vector<std::string>& assetsToLoad,
                           const std::vector<std::string>& soundsToLoad)
    : Scene(game)
    , m_nextSceneName(nextSceneName)
    , m_sceneFactory(sceneFactory)
    , m_assetsToLoad(assetsToLoad)
    , m_soundsToLoad(soundsToLoad)
{
    m_totalAssets = m_assetsToLoad.size() + m_soundsToLoad.size();
    std::cout << "Loading screen initialized for " << nextSceneName 
              << " with " << m_totalAssets << " assets to load" << std::endl;
    
    // Try to load additional assets from configuration file
    loadAssetsFromConfig(nextSceneName);
}

void Scene_Loading::init()
{
    // Register input for skipping loading screen
    registerAction(sf::Keyboard::Space, "SKIP");
    
    setupVisuals();
    
    // Start loading assets immediately
    if (m_totalAssets > 0) {
        loadNextAsset();
    } else {
        // No assets to load, create scene immediately
        m_nextScene = m_sceneFactory();
        m_loadingComplete = true;
    }
}

void Scene_Loading::setupVisuals()
{
    // Get the game view for proper positioning (same approach as menu scene)
    sf::View gameView = m_game->getGameView();
    sf::Vector2f viewSize = gameView.getSize();
    sf::Vector2f viewCenter = gameView.getCenter();
    
    // Set up background using the same approach as menu scene
    m_background.setSize(viewSize);
    m_background.setPosition(viewCenter.x - viewSize.x / 2, viewCenter.y - viewSize.y / 2);
    m_background.setFillColor(sf::Color(25, 35, 50)); // Dark blue-gray background
    
    // Set up loading text
    m_loadingText.setFont(m_game->getAssets().getFont("ShareTech"));
    m_loadingText.setCharacterSize(36);
    m_loadingText.setFillColor(sf::Color::White);
    m_loadingText.setString("Loading...");
    
    // Center loading text in game view
    sf::FloatRect textBounds = m_loadingText.getLocalBounds();
    m_loadingText.setPosition(
        viewCenter.x - textBounds.width / 2,
        viewCenter.y - 80
    );
    
    // Set up progress text
    m_progressText.setFont(m_game->getAssets().getFont("ShareTech"));
    m_progressText.setCharacterSize(18);
    m_progressText.setFillColor(sf::Color(180, 180, 200));
    
    // Set up progress bar in game view coordinates
    float barWidth = std::min(400.0f, viewSize.x * 0.6f); // Responsive width
    float barHeight = 24;
    float barX = viewCenter.x - barWidth / 2;
    float barY = viewCenter.y + 20;
    
    m_progressBarBackground.setSize(sf::Vector2f(barWidth, barHeight));
    m_progressBarBackground.setPosition(barX, barY);
    m_progressBarBackground.setFillColor(sf::Color(40, 40, 60));
    m_progressBarBackground.setOutlineColor(sf::Color(120, 120, 140));
    m_progressBarBackground.setOutlineThickness(2);
    
    m_progressBarFill.setSize(sf::Vector2f(0, barHeight));
    m_progressBarFill.setPosition(barX, barY);
    m_progressBarFill.setFillColor(sf::Color(80, 160, 220)); // Nice blue fill
    
    updateProgress();
}

void Scene_Loading::loadNextAsset()
{
    if (m_currentAssetIndex < m_assetsToLoad.size()) {
        // Load texture asset
        std::string assetName = m_assetsToLoad[m_currentAssetIndex];
        std::cout << "Loading texture: " << assetName << std::endl;
        
        try {
            // Force load the texture (this will cache it in Assets)
            m_game->getAssets().getTexture(assetName);
            std::cout << "✓ Loaded texture: " << assetName << std::endl;
        } catch (const std::exception& e) {
            std::cout << "✗ Failed to load texture: " << assetName << " - " << e.what() << std::endl;
        }
        
    } else if (m_currentAssetIndex < m_assetsToLoad.size() + m_soundsToLoad.size()) {
        // Load sound asset
        size_t soundIndex = m_currentAssetIndex - m_assetsToLoad.size();
        std::string soundName = m_soundsToLoad[soundIndex];
        std::cout << "Loading sound: " << soundName << std::endl;
        
        try {
            // Force load the sound (this will cache it in Assets)
            m_game->getAssets().getSoundBuffer(soundName);
            std::cout << "✓ Loaded sound: " << soundName << std::endl;
        } catch (const std::exception& e) {
            std::cout << "✗ Failed to load sound: " << soundName << " - " << e.what() << std::endl;
        }
    }
    
    m_currentAssetIndex++;
    updateProgress();
    
    // Check if all assets are loaded
    if (m_currentAssetIndex >= m_totalAssets) {
        std::cout << "All assets loaded! Creating next scene..." << std::endl;
        m_nextScene = m_sceneFactory();
        m_loadingComplete = true;
    }
}

void Scene_Loading::updateProgress()
{
    if (m_totalAssets == 0) {
        m_progressText.setString("Initializing...");
        return;
    }
    
    float progress = static_cast<float>(m_currentAssetIndex) / static_cast<float>(m_totalAssets);
    int percentage = static_cast<int>(progress * 100);
    
    // Update progress text
    std::string progressStr = "Loading assets... " + std::to_string(m_currentAssetIndex) + 
                             "/" + std::to_string(m_totalAssets) + " (" + 
                             std::to_string(percentage) + "%)";
    m_progressText.setString(progressStr);
    
    // Center progress text in game view
    sf::View gameView = m_game->getGameView();
    sf::Vector2f viewSize = gameView.getSize();
    sf::Vector2f viewCenter = gameView.getCenter();
    sf::FloatRect textBounds = m_progressText.getLocalBounds();
    m_progressText.setPosition(
        viewCenter.x - textBounds.width / 2,
        viewCenter.y + 60
    );
    
    // Update progress bar with responsive width
    float barWidth = std::min(400.0f, viewSize.x * 0.6f); // Match setupVisuals
    float fillWidth = barWidth * progress;
    m_progressBarFill.setSize(sf::Vector2f(fillWidth, 24)); // Match height from setupVisuals
}

void Scene_Loading::update()
{
    m_loadingTimer += m_game->getDeltaTime();
    
    // Continue loading assets if not complete
    if (!m_loadingComplete && m_currentAssetIndex < m_totalAssets) {
        // Load one asset per frame to avoid blocking
        loadNextAsset();
    }
    
    // Call render method (this is required since GameEngine doesn't call sRender separately)
    sRender();
    
    // Transition to next scene when loading is complete and minimum time has passed
    if (m_loadingComplete && m_loadingTimer >= m_minLoadingTime && m_nextScene) {
        std::cout << "Loading complete! Transitioning to " << m_nextSceneName << std::endl;
        m_game->changeScene(m_nextSceneName, m_nextScene);
    }
}

void Scene_Loading::sDoAction(const Action& action)
{
    // Allow skipping loading screen with any key (but only after minimum time)
    if (action.getType() == "START" && (action.getName() == "SKIP") && 
        m_loadingComplete && m_loadingTimer >= m_minLoadingTime) {
        if (m_nextScene) {
            std::cout << "Loading screen skipped by user input" << std::endl;
            m_game->changeScene(m_nextSceneName, m_nextScene);
        }
    }
}

void Scene_Loading::sRender()
{
    // Draw background
    m_game->window().draw(m_background);
    
    // Draw loading text
    m_game->window().draw(m_loadingText);
    
    // Draw progress text
    m_game->window().draw(m_progressText);
    
    // Draw progress bar
    m_game->window().draw(m_progressBarBackground);
    m_game->window().draw(m_progressBarFill);
    
    // Add a smooth loading animation (pulsing dots)
    static float animTimer = 0;
    animTimer += m_game->getDeltaTime();
    
    // Create animated dots with different phases
    sf::View gameView = m_game->getGameView();
    sf::Vector2f viewCenter = gameView.getCenter();
    
    for (int i = 0; i < 3; i++) {
        sf::CircleShape dot(4);
        float phase = animTimer * 3.0f + i * 0.5f;
        float alpha = (sin(phase) + 1.0f) * 0.5f; // Oscillate between 0 and 1
        
        dot.setFillColor(sf::Color(80, 160, 220, static_cast<sf::Uint8>(alpha * 255)));
        dot.setPosition(
            viewCenter.x + 120 + i * 15, 
            viewCenter.y - 90
        );
        m_game->window().draw(dot);
    }
    
    // Add a subtle progress glow effect
    if (m_totalAssets > 0) {
        float progress = static_cast<float>(m_currentAssetIndex) / static_cast<float>(m_totalAssets);
        if (progress > 0) {
            sf::RectangleShape glow = m_progressBarFill;
            glow.setFillColor(sf::Color(80, 160, 220, 60)); // Semi-transparent blue
            glow.setSize(sf::Vector2f(glow.getSize().x, glow.getSize().y + 4));
            glow.setPosition(glow.getPosition().x, glow.getPosition().y - 2);
            m_game->window().draw(glow);
        }
    }
}

void Scene_Loading::onEnd()
{
    std::cout << "Loading screen ended" << std::endl;
}

void Scene_Loading::loadAssetsFromConfig(const std::string& sceneName)
{
    std::ifstream configFile("metadata/scene_assets.txt");
    if (!configFile.is_open()) {
        std::cout << "No scene asset configuration file found, using defaults" << std::endl;
        return;
    }
    
    std::string line;
    std::vector<std::string> additionalTextures;
    std::vector<std::string> additionalSounds;
    
    while (std::getline(configFile, line)) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') continue;
        
        std::istringstream iss(line);
        std::string sceneNameInFile, assetType, assetName;
        
        if (iss >> sceneNameInFile >> assetType >> assetName) {
            if (sceneNameInFile == sceneName) {
                if (assetType == "texture") {
                    // Check if not already in the list
                    if (std::find(m_assetsToLoad.begin(), m_assetsToLoad.end(), assetName) == m_assetsToLoad.end()) {
                        additionalTextures.push_back(assetName);
                    }
                } else if (assetType == "sound") {
                    // Check if not already in the list
                    if (std::find(m_soundsToLoad.begin(), m_soundsToLoad.end(), assetName) == m_soundsToLoad.end()) {
                        additionalSounds.push_back(assetName);
                    }
                }
            }
        }
    }
    
    // Add additional assets to the loading lists
    m_assetsToLoad.insert(m_assetsToLoad.end(), additionalTextures.begin(), additionalTextures.end());
    m_soundsToLoad.insert(m_soundsToLoad.end(), additionalSounds.begin(), additionalSounds.end());
    
    // Update total count
    m_totalAssets = m_assetsToLoad.size() + m_soundsToLoad.size();
    
    if (additionalTextures.size() > 0 || additionalSounds.size() > 0) {
        std::cout << "Loaded " << additionalTextures.size() << " additional textures and " 
                  << additionalSounds.size() << " additional sounds from config for " << sceneName << std::endl;
    }
    
    configFile.close();
}

// Static helper methods for common scene transitions
void Scene_Loading::loadPlayScene(GameEngine* game, const std::string& levelPath)
{
    // Define assets needed for play scene
    std::vector<std::string> playAssets = {
        "Ground", "Wall", "Bush", "Player"
    };
    
    std::vector<std::string> playSounds = {
        "walk"
    };
    
    auto sceneFactory = [game, levelPath]() {
        return std::make_shared<Scene_Play>(game, levelPath);
    };
    
    auto loadingScene = std::make_shared<Scene_Loading>(game, "Play", sceneFactory, playAssets, playSounds);
    game->changeScene("Loading", loadingScene);
}

void Scene_Loading::loadMenuScene(GameEngine* game)
{
    // Define assets needed for menu scene
    std::vector<std::string> menuAssets = {
        // Add menu-specific assets here if any
    };
    
    std::vector<std::string> menuSounds = {
        // Add menu-specific sounds here if any
    };
    
    auto sceneFactory = [game]() {
        return std::make_shared<Scene_Menu>(game);
    };
    
    auto loadingScene = std::make_shared<Scene_Loading>(game, "Menu", sceneFactory, menuAssets, menuSounds);
    game->changeScene("Loading", loadingScene);
}

void Scene_Loading::loadMapEditorScene(GameEngine* game)
{
    // Define assets needed for map editor scene
    std::vector<std::string> editorAssets = {
        "Ground", "Wall", "Bush", "Player"
    };
    
    std::vector<std::string> editorSounds = {
        // Add editor-specific sounds here if any
    };
    
    auto sceneFactory = [game]() {
        return std::make_shared<Scene_MapEditor>(game);
    };
    
    auto loadingScene = std::make_shared<Scene_Loading>(game, "MapEditor", sceneFactory, editorAssets, editorSounds);
    game->changeScene("Loading", loadingScene);
}
