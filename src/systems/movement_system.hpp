#pragma once
#include "system.hpp"
#include "../components/engine_components.hpp"

class MovementSystem : public System {
public:
    void update(EntityManager& entities, float dt) override {
        // Update grid movement
        for (auto& entity : entities.getEntitiesWith<CTransform, CGridMovement>()) {
            auto transform = entity->getComponent<CTransform>();
            auto gridMovement = entity->getComponent<CGridMovement>();
            
            gridMovement->update(dt);
            transform->pos = gridMovement->getWorldPosition();
        }
        
        // Update regular velocity-based movement
        for (auto& entity : entities.getEntitiesWith<CTransform>()) {
            auto transform = entity->getComponent<CTransform>();
            
            if (!entity->hasComponent<CGridMovement>()) {
                transform->pos = transform->pos + (transform->velocity * dt);
            }
        }
    }
};
