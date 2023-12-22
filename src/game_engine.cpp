#include "game_engine.hpp"
void GameEngine::sMovement(EntityVec &entities)
{
    for (auto &e : entities)
    {
        if (e->cTransform)
        {
            e->cTransform->pos += e->cTransform->speed;
        }
    }
}

void GameEngine::sCollisions(EntityVec &entities)
{
}

void GameEngine::sRender(EntityVec &entities, sf::RenderWindow& win)
{
    for (auto &e : entities)
    {
        if (e->cTransform && e->cShape)
        {
            e->cShape->shape.setFillColor(sf::Color::White);
            e->cShape->shape.setRadius(10.0f);
            win.draw(e->cShape->shape);
        }
    }
}

void GameEngine::sUserInput(EntityVec &entities) const
{
}
