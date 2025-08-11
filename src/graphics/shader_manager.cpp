#include "shader_manager.hpp"
#include <fstream>
#include <sstream>

bool ShaderManager::loadFragmentShader(const std::string& name, const std::string& fragmentPath)
{
    if (!sf::Shader::isAvailable()) {
        std::cout << "Shaders are not supported on this system!" << std::endl;
        return false;
    }
    
    auto shader = std::make_unique<sf::Shader>();
    
    if (!shader->loadFromFile(fragmentPath, sf::Shader::Fragment)) {
        std::cout << "Failed to load fragment shader: " << fragmentPath << std::endl;
        return false;
    }
    
    m_shaders[name] = std::move(shader);
    std::cout << "Loaded fragment shader: " << name << " from " << fragmentPath << std::endl;
    return true;
}

bool ShaderManager::loadVertexShader(const std::string& name, const std::string& vertexPath)
{
    if (!sf::Shader::isAvailable()) {
        std::cout << "Shaders are not supported on this system!" << std::endl;
        return false;
    }
    
    auto shader = std::make_unique<sf::Shader>();
    
    if (!shader->loadFromFile(vertexPath, sf::Shader::Vertex)) {
        std::cout << "Failed to load vertex shader: " << vertexPath << std::endl;
        return false;
    }
    
    m_shaders[name] = std::move(shader);
    std::cout << "Loaded vertex shader: " << name << " from " << vertexPath << std::endl;
    return true;
}

bool ShaderManager::loadShader(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath)
{
    if (!sf::Shader::isAvailable()) {
        std::cout << "Shaders are not supported on this system!" << std::endl;
        return false;
    }
    
    auto shader = std::make_unique<sf::Shader>();
    
    if (!shader->loadFromFile(vertexPath, fragmentPath)) {
        std::cout << "Failed to load shader: " << name << " (vertex: " << vertexPath << ", fragment: " << fragmentPath << ")" << std::endl;
        return false;
    }
    
    m_shaders[name] = std::move(shader);
    std::cout << "Loaded shader: " << name << " (vertex + fragment)" << std::endl;
    return true;
}

bool ShaderManager::loadFragmentShaderFromString(const std::string& name, const std::string& fragmentSource)
{
    if (!sf::Shader::isAvailable()) {
        std::cout << "Shaders are not supported on this system!" << std::endl;
        return false;
    }
    
    auto shader = std::make_unique<sf::Shader>();
    
    if (!shader->loadFromMemory(fragmentSource, sf::Shader::Fragment)) {
        std::cout << "Failed to load fragment shader from string: " << name << std::endl;
        return false;
    }
    
    m_shaders[name] = std::move(shader);
    std::cout << "Loaded fragment shader from string: " << name << std::endl;
    return true;
}

sf::Shader* ShaderManager::getShader(const std::string& name)
{
    auto it = m_shaders.find(name);
    if (it != m_shaders.end()) {
        return it->second.get();
    }
    return nullptr;
}

bool ShaderManager::hasShader(const std::string& name) const
{
    return m_shaders.find(name) != m_shaders.end();
}

void ShaderManager::removeShader(const std::string& name)
{
    auto it = m_shaders.find(name);
    if (it != m_shaders.end()) {
        m_shaders.erase(it);
        std::cout << "Removed shader: " << name << std::endl;
    }
}

void ShaderManager::clear()
{
    m_shaders.clear();
    std::cout << "Cleared all shaders" << std::endl;
}

bool ShaderManager::isSupported()
{
    return sf::Shader::isAvailable();
}
