# ECS Architecture Guide

## Overview

The game engine now follows proper Entity-Component-System (ECS) architecture patterns:

- **Entities**: Containers that hold components (pure IDs with component storage)
- **Components**: Pure data structures (no logic)
- **Systems**: Logic processors that operate on entities with specific components

## Core Classes

### System Base Class
```cpp
class System {
public:
    virtual ~System() = default;
    virtual void update(EntityManager& entities, float dt) = 0;
};
```

### SystemManager
Manages and updates all systems in the correct order:
```cpp
SystemManager systemManager;
systemManager.addSystem<MovementSystem>();
systemManager.addSystem<RenderSystem>(window);
systemManager.updateAll(entityManager, deltaTime);
```

## Available Systems

### Core Systems
- **InputSystem**: Handles keyboard input for entities with CInput
- **MovementSystem**: Updates positions based on velocity and grid movement
- **AnimationSystem**: Updates sprite animations
- **CameraSystem**: Manages camera following and viewport
- **RenderSystem**: Renders all entities with sprites in layer order
- **PlayerMovementSystem**: Specialized system for player grid-based movement

### System Execution Order
Systems execute in the order they're added:
1. InputSystem (capture input)
2. PlayerMovementSystem (process player input)
3. MovementSystem (update positions)
4. AnimationSystem (update animations)
5. CameraSystem (update camera)
6. RenderSystem (draw everything)

## Component Guidelines

### ✅ Good Components (Pure Data)
```cpp
struct CTransform : public Component {
    Vec2 position{0, 0};
    Vec2 velocity{0, 0};
    float rotation = 0.0f;
};
```

### ❌ Bad Components (Contains Logic)
```cpp
struct CTransform : public Component {
    Vec2 position;
    void update() { /* logic here - WRONG! */ }
};
```

## Scene Integration

### Setting Up Systems in a Scene
```cpp
class MyScene : public Scene {
    void initSystems() override {
        // Add systems in execution order
        m_systemManager.addSystem<InputSystem>();
        m_systemManager.addSystem<MovementSystem>();
        m_systemManager.addSystem<RenderSystem>(m_game->window());
    }
    
    void init() override {
        initSystems();
        createEntities();
    }
};
```

### Creating Entities
```cpp
void createPlayer() {
    auto player = m_entityManager.addEntity("player");
    
    // Add pure data components
    player->addComponent<CTransform>(std::make_shared<CTransform>(Vec2(100, 100)));
    player->addComponent<CInput>(std::make_shared<CInput>());
    player->addComponent<CGridMovement>(std::make_shared<CGridMovement>());
    
    if (auto texture = m_game->getAssets().getTexture("player")) {
        player->addComponent<CSprite>(std::make_shared<CSprite>("player", *texture));
    }
}
```

## Migration from Old Code

### Before (Logic in Components)
```cpp
// In update loop
for (auto& entity : entities) {
    if (auto animation = entity->getComponent<CAnimation>()) {
        animation->update(dt, sprite); // Logic in component - BAD
    }
}
```

### After (Logic in Systems)
```cpp
// AnimationSystem handles this automatically
class AnimationSystem : public System {
    void update(EntityManager& entities, float dt) override {
        for (auto& entity : entities.getEntitiesWith<CAnimation, CSprite>()) {
            // Update animation logic here
        }
    }
};
```

## Benefits

1. **Performance**: Systems process components in batches (cache-friendly)
2. **Modularity**: Easy to add/remove/reorder systems
3. **Maintainability**: Clear separation of data and logic
4. **Testability**: Systems can be tested independently
5. **Flexibility**: Easy to create new entity types by combining components

## Example Usage

See `src/scenes/scene_example_ecs.hpp` for a complete example of proper ECS usage.
