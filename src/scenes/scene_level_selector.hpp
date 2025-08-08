#pragma once

#include "scene.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <filesystem>

class Scene_LevelSelector : public Scene {
private:
    std::vector<std::string> m_availableLevels;
    size_t m_selectedLevel = 0;
    
    // UI elements
    sf::Text m_titleText;
    sf::Text m_levelText;
    sf::RectangleShape m_background;
    
    // Level info
    std::string m_levelsDirectory = "metadata/levels/";
    
    void scanAvailableLevels();
    void loadSelectedLevel();
    
public:
    Scene_LevelSelector(GameEngine* game);
    
    void init() override;
    void update() override;
    void sDoAction(const Action& action) override;
    void sRender() override;
    void onEnd() override;
};
