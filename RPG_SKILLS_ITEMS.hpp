#pragma once

#include "RPG_COMPONENTS.hpp"
#include <string>
#include <vector>
#include <memory>
#include <functional>

// Forward declarations
class Entity;

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

