#pragma once
#include <memory>
#include "components.hpp"

class Entity
{
    size_t m_id;
    const std::string m_tag = "Default";
    bool m_active = true;

    Entity() {};
    Entity(const std::string &tag, size_t id) : m_tag{tag}, m_id{id} {};

    friend class EntityManager;
public:
    std::shared_ptr<CCollision> cCollision;
    std::shared_ptr<CTransform> cTransform;
    std::shared_ptr<CCShape> cShape;
    std::shared_ptr<CInput> cInput;
    std::shared_ptr<CScore> cScore;
    std::shared_ptr<CLifespan> cLifespan;
    std::shared_ptr<CRotate> cRotate;
    ~Entity(){};
    const std::string& tag() const;
    size_t id() const;
    void destroy();
    bool isActive() const;
    void print() const;
};
