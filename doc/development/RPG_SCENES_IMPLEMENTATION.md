# RPG Scene Architecture & Implementation

Detailed implementation guide for RPG-specific scenes and systems.

## Scene Architecture Overview

### Core Scene Types
1. **Scene_Overworld**: World exploration, NPC interaction, random encounters
2. **Scene_Battle**: Turn-based combat system
3. **Scene_Menu**: Character status, inventory, equipment, save/load
4. **Scene_Dialogue**: Story sequences, NPC conversations
5. **Scene_Shop**: Item buying/selling
6. **Scene_Inn**: Rest, save, party management

---

## Scene_Overworld Implementation

### Core Features
- Grid-based movement with encounter zones
- NPC interaction and dialogue triggers
- Treasure chests and interactive objects
- Zone transitions (towns, dungeons, world map)
- Random encounter system

```cpp
class Scene_Overworld : public Scene {
private:
    std::shared_ptr<Entity> m_player;
    std::vector<std::shared_ptr<Entity>> m_npcs;
    std::vector<std::shared_ptr<Entity>> m_interactables;
    std::shared_ptr<Entity> m_encounterZone;
    
    // Map data
    std::vector<std::vector<int>> m_mapData;
    std::string m_currentMap;
    
    // Encounter system
    int m_stepCounter = 0;
    float m_encounterRate = 0.05f;
    
public:
    Scene_Overworld(GameEngine* game, const std::string& mapPath);
    
    void update() override;
    void sMovement();
    void sInteraction();
    void sEncounters();
    void sRender() override;
    
private:
    void loadMap(const std::string& mapPath);
    void checkForInteraction();
    bool checkForEncounter();
    void triggerBattle(const std::vector<std::string>& enemies);
    void handleZoneTransition(const std::string& newZone, Vec2 spawnPos);
};

// Enhanced player movement with encounter checking
void Scene_Overworld::sMovement() {
    if (!m_player || !m_player->hasComponent<CInput>()) return;
    
    auto input = m_player->getComponent<CInput>();
    auto transform = m_player->getComponent<CTransform>();
    auto gridMovement = m_player->getComponent<CGridMovement>();
    
    bool moved = false;
    
    // Process movement input
    if (input->upPressed && canMoveTo(transform->pos + Vec2{0, 1})) {
        gridMovement->startMove(Vec2{0, 1});
        moved = true;
    }
    // ... other directions
    
    if (moved) {
        m_stepCounter++;
        
        // Check for random encounters
        if (checkForEncounter()) {
            // Trigger battle scene
            auto encounterZone = getEncounterZone(transform->pos);
            if (encounterZone) {
                auto encounter = encounterZone->selectRandomEncounter();
                triggerBattle(encounter.enemyTypes);
            }
        }
    }
}

bool Scene_Overworld::checkForEncounter() {
    // Simple encounter rate calculation
    float encounterChance = m_encounterRate * m_stepCounter;
    
    if (encounterChance > 0.1f) { // Max 10% chance
        encounterChance = 0.1f;
    }
    
    if ((rand() / float(RAND_MAX)) < encounterChance) {
        m_stepCounter = 0; // Reset counter
        return true;
    }
    
    return false;
}
```

### Map Data Structure
```cpp
// Map tile types
enum class TileType {
    GRASS = 0, WATER = 1, MOUNTAIN = 2, FOREST = 3,
    TOWN = 4, DUNGEON = 5, TREASURE = 6, NPC = 7
};

// Map loading from file
void Scene_Overworld::loadMap(const std::string& mapPath) {
    std::ifstream file(mapPath);
    std::string line;
    
    // Load tile data
    while (std::getline(file, line)) {
        std::vector<int> row;
        std::stringstream ss(line);
        std::string tile;
        
        while (std::getline(ss, tile, ',')) {
            row.push_back(std::stoi(tile));
        }
        m_mapData.push_back(row);
    }
    
    // Create entities based on map data
    createMapEntities();
}
```

---

## Scene_Battle Implementation

### Battle Scene Architecture
```cpp
class Scene_Battle : public Scene {
private:
    // Battle participants
    std::vector<std::shared_ptr<Entity>> m_playerParty;
    std::vector<std::shared_ptr<Entity>> m_enemyParty;
    
    // Battle state
    BattleState m_battleState = BattleState::BATTLE_START;
    std::queue<std::shared_ptr<Entity>> m_turnOrder;
    std::shared_ptr<Entity> m_currentActor;
    
    // UI elements
    std::shared_ptr<BattleUI> m_battleUI;
    std::vector<std::string> m_battleLog;
    
    // Battle systems
    std::shared_ptr<WeaknessSystem> m_weaknessSystem;
    std::shared_ptr<AISystem> m_aiSystem;
    
public:
    Scene_Battle(GameEngine* game, const std::vector<std::string>& enemyTypes);
    
    void update() override;
    void sInput() override;
    void sBattleLogic();
    void sAnimation();
    void sRender() override;
    
private:
    void initializeBattle();
    void calculateTurnOrder();
    void processPlayerTurn();
    void processEnemyTurn();
    void executeAction(const BattleAction& action);
    void checkBattleEnd();
    void endBattle(bool victory);
};

// Battle action processing
void Scene_Battle::executeAction(const BattleAction& action) {
    auto actor = action.actor;
    auto target = action.target;
    
    switch (action.type) {
        case ActionType::ATTACK: {
            auto result = m_weaknessSystem->processAttack(actor, target, action.skill);
            
            // Apply damage
            auto targetChar = target->getComponent<CCharacter>();
            if (result.wasAbsorbed) {
                targetChar->currentHP = std::min(targetChar->maxHP, 
                                               targetChar->currentHP - result.damage);
                m_battleLog.push_back(target->getTag() + " absorbed the attack!");
            } else {
                targetChar->currentHP -= result.damage;
                m_battleLog.push_back(result.message);
            }
            
            // Check for weakness exploitation (extra turn in Persona style)
            if (result.hitWeakness && actor->getComponent<CCharacter>()->type == CharacterType::PLAYER) {
                m_battleLog.push_back("Hit weakness! Extra turn!");
                // Don't advance turn order
                return;
            }
            
            break;
        }
        
        case ActionType::MAGIC: {
            // Similar to attack but with MP cost
            auto actorChar = actor->getComponent<CCharacter>();
            if (actorChar->currentMP >= action.skill->mpCost) {
                actorChar->currentMP -= action.skill->mpCost;
                auto result = m_weaknessSystem->processAttack(actor, target, action.skill);
                // Process result...
            }
            break;
        }
        
        case ActionType::ITEM: {
            // Use item from inventory
            action.item->use(target);
            // Remove item from inventory
            break;
        }
    }
    
    // Advance to next turn
    advanceTurn();
}
```

### AI System for Enemies
```cpp
class AISystem {
public:
    BattleAction selectAction(std::shared_ptr<Entity> enemy, 
                             const std::vector<std::shared_ptr<Entity>>& targets) {
        auto enemyChar = enemy->getComponent<CCharacter>();
        BattleAction action;
        action.actor = enemy;
        
        // Simple AI strategy
        // 1. Use healing if HP < 30%
        if (enemyChar->currentHP < enemyChar->maxHP * 0.3f) {
            auto healSkill = findHealingSkill(enemyChar->knownSkills);
            if (healSkill && enemyChar->currentMP >= healSkill->mpCost) {
                action.type = ActionType::MAGIC;
                action.skill = healSkill;
                action.target = enemy; // Self-heal
                return action;
            }
        }
        
        // 2. Target player weaknesses
        auto bestTarget = findWeakestTarget(targets, enemyChar->knownSkills);
        if (bestTarget.target) {
            action.type = ActionType::MAGIC;
            action.skill = bestTarget.skill;
            action.target = bestTarget.target;
            return action;
        }
        
        // 3. Default to physical attack on random target
        action.type = ActionType::ATTACK;
        action.target = targets[rand() % targets.size()];
        action.skill = getBasicAttackSkill();
        
        return action;
    }
    
private:
    struct TargetInfo {
        std::shared_ptr<Entity> target;
        std::shared_ptr<Skill> skill;
        float effectiveness;
    };
    
    TargetInfo findWeakestTarget(const std::vector<std::shared_ptr<Entity>>& targets,
                                const std::vector<std::shared_ptr<Skill>>& skills) {
        TargetInfo best;
        best.effectiveness = 0.0f;
        
        for (auto& target : targets) {
            auto targetChar = target->getComponent<CCharacter>();
            
            for (auto& skill : skills) {
                // Check if this skill would hit a weakness
                auto it = targetChar->elementalAffinities.find(skill->element);
                if (it != targetChar->elementalAffinities.end() && it->second == -2) {
                    if (skill->power > best.effectiveness) {
                        best.target = target;
                        best.skill = skill;
                        best.effectiveness = skill->power * 2.0f; // Weakness multiplier
                    }
                }
            }
        }
        
        return best;
    }
};
```

---

## Menu System Implementation

### Scene_Menu Architecture
```cpp
class Scene_Menu : public Scene {
private:
    enum class MenuState {
        MAIN_MENU, CHARACTER_STATUS, INVENTORY, EQUIPMENT, 
        SKILLS, SAVE_LOAD, OPTIONS
    };
    
    MenuState m_currentMenu = MenuState::MAIN_MENU;
    int m_selectedIndex = 0;
    
    // Party data
    std::vector<std::shared_ptr<Entity>> m_party;
    int m_selectedCharacter = 0;
    
    // UI elements
    sf::Text m_menuText;
    std::vector<std::string> m_menuOptions;
    
public:
    Scene_Menu(GameEngine* game);
    
    void update() override;
    void sInput() override;
    void sRender() override;
    
private:
    void updateMainMenu();
    void updateCharacterStatus();
    void updateInventory();
    void updateEquipment();
    void renderCharacterStats(std::shared_ptr<Entity> character, Vec2 position);
    void renderInventoryGrid();
};

// Character status display
void Scene_Menu::renderCharacterStats(std::shared_ptr<Entity> character, Vec2 position) {
    auto charComp = character->getComponent<CCharacter>();
    auto transform = character->getComponent<CTransform>();
    
    // Character portrait
    if (character->hasComponent<CSprite>()) {
        auto sprite = character->getComponent<CSprite>();
        sprite->sprite.setPosition(position.x, position.y);
        m_game->window().draw(sprite->sprite);
    }
    
    // Stats display
    std::vector<std::string> stats = {
        "Level: " + std::to_string(charComp->level),
        "HP: " + std::to_string(charComp->currentHP) + "/" + std::to_string(charComp->maxHP),
        "MP: " + std::to_string(charComp->currentMP) + "/" + std::to_string(charComp->maxMP),
        "ATK: " + std::to_string(charComp->attack),
        "DEF: " + std::to_string(charComp->defense),
        "SPD: " + std::to_string(charComp->speed)
    };
    
    for (size_t i = 0; i < stats.size(); i++) {
        m_menuText.setString(stats[i]);
        m_menuText.setPosition(position.x + 100, position.y + i * 25);
        m_game->window().draw(m_menuText);
    }
    
    // Elemental affinities
    renderElementalAffinities(charComp->elementalAffinities, 
                             Vec2{position.x + 300, position.y});
}
```

---

## Save/Load System

### Game State Management
```cpp
class SaveSystem {
public:
    struct GameSave {
        // Player progress
        int playTime;
        std::string currentMap;
        Vec2 playerPosition;
        
        // Party data
        std::vector<CharacterSaveData> party;
        
        // Inventory
        std::map<std::string, int> inventory;
        int gold;
        
        // Story flags
        std::map<std::string, bool> storyFlags;
        
        // Settings
        float musicVolume;
        float sfxVolume;
    };
    
    bool saveGame(const GameSave& saveData, int slot);
    bool loadGame(GameSave& saveData, int slot);
    std::vector<GameSave> getSaveSlots();
    
private:
    std::string getSaveFilePath(int slot);
    void serializeToJson(const GameSave& save, const std::string& filepath);
    bool deserializeFromJson(GameSave& save, const std::string& filepath);
};

struct CharacterSaveData {
    std::string characterId;
    int level;
    int experience;
    int currentHP;
    int currentMP;
    std::vector<std::string> knownSkills;
    std::string equippedWeapon;
    std::string equippedArmor;
    std::string equippedAccessory;
};
```

---

## Development Phase Structure

### Phase 1: Core Battle System (Month 1-2)
1. **Week 1-2**: Implement basic battle scene
   - Turn order calculation
   - Basic attack/defend actions
   - Simple damage calculation
   - Battle UI framework

2. **Week 3-4**: Elemental weakness system
   - Element types and affinities
   - Weakness exploitation mechanics
   - Battle message system
   - Visual feedback for weaknesses

3. **Week 5-6**: Skills and magic system
   - Skill database and loading
   - MP system and costs
   - Area-of-effect abilities
   - Status effects framework

4. **Week 7-8**: Enemy AI and balancing
   - Basic AI decision making
   - Enemy skill usage
   - Battle difficulty tuning
   - Victory/defeat conditions

### Phase 2: Character Progression (Month 3)
1. **Week 1-2**: Character stats and leveling
   - Experience point system
   - Stat growth on level up
   - Character class differences
   - Equipment stat bonuses

2. **Week 3-4**: Equipment system
   - Weapon/armor database
   - Equipment effects on stats
   - Equipment comparison UI
   - Rare/unique equipment

### Phase 3: World Exploration (Month 4-5)
1. **Week 1-2**: Overworld scene enhancement
   - Map loading system
   - NPC interaction framework
   - Zone transition system
   - Treasure chest mechanics

2. **Week 3-4**: Random encounter system
   - Encounter zones and rates
   - Enemy group composition
   - Encounter balancing by area
   - Escape mechanics

3. **Week 5-6**: Dialogue and story system
   - Dialogue tree implementation
   - Story flag management
   - Cutscene framework
   - Quest system basics

### Phase 4: Content Creation (Month 6-8)
1. **Character Design**: Create 6-8 party members with unique abilities
2. **Monster Design**: 50+ enemies with varied elemental affinities
3. **Skill Database**: 100+ skills across different elements
4. **Equipment Database**: 200+ weapons, armor, accessories
5. **Map Creation**: 5-8 major areas with unique encounters
6. **Story Content**: Main quest line and side quests

This covers the core architecture and implementation strategy. Would you like me to continue with specific component implementations or focus on a particular system?
