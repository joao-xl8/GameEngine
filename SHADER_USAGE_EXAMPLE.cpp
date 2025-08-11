// Example: How to add shader effects to any existing scene
// This shows the minimal code needed to add shader support

#include "scene.hpp"
#include <SFML/Graphics.hpp>

class MyScene : public Scene
{
private:
    sf::Sprite m_sprite;
    sf::Clock m_clock;
    
public:
    MyScene(GameEngine* gameEngine) : Scene(gameEngine)
    {
        // 1. Load a shader (do this once in constructor)
        auto& shaderManager = m_game->getAssets().getShaderManager();
        
        // Check if shaders are supported first
        if (ShaderManager::isSupported()) {
            // Load from file
            shaderManager.loadFragmentShader("glow", "assets/shaders/glow.frag");
            
            // Or load from string for simple effects
            std::string simpleShader = R"(
                #version 120
                uniform sampler2D texture;
                uniform float time;
                
                void main() {
                    vec4 color = texture2D(texture, gl_TexCoord[0].xy);
                    float pulse = sin(time * 3.0) * 0.3 + 0.7;
                    gl_FragColor = color * vec4(pulse, pulse, pulse, 1.0) * gl_Color;
                }
            )";
            shaderManager.loadFragmentShaderFromString("pulse", simpleShader);
        }
        
        // Set up your sprite as normal
        m_sprite.setTexture(m_game->getAssets().getTexture("MyTexture"));
        m_sprite.setPosition(100, 100);
    }
    
    void sRender() override
    {
        // 2. Get the shader and set uniforms
        sf::Shader* shader = m_game->getAssets().getShader("pulse");
        
        if (shader) {
            // Set dynamic values (uniforms)
            shader->setUniform("time", m_clock.getElapsedTime().asSeconds());
            
            // 3. Draw with shader
            sf::RenderStates states;
            states.shader = shader;
            m_game->window().draw(m_sprite, states);
        } else {
            // 4. Fallback: draw without shader if not supported
            m_game->window().draw(m_sprite);
        }
    }
};

// That's it! Your sprite now pulses with a shader effect.
// The shader system handles:
// - Checking if shaders are supported
// - Loading and managing shader files
// - Providing fallback rendering for unsupported systems
