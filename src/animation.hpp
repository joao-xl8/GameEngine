#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "vec2.hpp"

class Animation {
    sf::Sprite m_sprite;
    size_t m_frameCount = 1;
    size_t m_currentFrame = 0;
    Vec2 m_frameSize = { 1, 1};
    std::string m_name = "none";
public:
    Animation(const std::string &name, const sf::Texture &tex, size_t frameCount, size_t speed);
    Animation(const std::string &name, const sf::Texture &tex);
    void update();
    bool hasEnded();
    const std::string& getName() const;
    const Vec2& getFrameSize() const;
    sf::Sprite& getSprite();
};
