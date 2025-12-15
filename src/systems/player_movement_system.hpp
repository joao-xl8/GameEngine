#pragma once
#include "system.hpp"
#include "../components/engine_components.hpp"

class PlayerMovementSystem : public System {
public:
    void update(EntityManager& entities, float dt) override {
        // Handle player input and grid movement
        for (auto& entity : entities.getEntitiesWith<CInput, CGridMovement, CTransform>()) {
            auto input = entity->getComponent<CInput>();
            auto gridMovement = entity->getComponent<CGridMovement>();
            auto transform = entity->getComponent<CTransform>();
            
            // Only process input if not currently moving (for grid-based movement)
            if (!gridMovement->isMoving && gridMovement->canMove) {
                Vec2 direction(0, 0);
                
                // Check for movement input (use pressed for grid movement)
                if (input->upPressed) direction.y = -1;
                else if (input->downPressed) direction.y = 1;
                else if (input->leftPressed) direction.x = -1;
                else if (input->rightPressed) direction.x = 1;
                
                // Try to move if there's input
                if (direction.x != 0 || direction.y != 0) {
                    gridMovement->tryMove(static_cast<int>(direction.x), static_cast<int>(direction.y));
                }
            }
            
            // Reset input press flags after processing
            input->resetPressFlags();
        }
    }
};
