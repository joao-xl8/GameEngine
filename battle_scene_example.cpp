// Example of how to integrate the battle scene into your GameEngine
// This shows how you would transition to battle from the overworld

#include "src/scene_battle.hpp"
#include "src/game_engine.hpp"

// In your Scene_Play or main game scene, you would trigger battles like this:

void Scene_Play::checkForBattleEncounter() {
    // Example: Check if player stepped on an encounter zone
    auto playerPos = m_player->getComponent<CTransform>()->pos;
    
    for (auto& entity : m_entityManager.getEntities("EncounterZone")) {
        if (entity->hasComponent<CEncounterZone>()) {
            auto encounterZone = entity->getComponent<CEncounterZone>();
            auto transform = entity->getComponent<CTransform>();
            
            // Check if player is in encounter zone
            if (isPlayerInZone(playerPos, transform->pos)) {
                if (encounterZone->checkForEncounter()) {
                    // Start battle!
                    startRandomBattle(encounterZone);
                    break;
                }
            }
        }
    }
}

void Scene_Play::startRandomBattle(std::shared_ptr<CEncounterZone> encounterZone) {
    // Get random encounter from the zone
    auto encounter = encounterZone->selectRandomEncounter();
    
    // Create list of enemy types for battle
    std::vector<std::string> enemyTypes;
    for (size_t i = 0; i < encounter.enemyTypes.size(); ++i) {
        for (int j = 0; j < encounter.enemyCounts[i]; ++j) {
            enemyTypes.push_back(encounter.enemyTypes[i]);
        }
    }
    
    // Transition to battle scene
    m_game->changeScene("BATTLE", std::make_shared<Scene_Battle>(m_game, enemyTypes));
}

// In your GameEngine class, you would handle scene transitions:

void GameEngine::changeScene(const std::string& sceneName, std::shared_ptr<Scene> scene) {
    m_scenes[sceneName] = scene;
    m_currentScene = sceneName;
}

// Example of setting up a battle manually:
void setupExampleBattle(GameEngine* game) {
    auto battleScene = std::make_shared<Scene_Battle>(game);
    
    // Add custom player party
    battleScene->addPlayerCharacter("Hero", 100, 25, 15, 12);
    battleScene->addPlayerCharacter("Mage", 60, 30, 8, 15);
    battleScene->addPlayerCharacter("Warrior", 120, 20, 20, 8);
    
    // Add custom enemies
    battleScene->addEnemy("Dragon", 200, 40, 25, 10);
    battleScene->addEnemy("Goblin Shaman", 80, 35, 12, 14);
    
    game->changeScene("BATTLE", battleScene);
}

// Example of battle rewards handling (add this to Scene_Battle::onEnd())
void Scene_Battle::handleBattleRewards() {
    if (m_battleState == BattleState::VICTORY) {
        // Calculate rewards
        int totalGold = 0;
        int totalExp = 0;
        
        for (const auto& enemy : m_enemies) {
            totalGold += 10 + enemy.maxHP / 5; // Gold based on enemy strength
            totalExp += 5 + enemy.attack;      // Experience based on enemy attack
        }
        
        // Give rewards to player party
        for (auto& player : m_playerParty) {
            if (player.isAlive) {
                // Add experience, level up logic, etc.
                std::cout << player.name << " gains " << totalExp << " experience!\n";
            }
        }
        
        std::cout << "You found " << totalGold << " gold!\n";
        
        // You could also handle item drops here
        std::vector<std::string> possibleDrops = {"Health Potion", "Magic Scroll", "Iron Sword"};
        // Random drop logic...
    }
}

// Example of how to load battle backgrounds and character sprites
void Scene_Battle::loadBattleAssets() {
    // Load battle backgrounds
    m_game->getAssets().addTexture("BattleBackground_Forest", "assets/backgrounds/forest_battle.png");
    m_game->getAssets().addTexture("BattleBackground_Cave", "assets/backgrounds/cave_battle.png");
    m_game->getAssets().addTexture("BattleBackground_Castle", "assets/backgrounds/castle_battle.png");
    
    // Load character battle sprites
    m_game->getAssets().addTexture("Hero_Battle", "assets/characters/hero_battle.png");
    m_game->getAssets().addTexture("Mage_Battle", "assets/characters/mage_battle.png");
    m_game->getAssets().addTexture("Warrior_Battle", "assets/characters/warrior_battle.png");
    
    // Load enemy sprites
    m_game->getAssets().addTexture("Goblin_Battle", "assets/enemies/goblin_battle.png");
    m_game->getAssets().addTexture("Orc_Battle", "assets/enemies/orc_battle.png");
    m_game->getAssets().addTexture("Dragon_Battle", "assets/enemies/dragon_battle.png");
    
    // Load UI elements
    m_game->getAssets().addTexture("UI_Panel", "assets/ui/battle_panel.png");
    m_game->getAssets().addTexture("HP_Bar", "assets/ui/hp_bar.png");
    
    // Load battle sounds
    m_game->getAssets().addSound("Battle_Music", "assets/audio/battle_theme.ogg");
    m_game->getAssets().addSound("Attack_Sound", "assets/audio/sword_slash.wav");
    m_game->getAssets().addSound("Magic_Sound", "assets/audio/magic_cast.wav");
    m_game->getAssets().addSound("Victory_Sound", "assets/audio/victory_fanfare.wav");
}
