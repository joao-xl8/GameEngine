#include "scene_level_selector.hpp"
#include "scene_play.hpp"
#include "scene_menu.hpp"
#include "scene_loading.hpp"
#include "../game_engine.hpp"
#include "../action_types.hpp"
#include <iostream>
#include <algorithm>

Scene_LevelSelector::Scene_LevelSelector(GameEngine* game) : Scene(game)
{
}

void Scene_LevelSelector::init()
{
    // Standard navigation controls
    registerAction(sf::Keyboard::W, ActionTypes::UP);
    registerAction(sf::Keyboard::S, ActionTypes::DOWN);
    
    // Standard confirm/cancel controls
    registerAction(sf::Keyboard::Space, ActionTypes::CONFIRM);
    registerAction(sf::Keyboard::C, ActionTypes::CANCEL);
    
    // Setup UI elements
    m_titleText.setFont(m_game->getAssets().getFont("ShareTech"));
    m_titleText.setCharacterSize(32);
    m_titleText.setFillColor(sf::Color::White);
    m_titleText.setString("SELECT LEVEL");
    
    m_levelText.setFont(m_game->getAssets().getFont("ShareTech"));
    m_levelText.setCharacterSize(20);
    m_levelText.setFillColor(sf::Color::White);
    
    // Scan for available levels
    scanAvailableLevels();
    
    std::cout << "Level Selector initialized with " << m_availableLevels.size() << " levels" << std::endl;
}

void Scene_LevelSelector::scanAvailableLevels()
{
    m_availableLevels.clear();
    
    try {
        // Check if levels directory exists
        if (!std::filesystem::exists(m_levelsDirectory)) {
            std::cout << "Levels directory not found: " << m_levelsDirectory << std::endl;
            return;
        }
        
        // Scan for .txt files in levels directory
        for (const auto& entry : std::filesystem::directory_iterator(m_levelsDirectory)) {
            if (entry.is_regular_file() && entry.path().extension() == ".txt") {
                std::string filename = entry.path().filename().string();
                m_availableLevels.push_back(filename);
            }
        }
        
        // Sort levels alphabetically
        std::sort(m_availableLevels.begin(), m_availableLevels.end());
        
        // Reset selection to first level
        m_selectedLevel = 0;
        
        std::cout << "Found " << m_availableLevels.size() << " level files:" << std::endl;
        for (const auto& level : m_availableLevels) {
            std::cout << "  - " << level << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cout << "Error scanning levels directory: " << e.what() << std::endl;
    }
}

void Scene_LevelSelector::update()
{
    sRender();
}

void Scene_LevelSelector::sDoAction(const Action& action)
{
    if (action.getType() == "START") {
        if (action.getName() == "UP") {
            // Play menu navigation sound
            if (m_game->isSoundEnabled()) {
                if (auto globalSound = m_game->getGlobalSoundManager()) {
                    float volume = m_game->getMasterVolume() * m_game->getEffectsVolume() * 60.0f;
                    globalSound->playSound("menu_select", volume);
                }
            }
            
            if (m_selectedLevel > 0) {
                m_selectedLevel--;
            } else if (!m_availableLevels.empty()) {
                m_selectedLevel = m_availableLevels.size() - 1;
            }
        }
        else if (action.getName() == "DOWN") {
            // Play menu navigation sound
            if (m_game->isSoundEnabled()) {
                if (auto globalSound = m_game->getGlobalSoundManager()) {
                    float volume = m_game->getMasterVolume() * m_game->getEffectsVolume() * 60.0f;
                    globalSound->playSound("menu_select", volume);
                }
            }
            
            if (!m_availableLevels.empty()) {
                m_selectedLevel = (m_selectedLevel + 1) % m_availableLevels.size();
            }
        }
        else if (action.getName() == "CONFIRM") {
            // Play menu confirm sound
            if (m_game->isSoundEnabled()) {
                if (auto globalSound = m_game->getGlobalSoundManager()) {
                    float volume = m_game->getMasterVolume() * m_game->getEffectsVolume() * 80.0f;
                    globalSound->playSound("menu_confirm", volume);
                }
            }
            
            loadSelectedLevel();
        }
        else if (action.getName() == "CANCEL") {
            // Play menu back sound
            if (m_game->isSoundEnabled()) {
                if (auto globalSound = m_game->getGlobalSoundManager()) {
                    float volume = m_game->getMasterVolume() * m_game->getEffectsVolume() * 50.0f;
                    globalSound->playSound("menu_select", volume);
                }
            }
            
            // Return to main menu
            m_game->changeScene("Menu", std::make_shared<Scene_Menu>(m_game));
        }
    }
}

void Scene_LevelSelector::sRender()
{
    // Get game view for proper positioning
    sf::View gameView = m_game->getGameView();
    sf::Vector2f viewSize = gameView.getSize();
    sf::Vector2f viewCenter = gameView.getCenter();
    
    // Draw background
    sf::RectangleShape background;
    background.setSize(viewSize);
    background.setPosition(viewCenter.x - viewSize.x / 2, viewCenter.y - viewSize.y / 2);
    background.setFillColor(sf::Color(20, 30, 40));
    m_game->window().draw(background);
    
    // Draw title
    sf::FloatRect titleBounds = m_titleText.getLocalBounds();
    m_titleText.setPosition(
        viewCenter.x - titleBounds.width / 2,
        viewCenter.y - viewSize.y * 0.35f
    );
    m_game->window().draw(m_titleText);
    
    // Draw level list
    if (m_availableLevels.empty()) {
        sf::Text noLevelsText;
        noLevelsText.setFont(m_game->getAssets().getFont("ShareTech"));
        noLevelsText.setCharacterSize(18);
        noLevelsText.setFillColor(sf::Color::Red);
        noLevelsText.setString("No level files found in " + m_levelsDirectory);
        
        sf::FloatRect noLevelsBounds = noLevelsText.getLocalBounds();
        noLevelsText.setPosition(
            viewCenter.x - noLevelsBounds.width / 2,
            viewCenter.y
        );
        m_game->window().draw(noLevelsText);
    } else {
        // Calculate display range (show 7 levels at a time)
        size_t maxVisible = 7;
        size_t startIndex = 0;
        
        if (m_availableLevels.size() > maxVisible) {
            if (m_selectedLevel >= maxVisible / 2) {
                startIndex = std::min(m_selectedLevel - maxVisible / 2, 
                                    m_availableLevels.size() - maxVisible);
            }
        }
        
        size_t endIndex = std::min(startIndex + maxVisible, m_availableLevels.size());
        
        // Draw visible levels
        float startY = viewCenter.y - viewSize.y * 0.15f;
        float spacing = 35.0f;
        
        for (size_t i = startIndex; i < endIndex; i++) {
            m_levelText.setString(m_availableLevels[i]);
            
            // Highlight selected level
            if (i == m_selectedLevel) {
                m_levelText.setFillColor(sf::Color::Yellow);
                m_levelText.setString("> " + m_availableLevels[i] + " <");
            } else {
                m_levelText.setFillColor(sf::Color::White);
            }
            
            sf::FloatRect levelBounds = m_levelText.getLocalBounds();
            m_levelText.setPosition(
                viewCenter.x - levelBounds.width / 2,
                startY + (i - startIndex) * spacing
            );
            
            m_game->window().draw(m_levelText);
        }
        
        // Show scroll indicators if needed
        if (m_availableLevels.size() > maxVisible) {
            sf::Text scrollText;
            scrollText.setFont(m_game->getAssets().getFont("ShareTech"));
            scrollText.setCharacterSize(14);
            scrollText.setFillColor(sf::Color(150, 150, 150));
            
            std::string scrollInfo = "(" + std::to_string(m_selectedLevel + 1) + 
                                   " of " + std::to_string(m_availableLevels.size()) + ")";
            scrollText.setString(scrollInfo);
            
            sf::FloatRect scrollBounds = scrollText.getLocalBounds();
            scrollText.setPosition(
                viewCenter.x - scrollBounds.width / 2,
                startY + maxVisible * spacing + 10
            );
            m_game->window().draw(scrollText);
        }
    }
    
    // Draw command overlay
    renderCommandOverlay();
}

void Scene_LevelSelector::loadSelectedLevel()
{
    if (m_availableLevels.empty() || m_selectedLevel >= m_availableLevels.size()) {
        std::cout << "No level selected or invalid selection" << std::endl;
        return;
    }
    
    std::string selectedLevel = m_availableLevels[m_selectedLevel];
    std::string levelPath = m_levelsDirectory + selectedLevel;
    
    std::cout << "Loading level: " << levelPath << std::endl;
    
    try {
        // Use the loading screen to transition to the selected level
        Scene_Loading::loadPlayScene(m_game, levelPath);
    } catch (const std::exception& e) {
        std::cout << "Error loading level: " << e.what() << std::endl;
        // Could show an error message to the user here
    }
}

void Scene_LevelSelector::onEnd()
{
    // Cleanup if needed
}
