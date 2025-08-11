#pragma once

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>
#include <memory>
#include <iostream>

class ShaderManager
{
private:
    std::unordered_map<std::string, std::unique_ptr<sf::Shader>> m_shaders;
    
public:
    ShaderManager() = default;
    ~ShaderManager() = default;
    
    // Load fragment shader from file
    bool loadFragmentShader(const std::string& name, const std::string& fragmentPath);
    
    // Load vertex shader from file
    bool loadVertexShader(const std::string& name, const std::string& vertexPath);
    
    // Load both vertex and fragment shaders
    bool loadShader(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath);
    
    // Load fragment shader from string
    bool loadFragmentShaderFromString(const std::string& name, const std::string& fragmentSource);
    
    // Get shader by name
    sf::Shader* getShader(const std::string& name);
    
    // Check if shader exists
    bool hasShader(const std::string& name) const;
    
    // Remove shader
    void removeShader(const std::string& name);
    
    // Clear all shaders
    void clear();
    
    // Check if shaders are supported on this system
    static bool isSupported();
};
