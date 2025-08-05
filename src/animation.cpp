#include "animation.hpp"

Animation::Animation(const std::string &name, const sf::Texture &tex, size_t frameCount, size_t speed)
    : m_frameCount(frameCount), m_frameSpeed(speed), m_name(name)
{
    m_frameSize = {static_cast<float>(tex.getSize().x) / frameCount, static_cast<float>(tex.getSize().y)};
    m_sprite.setOrigin(m_frameSize.x / 2, m_frameSize.y / 2);
    m_sprite.setTextureRect(sf::IntRect(std::floor(m_currentFrame) * m_frameSize.x, 0, m_frameSize.x, m_frameSize.y));
}

Animation::Animation(const std::string &name, const sf::Texture &tex)
{
    m_frameSize = {static_cast<float>(tex.getSize().x), static_cast<float>(tex.getSize().y)};
    m_sprite.setOrigin(m_frameSize.x / 2, m_frameSize.y / 2);
    m_sprite.setTextureRect(sf::IntRect(0, 0, m_frameSize.x, m_frameSize.y));
}

void Animation::update()
{
    if (m_currentFrame == m_frameCount)
    {
        return;
    }
    m_currentFrame++;
    if (m_currentFrame == m_frameCount)
    {
        m_currentFrame = 0;
    }
    m_sprite.setTextureRect(sf::IntRect(std::floor(m_currentFrame) * m_frameSize.x, 0, m_frameSize.x, m_frameSize.y));
}

bool Animation::hasEnded()
{
    return m_currentFrame == m_frameCount;
}

const std::string &Animation::getName() const
{
    return m_name;
}

const Vec2 &Animation::getFrameSize() const
{
    return m_frameSize;
}

sf::Sprite &Animation::getSprite()
{
    return m_sprite;
}
