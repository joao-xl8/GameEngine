#pragma once
#include <iostream>    
class Vec2
{
public:
    float x = 0;
    float y = 0;
    Vec2(float xin, float yin);
    ~Vec2(){};
    Vec2 operator+(const Vec2 &rhs) const;
    void operator+=(const Vec2 &rhs);
    Vec2 operator-(const Vec2 &rhs) const;
    void operator-=(const Vec2 &rhs);
    Vec2 operator*(const Vec2 &rhs) const;
    Vec2 operator*(const float &rhs) const;
    void operator*=(const Vec2 &rhs);
    Vec2 operator/(const Vec2 &rhs) const;
    void operator/=(const Vec2 &rhs);
    bool operator==(const Vec2 &rhs);
    bool operator!=(const Vec2 &rhs);
    Vec2& add(const Vec2& v);
    Vec2& remove(const Vec2& v);
    Vec2& scale(float s);
    float dist(const Vec2& v) const;
    float dot(const Vec2& v) const;
    float cross(const Vec2& v) const;
    float angle(const Vec2& v) const;
    void normalize();
    Vec2 normalized() const;
    double length() const;
};