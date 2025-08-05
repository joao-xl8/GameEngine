#pragma once
#include "scene.hpp"

class Scene_Options : public Scene
{
protected:
    std::string m_title;
    std::vector<std::string> m_menuStrings;
    sf::Text m_menuText;
    std::vector<std::string> m_levelPaths;
    size_t m_menuIndex = 0;
    
    void init();
    void update();
    void onEnd();
    void sDoAction(const Action &action);
public:
    Scene_Options(GameEngine* game);
    void sRender();
};