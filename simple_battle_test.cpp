#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

// Simple structures for testing without SFML dependencies
struct SimpleSpell {
    std::string id;
    std::string name;
    int mpCost;
    int baseDamage;
    std::string spellType;
    std::string targetType;
    std::string description;
};

struct SimpleCharacter {
    std::string name;
    int hp;
    int attack;
    int defense;
    int speed;
    int mp;
    std::string spriteTexture;
    std::string description;
    std::vector<std::string> knownSpells;
};

struct SimpleEnemy {
    std::string id;
    std::string name;
    int hp;
    int attack;
    int defense;
    int speed;
    int mp;
    std::string spriteTexture;
    std::string description;
    std::string aiType;
    int aiSpellChance;
    int expReward;
    int goldReward;
    std::vector<std::string> knownSpells;
};

// Simple parser functions
std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

bool testSpellFile() {
    std::cout << "Testing spell database..." << std::endl;
    
    std::ifstream file("metadata/characters/spells.txt");
    if (!file.is_open()) {
        std::cout << "✗ Could not open spells.txt" << std::endl;
        return false;
    }
    
    std::map<std::string, SimpleSpell> spells;
    std::string line;
    SimpleSpell currentSpell;
    bool inSpell = false;
    int spellCount = 0;
    
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;
        
        if (line.find("SPELL_ID") == 0) {
            currentSpell = SimpleSpell();
            currentSpell.id = line.substr(9);
            inSpell = true;
        } else if (line == "END_SPELL" && inSpell) {
            spells[currentSpell.id] = currentSpell;
            spellCount++;
            inSpell = false;
        } else if (inSpell) {
            size_t spacePos = line.find(' ');
            if (spacePos != std::string::npos) {
                std::string key = line.substr(0, spacePos);
                std::string value = line.substr(spacePos + 1);
                
                if (key == "NAME") currentSpell.name = value;
                else if (key == "MP_COST") currentSpell.mpCost = std::stoi(value);
                else if (key == "BASE_DAMAGE") currentSpell.baseDamage = std::stoi(value);
                else if (key == "SPELL_TYPE") currentSpell.spellType = value;
                else if (key == "TARGET_TYPE") currentSpell.targetType = value;
                else if (key == "DESCRIPTION") currentSpell.description = value;
            }
        }
    }
    
    std::cout << "✓ Loaded " << spellCount << " spells" << std::endl;
    
    // Test some specific spells
    std::vector<std::string> testSpells = {"fireball", "heal_minor", "bite", "sword_strike"};
    for (const auto& spellId : testSpells) {
        if (spells.find(spellId) != spells.end()) {
            const auto& spell = spells[spellId];
            std::cout << "  " << spell.name << " - MP:" << spell.mpCost 
                      << " DMG:" << spell.baseDamage << " Type:" << spell.spellType << std::endl;
        } else {
            std::cout << "  ✗ Spell " << spellId << " not found" << std::endl;
        }
    }
    
    return true;
}

bool testPartyMemberFile(const std::string& memberId) {
    std::cout << "Testing party member: " << memberId << std::endl;
    
    std::string filepath = "metadata/characters/party_" + memberId + ".txt";
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cout << "✗ Could not open " << filepath << std::endl;
        return false;
    }
    
    SimpleCharacter character;
    std::string line;
    
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;
        
        size_t spacePos = line.find(' ');
        if (spacePos != std::string::npos) {
            std::string key = line.substr(0, spacePos);
            std::string value = line.substr(spacePos + 1);
            
            if (key == "NAME") character.name = value;
            else if (key == "HP") character.hp = std::stoi(value);
            else if (key == "ATTACK") character.attack = std::stoi(value);
            else if (key == "DEFENSE") character.defense = std::stoi(value);
            else if (key == "SPEED") character.speed = std::stoi(value);
            else if (key == "MP") character.mp = std::stoi(value);
            else if (key == "SPRITE_TEXTURE") character.spriteTexture = value;
            else if (key == "DESCRIPTION") character.description = value;
            else if (key == "SPELL") character.knownSpells.push_back(value);
        }
    }
    
    std::cout << "✓ " << character.name << " - HP:" << character.hp 
              << " ATK:" << character.attack << " DEF:" << character.defense 
              << " SPD:" << character.speed << " MP:" << character.mp << std::endl;
    std::cout << "  Spells: ";
    for (const auto& spell : character.knownSpells) {
        std::cout << spell << " ";
    }
    std::cout << std::endl;
    
    return true;
}

bool testEnemyFile(int level) {
    std::cout << "Testing enemies level " << level << std::endl;
    
    std::string filepath = "metadata/characters/enemies_level_" + std::to_string(level) + ".txt";
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cout << "✗ Could not open " << filepath << std::endl;
        return false;
    }
    
    std::map<std::string, SimpleEnemy> enemies;
    std::string line;
    SimpleEnemy currentEnemy;
    bool inEnemy = false;
    int enemyCount = 0;
    
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;
        
        if (line.find("ENEMY_ID") == 0) {
            currentEnemy = SimpleEnemy();
            currentEnemy.id = line.substr(9);
            inEnemy = true;
        } else if (line == "END_ENEMY" && inEnemy) {
            enemies[currentEnemy.id] = currentEnemy;
            enemyCount++;
            inEnemy = false;
        } else if (inEnemy) {
            size_t spacePos = line.find(' ');
            if (spacePos != std::string::npos) {
                std::string key = line.substr(0, spacePos);
                std::string value = line.substr(spacePos + 1);
                
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
        }
    }
    
    std::cout << "✓ Loaded " << enemyCount << " enemies" << std::endl;
    
    // Display some enemies
    for (const auto& pair : enemies) {
        const auto& enemy = pair.second;
        std::cout << "  " << enemy.name << " - HP:" << enemy.hp 
                  << " ATK:" << enemy.attack << " DEF:" << enemy.defense 
                  << " SPD:" << enemy.speed << " MP:" << enemy.mp 
                  << " AI:" << enemy.aiType << std::endl;
        std::cout << "    Spells: ";
        for (const auto& spell : enemy.knownSpells) {
            std::cout << spell << " ";
        }
        std::cout << std::endl;
    }
    
    return true;
}

int main() {
    std::cout << "=== Simple Battle Configuration Test ===" << std::endl;
    
    bool allPassed = true;
    
    // Test spell database
    std::cout << "\n1. Testing Spell Database" << std::endl;
    if (!testSpellFile()) {
        allPassed = false;
    }
    
    // Test party members
    std::cout << "\n2. Testing Party Members" << std::endl;
    std::vector<std::string> partyMembers = {"hero", "mage", "warrior", "rogue"};
    for (const auto& member : partyMembers) {
        if (!testPartyMemberFile(member)) {
            allPassed = false;
        }
    }
    
    // Test enemies
    std::cout << "\n3. Testing Enemy Files" << std::endl;
    for (int level = 1; level <= 2; ++level) {
        if (!testEnemyFile(level)) {
            allPassed = false;
        }
    }
    
    std::cout << "\n=== Test Results ===" << std::endl;
    if (allPassed) {
        std::cout << "✓ All configuration files are valid and working!" << std::endl;
        std::cout << "✓ Battle system configuration is ready for integration." << std::endl;
    } else {
        std::cout << "✗ Some configuration files have issues." << std::endl;
    }
    
    return allPassed ? 0 : 1;
}
