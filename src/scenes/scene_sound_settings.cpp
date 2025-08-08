#include "scene_sound_settings.hpp"
#include "scene_menu.hpp"
#include "scene_options.hpp"
#include "../game_engine.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

Scene_SoundSettings::Scene_SoundSettings(GameEngine* game) : Scene(game)
{
    m_menuOptions.push_back("Master Volume");
    m_menuOptions.push_back("Music Volume");
    m_menuOptions.push_back("Effects Volume");
    m_menuOptions.push_back("Sound Enabled");
    m_menuOptions.push_back("Apply Changes");
    m_menuOptions.push_back("Reset to Default");
    m_menuOptions.push_back("Back to Options");
}

void Scene_SoundSettings::init()
{
    // Standard navigation controls
    registerAction(sf::Keyboard::W, "UP");
    registerAction(sf::Keyboard::S, "DOWN");
    registerAction(sf::Keyboard::A, "LEFT");
    registerAction(sf::Keyboard::D, "RIGHT");
    
    // Standard confirm/cancel controls
    registerAction(sf::Keyboard::Space, "CONFIRM");
    registerAction(sf::Keyboard::C, "CANCEL");
    registerAction(sf::Keyboard::C, "BACK");
    
    setupUI();
    loadSoundSettings();
}

void Scene_SoundSettings::setupUI()
{
    // Title text
    m_titleText.setFont(m_game->getAssets().getFont("ShareTech"));
    m_titleText.setCharacterSize(34);  // Increased from 32 to 34
    m_titleText.setFillColor(sf::Color::White);
    m_titleText.setString("SOUND SETTINGS");
    
    // Menu text
    m_menuText.setFont(m_game->getAssets().getFont("ShareTech"));
    m_menuText.setCharacterSize(20);  // Increased from 18 to 20
    m_menuText.setFillColor(sf::Color::White);
    
    // Instructions text
    m_instructionsText.setFont(m_game->getAssets().getFont("ShareTech"));
    m_instructionsText.setCharacterSize(18);  // Increased from 16 to 18
    m_instructionsText.setFillColor(sf::Color(200, 200, 200));
    m_instructionsText.setString("Adjust audio settings for the best gaming experience");
}

void Scene_SoundSettings::update()
{
    sRender();
}

void Scene_SoundSettings::sDoAction(const Action& action)
{
    if (action.getType() == "START") {
        if (action.getName() == "UP") {
            // Play menu navigation sound (only if sound is enabled)
            if (m_game->isSoundEnabled()) {
                if (auto globalSound = m_game->getGlobalSoundManager()) {
                    float volume = m_game->getMasterVolume() * m_game->getEffectsVolume() * 60.0f;
                    globalSound->playSound("menu_select", volume);
                }
            }
            
            if (m_selectedOption > 0) {
                m_selectedOption--;
            } else {
                m_selectedOption = m_menuOptions.size() - 1;
            }
        } else if (action.getName() == "DOWN") {
            // Play menu navigation sound (only if sound is enabled)
            if (m_game->isSoundEnabled()) {
                if (auto globalSound = m_game->getGlobalSoundManager()) {
                    float volume = m_game->getMasterVolume() * m_game->getEffectsVolume() * 60.0f;
                    globalSound->playSound("menu_select", volume);
                }
            }
            
            m_selectedOption = (m_selectedOption + 1) % m_menuOptions.size();
        } else if (action.getName() == "LEFT") {
            // Play menu navigation sound (only if sound is enabled)
            if (m_game->isSoundEnabled()) {
                if (auto globalSound = m_game->getGlobalSoundManager()) {
                    float volume = m_game->getMasterVolume() * m_game->getEffectsVolume() * 60.0f;
                    globalSound->playSound("menu_select", volume);
                }
            }
            
            // Adjust current option value
            if (m_selectedOption == 0) { // Master Volume
                m_masterVolume = std::max(0.0f, m_masterVolume - 0.1f);
            } else if (m_selectedOption == 1) { // Music Volume
                m_musicVolume = std::max(0.0f, m_musicVolume - 0.1f);
            } else if (m_selectedOption == 2) { // Effects Volume
                m_effectsVolume = std::max(0.0f, m_effectsVolume - 0.1f);
            } else if (m_selectedOption == 3) { // Sound Enabled
                m_soundEnabled = !m_soundEnabled;
            }
        } else if (action.getName() == "RIGHT") {
            // Play menu navigation sound (only if sound is enabled)
            if (m_game->isSoundEnabled()) {
                if (auto globalSound = m_game->getGlobalSoundManager()) {
                    float volume = m_game->getMasterVolume() * m_game->getEffectsVolume() * 60.0f;
                    globalSound->playSound("menu_select", volume);
                }
            }
            
            // Adjust current option value
            if (m_selectedOption == 0) { // Master Volume
                m_masterVolume = std::min(1.0f, m_masterVolume + 0.1f);
            } else if (m_selectedOption == 1) { // Music Volume
                m_musicVolume = std::min(1.0f, m_musicVolume + 0.1f);
            } else if (m_selectedOption == 2) { // Effects Volume
                m_effectsVolume = std::min(1.0f, m_effectsVolume + 0.1f);
            } else if (m_selectedOption == 3) { // Sound Enabled
                m_soundEnabled = !m_soundEnabled;
            }
        } else if (action.getName() == "CONFIRM") {
            // Play menu confirm sound (only if sound is enabled)
            if (m_game->isSoundEnabled()) {
                if (auto globalSound = m_game->getGlobalSoundManager()) {
                    float volume = m_game->getMasterVolume() * m_game->getEffectsVolume() * 80.0f;
                    globalSound->playSound("menu_confirm", volume);
                }
            }
            
            if (m_selectedOption == 4) { // Apply Changes
                applySoundSettings();
            } else if (m_selectedOption == 5) { // Reset to Default
                m_masterVolume = 1.0f;
                m_musicVolume = 1.0f;
                m_effectsVolume = 1.0f;
                m_soundEnabled = true;
                applySoundSettings();
            } else if (m_selectedOption == 6) { // Back to Options
                m_game->changeScene("Options", std::make_shared<Scene_Options>(m_game));
            }
        } else if (action.getName() == "BACK" || action.getName() == "CANCEL") {
            // Play menu back sound (only if sound is enabled)
            if (m_game->isSoundEnabled()) {
                if (auto globalSound = m_game->getGlobalSoundManager()) {
                    float volume = m_game->getMasterVolume() * m_game->getEffectsVolume() * 50.0f;
                    globalSound->playSound("menu_select", volume);
                }
            }
            
            m_game->changeScene("Options", std::make_shared<Scene_Options>(m_game));
        }
    }
}

void Scene_SoundSettings::sRender()
{
    // Get game view for proper positioning
    sf::View gameView = m_game->getGameView();
    sf::Vector2f viewSize = gameView.getSize();
    sf::Vector2f viewCenter = gameView.getCenter();
    
    // Draw background
    sf::RectangleShape background;
    background.setSize(viewSize);
    background.setPosition(viewCenter.x - viewSize.x / 2, viewCenter.y - viewSize.y / 2);
    background.setFillColor(sf::Color(30, 25, 45));
    m_game->window().draw(background);
    
    // Draw title
    sf::FloatRect titleBounds = m_titleText.getLocalBounds();
    m_titleText.setPosition(
        viewCenter.x - titleBounds.width / 2, 
        viewCenter.y - viewSize.y * 0.35f
    );
    m_game->window().draw(m_titleText);
    
    // Draw menu options
    float startY = viewCenter.y - viewSize.y * 0.15f;
    float spacing = viewSize.y * 0.06f;
    
    for (size_t i = 0; i < m_menuOptions.size(); i++) {
        sf::Text optionText;
        optionText.setFont(m_game->getAssets().getFont("ShareTech"));
        optionText.setCharacterSize(20);  // Increased from 18 to 20
        optionText.setFillColor(sf::Color::White);
        
        std::string displayText = m_menuOptions[i];
        
        // Add current values for configurable options
        if (i == 0) { // Master Volume
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(1) << (m_masterVolume * 100) << "%";
            displayText += ": " + oss.str();
        } else if (i == 1) { // Music Volume
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(1) << (m_musicVolume * 100) << "%";
            displayText += ": " + oss.str();
        } else if (i == 2) { // Effects Volume
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(1) << (m_effectsVolume * 100) << "%";
            displayText += ": " + oss.str();
        } else if (i == 3) { // Sound Enabled
            displayText += ": " + std::string(m_soundEnabled ? "ON" : "OFF");
        }
        
        // Highlight selected option
        if (i == m_selectedOption) {
            optionText.setFillColor(sf::Color::Yellow);
            displayText = "> " + displayText + " <";
        }
        
        optionText.setString(displayText);
        
        sf::FloatRect textBounds = optionText.getLocalBounds();
        optionText.setPosition(
            viewCenter.x - textBounds.width / 2,
            startY + i * spacing
        );
        
        m_game->window().draw(optionText);
    }
    
    // Draw instructions
    sf::FloatRect instrBounds = m_instructionsText.getLocalBounds();
    m_instructionsText.setPosition(
        viewCenter.x - instrBounds.width / 2,
        viewCenter.y + viewSize.y * 0.35f
    );
    m_game->window().draw(m_instructionsText);
    
    // Draw command overlay (always on top)
    renderCommandOverlay();
}

void Scene_SoundSettings::applySoundSettings()
{
    // Apply sound settings to the game engine's global sound system
    if (m_game && m_game->getGlobalSoundManager()) {
        auto soundManager = m_game->getGlobalSoundManager();
        
        // Always stop current background music first
        if (soundManager->isMusicPlaying("background")) {
            soundManager->stopMusic("background");
        }
        
        // Start background music if sound is enabled
        if (m_soundEnabled) {
            float adjustedVolume = m_masterVolume * m_musicVolume * 25.0f; // Base volume was 25%
            soundManager->playMusic("background", true, adjustedVolume);
        }
        
        // Update the game engine's sound settings
        m_game->updateSoundSettings(m_masterVolume, m_musicVolume, m_effectsVolume, m_soundEnabled);
        
        std::cout << "Applied sound settings: Master=" << (m_masterVolume * 100) << "%, "
                  << "Music=" << (m_musicVolume * 100) << "%, "
                  << "Effects=" << (m_effectsVolume * 100) << "%, "
                  << "Enabled=" << (m_soundEnabled ? "ON" : "OFF") << std::endl;
    } else {
        std::cout << "Warning: Could not apply sound settings - sound manager not available" << std::endl;
    }
    
    saveSoundSettings();
}

void Scene_SoundSettings::loadSoundSettings()
{
    // Load current settings from the game engine
    if (m_game) {
        m_masterVolume = m_game->getMasterVolume();
        m_musicVolume = m_game->getMusicVolume();
        m_effectsVolume = m_game->getEffectsVolume();
        m_soundEnabled = m_game->isSoundEnabled();
        
        std::cout << "Sound settings loaded from game engine: Master=" << (m_masterVolume * 100) << "%, "
                  << "Music=" << (m_musicVolume * 100) << "%, "
                  << "Effects=" << (m_effectsVolume * 100) << "%, "
                  << "Enabled=" << (m_soundEnabled ? "ON" : "OFF") << std::endl;
    } else {
        // Fallback to file loading if game engine not available
        std::ifstream file("metadata/sound_config.txt");
        if (!file.is_open()) {
            std::cout << "No sound configuration file found, using defaults" << std::endl;
            return;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;
            
            std::istringstream iss(line);
            std::string key, value;
            if (std::getline(iss, key, '=') && std::getline(iss, value)) {
                if (key == "master_volume") {
                    m_masterVolume = std::stof(value);
                } else if (key == "music_volume") {
                    m_musicVolume = std::stof(value);
                } else if (key == "effects_volume") {
                    m_effectsVolume = std::stof(value);
                } else if (key == "sound_enabled") {
                    m_soundEnabled = (value == "1" || value == "true");
                }
            }
        }
        
        std::cout << "Sound configuration loaded from file" << std::endl;
    }
}

void Scene_SoundSettings::saveSoundSettings()
{
    std::ofstream file("metadata/sound_config.txt");
    if (!file.is_open()) {
        std::cout << "Failed to save sound configuration" << std::endl;
        return;
    }
    
    file << "# Sound Configuration\n";
    file << "# Values range from 0.0 to 1.0\n\n";
    file << "master_volume=" << m_masterVolume << "\n";
    file << "music_volume=" << m_musicVolume << "\n";
    file << "effects_volume=" << m_effectsVolume << "\n";
    file << "sound_enabled=" << (m_soundEnabled ? "1" : "0") << "\n";
    
    std::cout << "Sound configuration saved" << std::endl;
}

void Scene_SoundSettings::onEnd()
{
    // Cleanup if needed
}
