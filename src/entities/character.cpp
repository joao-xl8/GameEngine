#include "character.hpp"
#include "../game_engine.hpp"
#include "../entity_manager.hpp"

Character::Character(GameEngine* game, EntityManager* entityManager, const std::string& name, const Vec2& tileSize)
    : m_game(game), m_entityManager(entityManager), m_name(name), m_tileSize(tileSize) {
}

void Character::createEntity(const std::string& entityType) {
    if (m_entityManager) {
        m_entity = m_entityManager->addEntity(entityType);
    }
}

Vec2 Character::getPosition() const {
    if (m_entity && m_entity->hasComponent<CTransform>()) {
        return m_entity->getComponent<CTransform>()->pos;
    }
    return Vec2{0, 0};
}

void Character::setPosition(const Vec2& position) {
    if (m_entity && m_entity->hasComponent<CTransform>()) {
        m_entity->getComponent<CTransform>()->pos = position;
    }
}
