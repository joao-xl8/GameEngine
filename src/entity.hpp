#pragma once
#include <memory>
#include <map>
#include <typeindex>
#include <string>
#include "components/base_component.hpp"


class Entity {
private:
    size_t m_id;
    const std::string m_tag = "Default";
    bool m_active = true;

    // Map to store components of various types
    std::map<std::type_index, std::shared_ptr<Component>> m_components;

public:
    Entity();
    Entity(const std::string& tag, size_t id);

    ~Entity();

    const std::string& tag() const;
    size_t id() const;
    void destroy();
    bool isActive() const;
    void print() const;
    
    // Methods to add, retrieve, check, and remove components
    template<typename C>
    void addComponent(std::shared_ptr<C> component);

    template<typename C>
    std::shared_ptr<C> getComponent() const;

    template<typename C>
    bool hasComponent() const;

    template<typename C>
    void removeComponent();
};

#include "entity.tpp"
