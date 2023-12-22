#include "src/game_engine.hpp"

int main()
{
    // EntityManager m_entities;

    // auto e = m_entities.addEntity("enemy");
    // Vec2 v1 = {2, 2};
    // Vec2 v2 = {2, 2};
    // e->cTransform = std::make_shared<CTransform>(v1, v2);

    GameEngine game_engine;
    
    game_engine.update();

    return 0;
}