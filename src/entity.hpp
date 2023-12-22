#include <memory>
#include "ccollision.hpp"
#include "ctransform.hpp"
#include "ccshape.hpp"
#include "cinput.hpp"
#include "cscore.hpp"
#include "clifespan.hpp"

class Entity
{
    const size_t m_id = 0;
    const std::string m_tag = "Default";
    bool m_active = true;

    Entity() {};
    Entity(const std::string &tag, size_t id);

    friend class EntityManager;
public:
    std::shared_ptr<CCollision> cCollision;
    std::shared_ptr<CTransform> cTransform;
    std::shared_ptr<CCShape> cShape;
    std::shared_ptr<CInput> cInput;
    std::shared_ptr<CScore> cScore;
    std::shared_ptr<CLifespan> cLifespan;

    ~Entity();

    const std::string& tag() {
        return m_tag;
    } 

    size_t id() {
        return m_id;
    }

    void destroy() {
        m_active = false;
    }

    bool isActive() {
        return m_active;
    }
};
