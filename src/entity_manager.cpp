#include "entity_manager.hpp"


void EntityManager::removeDeadEntities()
{

    m_entities.erase(std::remove_if(m_entities.begin(), m_entities.end(), [](auto &e) { return !e->isActive(); }), m_entities.end());

    for (auto &e : m_entities)
    {
        const std::string &tag = e->tag();
        m_entityMap[tag].erase(std::remove_if(m_entityMap[tag].begin(), m_entityMap[tag].end(), [](auto &e) { return !e->isActive(); }), m_entityMap[tag].end());
    }
}

void EntityManager::update()
{
    removeDeadEntities();
    for (auto e : m_toAdd)
    {
        m_entities.push_back(e);
        m_entityMap[e->tag()].push_back(e);
    }
    m_toAdd.clear();
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string &tag)
{
    auto e = std::shared_ptr<Entity>(new Entity(tag, m_totalEntities++));
    m_toAdd.push_back(e);
    return e;
}

EntityVec &EntityManager::getEntities()
{
    return m_entities;
}

EntityVec &EntityManager::getEntities(const std::string &tag)
{
    return m_entityMap[tag];
}
