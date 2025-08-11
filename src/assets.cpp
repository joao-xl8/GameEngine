#include "assets.hpp"
#include <fstream>
// assets file
// Texture TexGround assets/imgs/ground.png
// Animation Stand Text Stand 1 0
// Font ShareTech assets/fonts/ShareTech-Regular.ttf
// Font Techno assets/fonts/Techno.ttf
Assets::Assets()
{
}

void Assets::loadAssets(const std::string &filename)
{
    std::ifstream fin(filename);
    std::string type;

    while (fin >> type)
    {
        if (type == "Texture")
        {
            std::string name, filename;
            fin >> name >> filename;
            addTexture(name, filename);
        }
        else if (type == "Font")
        {
            std::string name, filename;
            fin >> name >> filename;
            addFont(name, filename);
        }
        else if (type == "Sound")
        {
            std::string name, filename;
            fin >> name >> filename;
            addSound(name, filename);
        }
    }
}
Assets::~Assets()
{
}

void Assets::addTexture(const std::string &name, const std::string &filename)
{
    sf::Texture texture;
    if (!texture.loadFromFile(filename))
    {
        std::cerr << "Failed to load texture: " << filename << std::endl;
        return;
    }
    if(m_textures.find(name) == m_textures.end()){
        std::printf("Adding texture: %s\n", name.c_str());
        m_textures[name] = texture;
    }
    else
    {
        std::printf("Font already exists: %s\n", name.c_str());
    }   
}

const sf::Texture &Assets::getTexture(const std::string &name) const
{
    auto it = m_textures.find(name);
    if (it == m_textures.end())
    {
        std::cerr << "Error: Texture '" << name << "' not found!" << std::endl;
        std::cerr << "Available textures: ";
        for (const auto& pair : m_textures)
        {
            std::cerr << pair.first << " ";
        }
        std::cerr << std::endl;
        throw std::runtime_error("Texture not found: " + name);
    }
    return it->second;
}

void Assets::addFont(const std::string &name, const std::string &filename)
{
    sf::Font font;
    if (!font.loadFromFile(filename))
    {
        std::cerr << "Failed to load font: " << filename << std::endl;
        return;
    }
    if (m_fonts.find(name) == m_fonts.end())
    {
        m_fonts[name] = font;
    }
    else
    {
        std::printf("Font already exists: %s\n", name.c_str());
    }    
}

const sf::Font &Assets::getFont(const std::string &name) const
{
    auto it = m_fonts.find(name);
    if (it == m_fonts.end())
    {
        std::cerr << "Error: Font '" << name << "' not found!" << std::endl;
        std::cerr << "Available fonts: ";
        for (const auto& pair : m_fonts)
        {
            std::cerr << pair.first << " ";
        }
        std::cerr << std::endl;
        throw std::runtime_error("Font not found: " + name);
    }
    return it->second;
}

void Assets::addSound(const std::string &name, const std::string &filename)
{
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile(filename))
    {
        std::cerr << "Failed to load sound: " << filename << std::endl;
        return;
    }
    if (m_soundBuffers.find(name) == m_soundBuffers.end())
    {
        std::printf("Adding sound: %s\n", name.c_str());
        m_soundBuffers[name] = buffer;
    }
    else
    {
        std::printf("Sound already exists: %s\n", name.c_str());
    }    
}

const sf::SoundBuffer &Assets::getSoundBuffer(const std::string &name) const
{
    auto it = m_soundBuffers.find(name);
    if (it == m_soundBuffers.end())
    {
        std::cerr << "Error: Sound '" << name << "' not found!" << std::endl;
        std::cerr << "Available sounds: ";
        for (const auto& pair : m_soundBuffers)
        {
            std::cerr << pair.first << " ";
        }
        std::cerr << std::endl;
        throw std::runtime_error("Sound not found: " + name);
    }
    return it->second;
}

void Assets::addShader(const std::string &name, const std::string &fragmentPath)
{
    if (!m_shaderManager.loadFragmentShader(name, fragmentPath))
    {
        std::cerr << "Failed to load shader: " << name << " from " << fragmentPath << std::endl;
    }
}

sf::Shader* Assets::getShader(const std::string &name)
{
    return m_shaderManager.getShader(name);
}

ShaderManager& Assets::getShaderManager()
{
    return m_shaderManager;
}

// const Animation &Assets::getAnimation(const std::string &name) const
// {
//     return m_animations.at(name);
// }
