#pragma once
#include "system.hpp"
#include "../components/engine_components.hpp"
#include <SFML/Graphics.hpp>
#include <algorithm>

class RenderSystem : public System {
private:
    sf::RenderWindow& m_window;

public:
    RenderSystem(sf::RenderWindow& window) : m_window(window) {}

    void update(EntityManager& entities, float dt) override {
        (void)dt; // Unused parameter
        
        // Get all renderable entities
        auto renderableEntities = entities.getEntitiesWith<CTransform, CSprite>();
        
        // Sort by layer and sublayer for proper rendering order
        std::sort(renderableEntities.begin(), renderableEntities.end(), 
            [](const std::shared_ptr<Entity>& a, const std::shared_ptr<Entity>& b) {
                auto layerA = a->getComponent<CLayer>();
                auto layerB = b->getComponent<CLayer>();
                
                if (layerA && layerB) {
                    return layerA->getRenderOrder() < layerB->getRenderOrder();
                } else if (layerA) {
                    return true;  // Entities with layers render first
                } else if (layerB) {
                    return false; // Entities without layers render last
                }
                return false; // Both have no layers, maintain order
            });
        
        // Render all entities
        for (auto& entity : renderableEntities) {
            auto transform = entity->getComponent<CTransform>();
            auto sprite = entity->getComponent<CSprite>();
            
            // Update sprite position and properties
            sprite->sprite.setPosition(transform->pos.x, transform->pos.y);
            sprite->sprite.setScale(transform->scale.x, transform->scale.y);
            sprite->sprite.setRotation(static_cast<float>(transform->angle));
            
            m_window.draw(sprite->sprite);
        }
    }
};
