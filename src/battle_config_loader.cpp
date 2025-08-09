#include "battle_config_loader.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

BattleConfigLoader::BattleConfigLoader() {
    // Constructor
}

BattleConfigLoader::~BattleConfigLoader() {
    // Destructor
}

bool BattleConfigLoader::loadSpells() {
    if (m_spellsLoaded) {
        return true; // Already loaded
    }
    
    std::string filepath = "metadata/characters/spells.txt";
    bool success = parseSpellFile(filepath);
    if (success) {
        m_spellsLoaded = true;
        std::cout << "Loaded " << m_spells.size() << " spells from " << filepath << std::endl;
    }
    return success;
}

bool BattleConfigLoader::loadPartyMember(const std::string& memberId) {
    if (m_partyMembers.find(memberId) != m_partyMembers.end()) {
        return true; // Already loaded
    }
    
    std::string filepath = "metadata/characters/party_" + memberId + ".txt";
    bool success = parsePartyMemberFile(filepath, memberId);
    if (success) {
        std::cout << "Loaded party member: " << memberId << std::endl;
    }
    return success;
}

bool BattleConfigLoader::loadEnemiesForLevel(int level) {
    std::string levelKey = "level_" + std::to_string(level);
    if (m_currentEnemyLevel == levelKey) {
        return true; // Already loaded this level
    }
    
    // Clear previous enemy data
    m_enemies.clear();
    m_enemyGroups.clear();
    m_encounterRates.clear();
    
    std::string filepath = "metadata/characters/enemies_level_" + std::to_string(level) + ".txt";
    bool success = parseEnemyFile(filepath);
    if (success) {
        m_currentEnemyLevel = levelKey;
        std::cout << "Loaded " << m_enemies.size() << " enemies for level " << level << std::endl;
    }
    return success;
}

const SpellData* BattleConfigLoader::getSpell(const std::string& spellId) const {
    auto it = m_spells.find(spellId);
    return (it != m_spells.end()) ? &it->second : nullptr;
}

const PartyMemberData* BattleConfigLoader::getPartyMember(const std::string& memberId) const {
    auto it = m_partyMembers.find(memberId);
    return (it != m_partyMembers.end()) ? &it->second : nullptr;
}

const EnemyData* BattleConfigLoader::getEnemy(const std::string& enemyId) const {
    auto it = m_enemies.find(enemyId);
    return (it != m_enemies.end()) ? &it->second : nullptr;
}

const EnemyGroup* BattleConfigLoader::getEnemyGroup(const std::string& groupId) const {
    auto it = m_enemyGroups.find(groupId);
    return (it != m_enemyGroups.end()) ? &it->second : nullptr;
}

Scene_Battle::BattleCharacter BattleConfigLoader::createPartyMember(const std::string& memberId, int level) const {
    const PartyMemberData* data = getPartyMember(memberId);
    if (!data) {
        std::cout << "Warning: Party member " << memberId << " not found!" << std::endl;
        return Scene_Battle::BattleCharacter("Unknown", 50, 10, 10, 10, 20, true);
    }
    
    // Apply level scaling
    int scaledHP = data->hp + (data->hpGrowth * (level - 1));
    int scaledAttack = data->attack + (data->attackGrowth * (level - 1));
    int scaledDefense = data->defense + (data->defenseGrowth * (level - 1));
    int scaledSpeed = data->speed + (data->speedGrowth * (level - 1));
    int scaledMP = data->mp + (data->mpGrowth * (level - 1));
    
    Scene_Battle::BattleCharacter character(data->name, scaledHP, scaledAttack, 
                                          scaledDefense, scaledSpeed, scaledMP, true);
    character.spriteTexture = data->spriteTexture;
    return character;
}

Scene_Battle::BattleCharacter BattleConfigLoader::createEnemy(const std::string& enemyId) const {
    const EnemyData* data = getEnemy(enemyId);
    if (!data) {
        std::cout << "Warning: Enemy " << enemyId << " not found!" << std::endl;
        return Scene_Battle::BattleCharacter("Unknown Enemy", 30, 8, 5, 10, 10, false);
    }
    
    Scene_Battle::BattleCharacter enemy(data->name, data->hp, data->attack, 
                                      data->defense, data->speed, data->mp, false);
    enemy.spriteTexture = data->spriteTexture;
    return enemy;
}

Scene_Battle::Spell BattleConfigLoader::createSpell(const std::string& spellId) const {
    const SpellData* data = getSpell(spellId);
    if (!data) {
        std::cout << "Warning: Spell " << spellId << " not found!" << std::endl;
        return Scene_Battle::Spell("Unknown Spell", 5, 10, "Unknown spell");
    }
    
    return Scene_Battle::Spell(data->name, data->mpCost, data->baseDamage, data->description);
}

std::string BattleConfigLoader::trim(const std::string& str) const {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::vector<std::string> BattleConfigLoader::split(const std::string& str, char delimiter) const {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(trim(token));
    }
    return tokens;
}

bool BattleConfigLoader::parseSpellFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cout << "Error: Could not open " << filepath << std::endl;
        return false;
    }
    
    std::string line;
    SpellData currentSpell;
    bool inSpell = false;
    
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;
        
        if (line.find("SPELL_ID") == 0) {
            currentSpell = SpellData(); // Reset
            currentSpell.id = line.substr(9);
            inSpell = true;
        } else if (line == "END_SPELL" && inSpell) {
            m_spells[currentSpell.id] = currentSpell;
            inSpell = false;
        } else if (inSpell) {
            auto tokens = split(line, ' ');
            if (tokens.size() >= 2) {
                std::string key = tokens[0];
                std::string value = line.substr(key.length() + 1);
                
                if (key == "NAME") currentSpell.name = value;
                else if (key == "MP_COST") currentSpell.mpCost = std::stoi(value);
                else if (key == "BASE_DAMAGE") currentSpell.baseDamage = std::stoi(value);
                else if (key == "SPELL_TYPE") currentSpell.spellType = value;
                else if (key == "TARGET_TYPE") currentSpell.targetType = value;
                else if (key == "DESCRIPTION") currentSpell.description = value;
            }
        }
    }
    
    return true;
}

bool BattleConfigLoader::parsePartyMemberFile(const std::string& filepath, const std::string& memberId) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cout << "Error: Could not open " << filepath << std::endl;
        return false;
    }
    
    PartyMemberData member;
    std::string line;
    
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;
        
        auto tokens = split(line, ' ');
        if (tokens.size() >= 2) {
            std::string key = tokens[0];
            std::string value = line.substr(key.length() + 1);
            
            if (key == "NAME") member.name = value;
            else if (key == "HP") member.hp = std::stoi(value);
            else if (key == "ATTACK") member.attack = std::stoi(value);
            else if (key == "DEFENSE") member.defense = std::stoi(value);
            else if (key == "SPEED") member.speed = std::stoi(value);
            else if (key == "MP") member.mp = std::stoi(value);
            else if (key == "SPRITE_TEXTURE") member.spriteTexture = value;
            else if (key == "DESCRIPTION") member.description = value;
            else if (key == "HP_GROWTH") member.hpGrowth = std::stoi(value);
            else if (key == "ATTACK_GROWTH") member.attackGrowth = std::stoi(value);
            else if (key == "DEFENSE_GROWTH") member.defenseGrowth = std::stoi(value);
            else if (key == "SPEED_GROWTH") member.speedGrowth = std::stoi(value);
            else if (key == "MP_GROWTH") member.mpGrowth = std::stoi(value);
            else if (key == "WEAPON_TYPE") member.weaponType = value;
            else if (key == "ARMOR_TYPE") member.armorType = value;
            else if (key == "ACCESSORY_TYPE") member.accessoryType = value;
            else if (key == "SPELL") member.knownSpells.push_back(value);
        }
    }
    
    m_partyMembers[memberId] = member;
    return true;
}

bool BattleConfigLoader::parseEnemyFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cout << "Error: Could not open " << filepath << std::endl;
        return false;
    }
    
    std::string line;
    EnemyData currentEnemy;
    bool inEnemy = false;
    
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;
        
        if (line.find("ENEMY_ID") == 0) {
            currentEnemy = EnemyData(); // Reset
            currentEnemy.id = line.substr(9);
            inEnemy = true;
        } else if (line == "END_ENEMY" && inEnemy) {
            m_enemies[currentEnemy.id] = currentEnemy;
            inEnemy = false;
        } else if (inEnemy) {
            auto tokens = split(line, ' ');
            if (tokens.size() >= 2) {
                std::string key = tokens[0];
                std::string value = line.substr(key.length() + 1);
                
                if (key == "NAME") currentEnemy.name = value;
                else if (key == "HP") currentEnemy.hp = std::stoi(value);
                else if (key == "ATTACK") currentEnemy.attack = std::stoi(value);
                else if (key == "DEFENSE") currentEnemy.defense = std::stoi(value);
                else if (key == "SPEED") currentEnemy.speed = std::stoi(value);
                else if (key == "MP") currentEnemy.mp = std::stoi(value);
                else if (key == "SPRITE_TEXTURE") currentEnemy.spriteTexture = value;
                else if (key == "DESCRIPTION") currentEnemy.description = value;
                else if (key == "AI_TYPE") currentEnemy.aiType = value;
                else if (key == "AI_SPELL_CHANCE") currentEnemy.aiSpellChance = std::stoi(value);
                else if (key == "EXP_REWARD") currentEnemy.expReward = std::stoi(value);
                else if (key == "GOLD_REWARD") currentEnemy.goldReward = std::stoi(value);
                else if (key == "SPELL") currentEnemy.knownSpells.push_back(value);
            }
        } else {
            // Handle groups and encounter rates
            auto tokens = split(line, ' ');
            if (tokens.size() >= 2) {
                std::string key = tokens[0];
                if (key.find("GROUP_") == 0) {
                    EnemyGroup group;
                    group.groupId = tokens[1];
                    for (size_t i = 2; i < tokens.size(); ++i) {
                        group.enemyIds.push_back(tokens[i]);
                    }
                    m_enemyGroups[group.groupId] = group;
                } else if (key.find("ENCOUNTER_RATE_") == 0) {
                    m_encounterRates[key] = std::stoi(tokens[1]);
                }
            }
        }
    }
    
    return true;
}
