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
    loadSoundSettings();
}

void Scene_SoundSettings::setupUI()
{
    // Title text
    m_titleText.setFont(m_game->getAssets().getFont("ShareTech"));
    m_titleText.setCharacterSize(32);
    m_titleText.setFillColor(sf::Color::White);
    m_titleText.setString("SOUND SETTINGS");
    
    // Menu text
    m_menuText.setFont(m_game->getAssets().getFont("ShareTech"));
    m_menuText.setCharacterSize(18);
    m_menuText.setFillColor(sf::Color::White);
    
    // Instructions text
    m_instructionsText.setFont(m_game->getAssets().getFont("ShareTech"));
    m_instructionsText.setCharacterSize(16);
    m_instructionsText.setFillColor(sf::Color(200, 200, 200));
    m_instructionsText.setString("Use WASD/Arrows to navigate, Left/Right to change values, Space/Enter to confirm, Backspace/C/ESC to cancel");
}

void Scene_SoundSettings::update()
{
    sRender();
}

void Scene_SoundSettings::sDoAction(const Action& action)
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
        optionText.setCharacterSize(18);
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
}

void Scene_SoundSettings::applySoundSettings()
{
    // Apply sound settings to the game engine
    // Note: This would integrate with the actual sound system
    std::cout << "Applied sound settings: Master=" << (m_masterVolume * 100) << "%, "
              << "Music=" << (m_musicVolume * 100) << "%, "
              << "Effects=" << (m_effectsVolume * 100) << "%, "
              << "Enabled=" << (m_soundEnabled ? "ON" : "OFF") << std::endl;
    
    saveSoundSettings();
}

void Scene_SoundSettings::loadSoundSettings()
{
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
    
    std::cout << "Sound configuration loaded" << std::endl;
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
