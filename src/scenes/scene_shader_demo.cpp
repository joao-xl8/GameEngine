#include "scene_shader_demo.hpp"
#include "../game_engine.hpp"
#include "../action_types.hpp"
#include "scene_loading.hpp"
#include <iostream>
#include <algorithm>

Scene_ShaderDemo::Scene_ShaderDemo(GameEngine* gameEngine) : Scene(gameEngine)
{
    init();
}

void Scene_ShaderDemo::init()
{
    // Register controls
    registerAction(sf::Keyboard::Escape, ActionTypes::BACK);
    registerAction(sf::Keyboard::Space, "NEXT_SHADER");
    registerAction(sf::Keyboard::R, "RELOAD_SHADERS");
    
    // Load shaders
    loadShaders();
    
    // Set up test sprite (use a colorful texture for better shader visibility)
    try {
        // Try to use the colorful test texture first
        try {
            m_testSprite.setTexture(m_game->getAssets().getTexture("ShaderTest"));
            m_currentTexture = "ShaderTest (Colorful Rainbow)";
            std::cout << "Using ShaderTest texture for shader demo" << std::endl;
        } catch (const std::exception& e) {
            try {
                m_testSprite.setTexture(m_game->getAssets().getTexture("LargeCastle"));
                m_currentTexture = "LargeCastle";
                std::cout << "Using LargeCastle texture for shader demo" << std::endl;
            } catch (const std::exception& e) {
                try {
                    m_testSprite.setTexture(m_game->getAssets().getTexture("Player"));
                    m_currentTexture = "Player";
                    std::cout << "Using Player texture for shader demo" << std::endl;
                } catch (const std::exception& e) {
                    m_testSprite.setTexture(m_game->getAssets().getTexture("Ground"));
                    m_currentTexture = "Ground";
                    std::cout << "Using Ground texture for shader demo" << std::endl;
                }
            }
        }
        // Center the sprite on screen with appropriate size
        sf::Vector2u windowSize = m_game->window().getSize();
        sf::Vector2u textureSize = m_testSprite.getTexture()->getSize();
        
        // Calculate scale to fit nicely on screen (max 1/3 of screen width/height)
        float maxWidth = windowSize.x * 0.4f;   // 40% of screen width
        float maxHeight = windowSize.y * 0.4f;  // 40% of screen height
        
        float scaleX = maxWidth / textureSize.x;
        float scaleY = maxHeight / textureSize.y;
        float scale = std::min(scaleX, scaleY); // Use smaller scale to maintain aspect ratio
        
        // Ensure minimum visibility but cap maximum size
        scale = std::max(1.0f, std::min(scale, 4.0f));
        
        m_testSprite.setScale(scale, scale);
        
        // Center the sprite on screen
        sf::FloatRect spriteBounds = m_testSprite.getGlobalBounds();
        float centerX = (windowSize.x - spriteBounds.width) / 2.0f;
        float centerY = (windowSize.y - spriteBounds.height) / 2.0f;
        
        m_testSprite.setPosition(centerX, centerY);
        
        std::cout << "Sprite scaled to " << scale << "x, positioned at (" << centerX << ", " << centerY << ")" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Warning: Could not load any test texture" << std::endl;
        m_currentTexture = "None (Error)";
    }
    
    // Set up info text
    m_infoText.setFont(m_game->getAssets().getFont("ShareTech"));
    m_infoText.setCharacterSize(18); // Slightly smaller for better fit
    m_infoText.setFillColor(sf::Color::White);
    m_infoText.setPosition(20, 20); // Top-left corner with some margin
    
    // Set up info background
    m_infoBackground.setFillColor(sf::Color(0, 0, 0, 150)); // Semi-transparent black
    m_infoBackground.setPosition(10, 10);
    
    std::cout << "Shader Demo Scene initialized" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "Space: Next shader" << std::endl;
    std::cout << "R: Reload shaders" << std::endl;
    std::cout << "Escape: Back to menu" << std::endl;
}

void Scene_ShaderDemo::onEnd()
{
    std::cout << "Shader Demo Scene ended" << std::endl;
}

void Scene_ShaderDemo::loadShaders()
{
    auto& shaderManager = m_game->getAssets().getShaderManager();
    
    // Check if shaders are supported
    if (!ShaderManager::isSupported()) {
        std::cout << "Shaders are not supported on this system!" << std::endl;
        return;
    }
    
    // Load example shaders
    shaderManager.loadFragmentShader("tint", "assets/shaders/tint.frag");
    shaderManager.loadFragmentShader("wave", "assets/shaders/wave.frag");
    shaderManager.loadFragmentShader("pixelate", "assets/shaders/pixelate.frag");
    shaderManager.loadFragmentShader("glow", "assets/shaders/glow.frag");
    
    // Load classic RPG effect shaders
    shaderManager.loadFragmentShader("damage_flash", "assets/shaders/damage_flash.frag");
    shaderManager.loadFragmentShader("poison_effect", "assets/shaders/poison_effect.frag");
    shaderManager.loadFragmentShader("magic_charge", "assets/shaders/magic_charge.frag");
    shaderManager.loadFragmentShader("stone_petrify", "assets/shaders/stone_petrify.frag");
    shaderManager.loadFragmentShader("invisibility", "assets/shaders/invisibility.frag");
    shaderManager.loadFragmentShader("haste_speed", "assets/shaders/haste_speed.frag");
    shaderManager.loadFragmentShader("freeze_ice", "assets/shaders/freeze_ice.frag");
    shaderManager.loadFragmentShader("berserk_rage", "assets/shaders/berserk_rage.frag");
    shaderManager.loadFragmentShader("holy_protect", "assets/shaders/holy_protect.frag");
    shaderManager.loadFragmentShader("darkness_blind", "assets/shaders/darkness_blind.frag");
    
    // Load test shader for debugging
    shaderManager.loadFragmentShader("wave_test", "assets/shaders/wave_test.frag");
    
    // Create a simple shader from string as example
    std::string simpleShader = R"(
        #version 120
        uniform sampler2D texture;
        uniform float time;
        
        void main()
        {
            vec2 texCoord = gl_TexCoord[0].xy;
            vec4 texColor = texture2D(texture, texCoord);
            
            // Simple color cycling
            float r = sin(time) * 0.5 + 0.5;
            float g = sin(time + 2.0) * 0.5 + 0.5;
            float b = sin(time + 4.0) * 0.5 + 0.5;
            
            gl_FragColor = texColor * vec4(r, g, b, 1.0) * gl_Color;
        }
    )";
    
    shaderManager.loadFragmentShaderFromString("rainbow", simpleShader);
    
    // Build shader list
    m_shaderNames = {
        "none", "tint", "wave", "wave_test", "pixelate", "glow", "rainbow",
        "damage_flash", "poison_effect", "magic_charge", "stone_petrify", 
        "invisibility", "haste_speed", "freeze_ice", "berserk_rage", 
        "holy_protect", "darkness_blind"
    };
    
    std::cout << "Loaded " << (m_shaderNames.size() - 1) << " shaders" << std::endl;
}

void Scene_ShaderDemo::updateShaderUniforms()
{
    float time = m_clock.getElapsedTime().asSeconds();
    auto& shaderManager = m_game->getAssets().getShaderManager();
    
    // Update shader uniforms based on current shader
    if (m_currentShader > 0 && static_cast<size_t>(m_currentShader) < m_shaderNames.size()) {
        std::string shaderName = m_shaderNames[m_currentShader];
        sf::Shader* shader = shaderManager.getShader(shaderName);
        
        if (shader) {
            if (shaderName == "tint") {
                shader->setUniform("tintColor", sf::Glsl::Vec4(1.0f, 0.5f, 0.5f, 1.0f)); // Red tint
                shader->setUniform("tintStrength", 0.5f);
            }
            else if (shaderName == "wave") {
                shader->setUniform("time", time);
                shader->setUniform("amplitude", 0.1f);  // Increased from 0.02f to 0.1f for more visible effect
                shader->setUniform("frequency", 15.0f); // Increased frequency for more waves
            }
            else if (shaderName == "wave_test") {
                shader->setUniform("time", time);
                std::cout << "Wave test shader active, time: " << time << std::endl;
            }
            else if (shaderName == "pixelate") {
                sf::Vector2u textureSize = m_testSprite.getTexture()->getSize();
                shader->setUniform("textureSize", sf::Glsl::Vec2(textureSize.x, textureSize.y));
                shader->setUniform("pixelSize", 8.0f);
            }
            else if (shaderName == "glow") {
                // Updated for the new simpler glow shader
                shader->setUniform("time", time);
                shader->setUniform("glowColor", sf::Glsl::Vec4(0.0f, 1.0f, 1.0f, 1.0f)); // Cyan glow
                shader->setUniform("glowIntensity", 1.5f); // Increased intensity for more visible glow
            }
            else if (shaderName == "rainbow") {
                shader->setUniform("time", time);
            }
            // Classic RPG Effect Shaders
            else if (shaderName == "damage_flash") {
                float flashIntensity = sin(time * 8.0) * 0.5 + 0.5; // Fast pulsing
                shader->setUniform("flashIntensity", flashIntensity);
                shader->setUniform("flashColor", sf::Glsl::Vec3(1.0f, 1.0f, 1.0f)); // White flash
            }
            else if (shaderName == "poison_effect") {
                shader->setUniform("time", time);
                shader->setUniform("poisonColor", sf::Glsl::Vec3(0.2f, 0.8f, 0.2f)); // Green poison
            }
            else if (shaderName == "magic_charge") {
                shader->setUniform("time", time);
                float chargeLevel = sin(time * 1.5) * 0.5 + 0.5; // Slow charging cycle
                shader->setUniform("chargeLevel", chargeLevel);
                shader->setUniform("magicColor", sf::Glsl::Vec3(0.3f, 0.6f, 1.0f)); // Blue magic
            }
            else if (shaderName == "stone_petrify") {
                float petrifyLevel = sin(time * 0.8) * 0.5 + 0.5; // Slow petrification cycle
                shader->setUniform("petrifyLevel", petrifyLevel);
                shader->setUniform("time", time);
            }
            else if (shaderName == "invisibility") {
                shader->setUniform("time", time);
                float invisLevel = sin(time * 1.2) * 0.4 + 0.5; // Partial invisibility
                shader->setUniform("invisLevel", invisLevel);
            }
            else if (shaderName == "haste_speed") {
                shader->setUniform("time", time);
                float speedLevel = sin(time * 2.0) * 0.3 + 0.7; // Speed boost intensity
                shader->setUniform("speedLevel", speedLevel);
            }
            else if (shaderName == "freeze_ice") {
                shader->setUniform("time", time);
                float freezeLevel = sin(time * 0.6) * 0.5 + 0.5; // Slow freeze cycle
                shader->setUniform("freezeLevel", freezeLevel);
            }
            else if (shaderName == "berserk_rage") {
                shader->setUniform("time", time);
                float rageLevel = sin(time * 1.8) * 0.4 + 0.6; // Intense rage
                shader->setUniform("rageLevel", rageLevel);
            }
            else if (shaderName == "holy_protect") {
                shader->setUniform("time", time);
                float protectionLevel = sin(time * 1.0) * 0.3 + 0.7; // Gentle protection
                shader->setUniform("protectionLevel", protectionLevel);
            }
            else if (shaderName == "darkness_blind") {
                shader->setUniform("time", time);
                float darknessLevel = sin(time * 1.3) * 0.4 + 0.6; // Darkness intensity
                shader->setUniform("darknessLevel", darknessLevel);
            }
        } else {
            std::cout << "Warning: Shader '" << shaderName << "' not found!" << std::endl;
        }
    }
}

void Scene_ShaderDemo::update()
{
    updateShaderUniforms();
    sRender();
}

void Scene_ShaderDemo::sDoAction(const Action& action)
{
    if (action.getType() == "START") {
        if (action.getName() == "BACK") {
            Scene_Loading::loadMenuScene(m_game);
        }
        else if (action.getName() == "NEXT_SHADER") {
            m_currentShader = (m_currentShader + 1) % m_shaderNames.size();
            std::cout << "Current shader: " << m_shaderNames[m_currentShader] << std::endl;
        }
        else if (action.getName() == "RELOAD_SHADERS") {
            std::cout << "Reloading shaders..." << std::endl;
            m_game->getAssets().getShaderManager().clear();
            loadShaders();
            m_currentShader = 0;
        }
    }
}

void Scene_ShaderDemo::sRender()
{
    m_game->window().clear(sf::Color(50, 50, 50)); // Dark gray background
    
    // Draw test sprite with current shader
    sf::RenderStates states;
    
    if (m_currentShader > 0 && static_cast<size_t>(m_currentShader) < m_shaderNames.size()) {
        std::string shaderName = m_shaderNames[m_currentShader];
        sf::Shader* shader = m_game->getAssets().getShaderManager().getShader(shaderName);
        if (shader) {
            states.shader = shader;
        }
    }
    
    m_game->window().draw(m_testSprite, states);
    
    // Draw info text
    std::string info = "Shader Demo\n\n";
    info += "Current Shader: " + m_shaderNames[m_currentShader] + "\n";
    info += "Shader " + std::to_string(m_currentShader + 1) + " of " + std::to_string(m_shaderNames.size()) + "\n";
    info += "Test Texture: " + m_currentTexture + "\n\n";
    info += "Controls:\n";
    info += "Space: Next shader\n";
    info += "R: Reload shaders\n";
    info += "Escape: Back to menu\n\n";
    
    if (!ShaderManager::isSupported()) {
        info += "WARNING: Shaders not supported!";
    }
    
    m_infoText.setString(info);
    
    // Update background size to fit text
    sf::FloatRect textBounds = m_infoText.getLocalBounds();
    m_infoBackground.setSize(sf::Vector2f(textBounds.width + 20, textBounds.height + 20));
    
    // Draw background first, then text
    m_game->window().draw(m_infoBackground);
    m_game->window().draw(m_infoText);
}
