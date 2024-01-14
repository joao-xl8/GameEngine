#include "entity.hpp"

const std::string &Entity::tag() const
{
    return m_tag;
}
size_t Entity::id() const
{
    return m_id;
}

void Entity::destroy()
{
    m_active = false;
}

bool Entity::isActive() const
{
    return m_active;
}

void Entity::print() const
{
    std::cout << "Entity: " << m_tag << " id: " << m_id << std::endl;
    // position
    if (cTransform)
    {
        std::cout << "Position: " << cTransform->pos.x << ", " << cTransform->pos.y << std::endl;
    }
    std::cout << "is active: " << m_active << std::endl;
    if (cLifespan)
    {
        std::cout << "Lifespan: " << cLifespan->remaining << std::endl;
    }
    // shape
    if (cShape)
    {
        std::cout << "Shape: " << cShape->shape.getRadius() << std::endl;
    }
}
