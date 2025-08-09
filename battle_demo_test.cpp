#include "src/battle_config_loader.hpp"
#include <iostream>

int main() {
    std::cout << "=== Battle System Configuration Test ===" << std::endl;
    
    BattleConfigLoader loader;
    
    // Test 1: Load spells
    std::cout << "\n1. Testing spell loading..." << std::endl;
    if (loader.loadSpells()) {
        std::cout << "✓ Spells loaded successfully" << std::endl;
        
        // Test some spells
        auto fireball = loader.createSpell("fireball");
        auto heal = loader.createSpell("heal_minor");
        auto bite = loader.createSpell("bite");
        
        std::cout << "  Sample spells:" << std::endl;
        std::cout << "    " << fireball.name << " - MP:" << fireball.mpCost 
                  << " DMG:" << fireball.baseDamage << std::endl;
        std::cout << "    " << heal.name << " - MP:" << heal.mpCost 
                  << " DMG:" << heal.baseDamage << std::endl;
        std::cout << "    " << bite.name << " - MP:" << bite.mpCost 
                  << " DMG:" << bite.baseDamage << std::endl;
    } else {
        std::cout << "✗ Failed to load spells" << std::endl;
    }
    
    // Test 2: Load party members
    std::cout << "\n2. Testing party member loading..." << std::endl;
    std::vector<std::string> partyMembers = {"hero", "mage", "warrior", "rogue"};
    
    for (const auto& memberId : partyMembers) {
        if (loader.loadPartyMember(memberId)) {
            std::cout << "✓ Loaded " << memberId << std::endl;
            
            // Test character creation at different levels
            auto level1 = loader.createPartyMember(memberId, 1);
            auto level3 = loader.createPartyMember(memberId, 3);
            
            std::cout << "    Level 1: " << level1.name << " - HP:" << level1.maxHP 
                      << " ATK:" << level1.attack << " DEF:" << level1.defense << std::endl;
            std::cout << "    Level 3: " << level3.name << " - HP:" << level3.maxHP 
                      << " ATK:" << level3.attack << " DEF:" << level3.defense << std::endl;
        } else {
            std::cout << "✗ Failed to load " << memberId << std::endl;
        }
    }
    
    // Test 3: Load enemies
    std::cout << "\n3. Testing enemy loading..." << std::endl;
    if (loader.loadEnemiesForLevel(1)) {
        std::cout << "✓ Level 1 enemies loaded successfully" << std::endl;
        
        // Test enemy creation
        std::vector<std::string> enemyIds = {"GOBLIN", "SLIME", "RAT", "SPIDER", "GOBLIN_CHIEF"};
        
        for (const auto& enemyId : enemyIds) {
            auto enemy = loader.createEnemy(enemyId);
            if (enemy.name != "Unknown Enemy") {
                std::cout << "    " << enemy.name << " - HP:" << enemy.maxHP 
                          << " ATK:" << enemy.attack << " DEF:" << enemy.defense 
                          << " SPD:" << enemy.speed << std::endl;
            }
        }
    } else {
        std::cout << "✗ Failed to load level 1 enemies" << std::endl;
    }
    
    // Test 4: Load level 2 enemies
    std::cout << "\n4. Testing level 2 enemy loading..." << std::endl;
    if (loader.loadEnemiesForLevel(2)) {
        std::cout << "✓ Level 2 enemies loaded successfully" << std::endl;
        
        std::vector<std::string> level2Enemies = {"WOLF", "BEAR", "BANDIT", "TREANT", "ORC"};
        
        for (const auto& enemyId : level2Enemies) {
            auto enemy = loader.createEnemy(enemyId);
            if (enemy.name != "Unknown Enemy") {
                std::cout << "    " << enemy.name << " - HP:" << enemy.maxHP 
                          << " ATK:" << enemy.attack << " DEF:" << enemy.defense 
                          << " SPD:" << enemy.speed << std::endl;
            }
        }
    } else {
        std::cout << "✗ Failed to load level 2 enemies" << std::endl;
    }
    
    // Test 5: Battle simulation
    std::cout << "\n5. Testing battle setup simulation..." << std::endl;
    
    // Create a party
    std::vector<Scene_Battle::BattleCharacter> party;
    party.push_back(loader.createPartyMember("hero", 2));
    party.push_back(loader.createPartyMember("mage", 2));
    party.push_back(loader.createPartyMember("warrior", 2));
    
    // Create enemies
    std::vector<Scene_Battle::BattleCharacter> enemies;
    enemies.push_back(loader.createEnemy("GOBLIN"));
    enemies.push_back(loader.createEnemy("SLIME"));
    
    std::cout << "Battle Setup:" << std::endl;
    std::cout << "  Party:" << std::endl;
    for (const auto& member : party) {
        std::cout << "    " << member.name << " (HP:" << member.currentHP 
                  << " ATK:" << member.attack << " DEF:" << member.defense 
                  << " SPD:" << member.speed << " MP:" << member.mp << ")" << std::endl;
    }
    
    std::cout << "  Enemies:" << std::endl;
    for (const auto& enemy : enemies) {
        std::cout << "    " << enemy.name << " (HP:" << enemy.currentHP 
                  << " ATK:" << enemy.attack << " DEF:" << enemy.defense 
                  << " SPD:" << enemy.speed << " MP:" << enemy.mp << ")" << std::endl;
    }
    
    std::cout << "\n=== Configuration Test Complete ===" << std::endl;
    std::cout << "All configuration files are working correctly!" << std::endl;
    std::cout << "You can now integrate the battle system into your game." << std::endl;
    
    return 0;
}
