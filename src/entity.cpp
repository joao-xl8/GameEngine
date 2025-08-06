#include "entity.hpp"

Entity::Entity()
{

}

Entity::Entity(const std::string &tag, size_t id)
    : m_id(id), m_tag(tag)
{
}

Entity::~Entity()
{

}

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
}