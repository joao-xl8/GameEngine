#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include "scenes/scene_battle.hpp"

struct SpellData {
    std::string id;
    std::string name;
    int mpCost;
    int baseDamage;
    std::string spellType;
    std::string targetType;
    std::string description;
};

struct PartyMemberData {
    std::string name;
    int hp;
    int attack;
    int defense;
    int speed;
    int mp;
    std::string spriteTexture;
    std::string description;
    
    // Growth stats
    int hpGrowth;
    int attackGrowth;
    int defenseGrowth;
    int speedGrowth;
    int mpGrowth;
    
    // Equipment
    std::string weaponType;
    std::string armorType;
    std::string accessoryType;
    
    // Known spells
    std::vector<std::string> knownSpells;
};

struct EnemyData {
    std::string id;
    std::string name;
    int hp;
    int attack;
    int defense;
    int speed;
    int mp;
    std::string spriteTexture;
    std::string description;
    
    // AI and behavior
    std::string aiType;
    int aiSpellChance;
    
    // Rewards
    int expReward;
    int goldReward;
    
    // Known spells
    std::vector<std::string> knownSpells;
};

struct EnemyGroup {
    std::string groupId;
    std::vector<std::string> enemyIds;
};

class BattleConfigLoader {
private:
    std::map<std::string, SpellData> m_spells;
    std::map<std::string, PartyMemberData> m_partyMembers;
    std::map<std::string, EnemyData> m_enemies;
    std::map<std::string, EnemyGroup> m_enemyGroups;
    std::map<std::string, int> m_encounterRates;
    
    bool m_spellsLoaded = false;
    std::string m_currentEnemyLevel = "";

public:
    BattleConfigLoader();
    ~BattleConfigLoader();
    
    // Loading methods
    bool loadSpells();
    bool loadPartyMember(const std::string& memberId);
    bool loadEnemiesForLevel(int level);
    
    // Getter methods
    const SpellData* getSpell(const std::string& spellId) const;
    const PartyMemberData* getPartyMember(const std::string& memberId) const;
    const EnemyData* getEnemy(const std::string& enemyId) const;
    const EnemyGroup* getEnemyGroup(const std::string& groupId) const;
    
    // Utility methods
    std::vector<std::string> getAvailablePartyMembers() const;
    std::vector<std::string> getAvailableEnemies() const;
    std::vector<std::string> getEnemyGroupsForLevel() const;
    
    // Battle setup helpers
    Scene_Battle::BattleCharacter createPartyMember(const std::string& memberId, int level = 1) const;
    Scene_Battle::BattleCharacter createEnemy(const std::string& enemyId) const;
    std::vector<Scene_Battle::BattleCharacter> createEnemyGroup(const std::string& groupId) const;
    
    // Spell creation
    Scene_Battle::Spell createSpell(const std::string& spellId) const;
    std::vector<Scene_Battle::Spell> getSpellsForCharacter(const std::string& characterId) const;
    
private:
    // File parsing helpers
    std::string trim(const std::string& str) const;
    std::vector<std::string> split(const std::string& str, char delimiter) const;
    bool parseSpellFile(const std::string& filepath);
    bool parsePartyMemberFile(const std::string& filepath, const std::string& memberId);
    bool parseEnemyFile(const std::string& filepath);
};
