#include "vec2.hpp"

Vec2 Vec2::operator+(const Vec2 &rhs) const
{
    return Vec2(x + rhs.x, y + rhs.y);
}

void Vec2::operator+=(const Vec2 &rhs)
{
    x += rhs.x;
    y += rhs.y;
}

Vec2 Vec2::operator-(const Vec2 &rhs) const
{
    return Vec2(x - rhs.x, y - rhs.y);
}

void Vec2::operator-=(const Vec2 &rhs)
{
    x -= rhs.x;
    y -= rhs.y;
}

Vec2 Vec2::operator*(const Vec2 &rhs) const
{
    return Vec2(x * rhs.x, y * rhs.y);
}

Vec2 Vec2::operator*(const float &rhs) const
{
    return Vec2(x * rhs, y * rhs);
}

void Vec2::operator*=(const Vec2 &rhs)
{
    x *= rhs.x;
    y *= rhs.y;
}

Vec2 Vec2::operator/(const Vec2 &rhs) const
{
    return Vec2(x / rhs.x, y / rhs.y);
}

void Vec2::operator/=(const Vec2 &rhs)
{
    x /= rhs.x;
    y /= rhs.y;
}

bool Vec2::operator==(const Vec2 &rhs)
{
    return (x == rhs.x && y == rhs.y);
}

bool Vec2::operator!=(const Vec2 &rhs)
{
    return (x != rhs.x || y != rhs.y);
}

Vec2& Vec2::add(const Vec2 &v)
{
    x += v.x;
    y += v.y;
    return *this;
}

Vec2& Vec2::remove(const Vec2 &v)
{
    x -= v.x;
    y -= v.y;
    return *this;
}

Vec2 &Vec2::scale(float s)
{
    x *= s;
    y *= s;
    return *this;
}

float Vec2::dist(const Vec2 &v) const
{
    return sqrtf((v.x - x) * (v.x - x) + (v.y - y) * (v.y - y));
}

float Vec2::dot(const Vec2 &v) const
{
    return x * v.x + y * v.y;
}

float Vec2::cross(const Vec2 &v) const
{
    return x * v.y - y * v.x;
}

float Vec2::angle(const Vec2 &v) const
{
    return atan2f(v.y - y, v.x - x);
}

void Vec2::normalize()
{
    float l = length();
    if (l > 0)
    {
        x /= l;
        y /= l;
    }
}

Vec2 Vec2::normalized() const
{
    const float l = length();
    if (l > 0)
    {
        return Vec2(x / l, y / l);
    }
    return Vec2(0, 0);
}

double Vec2::length() const
{
    return sqrt(x * x + y * y);
}
