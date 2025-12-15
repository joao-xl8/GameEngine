#pragma once
#include "scene.hpp"
#include "../systems/movement_system.hpp"
#include "../systems/animation_system.hpp"
#include "../systems/render_system.hpp"
#include "../systems/input_system.hpp"
#include "../systems/camera_system.hpp"

// Example scene showing proper ECS usage
class Scene_ExampleECS : public Scene {
private:
    std::shared_ptr<Entity> m_player;

public:
    Scene_ExampleECS(GameEngine* game) : Scene(game) {}

    void init() override {
        // Initialize systems in proper order
        initSystems();
        
        // Create entities
        createPlayer();
        createCamera();
    }

    void initSystems() override {
        // Add systems in the order they should execute
        m_systemManager.addSystem<InputSystem>();
        m_systemManager.addSystem<MovementSystem>();
        m_systemManager.addSystem<AnimationSystem>();
        m_systemManager.addSystem<CameraSystem>(m_game->getGameView());
        m_systemManager.addSystem<RenderSystem>(m_game->window());
    }

    void createPlayer() {
        m_player = m_entityManager.addEntity("player");
        
        // Add components (pure data)
        m_player->addComponent<CTransform>(std::make_shared<CTransform>(Vec2(100, 100)));
        m_player->addComponent<CInput>(std::make_shared<CInput>());
        m_player->addComponent<CGridMovement>(std::make_shared<CGridMovement>(64.0f, 4.0f, true));
        
        // Add sprite if texture is available
        if (auto texture = m_game->getAssets().getTexture("player")) {
            m_player->addComponent<CSprite>(std::make_shared<CSprite>("player", *texture));
            m_player->addComponent<CLayer>(std::make_shared<CLayer>(CLayer::ENTITY));
        }
    }

    void createCamera() {
        auto camera = m_entityManager.addEntity("camera");
        camera->addComponent<CCamera>(std::make_shared<CCamera>(Vec2(0, 0), Vec2(128, 128), 5.0f));
    }

    void update() override {
        // Handle input actions
        // Systems will automatically update via Scene::simulate()
    }

    void sDoAction(const Action& action) override {
        // Handle specific actions if needed
        // Input system handles basic movement automatically
        (void)action;
    }

    void sRender() override {
        // Rendering is handled by RenderSystem
        // Only add UI or debug rendering here
    }

    void onEnd() override {
        // Cleanup if needed
    }
};
