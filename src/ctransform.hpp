#include "vec2.hpp"

class CTransform
{
public:
    Vec2 pos = {0, 0};
    Vec2 speed = {0, 0};
    Vec2 scale = {1, 1};
    double angle;
    CTransform(const Vec2 &p, const Vec2 &v)
        : pos{p}, speed{v} {};
    ~CTransform(){};
};
