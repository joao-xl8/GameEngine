#pragma once

#include "components.hpp"
#include <map>
#include <vector>
#include <memory>
#include <functional>

// Forward declarations
class Skill;
class Item;
class Equipment;

// Enums for RPG systems
enum class ElementType {
    PHYSICAL = 0, FIRE = 1, ICE = 2, LIGHTNING = 3, 
    EARTH = 4, WIND = 5, LIGHT = 6, DARK = 7, HEALING = 8
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

// Enhanced Character Component for RPG
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
    
    // Stat calculation with equipment bonuses
    int getTotalAttack() const;
    int getTotalDefense() const;
    int getTotalMagicAttack() const;
    int getTotalMagicDefense() const;
    int getTotalSpeed() const;
    
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
    
private:
    void calculateExperienceReward();
    void calculateGoldReward();
    void calculateItemRewards();
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
        : zoneName(name), encounterRate(rate) {}
    
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
    
    // Stock management
    void restockItem(std::shared_ptr<Item> item, int amount);
    void setItemAvailability(std::shared_ptr<Item> item, bool available);
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

// Quest System Component (Optional for future expansion)
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

#endif // RPG_COMPONENTS_HPP
