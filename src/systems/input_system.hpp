#pragma once
#include "system.hpp"
#include "../components/engine_components.hpp"
#include <SFML/Window.hpp>

class InputSystem : public System {
public:
    void update(EntityManager& entities, float dt) override {
        (void)dt; // Unused parameter
        
        for (auto& entity : entities.getEntitiesWith<CInput>()) {
            auto input = entity->getComponent<CInput>();
            
            // Store previous state for press detection
            bool prevUp = input->up;
            bool prevDown = input->down;
            bool prevLeft = input->left;
            bool prevRight = input->right;
            
            // Update current state
            input->up = sf::Keyboard::isKeyPressed(sf::Keyboard::W) || 
                       sf::Keyboard::isKeyPressed(sf::Keyboard::Up);
            input->down = sf::Keyboard::isKeyPressed(sf::Keyboard::S) || 
                         sf::Keyboard::isKeyPressed(sf::Keyboard::Down);
            input->left = sf::Keyboard::isKeyPressed(sf::Keyboard::A) || 
                         sf::Keyboard::isKeyPressed(sf::Keyboard::Left);
            input->right = sf::Keyboard::isKeyPressed(sf::Keyboard::D) || 
                          sf::Keyboard::isKeyPressed(sf::Keyboard::Right);
            
            // Detect key presses (transition from false to true)
            input->upPressed = !prevUp && input->up;
            input->downPressed = !prevDown && input->down;
            input->leftPressed = !prevLeft && input->left;
            input->rightPressed = !prevRight && input->right;
        }
    }
};
