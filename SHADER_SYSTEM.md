# SFML Shader System for Game Engine

## Overview
This game engine now supports SFML shaders! You can use fragment shaders to create visual effects like color tinting, distortion, pixelation, glow effects, and more.

## 🎨 **Shader Support**
- **Fragment Shaders**: Modify pixel colors and effects
- **Vertex Shaders**: Transform vertex positions (can be added later)
- **GLSL Version**: 120 (compatible with older OpenGL)
- **Real-time Uniforms**: Pass dynamic values to shaders

## 📁 **File Structure**
```
assets/shaders/
├── tint.frag          # Color tinting shader
├── wave.frag          # Wave distortion shader
├── pixelate.frag      # Pixelation effect
└── glow.frag          # Glow/outline effect
```

## 🛠️ **How to Use Shaders**

### 1. Loading Shaders
```cpp
// In your scene constructor or init function
auto& shaderManager = m_game->getAssets().getShaderManager();

// Load fragment shader from file
shaderManager.loadFragmentShader("myShader", "assets/shaders/myshader.frag");

// Load from string (for simple shaders)
std::string shaderCode = R"(
    #version 120
    uniform sampler2D texture;
    uniform float time;
    
    void main() {
        vec4 texColor = texture2D(texture, gl_TexCoord[0].xy);
        float pulse = sin(time) * 0.5 + 0.5;
        gl_FragColor = texColor * vec4(pulse, pulse, pulse, 1.0);
    }
)";
shaderManager.loadFragmentShaderFromString("pulse", shaderCode);
```

### 2. Using Shaders in Rendering
```cpp
void MyScene::sRender()
{
    // Get the shader
    sf::Shader* shader = m_game->getAssets().getShader("myShader");
    
    if (shader) {
        // Set shader uniforms (dynamic values)
        shader->setUniform("time", m_clock.getElapsedTime().asSeconds());
        shader->setUniform("tintColor", sf::Glsl::Vec4(1.0f, 0.5f, 0.5f, 1.0f));
        
        // Create render states with shader
        sf::RenderStates states;
        states.shader = shader;
        
        // Draw with shader applied
        m_game->window().draw(mySprite, states);
    } else {
        // Fallback: draw without shader
        m_game->window().draw(mySprite);
    }
}
```

### 3. Checking Shader Support
```cpp
if (!ShaderManager::isSupported()) {
    std::cout << "Shaders not supported on this system!" << std::endl;
    // Use fallback rendering
}
```

## 🎯 **Example Shaders**

### 1. Color Tint Shader (`tint.frag`)
```glsl
#version 120
uniform sampler2D texture;
uniform vec4 tintColor;
uniform float tintStrength;

void main()
{
    vec4 texColor = texture2D(texture, gl_TexCoord[0].xy);
    vec4 finalColor = mix(texColor, texColor * tintColor, tintStrength);
    finalColor.a = texColor.a;
    gl_FragColor = finalColor * gl_Color;
}
```

**Usage:**
```cpp
shader->setUniform("tintColor", sf::Glsl::Vec4(1.0f, 0.0f, 0.0f, 1.0f)); // Red tint
shader->setUniform("tintStrength", 0.5f); // 50% tint strength
```

### 2. Wave Distortion Shader (`wave.frag`)
```glsl
#version 120
uniform sampler2D texture;
uniform float time;
uniform float amplitude;
uniform float frequency;

void main()
{
    vec2 texCoord = gl_TexCoord[0].xy;
    float wave = sin(texCoord.y * frequency + time) * amplitude;
    texCoord.x += wave;
    vec4 texColor = texture2D(texture, texCoord);
    gl_FragColor = texColor * gl_Color;
}
```

**Usage:**
```cpp
shader->setUniform("time", clock.getElapsedTime().asSeconds());
shader->setUniform("amplitude", 0.02f);  // Wave strength
shader->setUniform("frequency", 10.0f);  // Wave frequency
```

### 3. Pixelate Shader (`pixelate.frag`)
```glsl
#version 120
uniform sampler2D texture;
uniform vec2 textureSize;
uniform float pixelSize;

void main()
{
    vec2 texCoord = gl_TexCoord[0].xy;
    vec2 pixelCoord = floor(texCoord * textureSize / pixelSize) * pixelSize / textureSize;
    vec4 texColor = texture2D(texture, pixelCoord);
    gl_FragColor = texColor * gl_Color;
}
```

**Usage:**
```cpp
sf::Vector2u size = sprite.getTexture()->getSize();
shader->setUniform("textureSize", sf::Glsl::Vec2(size.x, size.y));
shader->setUniform("pixelSize", 8.0f);  // Pixel size
```

## 🎮 **Shader Demo Scene**

A complete demo scene (`Scene_ShaderDemo`) is included that shows:
- Loading multiple shaders
- Switching between shaders with Space key
- Setting dynamic uniforms
- Handling systems without shader support

**Access:** Available from the main menu as "Shader Demo"

**Controls:**
- **Space**: Cycle through shaders
- **R**: Reload all shaders
- **Escape**: Back to menu

**Demo Shaders Included:**
1. **None**: No shader applied (original texture)
2. **Tint**: Red color tinting effect
3. **Wave**: Horizontal wave distortion
4. **Pixelate**: Retro pixelation effect
5. **Glow**: Cyan glow outline effect
6. **Rainbow**: Color cycling pulse effect

## 🔧 **Advanced Usage**

### Multiple Textures
```cpp
// In shader
uniform sampler2D texture;      // Primary texture (automatic)
uniform sampler2D noiseTexture; // Additional texture

// In code
shader->setUniform("noiseTexture", noiseTexture);
```

### Complex Effects
```cpp
// Time-based effects
float time = clock.getElapsedTime().asSeconds();
shader->setUniform("time", time);

// Mouse position effects
sf::Vector2i mousePos = sf::Mouse::getPosition(window);
shader->setUniform("mousePos", sf::Glsl::Vec2(mousePos.x, mousePos.y));

// Screen resolution
sf::Vector2u windowSize = window.getSize();
shader->setUniform("resolution", sf::Glsl::Vec2(windowSize.x, windowSize.y));
```

### Shader Combinations
```cpp
// Apply multiple effects by chaining render targets
sf::RenderTexture renderTexture;
renderTexture.create(800, 600);

// First pass: apply wave shader
renderTexture.draw(sprite, waveShader);

// Second pass: apply tint shader to result
sf::Sprite tempSprite(renderTexture.getTexture());
window.draw(tempSprite, tintShader);
```

## 🚀 **Performance Tips**

1. **Check Support**: Always check `ShaderManager::isSupported()` first
2. **Reuse Shaders**: Load shaders once, use many times
3. **Minimize Uniform Updates**: Only update uniforms when values change
4. **Batch Similar Objects**: Group objects using the same shader
5. **Fallback Rendering**: Always have non-shader fallback for compatibility

## 🎯 **Common Use Cases**

- **UI Effects**: Button hover effects, menu transitions
- **Gameplay Effects**: Damage flashes, power-up glows, screen distortion
- **Post-Processing**: Screen-wide effects like blur, color grading
- **Particle Effects**: Custom particle rendering with shaders
- **Lighting**: Simple lighting effects without complex 3D systems

## 📝 **Shader Uniform Types**

```cpp
// Float values
shader->setUniform("myFloat", 1.5f);

// Vector values
shader->setUniform("myVec2", sf::Glsl::Vec2(x, y));
shader->setUniform("myVec3", sf::Glsl::Vec3(r, g, b));
shader->setUniform("myVec4", sf::Glsl::Vec4(r, g, b, a));

// Textures (additional textures beyond the main one)
shader->setUniform("myTexture", sf::Shader::CurrentTexture); // Current texture
shader->setUniform("myTexture", myTexture); // Specific texture

// Matrices (for advanced effects)
shader->setUniform("myMatrix", sf::Glsl::Mat4(transformMatrix));
```

This shader system provides a solid foundation for adding visual effects to your game while maintaining compatibility with systems that don't support shaders!
