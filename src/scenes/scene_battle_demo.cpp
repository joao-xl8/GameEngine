#include "scene_battle_demo.hpp"
#include "../game_engine.hpp"
#include <iostream>

Scene_Battle_Demo::Scene_Battle_Demo(GameEngine* game) 
    : Scene_Battle(game), m_configLoader(std::make_unique<BattleConfigLoader>())
{
    init();
}

Scene_Battle_Demo::~Scene_Battle_Demo() {
    // Destructor
}

void Scene_Battle_Demo::init() {
    // Load configuration files
    std::cout << "=== Battle System Demo ===" << std::endl;
    std::cout << "Loading configuration files..." << std::endl;
    
    // Load spells first (required by other configs)
    if (!m_configLoader->loadSpells()) {
        std::cout << "Warning: Could not load spells database" << std::endl;
    }
    
    // Load some party members
    m_configLoader->loadPartyMember("hero");
    m_configLoader->loadPartyMember("mage");
    m_configLoader->loadPartyMember("warrior");
    m_configLoader->loadPartyMember("rogue");
    
    // Load level 1 enemies
    if (!m_configLoader->loadEnemiesForLevel(1)) {
        std::cout << "Warning: Could not load level 1 enemies" << std::endl;
    }
    
    setupDemoMenu();
    
    // Register controls
    registerAction(sf::Keyboard::Up, "UP");
    registerAction(sf::Keyboard::Down, "DOWN");
    registerAction(sf::Keyboard::Enter, "SELECT");
    registerAction(sf::Keyboard::Space, "SELECT");
    registerAction(sf::Keyboard::Escape, "BACK");
    registerAction(sf::Keyboard::B, "BACK");
    registerAction(sf::Keyboard::R, "RELOAD");
    
    // Battle demo actions
    registerAction(sf::Keyboard::A, "ATTACK");
    registerAction(sf::Keyboard::H, "HEAL");
    registerAction(sf::Keyboard::S, "SPELL");
    registerAction(sf::Keyboard::D, "DAMAGE_TEST");
    
    std::cout << "Demo initialized. Use UP/DOWN to navigate, ENTER to select." << std::endl;
    std::cout << "In battle: A=Attack, H=Heal, S=Spell, D=Damage Test" << std::endl;
    displayLoadedConfigurations();
}

void Scene_Battle_Demo::setupDemoMenu() {
    m_demoState = DemoState::MENU;
    m_menuSelection = 0;
    m_menuOptions = {
        "1. Test Level 1 Battle",
        "2. Test Level 2 Battle", 
        "3. Test Party Configuration",
        "4. Test Spell System",
        "5. Configuration Info",
        "6. Exit Demo"
    };
}

void Scene_Battle_Demo::update() {
    if (m_demoState != DemoState::MENU) {
        // Call parent battle update when in battle
        Scene_Battle::update();
    }
}

void Scene_Battle_Demo::sDoAction(const Action& action) {
    if (action.getType() == "START") {
        if (m_demoState == DemoState::MENU) {
            handleMenuInput(action);
        } else {
            // In battle mode, handle battle-specific actions
            if (action.getName() == "BACK") {
                std::cout << "Returning to demo menu..." << std::endl;
                returnToMenu();
            } else if (action.getName() == "ATTACK") {
                performDemoAttack();
            } else if (action.getName() == "HEAL") {
                performDemoHeal();
            } else if (action.getName() == "SPELL") {
                performDemoSpell();
            } else if (action.getName() == "DAMAGE_TEST") {
                performDamageTest();
            } else {
                // Pass other actions to parent battle system
                Scene_Battle::sDoAction(action);
            }
        }
    }
}

void Scene_Battle_Demo::handleMenuInput(const Action& action) {
    if (action.getName() == "UP") {
        m_menuSelection = (m_menuSelection - 1 + m_menuOptions.size()) % m_menuOptions.size();
        std::cout << "Selected: " << m_menuOptions[m_menuSelection] << std::endl;
    } else if (action.getName() == "DOWN") {
        m_menuSelection = (m_menuSelection + 1) % m_menuOptions.size();
        std::cout << "Selected: " << m_menuOptions[m_menuSelection] << std::endl;
    } else if (action.getName() == "SELECT") {
        startSelectedDemo();
    } else if (action.getName() == "BACK") {
        std::cout << "Exiting battle demo..." << std::endl;
        m_game->popScene();
    } else if (action.getName() == "RELOAD") {
        std::cout << "Reloading configurations..." << std::endl;
        init(); // Reload everything
    }
}

void Scene_Battle_Demo::startSelectedDemo() {
    switch (m_menuSelection) {
        case 0: // Level 1 Battle
            setupLevel1Battle();
            break;
        case 1: // Level 2 Battle
            setupLevel2Battle();
            break;
        case 2: // Party Test
            setupPartyTest();
            break;
        case 3: // Spell Test
            setupSpellTest();
            break;
        case 4: // Configuration Info
            displayLoadedConfigurations();
            break;
        case 5: // Exit
            std::cout << "Exiting battle demo..." << std::endl;
            m_game->popScene();
            break;
    }
}

void Scene_Battle_Demo::setupLevel1Battle() {
    std::cout << "\n=== Level 1 Battle Demo ===" << std::endl;
    
    // Clear existing battle data
    m_playerParty.clear();
    m_enemies.clear();
    
    // Create party from config
    createDefaultParty();
    
    // Create level 1 enemies
    createRandomEnemyEncounter(1);
    
    // Initialize battle
    m_battleState = BattleState::ENTERING;
    m_demoState = DemoState::LEVEL_1_BATTLE;
    
    logBattleStats();
    std::cout << "Battle started! Press B to return to menu." << std::endl;
}

void Scene_Battle_Demo::setupLevel2Battle() {
    std::cout << "\n=== Level 2 Battle Demo ===" << std::endl;
    
    // Load level 2 enemies if not already loaded
    if (!m_configLoader->loadEnemiesForLevel(2)) {
        std::cout << "Could not load level 2 enemies, using level 1 instead." << std::endl;
    }
    
    // Clear existing battle data
    m_playerParty.clear();
    m_enemies.clear();
    
    // Create stronger party
    createTestParty();
    
    // Create level 2 enemies
    createRandomEnemyEncounter(2);
    
    m_battleState = BattleState::ENTERING;
    m_demoState = DemoState::LEVEL_2_BATTLE;
    
    logBattleStats();
    std::cout << "Level 2 battle started! Press B to return to menu." << std::endl;
}

void Scene_Battle_Demo::setupPartyTest() {
    std::cout << "\n=== Party Configuration Test ===" << std::endl;
    testPartyConfiguration();
    std::cout << "Press any key to return to menu." << std::endl;
}

void Scene_Battle_Demo::setupSpellTest() {
    std::cout << "\n=== Spell System Test ===" << std::endl;
    testSpellSystem();
    std::cout << "Press any key to return to menu." << std::endl;
}

void Scene_Battle_Demo::createDefaultParty() {
    // Create basic party: Hero, Mage, Warrior
    auto hero = m_configLoader->createPartyMember("hero", 1);
    auto mage = m_configLoader->createPartyMember("mage", 1);
    auto warrior = m_configLoader->createPartyMember("warrior", 1);
    
    m_playerParty.push_back(hero);
    m_playerParty.push_back(mage);
    m_playerParty.push_back(warrior);
    
    std::cout << "Created default party: Hero, Mage, Warrior (Level 1)" << std::endl;
}

void Scene_Battle_Demo::createTestParty() {
    // Create higher level party with all members
    auto hero = m_configLoader->createPartyMember("hero", 3);
    auto mage = m_configLoader->createPartyMember("mage", 3);
    auto warrior = m_configLoader->createPartyMember("warrior", 3);
    auto rogue = m_configLoader->createPartyMember("rogue", 3);
    
    m_playerParty.push_back(hero);
    m_playerParty.push_back(mage);
    m_playerParty.push_back(warrior);
    m_playerParty.push_back(rogue);
    
    std::cout << "Created test party: Hero, Mage, Warrior, Rogue (Level 3)" << std::endl;
}

void Scene_Battle_Demo::createRandomEnemyEncounter(int level) {
    // Simple enemy creation for demo
    if (level == 1) {
        auto goblin = m_configLoader->createEnemy("GOBLIN");
        auto slime = m_configLoader->createEnemy("SLIME");
        m_enemies.push_back(goblin);
        m_enemies.push_back(slime);
        std::cout << "Created Level 1 encounter: Goblin + Slime" << std::endl;
    } else {
        // Try level 2 enemies, fallback to level 1
        auto wolf = m_configLoader->createEnemy("WOLF");
        auto bandit = m_configLoader->createEnemy("BANDIT");
        if (wolf.name != "Unknown Enemy") {
            m_enemies.push_back(wolf);
            m_enemies.push_back(bandit);
            std::cout << "Created Level 2 encounter: Wolf + Bandit" << std::endl;
        } else {
            // Fallback to level 1
            createRandomEnemyEncounter(1);
        }
    }
}

void Scene_Battle_Demo::sRender() {
    if (m_demoState == DemoState::MENU) {
        renderDemoMenu();
    } else {
        // Call parent battle rendering
        Scene_Battle::sRender();
        renderBattleInfo();
    }
}

void Scene_Battle_Demo::renderDemoMenu() {
    // Set the game view for proper rendering
    m_game->window().setView(m_game->getGameView());
    
    // Get view dimensions
    sf::View gameView = m_game->getGameView();
    sf::Vector2f viewSize = gameView.getSize();
    sf::Vector2f viewCenter = gameView.getCenter();
    
    float viewLeft = viewCenter.x - viewSize.x / 2;
    float viewTop = viewCenter.y - viewSize.y / 2;
    
    // Draw background
    sf::RectangleShape background;
    background.setSize(viewSize);
    background.setPosition(viewLeft, viewTop);
    background.setFillColor(sf::Color(20, 30, 50)); // Dark blue
    m_game->window().draw(background);
    
    // Draw menu text
    try {
        sf::Text titleText;
        titleText.setFont(m_game->getAssets().getFont("ShareTech"));
        titleText.setString("BATTLE SYSTEM DEMO");
        titleText.setCharacterSize(32);
        titleText.setFillColor(sf::Color::White);
        titleText.setPosition(viewCenter.x - 150, viewTop + 50);
        m_game->window().draw(titleText);
        
        // Draw menu options
        for (size_t i = 0; i < m_menuOptions.size(); ++i) {
            sf::Text optionText;
            optionText.setFont(m_game->getAssets().getFont("ShareTech"));
            optionText.setString(m_menuOptions[i]);
            optionText.setCharacterSize(20);
            optionText.setFillColor(i == m_menuSelection ? sf::Color::Yellow : sf::Color::White);
            optionText.setPosition(viewCenter.x - 200, viewTop + 120 + (i * 30));
            m_game->window().draw(optionText);
        }
        
        // Draw controls
        sf::Text controlsText;
        controlsText.setFont(m_game->getAssets().getFont("ShareTech"));
        controlsText.setString("UP/DOWN: Navigate  ENTER: Select  ESC: Exit  R: Reload");
        controlsText.setCharacterSize(16);
        controlsText.setFillColor(sf::Color(150, 150, 150));
        controlsText.setPosition(viewCenter.x - 250, viewTop + viewSize.y - 50);
        m_game->window().draw(controlsText);
        
    } catch (const std::exception& e) {
        std::cout << "Could not render demo menu: " << e.what() << std::endl;
    }
}

void Scene_Battle_Demo::renderBattleInfo() {
    // Additional battle info overlay
    try {
        sf::Text infoText;
        infoText.setFont(m_game->getAssets().getFont("ShareTech"));
        infoText.setString("DEMO MODE - Press B to return to menu");
        infoText.setCharacterSize(16);
        infoText.setFillColor(sf::Color::Yellow);
        
        sf::View gameView = m_game->getGameView();
        sf::Vector2f viewCenter = gameView.getCenter();
        sf::Vector2f viewSize = gameView.getSize();
        
        infoText.setPosition(viewCenter.x - 150, viewCenter.y - viewSize.y/2 + 10);
        m_game->window().draw(infoText);
        
        // Render battle field if in battle
        if (m_demoState != DemoState::MENU) {
            renderSimpleBattleField();
        }
        
    } catch (const std::exception& e) {
        // Ignore font errors in demo
    }
}

void Scene_Battle_Demo::renderSimpleBattleField() {
    sf::View gameView = m_game->getGameView();
    sf::Vector2f viewCenter = gameView.getCenter();
    sf::Vector2f viewSize = gameView.getSize();
    
    float viewLeft = viewCenter.x - viewSize.x / 2;
    float viewTop = viewCenter.y - viewSize.y / 2;
    
    // Draw battle areas
    // Player area (left side)
    sf::RectangleShape playerArea;
    playerArea.setSize(sf::Vector2f(viewSize.x * 0.4f, viewSize.y * 0.6f));
    playerArea.setPosition(viewLeft + 20, viewTop + viewSize.y * 0.2f);
    playerArea.setFillColor(sf::Color(0, 100, 0, 100)); // Semi-transparent green
    playerArea.setOutlineColor(sf::Color::Green);
    playerArea.setOutlineThickness(2);
    m_game->window().draw(playerArea);
    
    // Enemy area (right side)
    sf::RectangleShape enemyArea;
    enemyArea.setSize(sf::Vector2f(viewSize.x * 0.4f, viewSize.y * 0.6f));
    enemyArea.setPosition(viewLeft + viewSize.x * 0.55f, viewTop + viewSize.y * 0.2f);
    enemyArea.setFillColor(sf::Color(100, 0, 0, 100)); // Semi-transparent red
    enemyArea.setOutlineColor(sf::Color::Red);
    enemyArea.setOutlineThickness(2);
    m_game->window().draw(enemyArea);
    
    // Draw party members
    renderPartyMembers(playerArea);
    
    // Draw enemies
    renderEnemies(enemyArea);
    
    // Draw battle status
    renderBattleStatus();
}

void Scene_Battle_Demo::renderPartyMembers(const sf::RectangleShape& area) {
    sf::Vector2f areaPos = area.getPosition();
    sf::Vector2f areaSize = area.getSize();
    
    for (size_t i = 0; i < m_playerParty.size(); ++i) {
        const auto& member = m_playerParty[i];
        
        // Character representation (colored circle)
        sf::CircleShape character(20);
        character.setFillColor(getCharacterColor(member.name));
        character.setOutlineColor(sf::Color::White);
        character.setOutlineThickness(2);
        
        // Position characters vertically
        float x = areaPos.x + areaSize.x * 0.3f;
        float y = areaPos.y + (i + 1) * (areaSize.y / (m_playerParty.size() + 1)) - 20;
        character.setPosition(x, y);
        
        m_game->window().draw(character);
        
        // Character name and HP
        try {
            sf::Text nameText;
            nameText.setFont(m_game->getAssets().getFont("ShareTech"));
            nameText.setString(member.name);
            nameText.setCharacterSize(14);
            nameText.setFillColor(sf::Color::White);
            nameText.setPosition(x + 50, y);
            m_game->window().draw(nameText);
            
            sf::Text hpText;
            hpText.setFont(m_game->getAssets().getFont("ShareTech"));
            hpText.setString("HP: " + std::to_string(member.currentHP) + "/" + std::to_string(member.maxHP));
            hpText.setCharacterSize(12);
            hpText.setFillColor(sf::Color::Cyan);
            hpText.setPosition(x + 50, y + 20);
            m_game->window().draw(hpText);
            
            // HP Bar
            sf::RectangleShape hpBar;
            hpBar.setSize(sf::Vector2f(80, 6));
            hpBar.setPosition(x + 50, y + 35);
            hpBar.setFillColor(sf::Color::Red);
            m_game->window().draw(hpBar);
            
            sf::RectangleShape hpFill;
            float hpRatio = (float)member.currentHP / member.maxHP;
            hpFill.setSize(sf::Vector2f(80 * hpRatio, 6));
            hpFill.setPosition(x + 50, y + 35);
            hpFill.setFillColor(sf::Color::Green);
            m_game->window().draw(hpFill);
            
        } catch (const std::exception& e) {
            // Ignore font errors
        }
    }
}

void Scene_Battle_Demo::renderEnemies(const sf::RectangleShape& area) {
    sf::Vector2f areaPos = area.getPosition();
    sf::Vector2f areaSize = area.getSize();
    
    for (size_t i = 0; i < m_enemies.size(); ++i) {
        const auto& enemy = m_enemies[i];
        
        // Enemy representation (colored square)
        sf::RectangleShape enemyShape(sf::Vector2f(30, 30));
        enemyShape.setFillColor(getEnemyColor(enemy.name));
        enemyShape.setOutlineColor(sf::Color::White);
        enemyShape.setOutlineThickness(2);
        
        // Position enemies
        float x = areaPos.x + areaSize.x * 0.2f;
        float y = areaPos.y + (i + 1) * (areaSize.y / (m_enemies.size() + 1)) - 15;
        enemyShape.setPosition(x, y);
        
        m_game->window().draw(enemyShape);
        
        // Enemy name and HP
        try {
            sf::Text nameText;
            nameText.setFont(m_game->getAssets().getFont("ShareTech"));
            nameText.setString(enemy.name);
            nameText.setCharacterSize(14);
            nameText.setFillColor(sf::Color::White);
            nameText.setPosition(x + 40, y);
            m_game->window().draw(nameText);
            
            sf::Text hpText;
            hpText.setFont(m_game->getAssets().getFont("ShareTech"));
            hpText.setString("HP: " + std::to_string(enemy.currentHP) + "/" + std::to_string(enemy.maxHP));
            hpText.setCharacterSize(12);
            hpText.setFillColor(sf::Color::Yellow);
            hpText.setPosition(x + 40, y + 20);
            m_game->window().draw(hpText);
            
            // HP Bar
            sf::RectangleShape hpBar;
            hpBar.setSize(sf::Vector2f(60, 6));
            hpBar.setPosition(x + 40, y + 35);
            hpBar.setFillColor(sf::Color::Red);
            m_game->window().draw(hpBar);
            
            sf::RectangleShape hpFill;
            float hpRatio = (float)enemy.currentHP / enemy.maxHP;
            hpFill.setSize(sf::Vector2f(60 * hpRatio, 6));
            hpFill.setPosition(x + 40, y + 35);
            hpFill.setFillColor(sf::Color(255, 165, 0)); // Orange color
            m_game->window().draw(hpFill);
            
        } catch (const std::exception& e) {
            // Ignore font errors
        }
    }
}

void Scene_Battle_Demo::renderBattleStatus() {
    sf::View gameView = m_game->getGameView();
    sf::Vector2f viewCenter = gameView.getCenter();
    sf::Vector2f viewSize = gameView.getSize();
    
    // Battle status panel at bottom
    sf::RectangleShape statusPanel;
    statusPanel.setSize(sf::Vector2f(viewSize.x * 0.9f, 80));
    statusPanel.setPosition(viewCenter.x - viewSize.x * 0.45f, viewCenter.y + viewSize.y * 0.3f);
    statusPanel.setFillColor(sf::Color(0, 0, 0, 150));
    statusPanel.setOutlineColor(sf::Color::White);
    statusPanel.setOutlineThickness(2);
    m_game->window().draw(statusPanel);
    
    try {
        sf::Text statusText;
        statusText.setFont(m_game->getAssets().getFont("ShareTech"));
        
        std::string status = "Battle Demo - ";
        switch (m_demoState) {
            case DemoState::LEVEL_1_BATTLE:
                status += "Level 1 Battle";
                break;
            case DemoState::LEVEL_2_BATTLE:
                status += "Level 2 Battle";
                break;
            case DemoState::PARTY_TEST:
                status += "Party Test";
                break;
            case DemoState::SPELL_TEST:
                status += "Spell Test";
                break;
            default:
                status += "Demo Mode";
                break;
        }
        
        statusText.setString(status);
        statusText.setCharacterSize(16);
        statusText.setFillColor(sf::Color::White);
        statusText.setPosition(statusPanel.getPosition().x + 10, statusPanel.getPosition().y + 10);
        m_game->window().draw(statusText);
        
        // Instructions
        sf::Text instructText;
        instructText.setFont(m_game->getAssets().getFont("ShareTech"));
        instructText.setString("Press B to return to menu | This is a visual demo of the battle system");
        instructText.setCharacterSize(12);
        instructText.setFillColor(sf::Color(200, 200, 200));
        instructText.setPosition(statusPanel.getPosition().x + 10, statusPanel.getPosition().y + 35);
        m_game->window().draw(instructText);
        
        // Party vs Enemies summary
        sf::Text summaryText;
        summaryText.setFont(m_game->getAssets().getFont("ShareTech"));
        summaryText.setString("Party: " + std::to_string(m_playerParty.size()) + " members | Enemies: " + std::to_string(m_enemies.size()) + " foes");
        summaryText.setCharacterSize(12);
        summaryText.setFillColor(sf::Color(150, 150, 255));
        summaryText.setPosition(statusPanel.getPosition().x + 10, statusPanel.getPosition().y + 55);
        m_game->window().draw(summaryText);
        
    } catch (const std::exception& e) {
        // Ignore font errors
    }
}

sf::Color Scene_Battle_Demo::getCharacterColor(const std::string& name) {
    if (name == "Hero") return sf::Color::Blue;
    if (name == "Lyra") return sf::Color::Magenta;
    if (name == "Gareth") return sf::Color(139, 69, 19); // Brown
    if (name == "Kira") return sf::Color(128, 0, 128); // Purple
    return sf::Color::White;
}

sf::Color Scene_Battle_Demo::getEnemyColor(const std::string& name) {
    if (name == "Goblin") return sf::Color::Green;
    if (name == "Slime") return sf::Color(0, 255, 127); // Spring green
    if (name == "Giant_Rat") return sf::Color(139, 69, 19); // Brown
    if (name == "Cave_Spider") return sf::Color(64, 64, 64); // Dark gray
    if (name == "Goblin_Chief") return sf::Color(0, 128, 0); // Dark green
    if (name == "Forest_Wolf") return sf::Color(105, 105, 105); // Dim gray
    if (name == "Brown_Bear") return sf::Color(160, 82, 45); // Saddle brown
    if (name == "Bandit") return sf::Color(220, 20, 60); // Crimson
    if (name == "Young_Treant") return sf::Color(34, 139, 34); // Forest green
    if (name == "Orc_Warrior") return sf::Color(128, 128, 0); // Olive
    if (name == "Bandit_Leader") return sf::Color(178, 34, 34); // Fire brick
    return sf::Color::Red;
}

void Scene_Battle_Demo::logBattleStats() {
    std::cout << "\n--- Battle Statistics ---" << std::endl;
    std::cout << "Party Members:" << std::endl;
    for (const auto& member : m_playerParty) {
        std::cout << "  " << member.name << " - HP:" << member.currentHP 
                  << " ATK:" << member.attack << " DEF:" << member.defense 
                  << " SPD:" << member.speed << " MP:" << member.mp << std::endl;
    }
    
    std::cout << "Enemies:" << std::endl;
    for (const auto& enemy : m_enemies) {
        std::cout << "  " << enemy.name << " - HP:" << enemy.currentHP 
                  << " ATK:" << enemy.attack << " DEF:" << enemy.defense 
                  << " SPD:" << enemy.speed << " MP:" << enemy.mp << std::endl;
    }
    std::cout << "------------------------\n" << std::endl;
}

void Scene_Battle_Demo::displayLoadedConfigurations() {
    std::cout << "\n=== Loaded Configurations ===" << std::endl;
    
    // Display available party members
    std::cout << "Available Party Members:" << std::endl;
    std::vector<std::string> members = {"hero", "mage", "warrior", "rogue"};
    for (const auto& memberId : members) {
        const PartyMemberData* data = m_configLoader->getPartyMember(memberId);
        if (data) {
            std::cout << "  " << data->name << " (" << memberId << ") - " 
                      << data->description << std::endl;
            std::cout << "    Stats: HP:" << data->hp << " ATK:" << data->attack 
                      << " DEF:" << data->defense << " SPD:" << data->speed 
                      << " MP:" << data->mp << std::endl;
            std::cout << "    Spells: ";
            for (const auto& spell : data->knownSpells) {
                std::cout << spell << " ";
            }
            std::cout << std::endl;
        }
    }
    
    std::cout << "\nPress any key to continue..." << std::endl;
}

void Scene_Battle_Demo::returnToMenu() {
    // Reset to menu state
    m_demoState = DemoState::MENU;
    m_battleState = BattleState::ENTERING;
    
    // Clear battle data
    m_playerParty.clear();
    m_enemies.clear();
    
    std::cout << "Returned to battle demo menu" << std::endl;
}

void Scene_Battle_Demo::testSpellSystem() {
    std::cout << "Testing spell creation..." << std::endl;
    
    // Test some spells
    std::vector<std::string> testSpells = {"fireball", "heal_minor", "sword_strike", "poison_bite"};
    
    for (const auto& spellId : testSpells) {
        auto spell = m_configLoader->createSpell(spellId);
        std::cout << "  " << spell.name << " - MP:" << spell.mpCost 
                  << " DMG:" << spell.baseDamage << " - " << spell.description << std::endl;
    }
}

void Scene_Battle_Demo::testPartyConfiguration() {
    std::cout << "Testing party member creation at different levels..." << std::endl;
    
    std::vector<std::string> members = {"hero", "mage", "warrior"};
    std::vector<int> levels = {1, 3, 5};
    
    for (const auto& memberId : members) {
        std::cout << "\n" << memberId << " progression:" << std::endl;
        for (int level : levels) {
            auto character = m_configLoader->createPartyMember(memberId, level);
            std::cout << "  Level " << level << ": HP:" << character.maxHP 
                      << " ATK:" << character.attack << " DEF:" << character.defense 
                      << " SPD:" << character.speed << " MP:" << character.maxMP << std::endl;
        }
    }
}

void Scene_Battle_Demo::onEnd() {
    std::cout << "Battle demo ended." << std::endl;
}

// Demo battle actions
void Scene_Battle_Demo::performDemoAttack() {
    if (m_playerParty.empty() || m_enemies.empty()) return;
    
    // Hero attacks first enemy
    auto& attacker = m_playerParty[0];
    auto& target = m_enemies[0];
    
    int damage = attacker.attack - target.defense;
    if (damage < 1) damage = 1;
    
    target.currentHP -= damage;
    if (target.currentHP < 0) target.currentHP = 0;
    
    std::cout << attacker.name << " attacks " << target.name << " for " << damage << " damage!" << std::endl;
    std::cout << target.name << " HP: " << target.currentHP << "/" << target.maxHP << std::endl;
}

void Scene_Battle_Demo::performDemoHeal() {
    if (m_playerParty.empty()) return;
    
    // Find injured party member
    for (auto& member : m_playerParty) {
        if (member.currentHP < member.maxHP) {
            int healAmount = 25;
            member.currentHP += healAmount;
            if (member.currentHP > member.maxHP) member.currentHP = member.maxHP;
            
            std::cout << member.name << " heals for " << healAmount << " HP!" << std::endl;
            std::cout << member.name << " HP: " << member.currentHP << "/" << member.maxHP << std::endl;
            return;
        }
    }
    
    std::cout << "All party members are at full health!" << std::endl;
}

void Scene_Battle_Demo::performDemoSpell() {
    if (m_playerParty.size() < 2 || m_enemies.empty()) return;
    
    // Mage casts fireball
    auto& caster = m_playerParty[1]; // Mage
    auto& target = m_enemies[0];
    
    if (caster.mp >= 10) {
        caster.mp -= 10;
        int damage = 25;
        target.currentHP -= damage;
        if (target.currentHP < 0) target.currentHP = 0;
        
        std::cout << caster.name << " casts Fireball on " << target.name << " for " << damage << " damage!" << std::endl;
        std::cout << caster.name << " MP: " << caster.mp << "/" << caster.maxMP << std::endl;
        std::cout << target.name << " HP: " << target.currentHP << "/" << target.maxHP << std::endl;
    } else {
        std::cout << caster.name << " doesn't have enough MP for Fireball!" << std::endl;
    }
}

void Scene_Battle_Demo::performDamageTest() {
    // Damage a random party member for demo purposes
    if (m_playerParty.empty()) return;
    
    auto& target = m_playerParty[rand() % m_playerParty.size()];
    int damage = 10 + (rand() % 15);
    target.currentHP -= damage;
    if (target.currentHP < 0) target.currentHP = 0;
    
    std::cout << target.name << " takes " << damage << " damage from enemy attack!" << std::endl;
    std::cout << target.name << " HP: " << target.currentHP << "/" << target.maxHP << std::endl;
}
