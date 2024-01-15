#pragma once
#include <SFML/Graphics/CircleShape.hpp>
#include "vec2.hpp"

class CTransform
{
public:
    Vec2 pos = {0, 0};
    Vec2 speed = {0, 0};
    Vec2 scale = {1, 1};
    double angle;
    CTransform(const Vec2 &p, const Vec2 &v, double a) 
        : pos{p}, speed{v}, angle{a} {};
    CTransform(const Vec2 &p, const Vec2 &v, const Vec2 &s, double a) 
        : pos{p}, speed{v}, scale{s}, angle{a} {};
    ~CTransform(){};
};

class CRotate
{
public:
    double angle;
    CRotate(double a) : angle{a} {}
    ~CRotate() {};
};

class CCShape
{
public:
    sf::CircleShape shape;
    CCShape(float r, size_t p, const sf::Color& fill,   
            const sf::Color& outline, float thickness) 
        : shape{r, p} {
        shape.setFillColor(fill);
        shape.setOutlineColor(outline);
        shape.setOutlineThickness(thickness);
        shape.setOrigin(r, r);
    };
    ~CCShape(){};
};

class CCollision
{
    std::shared_ptr<CTransform> m_transform;
public:
    float radius = 0.f;
    CCollision(float r, std::shared_ptr<CTransform> transform) : radius{r}, m_transform{transform}  {};
    bool collidesWith(const CCollision &other) const
    {
        return m_transform->pos.dist(other.m_transform->pos) < radius + other.radius;
    }
    ~CCollision(){};
};

class CScore
{
public:
    int score = 0;
    CScore(int s) : score{s} {};
    ~CScore(){};
};

class CLifespan
{
public:
    float remaining = 0;
    float lifespan = 0;
    CLifespan(float ls) : lifespan{ls}, remaining{ls} {};
    ~CLifespan(){};
};

class CInput
{
public:
    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;
    bool shoot = false;
    
    CInput(){};
    ~CInput(){};
};
