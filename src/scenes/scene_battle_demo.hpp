#pragma once
#include "scene_battle.hpp"
#include "../battle_config_loader.hpp"
#include <memory>

class Scene_Battle_Demo : public Scene_Battle {
private:
    std::unique_ptr<BattleConfigLoader> m_configLoader;
    bool m_demoInitialized = false;
    int m_currentDemoLevel = 1;
    
    // Demo progression
    enum class DemoState {
        MENU,           // Choose demo type
        LEVEL_1_BATTLE, // Fight level 1 enemies
        LEVEL_2_BATTLE, // Fight level 2 enemies
        PARTY_TEST,     // Test different party combinations
        SPELL_TEST      // Test spell system
    };
    
    DemoState m_demoState = DemoState::MENU;
    int m_menuSelection = 0;
    std::vector<std::string> m_menuOptions;

public:
    Scene_Battle_Demo(GameEngine* game);
    ~Scene_Battle_Demo();
    
    void init() override;
    void update() override;
    void sDoAction(const Action& action) override;
    void sRender() override;
    void onEnd() override;

private:
    // Demo setup methods
    void setupDemoMenu();
    void setupLevel1Battle();
    void setupLevel2Battle();
    void setupPartyTest();
    void setupSpellTest();
    
    // Demo battle configurations
    void createDefaultParty();
    void createTestParty();
    void createRandomEnemyEncounter(int level);
    void createBossEncounter(int level);
    
    // UI rendering
    void renderDemoMenu();
    void renderBattleInfo();
    void renderConfigInfo();
    void renderSimpleBattleField();
    void renderPartyMembers(const sf::RectangleShape& area);
    void renderEnemies(const sf::RectangleShape& area);
    void renderBattleStatus();
    
    // Utility methods
    void logBattleStats();
    void displayLoadedConfigurations();
    sf::Color getCharacterColor(const std::string& name);
    sf::Color getEnemyColor(const std::string& name);
    
    // Demo controls
    void handleMenuInput(const Action& action);
    void startSelectedDemo();
    void returnToMenu();
    
    // Configuration testing
    void testSpellSystem();
    void testPartyConfiguration();
    void testEnemyConfiguration();
    
    // Demo battle actions
    void performDemoAttack();
    void performDemoHeal();
    void performDemoSpell();
    void performDamageTest();
};
