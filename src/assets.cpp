#include "assets.hpp"
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

// void Assets::addSound(const std::string &name, const std::string &filename)
// {
//     sf::SoundBuffer buffer;
//     if (!buffer.loadFromFile(filename))
//     {
//         std::cerr << "Failed to load sound: " << filename << std::endl;
//         return;
//     }
//     sf::Sound sound;
//     sound.setBuffer(buffer);
//     m_sounds[name] = sound;
// }

// const sf::Sound &Assets::getSound(const std::string &name) const
// {
//     return m_sounds.at(name);
// }

// const Animation &Assets::getAnimation(const std::string &name) const
// {
//     return m_animations.at(name);
// }
