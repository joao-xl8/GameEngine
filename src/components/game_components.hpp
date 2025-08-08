#pragma once

#include "base_component.hpp"
#include "vec2.hpp"
#include <map>
#include <vector>
#include <memory>
#include <functional>
#include <queue>
#include <algorithm>
#include <SFML/Graphics.hpp>

// Forward declarations
class Skill;
class Item;
class Equipment;
class Entity;

// Game-Specific Components - RPG Business Logic

// NPC dialogue state and management
class CNPCDialogue : public Component {
public:
    enum class DialogueState {
        AVAILABLE,      // Can start dialogue
        IN_PROGRESS,    // Currently in dialogue
        COMPLETED,      // Dialogue finished
        LOCKED          // Dialogue not available (quest requirements, etc.)
    };
    
    DialogueState state = DialogueState::AVAILABLE;
    std::string currentDialogueFile;
    std::string defaultDialogueFile;
    
    // Dialogue context and flags
    std::vector<std::string> dialogueFlags; // Story flags that affect dialogue
    std::string lastDialogueId;
    int dialogueCount = 0; // How many times player has talked to this NPC
    
    // Quest-related dialogue
    std::string questDialogueFile;
    bool hasQuest = false;
    bool questCompleted = false;
    
    CNPCDialogue(const std::string& defaultFile = "") 
        : defaultDialogueFile(defaultFile), currentDialogueFile(defaultFile) {}
    
    void setDialogueFile(const std::string& filename) {
        currentDialogueFile = filename;
    }
    
    void addDialogueFlag(const std::string& flag) {
        if (std::find(dialogueFlags.begin(), dialogueFlags.end(), flag) == dialogueFlags.end()) {
            dialogueFlags.push_back(flag);
        }
    }
    
    bool hasDialogueFlag(const std::string& flag) const {
        return std::find(dialogueFlags.begin(), dialogueFlags.end(), flag) != dialogueFlags.end();
    }
    
    void incrementDialogueCount() {
        dialogueCount++;
    }
    
    std::string getActiveDialogueFile() const {
        if (hasQuest && !questCompleted && !questDialogueFile.empty()) {
            return questDialogueFile;
        }
        return currentDialogueFile;
    }
};

// NPC interaction rules and requirements
class CNPCInteraction : public Component {
public:
    bool canInteract = true;
    float interactionRange = 80.0f;
    std::string interactionPrompt = "Press E to talk";
    
    // Interaction requirements
    std::vector<std::string> requiredItems;
    std::vector<std::string> requiredFlags;
    int requiredLevel = 0;
    
    // Interaction callbacks
    std::function<bool()> canInteractCallback;
    std::function<void()> onInteractionStart;
    std::function<void()> onInteractionEnd;
    
    // Interaction state
    bool isPlayerNearby = false;
    bool isInteracting = false;
    float lastInteractionTime = 0.0f;
    
    CNPCInteraction(float range = 80.0f) : interactionRange(range) {}
    
    bool checkRequirements() const {
        // This would check against player state, inventory, etc.
        // For now, just return true
        return canInteract;
    }
    
    void setInteractionCallback(std::function<bool()> callback) {
        canInteractCallback = callback;
    }
    
    void setInteractionPrompt(const std::string& prompt) {
        interactionPrompt = prompt;
    }
};

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

// Enums for RPG systems
enum class ElementType {
    PHYSICAL = 0, FIRE = 1, ICE = 2, LIGHTNING = 3, 
    WATER = 4, LIGHT = 5, DARK = 6, HEALING = 7
};

enum class CharacterType {
    PLAYER, ENEMY, NPC, SUMMON
};

enum class BattleState {
    BATTLE_START, PLAYER_TURN, ENEMY_TURN, ANIMATION, 
    BATTLE_END, VICTORY, DEFEAT
};

enum class ActionType {
    ATTACK, MAGIC, ITEM, DEFEND, RUN, SUMMON
};

enum class StatusEffectType {
    POISON, PARALYSIS, SLEEP, CONFUSION, CHARM, 
    ATTACK_UP, DEFENSE_UP, SPEED_UP, REGEN
};

// Status Effect System
struct StatusEffect {
    StatusEffectType type;
    int duration;          // Turns remaining
    int power;            // Effect strength
    std::string name;
    std::string description;
    
    StatusEffect(StatusEffectType t, int dur, int pow, const std::string& n) 
        : type(t), duration(dur), power(pow), name(n) {}
};

// RPG Character Component
class CCharacter : public Component {
public:
    // Basic Info
    std::string name = "Unknown";
    CharacterType type = CharacterType::PLAYER;
    int level = 1;
    int experience = 0;
    int experienceToNext = 100;
    
    // Core Stats
    int maxHP = 100;
    int currentHP = 100;
    int maxMP = 50;
    int currentMP = 50;
    
    // Battle Stats
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
    
    // Battle-specific
    bool isDefending = false;
    int turnsSinceAction = 0;
    
    CCharacter() {
        initializeDefaultAffinities();
    }
    
    CCharacter(const std::string& charName, CharacterType charType) 
        : name(charName), type(charType) {
        initializeDefaultAffinities();
    }
    
    // Status effect management
    void addStatusEffect(const StatusEffect& effect);
    void removeStatusEffect(StatusEffectType type);
    bool hasStatusEffect(StatusEffectType type) const;
    void updateStatusEffects(); // Called each turn
    
    // Experience and leveling
    void gainExperience(int exp);
    void levelUp();
    
    // HP/MP management
    void takeDamage(int damage);
    void heal(int amount);
    void restoreMP(int amount);
    bool isAlive() const { return currentHP > 0; }
    
private:
    void initializeDefaultAffinities();
};

// Battle Action Structure
struct BattleAction {
    ActionType type = ActionType::ATTACK;
    std::shared_ptr<Entity> actor;
    std::shared_ptr<Entity> target;
    std::vector<std::shared_ptr<Entity>> targets; // For multi-target skills
    std::shared_ptr<Skill> skill;
    std::shared_ptr<Item> item;
    
    BattleAction() = default;
    BattleAction(ActionType t, std::shared_ptr<Entity> a, std::shared_ptr<Entity> tgt) 
        : type(t), actor(a), target(tgt) {}
};

// Battle System Component
class CBattleSystem : public Component {
public:
    // Battle State
    BattleState currentState = BattleState::BATTLE_START;
    int currentTurn = 0;
    std::queue<std::shared_ptr<Entity>> turnOrder;
    std::shared_ptr<Entity> currentActor;
    
    // Participants
    std::vector<std::shared_ptr<Entity>> playerParty;
    std::vector<std::shared_ptr<Entity>> enemyParty;
    
    // Battle Results
    int experienceGained = 0;
    int goldGained = 0;
    std::vector<std::shared_ptr<Item>> itemsGained;
    
    // Battle Log
    std::vector<std::string> battleLog;
    int maxLogEntries = 10;
    
    CBattleSystem() = default;
    
    // Battle flow methods
    void initializeBattle(const std::vector<std::shared_ptr<Entity>>& enemies);
    void calculateTurnOrder();
    void processAction(const BattleAction& action);
    bool checkBattleEnd();
    void distributeBattleRewards();
    void advanceTurn();
    
    // Utility methods
    std::vector<std::shared_ptr<Entity>> getAlivePartyMembers(bool playerParty) const;
    std::shared_ptr<Entity> getNextActor();
    void addToBattleLog(const std::string& message);
};

// Inventory System Component
class CInventory : public Component {
public:
    std::map<std::shared_ptr<Item>, int> items;
    int maxCapacity = 99;
    int gold = 0;
    
    CInventory() = default;
    CInventory(int startingGold) : gold(startingGold) {}
    
    // Item management
    bool addItem(std::shared_ptr<Item> item, int quantity = 1);
    bool removeItem(std::shared_ptr<Item> item, int quantity = 1);
    bool hasItem(std::shared_ptr<Item> item, int quantity = 1) const;
    int getItemCount(std::shared_ptr<Item> item) const;
    
    // Gold management
    bool spendGold(int amount);
    void addGold(int amount);
    
    // Utility
    int getTotalItemCount() const;
    bool isFull() const;
    std::vector<std::shared_ptr<Item>> getAllItems() const;
};

// Dialogue System Component
class CDialogue : public Component {
public:
    struct DialogueChoice {
        std::string text;
        int nextNodeId;
        std::function<bool()> condition; // Optional condition to show choice
        std::function<void()> action;    // Optional action when selected
    };
    
    struct DialogueNode {
        int id;
        std::string speaker;
        std::string text;
        std::vector<DialogueChoice> choices;
        std::function<void()> onEnter;  // Called when node is entered
        std::function<void()> onExit;   // Called when leaving node
        bool isEndNode = false;
    };
    
    std::map<int, DialogueNode> dialogueTree;
    int currentNodeId = 0;
    int startingNodeId = 0;
    bool isActive = false;
    std::string npcName = "NPC";
    
    CDialogue() = default;
    
    // Dialogue control
    void startDialogue(int startNodeId = 0);
    void processChoice(int choiceIndex);
    void endDialogue();
    bool isDialogueComplete() const;
    
    // Content access
    const DialogueNode* getCurrentNode() const;
    std::vector<DialogueChoice> getAvailableChoices() const;
    
    // Dialogue building
    void addNode(const DialogueNode& node);
    void setStartingNode(int nodeId) { startingNodeId = nodeId; }
};

// Encounter Zone Component
class CEncounterZone : public Component {
public:
    struct EncounterGroup {
        std::vector<std::string> enemyTypes;
        std::vector<int> enemyCounts;  // How many of each type
        int minLevel = 1;
        int maxLevel = 5;
        float weight = 1.0f;           // Probability weight
        int goldReward = 50;
        std::vector<std::string> possibleItems;
    };
    
    std::vector<EncounterGroup> possibleEncounters;
    float encounterRate = 0.05f;      // Base encounter rate per step
    int stepsSinceLastEncounter = 0;
    int minStepsBeforeEncounter = 5;   // Minimum steps before encounter possible
    std::string zoneName = "Unknown";
    
    CEncounterZone() = default;
    CEncounterZone(const std::string& name, float rate) 
        : encounterRate(rate), zoneName(name) {}
    
    // Encounter checking
    bool checkForEncounter();
    EncounterGroup selectRandomEncounter() const;
    void resetEncounterCounter() { stepsSinceLastEncounter = 0; }
    
    // Configuration
    void addEncounterGroup(const EncounterGroup& group);
    void setEncounterRate(float rate) { encounterRate = rate; }
};

// Shop System Component
class CShop : public Component {
public:
    struct ShopItem {
        std::shared_ptr<Item> item;
        int price;
        int stock;              // -1 for infinite stock
        bool available = true;  // Can be disabled by story flags
    };
    
    std::vector<ShopItem> inventory;
    std::string shopName = "General Store";
    std::string shopkeeperName = "Shopkeeper";
    float buyPriceMultiplier = 1.0f;    // Price multiplier for buying
    float sellPriceMultiplier = 0.5f;   // Price multiplier for selling
    
    CShop() = default;
    CShop(const std::string& name) : shopName(name) {}
    
    // Shop operations
    bool buyItem(std::shared_ptr<Item> item, int quantity, std::shared_ptr<CInventory> playerInventory);
    bool sellItem(std::shared_ptr<Item> item, int quantity, std::shared_ptr<CInventory> playerInventory);
    int getBuyPrice(std::shared_ptr<Item> item) const;
    int getSellPrice(std::shared_ptr<Item> item) const;
    
    // Inventory management
    void addItem(std::shared_ptr<Item> item, int price, int stock = -1);
    void removeItem(std::shared_ptr<Item> item);
    bool hasItem(std::shared_ptr<Item> item) const;
    std::vector<ShopItem> getAvailableItems() const;
};

// Quest System Component
class CQuest : public Component {
public:
    enum class QuestStatus {
        NOT_STARTED, ACTIVE, COMPLETED, FAILED
    };
    
    struct QuestObjective {
        std::string description;
        bool completed = false;
        std::function<bool()> checkCompletion;
    };
    
    std::string questId;
    std::string title;
    std::string description;
    QuestStatus status = QuestStatus::NOT_STARTED;
    std::vector<QuestObjective> objectives;
    
    // Rewards
    int experienceReward = 0;
    int goldReward = 0;
    std::vector<std::shared_ptr<Item>> itemRewards;
    
    CQuest() = default;
    CQuest(const std::string& id, const std::string& questTitle) 
        : questId(id), title(questTitle) {}
    
    // Quest management
    void startQuest();
    void completeQuest();
    void failQuest();
    bool checkObjectives();
    void addObjective(const QuestObjective& objective);
    
    // Status queries
    bool isActive() const { return status == QuestStatus::ACTIVE; }
    bool isCompleted() const { return status == QuestStatus::COMPLETED; }
    float getCompletionPercentage() const;
};

// Save/Load System Component
class CSaveData : public Component {
public:
    struct CharacterSaveData {
        std::string characterId;
        std::string name;
        int level;
        int experience;
        int currentHP;
        int currentMP;
        std::vector<std::string> knownSkillIds;
        std::string equippedWeaponId;
        std::string equippedArmorId;
        std::string equippedAccessoryId;
        std::map<ElementType, int> elementalAffinities;
    };
    
    struct GameSaveData {
        // Metadata
        std::string saveName;
        std::string timestamp;
        int playTimeSeconds;
        int saveSlot;
        
        // World state
        std::string currentMap;
        Vec2 playerPosition;
        
        // Party data
        std::vector<CharacterSaveData> party;
        std::vector<std::string> availableCharacters; // Characters not in active party
        
        // Inventory
        std::map<std::string, int> inventory; // itemId -> quantity
        int gold;
        
        // Story progress
        std::map<std::string, bool> storyFlags;
        std::map<std::string, int> storyCounters;
        
        // Settings
        float musicVolume = 50.0f;
        float sfxVolume = 70.0f;
        bool fullscreen = false;
    };
    
    GameSaveData saveData;
    
    CSaveData() = default;
    
    // Save/Load operations
    bool saveToFile(const std::string& filename) const;
    bool loadFromFile(const std::string& filename);
    
    // Data conversion helpers
    void populateFromGameState(/* game state parameters */);
    void applyToGameState(/* game state parameters */);
    
    // Utility
    std::string generateTimestamp() const;
    bool isValidSave() const;
};


// Skill System
class Skill {
public:
    std::string id;
    std::string name;
    std::string description;
    ElementType element = ElementType::PHYSICAL;
    
    // Costs and targeting
    int mpCost = 0;
    int hpCost = 0;  // For skills that cost HP
    bool targetsSelf = false;
    bool targetsAllies = false;
    bool targetsEnemies = true;
    bool targetsAll = false;
    int maxTargets = 1;
    
    // Power and effects
    int basePower = 50;
    int accuracy = 95;  // Hit chance percentage
    int criticalChance = 5;  // Critical hit chance percentage
    
    // Status effects
    std::vector<StatusEffect> statusEffectsToApply;
    int statusEffectChance = 100;  // Chance to apply status effects
    
    // Special properties
    bool ignoresDefense = false;
    bool cannotMiss = false;
    bool healsInsteadOfDamages = false;
    bool revivesTarget = false;
    
    // Animation and visual
    std::string animationId = "default_attack";
    std::string soundEffectId = "default_hit";
    
    Skill() = default;
    Skill(const std::string& skillId, const std::string& skillName, ElementType elem, int power)
        : id(skillId), name(skillName), element(elem), basePower(power) {}
    
    // Skill execution
    virtual void execute(std::shared_ptr<Entity> caster, 
                        const std::vector<std::shared_ptr<Entity>>& targets) const;
    
    // Validation
    bool canUse(std::shared_ptr<Entity> caster) const;
    bool isValidTarget(std::shared_ptr<Entity> target, std::shared_ptr<Entity> caster) const;
    
    // Utility
    std::string getElementName() const;
    std::string getTargetingDescription() const;
    
protected:
    virtual int calculateDamage(std::shared_ptr<Entity> caster, std::shared_ptr<Entity> target) const;
    virtual bool rollHit(std::shared_ptr<Entity> caster, std::shared_ptr<Entity> target) const;
    virtual bool rollCritical() const;
};

// Specialized Skill Types
class AttackSkill : public Skill {
public:
    AttackSkill(const std::string& id, const std::string& name, ElementType elem, int power)
        : Skill(id, name, elem, power) {
        targetsEnemies = true;
        targetsAllies = false;
    }
    
    void execute(std::shared_ptr<Entity> caster, 
                const std::vector<std::shared_ptr<Entity>>& targets) const override;
};

class HealingSkill : public Skill {
public:
    HealingSkill(const std::string& id, const std::string& name, int healPower)
        : Skill(id, name, ElementType::HEALING, healPower) {
        targetsEnemies = false;
        targetsAllies = true;
        healsInsteadOfDamages = true;
        cannotMiss = true;
    }
    
    void execute(std::shared_ptr<Entity> caster, 
                const std::vector<std::shared_ptr<Entity>>& targets) const override;
};

class BuffSkill : public Skill {
public:
    BuffSkill(const std::string& id, const std::string& name, const StatusEffect& buff)
        : Skill(id, name, ElementType::LIGHT, 0) {
        targetsEnemies = false;
        targetsAllies = true;
        cannotMiss = true;
        statusEffectsToApply.push_back(buff);
    }
};

class DebuffSkill : public Skill {
public:
    DebuffSkill(const std::string& id, const std::string& name, const StatusEffect& debuff)
        : Skill(id, name, ElementType::DARK, 0) {
        targetsEnemies = true;
        targetsAllies = false;
        statusEffectsToApply.push_back(debuff);
    }
};

// Item System
class Item {
public:
    std::string id;
    std::string name;
    std::string description;
    int value = 10;  // Base gold value
    bool consumable = true;
    bool usableInBattle = true;
    bool usableInField = true;
    
    // Targeting (similar to skills)
    bool targetsSelf = false;
    bool targetsAllies = true;
    bool targetsEnemies = false;
    bool targetsAll = false;
    int maxTargets = 1;
    
    // Visual
    std::string iconId = "default_item";
    std::string useAnimationId = "item_use";
    std::string useSoundId = "item_use";
    
    Item() = default;
    Item(const std::string& itemId, const std::string& itemName, int itemValue)
        : id(itemId), name(itemName), value(itemValue) {}
    
    virtual ~Item() = default;
    
    // Item usage
    virtual bool use(std::shared_ptr<Entity> user, std::shared_ptr<Entity> target) = 0;
    virtual bool canUse(std::shared_ptr<Entity> user, std::shared_ptr<Entity> target) const;
    
    // Utility
    virtual std::string getUseDescription() const { return "Use " + name; }
    virtual Item* clone() const = 0;  // For copying items
};

// Consumable Items
class HealingItem : public Item {
public:
    int healAmount;
    bool percentageHeal = false;  // If true, healAmount is percentage
    
    HealingItem(const std::string& id, const std::string& name, int heal, int val)
        : Item(id, name, val), healAmount(heal) {
        targetsSelf = true;
        targetsAllies = true;
    }
    
    bool use(std::shared_ptr<Entity> user, std::shared_ptr<Entity> target) override;
    Item* clone() const override { return new HealingItem(*this); }
    std::string getUseDescription() const override;
};

class MPRestorationItem : public Item {
public:
    int mpAmount;
    bool percentageRestore = false;
    
    MPRestorationItem(const std::string& id, const std::string& name, int mp, int val)
        : Item(id, name, val), mpAmount(mp) {
        targetsSelf = true;
        targetsAllies = true;
    }
    
    bool use(std::shared_ptr<Entity> user, std::shared_ptr<Entity> target) override;
    Item* clone() const override { return new MPRestorationItem(*this); }
};

class StatusCureItem : public Item {
public:
    std::vector<StatusEffectType> curesStatusEffects;
    bool curesAllNegativeEffects = false;
    
    StatusCureItem(const std::string& id, const std::string& name, int val)
        : Item(id, name, val) {
        targetsSelf = true;
        targetsAllies = true;
    }
    
    bool use(std::shared_ptr<Entity> user, std::shared_ptr<Entity> target) override;
    Item* clone() const override { return new StatusCureItem(*this); }
    void addCuredEffect(StatusEffectType effect) { curesStatusEffects.push_back(effect); }
};

class ReviveItem : public Item {
public:
    int reviveHPPercentage = 25;  // Percentage of max HP to revive with
    
    ReviveItem(const std::string& id, const std::string& name, int val)
        : Item(id, name, val) {
        targetsSelf = false;
        targetsAllies = true;
        usableInField = false;  // Usually battle-only
    }
    
    bool use(std::shared_ptr<Entity> user, std::shared_ptr<Entity> target) override;
    bool canUse(std::shared_ptr<Entity> user, std::shared_ptr<Entity> target) const override;
    Item* clone() const override { return new ReviveItem(*this); }
};

// Equipment System
class Equipment : public Item {
public:
    enum class EquipmentType {
        WEAPON, ARMOR, ACCESSORY
    };
    
    EquipmentType equipmentType;
    
    // Stat bonuses
    int attackBonus = 0;
    int defenseBonus = 0;
    int magicAttackBonus = 0;
    int magicDefenseBonus = 0;
    int speedBonus = 0;
    int luckBonus = 0;
    int maxHPBonus = 0;
    int maxMPBonus = 0;
    
    // Elemental resistances
    std::map<ElementType, int> elementalResistances;
    
    // Status immunities
    std::vector<StatusEffectType> statusImmunities;
    
    // Special abilities granted by equipment
    std::vector<std::shared_ptr<Skill>> grantedSkills;
    
    // Requirements
    int levelRequirement = 1;
    std::vector<std::string> classRequirements;  // Which character classes can equip
    
    Equipment(const std::string& id, const std::string& name, EquipmentType type, int val)
        : Item(id, name, val), equipmentType(type) {
        consumable = false;
        usableInBattle = false;
        usableInField = false;
    }
    
    bool use(std::shared_ptr<Entity> user, std::shared_ptr<Entity> target) override;
    bool canEquip(std::shared_ptr<Entity> character) const;
    Item* clone() const override { return new Equipment(*this); }
    
    // Equipment-specific methods
    void applyBonuses(std::shared_ptr<CCharacter> character) const;
    void removeBonuses(std::shared_ptr<CCharacter> character) const;
    std::string getStatBonusDescription() const;
};

// Weapon specializations
class Weapon : public Equipment {
public:
    ElementType weaponElement = ElementType::PHYSICAL;
    int criticalBonus = 0;  // Additional critical hit chance
    std::vector<std::shared_ptr<Skill>> weaponSkills;  // Skills only usable with this weapon
    
    Weapon(const std::string& id, const std::string& name, int attack, int val)
        : Equipment(id, name, EquipmentType::WEAPON, val) {
        attackBonus = attack;
    }
    
    Item* clone() const override { return new Weapon(*this); }
};

class Armor : public Equipment {
public:
    int physicalDefenseBonus = 0;
    int magicalDefenseBonus = 0;
    
    Armor(const std::string& id, const std::string& name, int defense, int val)
        : Equipment(id, name, EquipmentType::ARMOR, val) {
        defenseBonus = defense;
    }
    
    Item* clone() const override { return new Armor(*this); }
};

class Accessory : public Equipment {
public:
    // Accessories typically provide special effects rather than raw stats
    std::string specialEffect;
    
    Accessory(const std::string& id, const std::string& name, int val)
        : Equipment(id, name, EquipmentType::ACCESSORY, val) {}
    
    Item* clone() const override { return new Accessory(*this); }
};

// Database/Factory classes for loading skills and items
class SkillDatabase {
private:
    std::map<std::string, std::shared_ptr<Skill>> skills;
    
public:
    static SkillDatabase& getInstance() {
        static SkillDatabase instance;
        return instance;
    }
    
    void loadSkillsFromFile(const std::string& filename);
    std::shared_ptr<Skill> getSkill(const std::string& skillId) const;
    std::vector<std::shared_ptr<Skill>> getAllSkills() const;
    std::vector<std::shared_ptr<Skill>> getSkillsByElement(ElementType element) const;
    
    // Predefined skill creation
    void createDefaultSkills();
    
private:
    SkillDatabase() { createDefaultSkills(); }
    std::shared_ptr<Skill> createSkillFromData(const std::map<std::string, std::string>& data);
};

class ItemDatabase {
private:
    std::map<std::string, std::shared_ptr<Item>> items;
    
public:
    static ItemDatabase& getInstance() {
        static ItemDatabase instance;
        return instance;
    }
    
    void loadItemsFromFile(const std::string& filename);
    std::shared_ptr<Item> getItem(const std::string& itemId) const;
    std::vector<std::shared_ptr<Item>> getAllItems() const;
    std::vector<std::shared_ptr<Item>> getItemsByType(const std::string& type) const;
    
    // Predefined item creation
    void createDefaultItems();
    
private:
    ItemDatabase() { createDefaultItems(); }
    std::shared_ptr<Item> createItemFromData(const std::map<std::string, std::string>& data);
};

// Utility functions for element system
class ElementalSystem {
public:
    static std::string getElementName(ElementType element);
    static sf::Color getElementColor(ElementType element);
    static std::vector<ElementType> getAllElements();
    static bool isOffensiveElement(ElementType element);
    static ElementType getOppositeElement(ElementType element);
    
    // Damage calculation with elemental modifiers
    static int calculateElementalDamage(int baseDamage, ElementType attackElement, 
                                       const std::map<ElementType, int>& targetAffinities);
    
    // Check for weakness/resistance
    static bool hitsWeakness(ElementType attackElement, 
                           const std::map<ElementType, int>& targetAffinities);
    static bool isResisted(ElementType attackElement, 
                          const std::map<ElementType, int>& targetAffinities);
    static bool isAbsorbed(ElementType attackElement, 
                          const std::map<ElementType, int>& targetAffinities);
    
    // Generate battle messages
    static std::string generateElementalMessage(ElementType element, bool hitWeakness, 
                                              bool wasResisted, bool wasAbsorbed);
};


