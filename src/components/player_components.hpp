#pragma once
#include "engine_components.hpp"
#include <string>
#include <unordered_map>

// Player-specific movement component with enhanced grid movement
class CPlayerMovement : public Component {
public:
    float moveSpeed = 3.0f;
    float gridSize = 64.0f;
    bool smoothMovement = true;
    bool canMove = true;
    
    // Movement state
    Vec2 targetPosition;
    Vec2 previousPosition;
    bool isMoving = false;
    float moveProgress = 0.0f;
    
    CPlayerMovement(float speed = 3.0f, float grid = 64.0f, bool smooth = true)
        : moveSpeed(speed), gridSize(grid), smoothMovement(smooth) {}
};

// Player input handling with action mapping
class CPlayerInput : public Component {
public:
    // Current input state
    bool up = false, down = false, left = false, right = false;
    bool upPressed = false, downPressed = false, leftPressed = false, rightPressed = false;
    bool interact = false, interactPressed = false;
    bool menu = false, menuPressed = false;
    
    // Input timing
    float inputCooldown = 0.0f;
    float maxInputCooldown = 0.1f;
    
    void resetPressFlags() {
        upPressed = downPressed = leftPressed = rightPressed = false;
        interactPressed = menuPressed = false;
    }
    
    void updateCooldown(float deltaTime) {
        if (inputCooldown > 0) {
            inputCooldown -= deltaTime;
        }
    }
    
    bool canProcessInput() const {
        return inputCooldown <= 0;
    }
    
    void setInputCooldown() {
        inputCooldown = maxInputCooldown;
    }
};

// Player stats and progression
class CPlayerStats : public Component {
public:
    int level = 1;
    int experience = 0;
    int experienceToNext = 100;
    
    int maxHealth = 100;
    int currentHealth = 100;
    int maxMana = 50;
    int currentMana = 50;
    
    // Stats
    int strength = 10;
    int defense = 10;
    int speed = 10;
    int intelligence = 10;
    
    CPlayerStats() = default;
    
    void takeDamage(int damage) {
        currentHealth = std::max(0, currentHealth - std::max(1, damage - defense));
    }
    
    void heal(int amount) {
        currentHealth = std::min(maxHealth, currentHealth + amount);
    }
    
    void gainExperience(int exp) {
        experience += exp;
        while (experience >= experienceToNext) {
            levelUp();
        }
    }
    
private:
    void levelUp() {
        level++;
        experience -= experienceToNext;
        experienceToNext = static_cast<int>(experienceToNext * 1.2f);
        
        // Increase stats on level up
        maxHealth += 10;
        currentHealth = maxHealth;
        maxMana += 5;
        currentMana = maxMana;
        strength += 2;
        defense += 2;
        speed += 1;
        intelligence += 1;
    }
};

// Player inventory system
class CPlayerInventory : public Component {
public:
    struct Item {
        std::string name;
        std::string description;
        int quantity;
        std::string type; // "weapon", "armor", "consumable", "key", etc.
        
        Item(const std::string& n, const std::string& desc, int qty = 1, const std::string& t = "misc")
            : name(n), description(desc), quantity(qty), type(t) {}
    };
    
    std::vector<Item> items;
    int maxSlots = 20;
    
    // Equipment slots
    std::string equippedWeapon = "";
    std::string equippedArmor = "";
    std::string equippedAccessory = "";
    
    bool addItem(const Item& item) {
        // Try to stack with existing item
        for (auto& existingItem : items) {
            if (existingItem.name == item.name) {
                existingItem.quantity += item.quantity;
                return true;
            }
        }
        
        // Add as new item if there's space
        if (items.size() < maxSlots) {
            items.push_back(item);
            return true;
        }
        
        return false; // Inventory full
    }
    
    bool removeItem(const std::string& itemName, int quantity = 1) {
        for (auto it = items.begin(); it != items.end(); ++it) {
            if (it->name == itemName) {
                it->quantity -= quantity;
                if (it->quantity <= 0) {
                    items.erase(it);
                }
                return true;
            }
        }
        return false;
    }
    
    bool hasItem(const std::string& itemName, int quantity = 1) const {
        for (const auto& item : items) {
            if (item.name == itemName && item.quantity >= quantity) {
                return true;
            }
        }
        return false;
    }
};

// Player state management
class CPlayerState : public Component {
public:
    enum class State {
        IDLE,
        WALKING,
        INTERACTING,
        IN_DIALOGUE,
        IN_MENU,
        ATTACKING,
        HURT,
        DEAD
    };
    
    State currentState = State::IDLE;
    State previousState = State::IDLE;
    float stateTimer = 0.0f;
    
    void setState(State newState) {
        if (currentState != newState) {
            previousState = currentState;
            currentState = newState;
            stateTimer = 0.0f;
        }
    }
    
    void updateTimer(float deltaTime) {
        stateTimer += deltaTime;
    }
    
    bool isState(State state) const {
        return currentState == state;
    }
    
    bool wasState(State state) const {
        return previousState == state;
    }
};
