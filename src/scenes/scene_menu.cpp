#include "scene_menu.hpp"
#include "scene_play_grid.hpp"
#include "scene_options.hpp"
#include "scene_grid_map_editor.hpp"
#include "scene_loading.hpp"
#include "scene_save_load.hpp"
#include "scene_level_selector.hpp"
#include "../game_engine.hpp"
#include "../action_types.hpp"
#include <exception>

void Scene_Menu::init()
{
    // Standard navigation controls
    registerAction(sf::Keyboard::W, ActionTypes::UP);
    registerAction(sf::Keyboard::S, ActionTypes::DOWN);
    
    // Standard confirm/quit controls
    registerAction(sf::Keyboard::Space, ActionTypes::CONFIRM);

    m_menuText.setFont(m_game->getAssets().getFont("ShareTech"));
    m_menuText.setFillColor(sf::Color::White);
    m_menuStrings.push_back("New Game");
    m_menuStrings.push_back("Load Game");
    m_menuStrings.push_back("Load Level");  // Add Load Level option
    m_menuStrings.push_back("Grid Map Editor");
    m_menuStrings.push_back("Options");
    m_menuStrings.push_back("Exit");
    
    // Background music is handled by global sound manager - already playing
    std::printf("Scene_Menu initialized (background music continues from global manager)\n");
}

void Scene_Menu::sRender()
{
    // Get game view for proper positioning
    sf::View gameView = m_game->getGameView();
    sf::Vector2f viewSize = gameView.getSize();
    sf::Vector2f viewCenter = gameView.getCenter();
    
    // Draw background
    sf::RectangleShape background;
    background.setSize(viewSize);
    background.setPosition(viewCenter.x - viewSize.x / 2, viewCenter.y - viewSize.y / 2);
    background.setFillColor(sf::Color(20, 20, 40));
    m_game->window().draw(background);
    
    // Draw title
    m_menuText.setString("GameEngine");
    m_menuText.setCharacterSize(34);  // Increased from 32 to 34
    m_menuText.setFillColor(sf::Color::White);
    
    sf::FloatRect titleBounds = m_menuText.getLocalBounds();
    m_menuText.setPosition(
        viewCenter.x - titleBounds.width / 2, 
        viewCenter.y - viewSize.y * 0.3f
    );
    m_game->window().draw(m_menuText);
    
    // Draw menu options
    float startY = viewCenter.y - viewSize.y * 0.1f;
    float spacing = viewSize.y * 0.06f;
    
    for (size_t i = 0; i < m_menuStrings.size(); i++)
    {
        m_menuText.setString(m_menuStrings[i]);
        m_menuText.setCharacterSize(22);  // Increased from 20 to 22
        
        if (i == m_menuIndex)
        {
            m_menuText.setFillColor(sf::Color::Yellow);
            m_menuText.setString("> " + m_menuStrings[i] + " <");
        }
        else
        {
            m_menuText.setFillColor(sf::Color::White);
        }
        
        sf::FloatRect bounds = m_menuText.getLocalBounds();
        m_menuText.setPosition(
            viewCenter.x - bounds.width / 2, 
            startY + i * spacing
        );
        m_game->window().draw(m_menuText);
    }
    
    // Draw command overlay (always on top)
    renderCommandOverlay();
}

void Scene_Menu::sDoAction(const Action &action)
{
    if (action.getType() == "START")
    {
        if (action.getName() == ActionTypes::UP)
        {
            // Play menu navigation sound (only if sound is enabled)
            if (m_game->isSoundEnabled()) {
                if (auto globalSound = m_game->getGlobalSoundManager()) {
                    float volume = m_game->getMasterVolume() * m_game->getEffectsVolume() * 60.0f;
                    globalSound->playSound("menu_select", volume);
                }
            }
            
            if (m_menuIndex > 0)
            {
                m_menuIndex--;
            }
            else
            {
                m_menuIndex = m_menuStrings.size() - 1;
            }
        }
        else if (action.getName() == ActionTypes::DOWN)
        {
            // Play menu navigation sound (only if sound is enabled)
            if (m_game->isSoundEnabled()) {
                if (auto globalSound = m_game->getGlobalSoundManager()) {
                    float volume = m_game->getMasterVolume() * m_game->getEffectsVolume() * 60.0f;
                    globalSound->playSound("menu_select", volume);
                }
            }
            
            m_menuIndex = (m_menuIndex + 1) % m_menuStrings.size();
        }
        else if (action.getName() == ActionTypes::CONFIRM)
        {
            // Play menu confirm sound (only if sound is enabled)
            if (m_game->isSoundEnabled()) {
                if (auto globalSound = m_game->getGlobalSoundManager()) {
                    float volume = m_game->getMasterVolume() * m_game->getEffectsVolume() * 80.0f;
                    globalSound->playSound("menu_confirm", volume);
                }
            }
            
            // Add bounds checking to prevent crashes
            if (m_menuIndex >= 0 && m_menuIndex < m_menuStrings.size())
            {
                if (m_menuStrings[m_menuIndex] == "Exit")
                {
                    m_game->quit();
                }
                else if (m_menuStrings[m_menuIndex] == "New Game")
                {
                    // Start a new game
                    try {
                        Scene_Loading::loadPlayScene(m_game, "metadata/levels/level_1.txt");
                    } catch (const std::exception& e) {
                        // If scene creation fails, just quit
                        m_game->quit();
                    }
                }
                else if (m_menuStrings[m_menuIndex] == "Load Game")
                {
                    // Open load game screen
                    try {
                        auto loadScene = std::make_shared<Scene_SaveLoad>(m_game, Scene_SaveLoad::LOAD_MODE);
                        m_game->changeScene("LoadGame", loadScene);
                    } catch (const std::exception& e) {
                        // If scene creation fails, just quit
                        m_game->quit();
                    }
                }
                else if (m_menuStrings[m_menuIndex] == "Load Level")
                {
                    // Open level selector screen
                    try {
                        auto levelSelectorScene = std::make_shared<Scene_LevelSelector>(m_game);
                        m_game->changeScene("LevelSelector", levelSelectorScene);
                    } catch (const std::exception& e) {
                        // If scene creation fails, just quit
                        m_game->quit();
                    }
                }
                else if (m_menuStrings[m_menuIndex] == "Map Editor")
                {
                    // Use loading screen for Map Editor transition
                    try {
                        Scene_Loading::loadMapEditorScene(m_game);
                    } catch (const std::exception& e) {
                        // If scene creation fails, just quit to prevent crash
                        m_game->quit();
                    }
                }
                else if (m_menuStrings[m_menuIndex] == "Options")
                {
                    // Use try-catch to handle potential memory allocation failures
                    try {
                        m_game->changeScene("Options", std::make_shared<Scene_Options>(m_game));
                    } catch (const std::exception& e) {
                        // If scene creation fails, just quit to prevent crash
                        m_game->quit();
                    }
                }
                else
                {
                    // Use loading screen for Play scene transition
                    try {
                        Scene_Loading::loadPlayScene(m_game, "metadata/levels/level_1.txt");
                    } catch (const std::exception& e) {
                        // If scene creation fails, just quit
                        m_game->quit();
                    }
                }
            }
        }
        else if (action.getName() == ActionTypes::QUIT)
        {
            m_game->quit();
        }
    }
}

Scene_Menu::Scene_Menu(GameEngine *game)
    : Scene(game) {}

void Scene_Menu::update()
{
    sRender();
}

void Scene_Menu::onEnd()
{
    m_currentScene = -1;
}
