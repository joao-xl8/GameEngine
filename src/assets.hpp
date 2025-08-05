#pragma once
#include <iostream>
#include <map>
#include <string>
#include <stdexcept>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "animation.hpp"
#include <fstream>

class Assets {
    std::map<std::string, sf::Texture> m_textures;
    std::map<std::string, sf::Font> m_fonts;
    std::map<std::string, sf::SoundBuffer> m_soundBuffers;
    std::map<std::string, Animation> m_animations;


    void addTexture(const std::string &name, const std::string &filename);
    void addFont(const std::string &name, const std::string &filename);
    void addSound(const std::string &name, const std::string &filename);
    // void addAnimation(const std::string &name, const std::string &tex, size_t frameCount, size_t speed);
public:
    Assets();
    ~Assets();

    void loadAssets(const std::string &filename);
    
    const sf::Texture& getTexture(const std::string &name) const;
    const sf::Font& getFont(const std::string &name) const;
    const sf::SoundBuffer& getSoundBuffer(const std::string &name) const;
    // const Animation& getAnimation(const std::string &name) const;
};