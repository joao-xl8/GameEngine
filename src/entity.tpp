#include "entity.hpp"

template<typename C>
void Entity::addComponent(std::shared_ptr<C> component) {
    m_components[typeid(C)] = std::static_pointer_cast<Component>(component);
}

template<typename C>
std::shared_ptr<C> Entity::getComponent() const {
    auto it = m_components.find(typeid(C));
    if (it != m_components.end()) {
        return std::static_pointer_cast<C>(it->second);
    }
    return nullptr;
}

template<typename C>
bool Entity::hasComponent() const {
    return m_components.find(typeid(C)) != m_components.end();
}

template<typename C>
void Entity::removeComponent() {
    m_components.erase(typeid(C));
}