#pragma once

#include "scene.hpp"
#include <vector>
#include <string>

class Scene_ScreenConfig : public Scene
{
private:
    // Menu state
    std::vector<std::string> m_menuOptions;
    size_t m_selectedOption = 0;
    
    // Configuration values
    std::vector<std::pair<int, int>> m_resolutions;
    size_t m_selectedResolution = 0;
    
    std::vector<std::string> m_scalingModes;
    size_t m_selectedScalingMode = 0;
    
    float m_zoomFactor = 0.9f;
    
    // UI elements
    sf::Text m_titleText;
    sf::Text m_instructionsText;
    std::vector<sf::Text> m_optionTexts;
    sf::Text m_currentConfigText;
    
    void setupUI();
    void updateOptionTexts();
    void applyConfiguration();
    std::string getCurrentConfigString();
    
public:
    Scene_ScreenConfig(GameEngine* game);
    
    void init() override;
    void update() override;
    void sDoAction(const Action& action) override;
    void sRender() override;
    void onEnd() override;
};
