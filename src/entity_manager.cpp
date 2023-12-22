#include "entity_manager.hpp"

void EntityManager::update()
{
    for (auto e: m_toAdd){
        m_entities.push_back(e);
        m_entityMap[e->tag()].push_back(e);
    }
    // for (auto e: m_entities)
    // {
    //     if (e->isActive())
    //     {
    //         m_entities.erase(std::remove(m_entities.begin(), m_entities.end(), e), m_entities.end());
    //         m_entityMap[e->tag()].erase(std::remove(m_entityMap[e->tag()].begin(), m_entityMap[e->tag()].end(), e), m_entityMap[e->tag()].end());
    //     }
        
    // }
    for (auto it = m_entities.rbegin(); it != m_entities.rend(); ++it) {
    if (!(*it)->isActive()) {
        // Erase from m_entities
        m_entities.erase(std::next(it).base());

        // Erase from m_entityMap
        const std::string& tag = (*it)->tag();
        m_entityMap[tag].erase(std::remove(m_entityMap[tag].begin(), m_entityMap[tag].end(), *it), m_entityMap[tag].end());
    }
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
    // TODO: insert return statement here
    return m_entities;
}

EntityVec& EntityManager::getEntities(const std::string &tag)
{
    return m_entityMap[tag];
}
