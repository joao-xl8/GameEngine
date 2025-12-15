#pragma once
#include "system.hpp"
#include <vector>
#include <memory>

class SystemManager {
private:
    std::vector<std::unique_ptr<System>> m_systems;

public:
    SystemManager() = default;
    ~SystemManager() = default;

    template<typename T, typename... Args>
    void addSystem(Args&&... args) {
        m_systems.push_back(std::make_unique<T>(std::forward<Args>(args)...));
    }

    void updateAll(EntityManager& entities, float dt) {
        for (auto& system : m_systems) {
            system->update(entities, dt);
        }
    }

    void clear() {
        m_systems.clear();
    }
};
