#pragma once
#include "../entity.hpp"
#include "../components/engine_components.hpp"
#include "../vec2.hpp"
#include <memory>
#include <string>

class GameEngine;
class EntityManager;

// Base class for all characters (Player, NPCs, etc.)
class Character {
protected:
    std::shared_ptr<Entity> m_entity;
    GameEngine* m_game;
    EntityManager* m_entityManager;
    std::string m_name;
    Vec2 m_tileSize;
    
public:
    Character(GameEngine* game, EntityManager* entityManager, const std::string& name, const Vec2& tileSize);
    virtual ~Character() = default;
    
    // Core functionality
    virtual void init(const Vec2& position) = 0;
    virtual void update(float deltaTime) {}
    
    // Entity management
    void createEntity(const std::string& entityType);
    
    // Getters
    std::shared_ptr<Entity> getEntity() const { return m_entity; }
    const std::string& getName() const { return m_name; }
    Vec2 getPosition() const;
    
    // Setters
    void setPosition(const Vec2& position);
    
    // Component helpers
    template<typename T>
    bool hasComponent() const {
        return m_entity && m_entity->hasComponent<T>();
    }
    
    template<typename T>
    std::shared_ptr<T> getComponent() const {
        return m_entity ? m_entity->getComponent<T>() : nullptr;
    }
    
    template<typename T>
    void addComponent(std::shared_ptr<T> component) {
        if (m_entity) {
            m_entity->addComponent<T>(component);
        }
    }
};
