# 🛠️ RPG Shader Usage Guide for Developers

## 🚀 **Quick Start**

### **Basic Integration**
```cpp
// 1. Load shader in your scene
ShaderManager& shaderManager = m_game->getShaderManager();
shaderManager.loadFragmentShader("damage_flash", "assets/shaders/damage_flash.frag");

// 2. Get shader reference
sf::Shader* shader = shaderManager.getShader("damage_flash");

// 3. Set parameters and draw
if (shader) {
    shader->setUniform("flashIntensity", 1.0f);
    shader->setUniform("flashColor", sf::Glsl::Vec3(1.0f, 0.0f, 0.0f)); // Red flash
    window.draw(sprite, shader);
}
```

---

## ⚔️ **Combat System Integration**

### **Damage Flash on Hit**
```cpp
class Character {
private:
    float m_hitFlashTimer = 0.0f;
    bool m_isHit = false;

public:
    void takeDamage(int damage) {
        m_isHit = true;
        m_hitFlashTimer = 0.5f; // Flash for 0.5 seconds
    }
    
    void update(float deltaTime) {
        if (m_hitFlashTimer > 0.0f) {
            m_hitFlashTimer -= deltaTime;
            if (m_hitFlashTimer <= 0.0f) {
                m_isHit = false;
            }
        }
    }
    
    void render(sf::RenderWindow& window) {
        if (m_isHit) {
            sf::Shader* shader = shaderManager.getShader("damage_flash");
            if (shader) {
                float intensity = m_hitFlashTimer / 0.5f; // Fade out over time
                shader->setUniform("flashIntensity", intensity);
                shader->setUniform("flashColor", sf::Glsl::Vec3(1.0f, 1.0f, 1.0f));
                window.draw(m_sprite, shader);
                return;
            }
        }
        window.draw(m_sprite); // Normal rendering
    }
};
```

---

## 🧪 **Status Effect System**

### **Status Effect Manager**
```cpp
enum class StatusType {
    NONE, POISON, FREEZE, STONE, INVISIBLE, BERSERK, HASTE, PROTECT, BLIND
};

class StatusEffectManager {
private:
    std::map<StatusType, float> m_statusTimers;
    std::map<StatusType, std::string> m_shaderNames = {
        {StatusType::POISON, "poison_effect"},
        {StatusType::FREEZE, "freeze_ice"},
        {StatusType::STONE, "stone_petrify"},
        {StatusType::INVISIBLE, "invisibility"},
        {StatusType::BERSERK, "berserk_rage"},
        {StatusType::HASTE, "haste_speed"},
        {StatusType::PROTECT, "holy_protect"},
        {StatusType::BLIND, "darkness_blind"}
    };

public:
    void applyStatus(StatusType status, float duration) {
        m_statusTimers[status] = duration;
    }
    
    void update(float deltaTime) {
        for (auto& [status, timer] : m_statusTimers) {
            if (timer > 0.0f) {
                timer -= deltaTime;
            }
        }
    }
    
    void renderCharacter(sf::RenderWindow& window, sf::Sprite& sprite, float gameTime) {
        sf::Shader* activeShader = nullptr;
        
        // Priority order for overlapping status effects
        std::vector<StatusType> priority = {
            StatusType::STONE, StatusType::FREEZE, StatusType::BERSERK,
            StatusType::POISON, StatusType::INVISIBLE, StatusType::BLIND,
            StatusType::HASTE, StatusType::PROTECT
        };
        
        for (StatusType status : priority) {
            if (m_statusTimers[status] > 0.0f) {
                activeShader = shaderManager.getShader(m_shaderNames[status]);
                if (activeShader) {
                    setStatusUniforms(activeShader, status, gameTime);
                    break;
                }
            }
        }
        
        if (activeShader) {
            window.draw(sprite, activeShader);
        } else {
            window.draw(sprite);
        }
    }
    
private:
    void setStatusUniforms(sf::Shader* shader, StatusType status, float time) {
        shader->setUniform("time", time);
        
        switch (status) {
            case StatusType::POISON:
                shader->setUniform("poisonColor", sf::Glsl::Vec3(0.2f, 0.8f, 0.2f));
                break;
            case StatusType::STONE:
                shader->setUniform("petrifyLevel", 0.8f); // Mostly petrified
                break;
            case StatusType::FREEZE:
                shader->setUniform("freezeLevel", 0.9f); // Almost frozen
                break;
            case StatusType::BERSERK:
                shader->setUniform("rageLevel", 0.8f); // High rage
                break;
            case StatusType::HASTE:
                shader->setUniform("speedLevel", 0.7f); // Speed boost
                break;
            case StatusType::PROTECT:
                shader->setUniform("protectionLevel", 0.6f); // Protection aura
                break;
            case StatusType::INVISIBLE:
                shader->setUniform("invisLevel", 0.5f); // Partial invisibility
                break;
            case StatusType::BLIND:
                shader->setUniform("darknessLevel", 0.7f); // Darkness effect
                break;
        }
    }
};
```

---

## ✨ **Magic System Integration**

### **Spell Casting with Charge Effect**
```cpp
class SpellCaster {
private:
    float m_castTime = 0.0f;
    float m_maxCastTime = 3.0f;
    bool m_isCasting = false;
    
public:
    void startCasting(float castTime) {
        m_isCasting = true;
        m_castTime = 0.0f;
        m_maxCastTime = castTime;
    }
    
    void update(float deltaTime) {
        if (m_isCasting) {
            m_castTime += deltaTime;
            if (m_castTime >= m_maxCastTime) {
                castSpell();
                m_isCasting = false;
            }
        }
    }
    
    void render(sf::RenderWindow& window, float gameTime) {
        if (m_isCasting) {
            sf::Shader* shader = shaderManager.getShader("magic_charge");
            if (shader) {
                float chargeLevel = m_castTime / m_maxCastTime;
                shader->setUniform("time", gameTime);
                shader->setUniform("chargeLevel", chargeLevel);
                shader->setUniform("magicColor", sf::Glsl::Vec3(0.3f, 0.6f, 1.0f)); // Blue magic
                window.draw(m_sprite, shader);
                return;
            }
        }
        window.draw(m_sprite);
    }
};
```

---

## 🎮 **Practical Examples**

### **Boss Battle Effects**
```cpp
// Enraged boss with berserk effect
if (boss.getHealthPercent() < 0.3f) {
    sf::Shader* rageShader = shaderManager.getShader("berserk_rage");
    rageShader->setUniform("time", gameTime);
    rageShader->setUniform("rageLevel", 1.0f); // Maximum rage
    window.draw(boss.getSprite(), rageShader);
}

// Protected ally with holy barrier
if (ally.hasProtection()) {
    sf::Shader* protectShader = shaderManager.getShader("holy_protect");
    protectShader->setUniform("time", gameTime);
    protectShader->setUniform("protectionLevel", 0.8f);
    window.draw(ally.getSprite(), protectShader);
}
```

### **Environmental Effects**
```cpp
// Frozen dungeon enemies
for (auto& enemy : frozenEnemies) {
    sf::Shader* iceShader = shaderManager.getShader("freeze_ice");
    iceShader->setUniform("time", gameTime);
    iceShader->setUniform("freezeLevel", 1.0f); // Completely frozen
    window.draw(enemy.getSprite(), iceShader);
}

// Poisonous swamp creatures
for (auto& creature : swampCreatures) {
    sf::Shader* poisonShader = shaderManager.getShader("poison_effect");
    poisonShader->setUniform("time", gameTime);
    poisonShader->setUniform("poisonColor", sf::Glsl::Vec3(0.4f, 0.8f, 0.1f)); // Swamp green
    window.draw(creature.getSprite(), poisonShader);
}
```

---

## ⚡ **Performance Tips**

### **Shader Batching**
```cpp
// Group entities by shader type for better performance
std::map<std::string, std::vector<Entity*>> shaderGroups;

// Sort entities by shader
for (auto& entity : entities) {
    std::string shaderName = entity.getActiveShader();
    shaderGroups[shaderName].push_back(&entity);
}

// Render by shader groups
for (auto& [shaderName, entityGroup] : shaderGroups) {
    sf::Shader* shader = shaderManager.getShader(shaderName);
    if (shader) {
        // Set common uniforms once
        shader->setUniform("time", gameTime);
        
        // Draw all entities with this shader
        for (Entity* entity : entityGroup) {
            entity->setShaderUniforms(shader); // Entity-specific uniforms
            window.draw(entity->getSprite(), shader);
        }
    }
}
```

### **Conditional Rendering**
```cpp
// Only apply shaders when necessary
void renderEntity(Entity& entity, sf::RenderWindow& window) {
    if (entity.hasStatusEffects() || entity.isInCombat()) {
        // Use shader rendering
        renderWithShader(entity, window);
    } else {
        // Fast path for normal entities
        window.draw(entity.getSprite());
    }
}
```

---

## 🎯 **Best Practices**

### **✅ Do's**
- **Cache shader references** instead of looking them up every frame
- **Group similar effects** for batch rendering
- **Use time-based animations** for smooth effects
- **Provide fallback rendering** when shaders aren't supported
- **Test on different hardware** for performance validation

### **❌ Don'ts**
- **Don't load shaders every frame** - load once, reuse many times
- **Don't apply multiple shaders** to the same sprite simultaneously
- **Don't use complex calculations** in fragment shaders
- **Don't forget to handle shader compilation failures**
- **Don't hardcode shader parameters** - make them configurable

---

## 🚀 **Ready to Implement!**

These RPG shaders are production-ready and optimized for real-time gameplay. Start with simple status effects and gradually build up to complex spell systems. Your players will love the professional visual feedback! ✨🎮
