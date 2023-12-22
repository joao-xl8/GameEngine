#pragma once

#include <iostream>

class Vec2
{
public:
    float x = 0;
    float y = 0;

    Vec2(float xin, float yin)
        : x{xin}, y{yin} {};

    ~Vec2(){};

    Vec2 operator+(const Vec2 &rhs) const
    {
        return Vec2(x + rhs.x, y + rhs.y);
    }

    void operator+=(const Vec2 &rhs)
    {
        x += rhs.x;
        y += rhs.y;
    }

    Vec2 operator-(const Vec2 &rhs) const
    {
        return Vec2(x - rhs.x, y - rhs.y);
    }

    void operator-=(const Vec2 &rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
    }

    Vec2 operator*(const Vec2 &rhs) const
    {
        return Vec2(x * rhs.x, y * rhs.y);
    }

    void operator*=(const Vec2 &rhs)
    {
        x *= rhs.x;
        y *= rhs.y;
    }

    Vec2 operator/(const Vec2 &rhs) const
    {
        return Vec2(x * rhs.x, y * rhs.y);
    }

    void operator/=(const Vec2 &rhs)
    {
        x *= rhs.x;
        y *= rhs.y;
    }

    bool operator==(const Vec2 &rhs)
    {
        return (x == rhs.x && y == rhs.y);
    }

    bool operator!=(const Vec2 &rhs)
    {
        return (x != rhs.x || y != rhs.y);
    }

    Vec2 &add(const Vec2 &v)
    {
        x += v.x;
        y += v.y;
        return *this;
    }

    Vec2 &remove(const Vec2 &v)
    {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    Vec2 &scale(float s)
    {
        x *= s;
        y *= s;
        return *this;
    }

    float dist(const Vec2 &v) const
    {
        return sqrtf((v.x - x) * (v.x - x) + (v.y - y) * (v.y - y));
    }

    void normalize() {};
    double length() { return 0.f; };
};