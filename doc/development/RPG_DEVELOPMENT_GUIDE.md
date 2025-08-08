# FF-Style RPG with Persona Weakness System - Development Guide

A comprehensive guide to developing a Final Fantasy-inspired RPG with Persona's weakness/strength combat system.

## Table of Contents
1. [Game Vision & Core Concepts](#game-vision--core-concepts)
2. [Architecture Overview](#architecture-overview)
3. [Core Systems Design](#core-systems-design)
4. [Component System Extensions](#component-system-extensions)
5. [Combat System Design](#combat-system-design)
6. [Development Phases](#development-phases)

---

## Game Vision & Core Concepts

### Core Inspiration
- **Final Fantasy (1-6)**: Classic turn-based combat, party management, world exploration
- **Persona Series**: Elemental weakness/strength system, demon/persona collection (Avatar's)
- **Classic JRPGs**: Story-driven, character progression, equipment systems

### Key Features
- **Turn-based Combat**: Strategic, timing-based battles
- **Elemental System**: Fire, Ice, Lightning, Water, Light, Dark, Physical
- **Weakness Exploitation**: Extra turns/damage when hitting weaknesses
- **Party Management**: 3-character active party from larger roster
- **Monster Collection**: Recruit/summon creatures with unique abilities
- **Equipment System**: Weapons, armor, accessories with stat bonuses
- **Skill Trees**: Character progression through abilities
- **World Exploration**: Overworld, dungeons, towns, NPCs

### Target Scope
- **Campaign Length**: 20-40 hours
- **Party Size**: 3 active and total characters
- **Monster Types**: 50-100 unique creatures
- **Dungeons**: 3 major dungeons
- **Towns**: 2 major settlements
- **Skills**: 100+ unique abilities

---

## Architecture Overview

### Current Engine Strengths
Your existing GameEngine provides excellent foundations:
- ✅ **ECS Architecture**: Perfect for RPG entities (characters, monsters, items)
- ✅ **Scene Management**: Ideal for overworld, battle, menu transitions
- ✅ **Grid Movement**: Great for tactical positioning and exploration
- ✅ **Animation System**: Ready for character sprites and effects
- ✅ **Sound System**: Background music and battle sound effects
- ✅ **Input System**: Action-based input perfect for turn-based gameplay

### Required Extensions
- **Battle System**: Turn-based combat with elemental calculations
- **Character Stats**: HP, MP, Attack, Defense, Speed, Elemental Affinities
- **Inventory System**: Items, equipment, consumables management
- **Dialogue System**: NPC conversations, story sequences
- **Save/Load System**: Game state persistence
- **Menu Systems**: Character status, inventory, skills, options
- **World Map**: Large-scale exploration with encounters

---

## Core Systems Design

### 1. Character System
```cpp
// Enhanced character component
class CCharacter : public Component {
public:
    // Basic Stats
    int level = 1;
    int experience = 0;
    int experienceToNext = 100;
    
    // Core Stats
    int maxHP = 100;
    int currentHP = 100;
    int maxMP = 50;
    int currentMP = 50;
    int attack = 10;
    int defense = 8;
    int magicAttack = 12;
    int magicDefense = 10;
    int speed = 15;
    int luck = 5;
    
    // Elemental Affinities (-2=weak, -1=resist, 0=normal, 1=strong, 2=absorb)
    std::map<ElementType, int> elementalAffinities;
    
    // Status Effects
    std::vector<StatusEffect> statusEffects;
    
    // Equipment Slots
    std::shared_ptr<Equipment> weapon;
    std::shared_ptr<Equipment> armor;
    std::shared_ptr<Equipment> accessory;
    
    // Known Skills
    std::vector<std::shared_ptr<Skill>> knownSkills;
    
    // Character Type
    CharacterType type = CharacterType::PLAYER; // PLAYER, ENEMY, NPC
};

enum class ElementType {
    PHYSICAL, FIRE, ICE, LIGHTNING, EARTH, WIND, LIGHT, DARK, HEALING
};

enum class CharacterType {
    PLAYER, ENEMY, NPC, SUMMON
};
```

### 2. Battle System Architecture
```cpp
class CBattleSystem : public Component {
public:
    // Battle State
    BattleState currentState = BattleState::PLAYER_TURN;
    int currentTurn = 0;
    std::queue<std::shared_ptr<Entity>> turnOrder;
    
    // Participants
    std::vector<std::shared_ptr<Entity>> playerParty;
    std::vector<std::shared_ptr<Entity>> enemyParty;
    
    // Battle Results
    int experienceGained = 0;
    int goldGained = 0;
    std::vector<std::shared_ptr<Item>> itemsGained;
    
    // Methods
    void initializeBattle(const std::vector<std::shared_ptr<Entity>>& enemies);
    void calculateTurnOrder();
    void processPlayerAction(const BattleAction& action);
    void processEnemyAI(std::shared_ptr<Entity> enemy);
    bool checkBattleEnd();
    void distributeBattleRewards();
};

enum class BattleState {
    BATTLE_START, PLAYER_TURN, ENEMY_TURN, BATTLE_END, VICTORY, DEFEAT
};

struct BattleAction {
    ActionType type;
    std::shared_ptr<Entity> actor;
    std::shared_ptr<Entity> target;
    std::shared_ptr<Skill> skill;
    std::shared_ptr<Item> item;
};
```

### 3. Elemental Weakness System
```cpp
class CElementalSystem : public Component {
public:
    // Calculate damage with elemental modifiers
    int calculateDamage(int baseDamage, ElementType attackElement, 
                       const std::map<ElementType, int>& targetAffinities) {
        float modifier = 1.0f;
        
        if (targetAffinities.find(attackElement) != targetAffinities.end()) {
            int affinity = targetAffinities.at(attackElement);
            switch (affinity) {
                case -2: modifier = 2.0f; break;  // Weak (double damage)
                case -1: modifier = 0.5f; break; // Resist (half damage)
                case 0:  modifier = 1.0f; break; // Normal
                case 1:  modifier = 0.25f; break; // Strong resist
                case 2:  modifier = -1.0f; break; // Absorb (heal instead)
            }
        }
        
        int finalDamage = static_cast<int>(baseDamage * modifier);
        return finalDamage;
    }
    
    // Check if attack hits weakness (for extra turn mechanics)
    bool hitsWeakness(ElementType attackElement, 
                     const std::map<ElementType, int>& targetAffinities) {
        auto it = targetAffinities.find(attackElement);
        return (it != targetAffinities.end() && it->second == -2);
    }
};
```

---

## Component System Extensions

### New Components Needed

#### 1. Inventory System
```cpp
class CInventory : public Component {
public:
    std::map<std::shared_ptr<Item>, int> items;
    int maxCapacity = 99;
    int gold = 0;
    
    bool addItem(std::shared_ptr<Item> item, int quantity = 1);
    bool removeItem(std::shared_ptr<Item> item, int quantity = 1);
    bool hasItem(std::shared_ptr<Item> item, int quantity = 1);
    int getItemCount(std::shared_ptr<Item> item);
};

class Item {
public:
    std::string name;
    std::string description;
    ItemType type;
    int value;
    bool consumable;
    
    // Effects when used
    virtual void use(std::shared_ptr<Entity> target) = 0;
};
```

#### 2. Dialogue System
```cpp
class CDialogue : public Component {
public:
    std::vector<DialogueNode> dialogueTree;
    int currentNodeIndex = 0;
    bool isActive = false;
    
    void startDialogue(const std::string& dialogueId);
    void processChoice(int choiceIndex);
    bool isDialogueComplete();
};

struct DialogueNode {
    std::string speaker;
    std::string text;
    std::vector<DialogueChoice> choices;
    std::function<void()> onComplete;
};
```

#### 3. World Encounter System
```cpp
class CEncounterZone : public Component {
public:
    std::vector<EncounterGroup> possibleEncounters;
    float encounterRate = 0.05f; // 5% chance per step
    int stepsSinceLastEncounter = 0;
    
    bool checkForEncounter();
    EncounterGroup selectRandomEncounter();
};

struct EncounterGroup {
    std::vector<std::string> enemyTypes;
    int minLevel;
    int maxLevel;
    float weight; // Probability weight
};
```

---

## Combat System Design

### Turn-Based Combat Flow
1. **Battle Initialization**
   - Load enemy party
   - Calculate turn order based on Speed stats
   - Initialize battle UI

2. **Player Turn**
   - Show action menu (Attack, Magic, Item, Defend, Run)
   - Process player selection
   - Calculate damage/effects
   - Check for weakness exploitation (extra turn)

3. **Enemy Turn**
   - AI selects action based on strategy
   - Process enemy action
   - Apply damage/effects

4. **Turn Resolution**
   - Apply status effects
   - Check for battle end conditions
   - Advance to next character's turn

### Weakness System Implementation
```cpp
class WeaknessSystem {
public:
    struct AttackResult {
        int damage;
        bool hitWeakness;
        bool wasResisted;
        bool wasAbsorbed;
        std::string message;
    };
    
    AttackResult processAttack(std::shared_ptr<Entity> attacker,
                              std::shared_ptr<Entity> target,
                              std::shared_ptr<Skill> skill) {
        AttackResult result;
        
        // Get character components
        auto attackerChar = attacker->getComponent<CCharacter>();
        auto targetChar = target->getComponent<CCharacter>();
        
        // Calculate base damage
        int baseDamage = calculateBaseDamage(attackerChar, targetChar, skill);
        
        // Apply elemental modifiers
        result.damage = calculateElementalDamage(baseDamage, skill->element, 
                                                targetChar->elementalAffinities);
        
        // Check for special conditions
        result.hitWeakness = hitsWeakness(skill->element, targetChar->elementalAffinities);
        result.wasResisted = (result.damage < baseDamage);
        result.wasAbsorbed = (result.damage < 0);
        
        // Generate battle message
        result.message = generateBattleMessage(attacker, target, skill, result);
        
        return result;
    }
};
```

This is Part 1 of the guide. Would you like me to continue with the remaining sections covering specific implementation details, scene structures, and development phases?
