#pragma once
#include <SFML/Graphics.hpp>
#include "vec2.hpp"

class Component {
public:
    virtual ~Component() = default;
};

class CTransform : public Component {
public:
    Vec2 pos = {0, 0};
    Vec2 velocity = {0, 0};
    Vec2 scale = {1, 1};
    double angle = 0;

    CTransform(const Vec2& p) : pos{p} {};
    ~CTransform(){};
};

class CBoundingBox : public Component {
public:
    Vec2 size;
    public:
    CBoundingBox(const Vec2 &s) : size{s} {};
    ~CBoundingBox(){};
};

class CSprite : public Component {
public:
    sf::Sprite sprite;
    std::string name;

    CSprite(const std::string& n,const sf::Texture &texture) : sprite{texture}, name{n} {};
    ~CSprite(){};
};