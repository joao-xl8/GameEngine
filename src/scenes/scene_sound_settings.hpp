#pragma once
#include "scene.hpp"
#include <vector>

class Scene_SoundSettings : public Scene
{
protected:
    std::vector<std::string> m_menuStrings;
    std::vector<std::string> m_menuOptions;
    sf::Text m_menuText;
    sf::Text m_titleText;
    sf::Text m_instructionsText;
    size_t m_menuIndex = 0;
    
    // Sound settings
    float m_masterVolume = 1.0f;
    float m_musicVolume = 1.0f;
    float m_effectsVolume = 1.0f;
    bool m_soundEnabled = true;
    size_t m_selectedOption = 0;

public:
    Scene_SoundSettings(GameEngine* game);
    void init();
    void update();
    void sDoAction(const Action& action);
    void sRender();
    void onEnd();
    
private:
    void setupUI();
    void applySoundSettings();
    void loadSoundSettings();
    void saveSoundSettings();
};
