#pragma once

class EntityManager;

class System {
public:
    virtual ~System() = default;
    virtual void update(EntityManager& entities, float dt) = 0;
};
