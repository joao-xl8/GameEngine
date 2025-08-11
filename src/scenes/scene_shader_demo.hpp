#pragma once

#include "scene.hpp"
#include <SFML/Graphics.hpp>

class Scene_ShaderDemo : public Scene
{
private:
    sf::Sprite m_testSprite;
    sf::Clock m_clock;
    int m_currentShader = 0;
    std::vector<std::string> m_shaderNames;
    sf::Text m_infoText;
    sf::RectangleShape m_infoBackground; // Background for info text
    std::string m_currentTexture = "Unknown"; // Track which texture is being used
    
    void loadShaders();
    void updateShaderUniforms();
    
public:
    Scene_ShaderDemo(GameEngine* gameEngine);
    
    void init() override;
    void onEnd() override;
    void update() override;
    void sDoAction(const Action& action) override;
    void sRender() override;
};
