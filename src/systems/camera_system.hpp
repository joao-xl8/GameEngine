#pragma once
#include "system.hpp"
#include "../components/engine_components.hpp"
#include <SFML/Graphics.hpp>

class CameraSystem : public System {
private:
    sf::View& m_view;

public:
    CameraSystem(sf::View& view) : m_view(view) {}

    void update(EntityManager& entities, float dt) override {
        // Find camera entity
        auto cameraEntities = entities.getEntitiesWith<CCamera>();
        if (cameraEntities.empty()) return;
        
        auto camera = cameraEntities[0]->getComponent<CCamera>();
        
        // Find player entity to follow
        auto playerEntities = entities.getEntities("player");
        if (!playerEntities.empty()) {
            auto playerTransform = playerEntities[0]->getComponent<CTransform>();
            if (playerTransform) {
                camera->followTarget(playerTransform->pos, dt);
            }
        }
        
        // Update view center
        m_view.setCenter(camera->position.x, camera->position.y);
    }
};
