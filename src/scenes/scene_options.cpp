#include "scene_menu.hpp"
#include "scene_options.hpp"
#include "scene_viewport_config.hpp"
#include "scene_sound_settings.hpp"
#include "../game_engine.hpp"
#include "../action_types.hpp"
#include <exception>

void Scene_Options::init()
{
    // Standard navigation controls
    registerAction(sf::Keyboard::W, ActionTypes::UP);
    registerAction(sf::Keyboard::S, ActionTypes::DOWN);
    
    // Standard confirm/cancel controls
    registerAction(sf::Keyboard::Space, ActionTypes::CONFIRM);
    registerAction(sf::Keyboard::C, ActionTypes::BACK);

    m_menuText.setFont(m_game->getAssets().getFont("ShareTech"));
    m_menuText.setFillColor(sf::Color::White);
    m_menuStrings.push_back("Screen Settings");
    m_menuStrings.push_back("Sound Settings");
    m_menuStrings.push_back("Back");
}

void Scene_Options::sRender()
{
    // Get game view for proper positioning
    sf::View gameView = m_game->getGameView();
    sf::Vector2f viewSize = gameView.getSize();
    sf::Vector2f viewCenter = gameView.getCenter();
    
    // Draw background
    sf::RectangleShape background;
    background.setSize(viewSize);
    background.setPosition(viewCenter.x - viewSize.x / 2, viewCenter.y - viewSize.y / 2);
    background.setFillColor(sf::Color(25, 25, 45));
    m_game->window().draw(background);
    
    // Draw title
    m_menuText.setString("Options");
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
    float spacing = viewSize.y * 0.08f;
    
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

void Scene_Options::sDoAction(const Action &action)
{
    if (action.getType() == "START")
    {
        if (action.getName() == "UP")
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
        else if (action.getName() == "DOWN")
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
        else if (action.getName() == "CONFIRM")
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
                if (m_menuStrings[m_menuIndex] == "Back")
                {
                    // Use try-catch to handle potential memory allocation failures
                    try {
                        m_game->changeScene("Menu", std::make_shared<Scene_Menu>(m_game));
                    } catch (const std::exception& e) {
                        // If scene creation fails, just quit to prevent crash
                        m_game->quit();
                    }
                }
                else if (m_menuStrings[m_menuIndex] == "Screen Settings")
                {
                    try {
                        m_game->changeScene("ScreenConfig", std::make_shared<Scene_ScreenConfig>(m_game));
                    } catch (const std::exception& e) {
                        // If scene creation fails, just quit to prevent crash
                        m_game->quit();
                    }
                }
                else if (m_menuStrings[m_menuIndex] == "Sound Settings")
                {
                    try {
                        m_game->changeScene("SoundSettings", std::make_shared<Scene_SoundSettings>(m_game));
                    } catch (const std::exception& e) {
                        // If scene creation fails, just quit to prevent crash
                        m_game->quit();
                    }
                }
            }
        }
        else if (action.getName() == "BACK")
        {
            // Play menu back sound (only if sound is enabled)
            if (m_game->isSoundEnabled()) {
                if (auto globalSound = m_game->getGlobalSoundManager()) {
                    float volume = m_game->getMasterVolume() * m_game->getEffectsVolume() * 50.0f;
                    globalSound->playSound("menu_select", volume);
                }
            }
            
            // Use try-catch for scene change
            try {
                m_game->changeScene("Menu", std::make_shared<Scene_Menu>(m_game));
            } catch (const std::exception& e) {
                // If scene creation fails, just quit
                m_game->quit();
            }
        }
    }
}

Scene_Options::Scene_Options(GameEngine *game)
    : Scene(game) {}

void Scene_Options::update()
{
    sRender();
}

void Scene_Options::onEnd()
{
    m_currentScene = -1;
}
