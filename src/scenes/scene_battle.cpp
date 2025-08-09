#include "scene_battle.hpp"
#include "../game_engine.hpp"
#include "../battle_config_loader.hpp"
#include "scene_play.hpp"
#include <iostream>

Scene_Battle::Scene_Battle(GameEngine* game) 
    : Scene(game), m_configLoader(std::make_unique<BattleConfigLoader>())
{
    init();
}

Scene_Battle::Scene_Battle(GameEngine* game, const std::vector<std::string>& enemyTypes)
    : Scene(game), m_configLoader(std::make_unique<BattleConfigLoader>())
{
    init();
    // Load configurations and create battle from enemy types
    if (loadBattleConfigurations()) {
        // Create default party
        loadPartyFromConfig({"hero", "mage", "warrior"}, 1);
        
        // Create enemies from types (assume level 1 for now)
        if (!enemyTypes.empty()) {
            loadEnemiesFromConfig(1, enemyTypes);
        } else {
            loadRandomEncounter(1);
        }
        
        // Build turn queue after loading characters
        buildTurnQueue();
        
        startBattle();
    }
}

Scene_Battle::Scene_Battle(GameEngine* game, const std::vector<std::string>& enemyTypes,
                          const std::string& returnLevel, const Vec2& returnPos, 
                          int returnHealth, float returnPlayTime)
    : Scene(game), m_configLoader(std::make_unique<BattleConfigLoader>())
{
    // Store state preservation data
    m_returnLevel = returnLevel;
    m_returnPosition = returnPos;
    m_returnHealth = returnHealth;
    m_returnPlayTime = returnPlayTime;
    m_preserveState = true;
    
    init();
    
    // Load configurations and create battle
    if (loadBattleConfigurations()) {
        loadPartyFromConfig({"hero", "mage", "warrior"}, 1);
        
        if (!enemyTypes.empty()) {
            loadEnemiesFromConfig(1, enemyTypes);
        } else {
            loadRandomEncounter(1);
        }
        
        // Build turn queue after loading characters
        buildTurnQueue();
        
        startBattle();
    }
}

Scene_Battle::~Scene_Battle() {
    // Destructor implementation needed for unique_ptr with forward declaration
}

void Scene_Battle::init() {
    // Register battle controls
    registerAction(sf::Keyboard::B, "BACK");
    registerAction(sf::Keyboard::Escape, "BACK");
    
    // Menu navigation controls
    registerAction(sf::Keyboard::Space, "SELECT");
    registerAction(sf::Keyboard::C, "CANCEL");
    
    // WASD navigation
    registerAction(sf::Keyboard::A, "LEFT");
    registerAction(sf::Keyboard::D, "RIGHT");
    registerAction(sf::Keyboard::W, "UP");
    registerAction(sf::Keyboard::S, "DOWN");
    
    // Initialize battle state
    m_battleState = BattleState::ENTERING;
    m_selectedPlayerIndex = 0;
    m_selectedActionIndex = 0;
    m_selectedTargetIndex = 0;
    m_selectingTarget = false;
    m_selectingSpell = false;
    m_selectedSpellIndex = 0;
    
    // Initialize turn system
    m_currentTurnIndex = 0;
    m_waitingForPlayerAction = false;
    m_turnTimer = 0.0f;
    
    // Initialize cursor system
    initializeCursor();
    
    // Initialize battle menu system
    initializeBattleMenu();
    
    // Build initial turn queue if we have characters
    if (!m_playerParty.empty() || !m_enemies.empty()) {
        buildTurnQueue();
    }
    
    std::cout << "Battle scene initialized - Menu-driven battle system ready!" << std::endl;
}

void Scene_Battle::update() {
    // Update turn timer
    static sf::Clock deltaClock;
    float deltaTime = deltaClock.restart().asSeconds();
    updateTurnTimer(deltaTime);
    
    // Process current turn if not waiting for player action
    if (!m_waitingForPlayerAction && m_turnTimer <= 0.0f) {
        if (isCurrentTurnPlayer()) {
            // Player turn - wait for input
            m_waitingForPlayerAction = true;
        } else {
            // Enemy turn - execute AI
            TurnEntry currentTurn = getCurrentTurn();
            enemyAI(currentTurn.characterIndex);
            advanceTurn();
        }
    }
    
    // Call sRender like other scenes do
    sRender();
}

void Scene_Battle::sDoAction(const Action& action) {
    if (action.getType() == "START") {
        if (action.getName() == "BACK") {
            std::cout << "Exiting battle scene" << std::endl;
            // Return to the previous scene that called this battle
            m_game->popScene();
        }
        // Handle input based on current state
        else if (m_waitingForPlayerAction && isCurrentTurnPlayer()) {
            // During player's turn - handle menu or target selection
            if (m_menuState == BattleMenuState::TARGET_SELECT) {
                // Target selection mode
                if (action.getName() == "LEFT" || action.getName() == "RIGHT") {
                    handleTargetNavigation("HORIZONTAL");
                } else if (action.getName() == "UP" || action.getName() == "DOWN") {
                    handleTargetNavigation("VERTICAL");
                } else if (action.getName() == "SELECT") {
                    executeActionOnTarget();
                } else if (action.getName() == "CANCEL") {
                    // Cancel target selection, return to previous menu
                    std::cout << "Cancelled target selection" << std::endl;
                    if (m_pendingAction.substr(0, 6) == "SPELL:") {
                        showSpellMenu();
                    } else if (m_pendingAction.substr(0, 5) == "ITEM:") {
                        showItemMenu();
                    } else {
                        showMainMenu();
                    }
                }
            } else {
                // Menu navigation mode
                if (action.getName() == "UP" || action.getName() == "DOWN") {
                    handleMenuNavigation(action.getName() == "UP" ? "UP" : "DOWN");
                } else if (action.getName() == "SELECT") {
                    handleMenuSelection();
                } else if (action.getName() == "CANCEL") {
                    // Cancel current menu, go back to previous level
                    if (m_menuState == BattleMenuState::SPELL_MENU || m_menuState == BattleMenuState::ITEM_MENU) {
                        std::cout << "Cancelled submenu" << std::endl;
                        showMainMenu();
                    } else if (m_menuState == BattleMenuState::MAIN_MENU) {
                        // Can't cancel main menu during your turn - show message
                        std::cout << "Must choose an action during your turn" << std::endl;
                    }
                }
                // Character navigation still works in menu mode
                else if (action.getName() == "LEFT") {
                    moveCursorLeft();
                } else if (action.getName() == "RIGHT") {
                    moveCursorRight();
                }
            }
        } else {
            // Outside of player turn - only character navigation
            if (action.getName() == "LEFT") {
                moveCursorLeft();
            } else if (action.getName() == "CURSOR_RIGHT") {
                moveCursorRight();
            } else if (action.getName() == "UP") {
                moveCursorUp();
            } else if (action.getName() == "DOWN") {
                moveCursorDown();
            } else if (action.getName() == "SELECT") {
                std::cout << "Not your turn! Wait for your character's turn." << std::endl;
            } else if (action.getName() == "CANCEL") {
                std::cout << "Not your turn! Wait for your character's turn." << std::endl;
            }
        }
    }
}

void Scene_Battle::sRender() {
    // Debug: Check if render is being called
    static int renderCount = 0;
    renderCount++;
    if (renderCount % 60 == 1) { // Print every 60 frames (once per second at 60fps)
        std::cout << "Scene_Battle::sRender() called (frame " << renderCount << ")" << std::endl;
    }
    
    // Get game view for proper positioning (exactly like scene_menu)
    sf::View gameView = m_game->getGameView();
    sf::Vector2f viewSize = gameView.getSize();
    sf::Vector2f viewCenter = gameView.getCenter();
    
    if (renderCount % 60 == 1) {
        std::cout << "View size: " << viewSize.x << "x" << viewSize.y << ", center: " << viewCenter.x << "," << viewCenter.y << std::endl;
    }
    
    // Draw background (exactly like scene_menu)
    sf::RectangleShape background;
    background.setSize(viewSize);
    background.setPosition(viewCenter.x - viewSize.x / 2, viewCenter.y - viewSize.y / 2);
    background.setFillColor(sf::Color(20, 30, 50)); // Dark blue for battle
    m_game->window().draw(background);
    
    // Render battle interface if we have party and enemies
    if (!m_playerParty.empty() || !m_enemies.empty()) {
        if (renderCount % 60 == 1) {
            std::cout << "Rendering battle field with " << m_playerParty.size() << " party, " << m_enemies.size() << " enemies" << std::endl;
        }
        renderBattleField();
    } else {
        if (renderCount % 60 == 1) {
            std::cout << "No party/enemies loaded, showing default text" << std::endl;
        }
        // Show simple battle text if no battle is loaded
        sf::Text battleText;
        try {
            battleText.setFont(m_game->getAssets().getFont("ShareTech"));
            battleText.setString("BATTLE SCENE\n\nPress A=Attack, H=Heal, S=Spell, D=Defend\nPress B or ESC to return");
            battleText.setCharacterSize(20);
            battleText.setFillColor(sf::Color::White);
            battleText.setPosition(viewCenter.x - 200, viewCenter.y - 50);
            m_game->window().draw(battleText);
        } catch (const std::exception& e) {
            std::cout << "Could not render text: " << e.what() << std::endl;
        }
    }
    
    // Render command overlay (always on top)
    renderCommandOverlay();
}

void Scene_Battle::onEnd() {
    std::cout << "Battle scene ended" << std::endl;
}

// Configuration loading methods
bool Scene_Battle::loadBattleConfigurations() {
    if (m_configLoaded) {
        return true; // Already loaded
    }
    
    std::cout << "Loading battle configurations..." << std::endl;
    
    // Load spells database first
    if (!m_configLoader->loadSpells()) {
        std::cout << "Warning: Could not load spell database" << std::endl;
        return false;
    }
    
    m_configLoaded = true;
    std::cout << "Battle configurations loaded successfully" << std::endl;
    return true;
}

void Scene_Battle::loadPartyFromConfig(const std::vector<std::string>& memberIds, int level) {
    m_playerParty.clear();
    
    for (const auto& memberId : memberIds) {
        if (m_configLoader->loadPartyMember(memberId)) {
            auto character = m_configLoader->createPartyMember(memberId, level);
            m_playerParty.push_back(character);
            std::cout << "Added party member: " << character.name << " (Level " << level << ")" << std::endl;
        } else {
            std::cout << "Warning: Could not load party member: " << memberId << std::endl;
        }
    }
}

void Scene_Battle::loadEnemiesFromConfig(int level, const std::vector<std::string>& enemyIds) {
    m_enemies.clear();
    
    // Load enemies for this level
    if (!m_configLoader->loadEnemiesForLevel(level)) {
        std::cout << "Warning: Could not load enemies for level " << level << std::endl;
        return;
    }
    
    if (enemyIds.empty()) {
        // Load a random encounter
        loadRandomEncounter(level);
        return;
    }
    
    // Load specific enemies
    for (const auto& enemyId : enemyIds) {
        auto enemy = m_configLoader->createEnemy(enemyId);
        if (enemy.name != "Unknown Enemy") {
            m_enemies.push_back(enemy);
            std::cout << "Added enemy: " << enemy.name << std::endl;
        } else {
            std::cout << "Warning: Could not create enemy: " << enemyId << std::endl;
        }
    }
}

void Scene_Battle::loadRandomEncounter(int level) {
    // Load enemies for this level first
    if (!m_configLoader->loadEnemiesForLevel(level)) {
        std::cout << "Warning: Could not load enemies for level " << level << std::endl;
        return;
    }
    
    // Create a simple random encounter based on level
    if (level == 1) {
        // Level 1: Simple encounter
        std::vector<std::string> encounter = {"GOBLIN", "SLIME"};
        for (const auto& enemyId : encounter) {
            auto enemy = m_configLoader->createEnemy(enemyId);
            if (enemy.name != "Unknown Enemy") {
                m_enemies.push_back(enemy);
            }
        }
        std::cout << "Created Level 1 random encounter: Goblin + Slime" << std::endl;
    } else if (level == 2) {
        // Level 2: Forest encounter
        std::vector<std::string> encounter = {"WOLF", "BANDIT"};
        for (const auto& enemyId : encounter) {
            auto enemy = m_configLoader->createEnemy(enemyId);
            if (enemy.name != "Unknown Enemy") {
                m_enemies.push_back(enemy);
            }
        }
        std::cout << "Created Level 2 random encounter: Wolf + Bandit" << std::endl;
    } else {
        // Fallback to level 1
        loadRandomEncounter(1);
    }
}

// Visual rendering methods
void Scene_Battle::renderBattleField() {
    // Get game view for proper positioning (exactly like scene_menu)
    sf::View gameView = m_game->getGameView();
    sf::Vector2f viewSize = gameView.getSize();
    sf::Vector2f viewCenter = gameView.getCenter();
    
    float viewLeft = viewCenter.x - viewSize.x / 2;
    float viewTop = viewCenter.y - viewSize.y / 2;
    
    // Player area (top-left corner of screen)
    sf::RectangleShape playerArea;
    playerArea.setSize(sf::Vector2f(viewSize.x * 0.45f, viewSize.y * 0.6f)); // Slightly smaller width
    playerArea.setPosition(viewLeft + 10, viewTop + 10); // Top-left with small margin
    playerArea.setFillColor(sf::Color(0, 100, 0, 100)); // Semi-transparent green
    playerArea.setOutlineColor(sf::Color::Green);
    playerArea.setOutlineThickness(2);
    m_game->window().draw(playerArea);
    
    // Enemy area (top-right corner of screen)
    sf::RectangleShape enemyArea;
    enemyArea.setSize(sf::Vector2f(viewSize.x * 0.45f, viewSize.y * 0.6f)); // Slightly smaller width
    enemyArea.setPosition(viewLeft + viewSize.x * 0.52f, viewTop + 10); // Top-right with margin
    enemyArea.setFillColor(sf::Color(100, 0, 0, 100)); // Semi-transparent red
    enemyArea.setOutlineColor(sf::Color::Red);
    enemyArea.setOutlineThickness(2);
    m_game->window().draw(enemyArea);
    
    // Draw party members
    renderPartyMembers(playerArea);
    
    // Draw enemies
    renderEnemies(enemyArea);
    
    // Render battle menu or target selection if it's player's turn (positioned at bottom)
    if (m_waitingForPlayerAction && isCurrentTurnPlayer()) {
        if (m_menuState == BattleMenuState::TARGET_SELECT) {
            renderTargetSelection();
        } else {
            renderBattleMenu();
        }
    }
}

void Scene_Battle::renderPartyMembers(const sf::RectangleShape& area) {
    sf::Vector2f areaPos = area.getPosition();
    sf::Vector2f areaSize = area.getSize();
    
    for (size_t i = 0; i < m_playerParty.size(); ++i) {
        const auto& member = m_playerParty[i];
        
        // Character representation (larger colored circle - increased by 20px)
        sf::CircleShape character(40); // Was 20, now 40 (20px increase in radius)
        character.setFillColor(getCharacterColor(member.name));
        
        // Position characters vertically (adjust for larger size)
        float x = areaPos.x + areaSize.x * 0.3f;
        float y = areaPos.y + (i + 1) * (areaSize.y / (m_playerParty.size() + 1)) - 40; // Adjust for larger radius
        character.setPosition(x, y);
        
        // Determine outline color and thickness with enhanced visuals
        sf::Color outlineColor = sf::Color::White;
        float outlineThickness = 2;
        
        // Target selection highlighting (highest priority)
        if (m_menuState == BattleMenuState::TARGET_SELECT && !m_targetingEnemies && 
            m_cursorPlayerIndex == static_cast<int>(i)) {
            outlineColor = sf::Color::Magenta;
            outlineThickness = 8; // Very thick for target selection
        }
        // Current turn character gets special highlight (high priority)
        else if (isCurrentTurnPlayer() && getCurrentTurn().characterIndex == static_cast<int>(i)) {
            outlineColor = sf::Color::Yellow;
            outlineThickness = 6; // Increased thickness for better visibility
        }
        // Cursor selection (medium priority)
        else if (m_cursorOnPlayerSide && m_cursorPlayerIndex == static_cast<int>(i)) {
            outlineColor = sf::Color::Cyan;
            outlineThickness = 4; // Increased thickness
        }
        
        character.setOutlineColor(outlineColor);
        character.setOutlineThickness(outlineThickness);
        
        m_game->window().draw(character);
        
        // Character name and HP (adjust positions for larger portraits)
        try {
            sf::Text nameText;
            nameText.setFont(m_game->getAssets().getFont("ShareTech"));
            nameText.setString(member.name);
            nameText.setCharacterSize(14);
            nameText.setFillColor(sf::Color::White);
            nameText.setPosition(x + 90, y + 10); // Adjusted for larger portrait
            m_game->window().draw(nameText);
            
            sf::Text hpText;
            hpText.setFont(m_game->getAssets().getFont("ShareTech"));
            hpText.setString("HP: " + std::to_string(member.currentHP) + "/" + std::to_string(member.maxHP));
            hpText.setCharacterSize(12);
            hpText.setFillColor(sf::Color::Cyan);
            hpText.setPosition(x + 90, y + 30); // Adjusted for larger portrait
            m_game->window().draw(hpText);
            
            // HP Bar (adjust position)
            sf::RectangleShape hpBar;
            hpBar.setSize(sf::Vector2f(80, 6));
            hpBar.setPosition(x + 90, y + 50); // Adjusted for larger portrait
            hpBar.setFillColor(sf::Color::Red);
            m_game->window().draw(hpBar);
            
            sf::RectangleShape hpFill;
            float hpRatio = (float)member.currentHP / member.maxHP;
            hpFill.setSize(sf::Vector2f(80 * hpRatio, 6));
            hpFill.setPosition(x + 90, y + 50); // Adjusted for larger portrait
            hpFill.setFillColor(sf::Color::Green);
            m_game->window().draw(hpFill);
            
        } catch (const std::exception& e) {
            // Ignore font errors
        }
    }
}

void Scene_Battle::renderEnemies(const sf::RectangleShape& area) {
    sf::Vector2f areaPos = area.getPosition();
    sf::Vector2f areaSize = area.getSize();
    
    for (size_t i = 0; i < m_enemies.size(); ++i) {
        const auto& enemy = m_enemies[i];
        
        // Enemy representation (larger colored square - increased by 20px)
        sf::RectangleShape enemyShape(sf::Vector2f(70, 70)); // Was 30x30, now 70x70 (40px increase total)
        enemyShape.setFillColor(getEnemyColor(enemy.name));
        
        // Position enemies (adjust for larger size)
        float x = areaPos.x + areaSize.x * 0.2f;
        float y = areaPos.y + (i + 1) * (areaSize.y / (m_enemies.size() + 1)) - 35; // Adjust for larger size
        enemyShape.setPosition(x, y);
        
        // Determine outline color and thickness with enhanced visuals
        sf::Color outlineColor = sf::Color::White;
        float outlineThickness = 2;
        
        // Target selection highlighting (highest priority)
        if (m_menuState == BattleMenuState::TARGET_SELECT && m_targetingEnemies && 
            m_cursorEnemyIndex == static_cast<int>(i)) {
            outlineColor = sf::Color::Magenta;
            outlineThickness = 8; // Very thick for target selection
        }
        // Current turn enemy gets special highlight (high priority)
        else if (!isCurrentTurnPlayer() && getCurrentTurn().characterIndex == static_cast<int>(i)) {
            outlineColor = sf::Color::Red;
            outlineThickness = 6; // Increased thickness for better visibility
        }
        // Cursor selection (medium priority)
        else if (!m_cursorOnPlayerSide && m_cursorEnemyIndex == static_cast<int>(i)) {
            outlineColor = sf::Color::Cyan;
            outlineThickness = 4; // Increased thickness
        }
        
        enemyShape.setOutlineColor(outlineColor);
        enemyShape.setOutlineThickness(outlineThickness);
        
        m_game->window().draw(enemyShape);
        
        // Enemy name and HP (adjust positions for larger portraits)
        try {
            sf::Text nameText;
            nameText.setFont(m_game->getAssets().getFont("ShareTech"));
            nameText.setString(enemy.name);
            nameText.setCharacterSize(14);
            nameText.setFillColor(sf::Color::White);
            nameText.setPosition(x + 80, y + 10); // Adjusted for larger portrait
            m_game->window().draw(nameText);
            
            sf::Text hpText;
            hpText.setFont(m_game->getAssets().getFont("ShareTech"));
            hpText.setString("HP: " + std::to_string(enemy.currentHP) + "/" + std::to_string(enemy.maxHP));
            hpText.setCharacterSize(12);
            hpText.setFillColor(sf::Color::Yellow);
            hpText.setPosition(x + 80, y + 30); // Adjusted for larger portrait
            m_game->window().draw(hpText);
            
            // HP Bar (adjust position)
            sf::RectangleShape hpBar;
            hpBar.setSize(sf::Vector2f(60, 6));
            hpBar.setPosition(x + 80, y + 50); // Adjusted for larger portrait
            hpBar.setFillColor(sf::Color::Red);
            m_game->window().draw(hpBar);
            
            sf::RectangleShape hpFill;
            float hpRatio = (float)enemy.currentHP / enemy.maxHP;
            hpFill.setSize(sf::Vector2f(60 * hpRatio, 6));
            hpFill.setPosition(x + 80, y + 50); // Adjusted for larger portrait
            hpFill.setFillColor(sf::Color(255, 165, 0)); // Orange color
            m_game->window().draw(hpFill);
            
        } catch (const std::exception& e) {
            // Ignore font errors
        }
    }
}

sf::Color Scene_Battle::getCharacterColor(const std::string& name) {
    if (name == "Hero") return sf::Color::Blue;
    if (name == "Lyra") return sf::Color::Magenta;
    if (name == "Gareth") return sf::Color(139, 69, 19); // Brown
    if (name == "Kira") return sf::Color(128, 0, 128); // Purple
    return sf::Color::White;
}

sf::Color Scene_Battle::getEnemyColor(const std::string& name) {
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

// Interactive battle actions
void Scene_Battle::performDemoAttack() {
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

void Scene_Battle::performDemoHeal() {
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

void Scene_Battle::performDemoSpell() {
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

void Scene_Battle::performDemoDefend() {
    if (m_playerParty.empty()) return;
    
    auto& defender = m_playerParty[0]; // Hero
    defender.isDefending = true;
    
    std::cout << defender.name << " takes a defensive stance! Defense increased." << std::endl;
}

// Turn queue system implementation
void Scene_Battle::buildTurnQueue() {
    m_turnQueue.clear();
    
    // Add all party members to turn queue
    for (size_t i = 0; i < m_playerParty.size(); ++i) {
        const auto& member = m_playerParty[i];
        if (member.currentHP > 0) { // Only alive characters
            TurnEntry entry;
            entry.isPlayer = true;
            entry.characterIndex = static_cast<int>(i);
            entry.speed = member.speed;
            entry.name = member.name;
            m_turnQueue.push_back(entry);
        }
    }
    
    // Add all enemies to turn queue
    for (size_t i = 0; i < m_enemies.size(); ++i) {
        const auto& enemy = m_enemies[i];
        if (enemy.currentHP > 0) { // Only alive enemies
            TurnEntry entry;
            entry.isPlayer = false;
            entry.characterIndex = static_cast<int>(i);
            entry.speed = enemy.speed;
            entry.name = enemy.name;
            m_turnQueue.push_back(entry);
        }
    }
    
    // Sort by speed (highest first)
    std::sort(m_turnQueue.begin(), m_turnQueue.end(), 
        [](const TurnEntry& a, const TurnEntry& b) {
            return a.speed > b.speed;
        });
    
    m_currentTurnIndex = 0;
    std::cout << "Turn queue built with " << m_turnQueue.size() << " participants" << std::endl;
    
    // Print turn order
    std::cout << "Turn order: ";
    for (const auto& entry : m_turnQueue) {
        std::cout << entry.name << "(" << entry.speed << ") ";
    }
    std::cout << std::endl;
}

void Scene_Battle::resetTurnQueue() {
    buildTurnQueue();
}

Scene_Battle::TurnEntry Scene_Battle::getCurrentTurn() {
    if (m_turnQueue.empty()) {
        // Return dummy entry if queue is empty
        TurnEntry dummy;
        dummy.isPlayer = true;
        dummy.characterIndex = 0;
        dummy.speed = 0;
        dummy.name = "None";
        return dummy;
    }
    
    return m_turnQueue[m_currentTurnIndex];
}

bool Scene_Battle::isCurrentTurnPlayer() {
    if (m_turnQueue.empty()) return true;
    return getCurrentTurn().isPlayer;
}

void Scene_Battle::advanceTurn() {
    if (m_turnQueue.empty()) return;
    
    m_currentTurnIndex = (m_currentTurnIndex + 1) % m_turnQueue.size();
    m_turnTimer = TURN_DELAY;
    
    // Check if we need to rebuild queue (remove dead characters)
    bool needsRebuild = false;
    for (const auto& entry : m_turnQueue) {
        if (entry.isPlayer) {
            if (entry.characterIndex >= static_cast<int>(m_playerParty.size()) || 
                m_playerParty[entry.characterIndex].currentHP <= 0) {
                needsRebuild = true;
                break;
            }
        } else {
            if (entry.characterIndex >= static_cast<int>(m_enemies.size()) || 
                m_enemies[entry.characterIndex].currentHP <= 0) {
                needsRebuild = true;
                break;
            }
        }
    }
    
    if (needsRebuild) {
        std::cout << "Rebuilding turn queue (dead characters removed)" << std::endl;
        buildTurnQueue();
    }
    
    TurnEntry nextTurn = getCurrentTurn();
    std::cout << "Next turn: " << nextTurn.name << std::endl;
}

void Scene_Battle::enemyAI(int enemyIndex) {
    if (enemyIndex >= static_cast<int>(m_enemies.size()) || m_enemies[enemyIndex].currentHP <= 0) {
        return;
    }
    
    auto& enemy = m_enemies[enemyIndex];
    
    // Simple AI: Attack a random alive player
    std::vector<int> alivePlayerIndices;
    for (size_t i = 0; i < m_playerParty.size(); ++i) {
        if (m_playerParty[i].currentHP > 0) {
            alivePlayerIndices.push_back(static_cast<int>(i));
        }
    }
    
    if (alivePlayerIndices.empty()) {
        std::cout << "No alive players to attack!" << std::endl;
        return;
    }
    
    // Pick random target
    int targetIndex = alivePlayerIndices[rand() % alivePlayerIndices.size()];
    auto& target = m_playerParty[targetIndex];
    
    // Calculate damage
    int damage = enemy.attack - target.defense;
    if (damage < 1) damage = 1;
    
    target.currentHP -= damage;
    if (target.currentHP < 0) target.currentHP = 0;
    
    std::cout << enemy.name << " attacks " << target.name << " for " << damage << " damage!" << std::endl;
    std::cout << target.name << " HP: " << target.currentHP << "/" << target.maxHP << std::endl;
    
    if (target.currentHP <= 0) {
        std::cout << target.name << " has been defeated!" << std::endl;
    }
}

void Scene_Battle::updateTurnTimer(float deltaTime) {
    if (m_turnTimer > 0.0f) {
        m_turnTimer -= deltaTime;
    }
}

// Cursor navigation system implementation
void Scene_Battle::initializeCursor() {
    m_cursorOnPlayerSide = true;  // Start on player side
    m_cursorPlayerIndex = 0;      // First player
    m_cursorEnemyIndex = 0;       // First enemy
    m_selectingAction = true;     // Start in action selection mode
    
    std::cout << "Cursor initialized - Use arrow keys to navigate" << std::endl;
}

void Scene_Battle::moveCursorLeft() {
    if (!m_cursorOnPlayerSide) {
        // Move from enemy side to player side
        m_cursorOnPlayerSide = true;
        // Only show feedback if we have valid selection
        if (m_cursorPlayerIndex < static_cast<int>(m_playerParty.size())) {
            std::cout << "Selected: " << m_playerParty[m_cursorPlayerIndex].name << std::endl;
        }
    }
}

void Scene_Battle::moveCursorRight() {
    if (m_cursorOnPlayerSide) {
        // Move from player side to enemy side
        m_cursorOnPlayerSide = false;
        // Only show feedback if we have valid selection
        if (m_cursorEnemyIndex < static_cast<int>(m_enemies.size())) {
            std::cout << "Selected: " << m_enemies[m_cursorEnemyIndex].name << std::endl;
        }
    }
}

void Scene_Battle::moveCursorUp() {
    if (m_cursorOnPlayerSide) {
        // Move up in player list
        if (m_cursorPlayerIndex > 0) {
            m_cursorPlayerIndex--;
            std::cout << "Selected: " << m_playerParty[m_cursorPlayerIndex].name << std::endl;
        }
    } else {
        // Move up in enemy list
        if (m_cursorEnemyIndex > 0) {
            m_cursorEnemyIndex--;
            std::cout << "Selected: " << m_enemies[m_cursorEnemyIndex].name << std::endl;
        }
    }
}

void Scene_Battle::moveCursorDown() {
    if (m_cursorOnPlayerSide) {
        // Move down in player list
        if (m_cursorPlayerIndex < static_cast<int>(m_playerParty.size()) - 1) {
            m_cursorPlayerIndex++;
            std::cout << "Selected: " << m_playerParty[m_cursorPlayerIndex].name << std::endl;
        }
    } else {
        // Move down in enemy list
        if (m_cursorEnemyIndex < static_cast<int>(m_enemies.size()) - 1) {
            m_cursorEnemyIndex++;
            std::cout << "Selected: " << m_enemies[m_cursorEnemyIndex].name << std::endl;
        }
    }
}

int Scene_Battle::getCurrentlySelectedPlayerIndex() {
    return m_cursorPlayerIndex;
}

int Scene_Battle::getCurrentlySelectedEnemyIndex() {
    return m_cursorEnemyIndex;
}

void Scene_Battle::highlightCurrentTurnCharacter() {
    // This is handled in the render methods by checking getCurrentTurn()
    // and applying special highlighting to the current turn character
}

// Battle menu system implementation
void Scene_Battle::initializeBattleMenu() {
    m_menuState = BattleMenuState::MAIN_MENU;
    m_selectedMenuIndex = 0;
    m_selectedAction = "";
    m_pendingAction = "";
    m_targetingEnemies = true; // Default to targeting enemies
    showMainMenu();
    
    std::cout << "Battle menu system initialized" << std::endl;
}

void Scene_Battle::showMainMenu() {
    m_menuState = BattleMenuState::MAIN_MENU;
    m_selectedMenuIndex = 0;
    m_currentMenuOptions.clear();
    m_currentMenuOptions.push_back("Attack");
    m_currentMenuOptions.push_back("Spells");
    m_currentMenuOptions.push_back("Items");
}

void Scene_Battle::showSpellMenu() {
    m_menuState = BattleMenuState::SPELL_MENU;
    m_selectedMenuIndex = 0;
    m_currentMenuOptions.clear();
    
    // Add available spells (for now, hardcoded examples)
    m_currentMenuOptions.push_back("Fireball");
    m_currentMenuOptions.push_back("Heal");
    m_currentMenuOptions.push_back("Lightning");
    m_currentMenuOptions.push_back("Back");
}

void Scene_Battle::showItemMenu() {
    m_menuState = BattleMenuState::ITEM_MENU;
    m_selectedMenuIndex = 0;
    m_currentMenuOptions.clear();
    
    // Add available items (for now, hardcoded examples)
    m_currentMenuOptions.push_back("Health Potion");
    m_currentMenuOptions.push_back("Mana Potion");
    m_currentMenuOptions.push_back("Antidote");
    m_currentMenuOptions.push_back("Back");
}

void Scene_Battle::handleMenuNavigation(const std::string& direction) {
    if (direction == "UP") {
        if (m_selectedMenuIndex > 0) {
            m_selectedMenuIndex--;
        }
    } else if (direction == "DOWN") {
        if (m_selectedMenuIndex < static_cast<int>(m_currentMenuOptions.size()) - 1) {
            m_selectedMenuIndex++;
        }
    }
}

void Scene_Battle::handleMenuSelection() {
    if (m_currentMenuOptions.empty()) return;
    
    std::string selectedOption = m_currentMenuOptions[m_selectedMenuIndex];
    
    switch (m_menuState) {
        case BattleMenuState::MAIN_MENU:
            if (selectedOption == "Attack") {
                enterTargetSelection("ATTACK");
            } else if (selectedOption == "Spells") {
                showSpellMenu();
            } else if (selectedOption == "Items") {
                showItemMenu();
            }
            break;
            
        case BattleMenuState::SPELL_MENU:
            if (selectedOption == "Back") {
                showMainMenu();
            } else {
                enterTargetSelection("SPELL:" + selectedOption);
            }
            break;
            
        case BattleMenuState::ITEM_MENU:
            if (selectedOption == "Back") {
                showMainMenu();
            } else {
                enterTargetSelection("ITEM:" + selectedOption);
            }
            break;
            
        case BattleMenuState::TARGET_SELECT:
            // This is handled by executeActionOnTarget()
            break;
    }
}

void Scene_Battle::executeSelectedAction() {
    if (m_selectedAction == "ATTACK") {
        performDemoAttack();
        std::cout << "Executed: Attack" << std::endl;
    } else if (m_selectedAction.substr(0, 6) == "SPELL:") {
        std::string spellName = m_selectedAction.substr(6);
        if (spellName == "Fireball") {
            performDemoSpell();
        } else if (spellName == "Heal") {
            performDemoHeal();
        } else {
            std::cout << "Cast " << spellName << "!" << std::endl;
        }
        std::cout << "Executed: " << spellName << " spell" << std::endl;
    } else if (m_selectedAction.substr(0, 5) == "ITEM:") {
        std::string itemName = m_selectedAction.substr(5);
        if (itemName == "Health Potion") {
            performDemoHeal();
        } else {
            std::cout << "Used " << itemName << "!" << std::endl;
        }
        std::cout << "Executed: Used " << itemName << std::endl;
    }
    
    // Reset menu and end turn
    showMainMenu();
    m_waitingForPlayerAction = false;
    advanceTurn();
}

void Scene_Battle::renderBattleMenu() {
    if (m_currentMenuOptions.empty()) return;
    
    // Get game view for proper positioning
    sf::View gameView = m_game->getGameView();
    sf::Vector2f viewSize = gameView.getSize();
    sf::Vector2f viewCenter = gameView.getCenter();
    
    float viewLeft = viewCenter.x - viewSize.x / 2;
    float viewBottom = viewCenter.y + viewSize.y / 2;
    
    // Menu panel (positioned at bottom center, half width with more margin)
    float menuHeight = static_cast<float>(m_currentMenuOptions.size() * 25 + 20); // Reduced height without instructions
    float menuWidth = viewSize.x * 0.3f; // Half the previous width (was 0.6f, now 0.3f)
    sf::RectangleShape menuPanel;
    menuPanel.setSize(sf::Vector2f(menuWidth, menuHeight));
    menuPanel.setPosition(viewLeft + viewSize.x * 0.35f, viewBottom - menuHeight - 50); // 10px more margin (was 40, now 50)
    menuPanel.setFillColor(sf::Color(0, 0, 0, 200));
    menuPanel.setOutlineColor(sf::Color::Yellow);
    menuPanel.setOutlineThickness(3);
    m_game->window().draw(menuPanel);
    
    try {
        // Menu title (centered)
        sf::Text titleText;
        titleText.setFont(m_game->getAssets().getFont("ShareTech"));
        
        std::string title = "Main Menu";
        if (m_menuState == BattleMenuState::SPELL_MENU) title = "Spells";
        else if (m_menuState == BattleMenuState::ITEM_MENU) title = "Items";
        
        titleText.setString(title);
        titleText.setCharacterSize(16);
        titleText.setFillColor(sf::Color::Yellow);
        
        // Center the title text
        sf::FloatRect titleBounds = titleText.getLocalBounds();
        titleText.setPosition(menuPanel.getPosition().x + (menuWidth - titleBounds.width) / 2, 
                             menuPanel.getPosition().y + 5);
        m_game->window().draw(titleText);
        
        // Menu options (centered)
        for (size_t i = 0; i < m_currentMenuOptions.size(); ++i) {
            sf::Text optionText;
            optionText.setFont(m_game->getAssets().getFont("ShareTech"));
            
            std::string optionString = m_currentMenuOptions[i];
            if (static_cast<int>(i) == m_selectedMenuIndex) {
                optionString = "> " + optionString;
                optionText.setFillColor(sf::Color::Cyan);
            } else {
                optionText.setFillColor(sf::Color::White);
            }
            
            optionText.setString(optionString);
            optionText.setCharacterSize(14);
            
            // Center the option text
            sf::FloatRect optionBounds = optionText.getLocalBounds();
            optionText.setPosition(menuPanel.getPosition().x + (menuWidth - optionBounds.width) / 2,
                                 menuPanel.getPosition().y + 25 + i * 25);
            m_game->window().draw(optionText);
        }
        
    } catch (const std::exception& e) {
        // Ignore font errors
    }
}

// Target selection system implementation
void Scene_Battle::enterTargetSelection(const std::string& action) {
    m_menuState = BattleMenuState::TARGET_SELECT;
    m_pendingAction = action;
    
    // Determine if this action targets enemies or allies
    if (isOffensiveAction(action)) {
        m_targetingEnemies = true;
        // Set cursor to first alive enemy
        m_cursorOnPlayerSide = false;
        m_cursorEnemyIndex = 0;
        // Find first alive enemy
        for (size_t i = 0; i < m_enemies.size(); ++i) {
            if (m_enemies[i].currentHP > 0) {
                m_cursorEnemyIndex = static_cast<int>(i);
                break;
            }
        }
        std::cout << "Select target enemy for " << action << std::endl;
    } else {
        m_targetingEnemies = false;
        // Set cursor to first alive party member
        m_cursorOnPlayerSide = true;
        m_cursorPlayerIndex = 0;
        // Find first alive party member
        for (size_t i = 0; i < m_playerParty.size(); ++i) {
            if (m_playerParty[i].currentHP > 0) {
                m_cursorPlayerIndex = static_cast<int>(i);
                break;
            }
        }
        std::cout << "Select target party member for " << action << std::endl;
    }
}

void Scene_Battle::handleTargetNavigation(const std::string& direction) {
    if (direction == "HORIZONTAL") {
        // A/D keys - switch between party and enemies (if action allows)
        if (m_targetingEnemies) {
            // For offensive actions, can only target enemies
            std::cout << "This action can only target enemies" << std::endl;
        } else {
            // For defensive actions, can only target party members
            std::cout << "This action can only target party members" << std::endl;
        }
    } else if (direction == "VERTICAL") {
        // W/S keys - navigate within current group
        if (m_targetingEnemies) {
            // Navigate through enemies
            if (m_cursorEnemyIndex > 0) {
                m_cursorEnemyIndex--;
            } else {
                m_cursorEnemyIndex = static_cast<int>(m_enemies.size()) - 1;
            }
            // Skip dead enemies
            while (m_cursorEnemyIndex >= 0 && m_cursorEnemyIndex < static_cast<int>(m_enemies.size()) && 
                   m_enemies[m_cursorEnemyIndex].currentHP <= 0) {
                m_cursorEnemyIndex--;
                if (m_cursorEnemyIndex < 0) {
                    m_cursorEnemyIndex = static_cast<int>(m_enemies.size()) - 1;
                }
            }
            if (m_cursorEnemyIndex >= 0 && m_cursorEnemyIndex < static_cast<int>(m_enemies.size())) {
                std::cout << "Targeting: " << m_enemies[m_cursorEnemyIndex].name << std::endl;
            }
        } else {
            // Navigate through party members
            if (m_cursorPlayerIndex > 0) {
                m_cursorPlayerIndex--;
            } else {
                m_cursorPlayerIndex = static_cast<int>(m_playerParty.size()) - 1;
            }
            // Skip dead party members
            while (m_cursorPlayerIndex >= 0 && m_cursorPlayerIndex < static_cast<int>(m_playerParty.size()) && 
                   m_playerParty[m_cursorPlayerIndex].currentHP <= 0) {
                m_cursorPlayerIndex--;
                if (m_cursorPlayerIndex < 0) {
                    m_cursorPlayerIndex = static_cast<int>(m_playerParty.size()) - 1;
                }
            }
            if (m_cursorPlayerIndex >= 0 && m_cursorPlayerIndex < static_cast<int>(m_playerParty.size())) {
                std::cout << "Targeting: " << m_playerParty[m_cursorPlayerIndex].name << std::endl;
            }
        }
    }
}

void Scene_Battle::executeActionOnTarget() {
    // Execute the pending action on the selected target
    if (m_pendingAction == "ATTACK") {
        performDemoAttack();
        std::cout << "Attacked target!" << std::endl;
    } else if (m_pendingAction.substr(0, 6) == "SPELL:") {
        std::string spellName = m_pendingAction.substr(6);
        if (spellName == "Fireball" || spellName == "Lightning") {
            performDemoSpell();
        } else if (spellName == "Heal") {
            performDemoHeal();
        } else {
            std::cout << "Cast " << spellName << " on target!" << std::endl;
        }
    } else if (m_pendingAction.substr(0, 5) == "ITEM:") {
        std::string itemName = m_pendingAction.substr(5);
        if (itemName == "Health Potion") {
            performDemoHeal();
        } else {
            std::cout << "Used " << itemName << " on target!" << std::endl;
        }
    }
    
    // Reset to main menu and end turn
    showMainMenu();
    m_waitingForPlayerAction = false;
    advanceTurn();
}

bool Scene_Battle::isOffensiveAction(const std::string& action) {
    if (action == "ATTACK") return true;
    if (action.substr(0, 6) == "SPELL:") {
        std::string spellName = action.substr(6);
        return (spellName == "Fireball" || spellName == "Lightning");
    }
    return false; // Items are generally defensive/supportive
}

bool Scene_Battle::isDefensiveAction(const std::string& action) {
    return !isOffensiveAction(action);
}

void Scene_Battle::renderTargetSelection() {
    // Get game view for proper positioning
    sf::View gameView = m_game->getGameView();
    sf::Vector2f viewSize = gameView.getSize();
    sf::Vector2f viewCenter = gameView.getCenter();
    
    float viewLeft = viewCenter.x - viewSize.x / 2;
    float viewBottom = viewCenter.y + viewSize.y / 2;
    
    // Target selection panel (positioned at bottom center, half width with more margin)
    float panelWidth = viewSize.x * 0.3f; // Same width as battle menu
    sf::RectangleShape targetPanel;
    targetPanel.setSize(sf::Vector2f(panelWidth, 60)); // Reduced height without instructions
    targetPanel.setPosition(viewLeft + viewSize.x * 0.35f, viewBottom - 110); // Adjusted position
    targetPanel.setFillColor(sf::Color(0, 0, 100, 200)); // Blue tint for target selection
    targetPanel.setOutlineColor(sf::Color::Cyan);
    targetPanel.setOutlineThickness(3);
    m_game->window().draw(targetPanel);
    
    try {
        // Title (centered)
        sf::Text titleText;
        titleText.setFont(m_game->getAssets().getFont("ShareTech"));
        titleText.setString("Select Target");
        titleText.setCharacterSize(16);
        titleText.setFillColor(sf::Color::Cyan);
        
        // Center the title text
        sf::FloatRect titleBounds = titleText.getLocalBounds();
        titleText.setPosition(targetPanel.getPosition().x + (panelWidth - titleBounds.width) / 2,
                             targetPanel.getPosition().y + 5);
        m_game->window().draw(titleText);
        
        // Action info (centered)
        sf::Text actionText;
        actionText.setFont(m_game->getAssets().getFont("ShareTech"));
        actionText.setString("Action: " + m_pendingAction);
        actionText.setCharacterSize(12);
        actionText.setFillColor(sf::Color::White);
        
        // Center the action text
        sf::FloatRect actionBounds = actionText.getLocalBounds();
        actionText.setPosition(targetPanel.getPosition().x + (panelWidth - actionBounds.width) / 2,
                              targetPanel.getPosition().y + 25);
        m_game->window().draw(actionText);
        
        // Target info (centered)
        sf::Text targetText;
        targetText.setFont(m_game->getAssets().getFont("ShareTech"));
        std::string targetInfo = "Target: ";
        if (m_targetingEnemies && m_cursorEnemyIndex < static_cast<int>(m_enemies.size())) {
            targetInfo += m_enemies[m_cursorEnemyIndex].name;
        } else if (!m_targetingEnemies && m_cursorPlayerIndex < static_cast<int>(m_playerParty.size())) {
            targetInfo += m_playerParty[m_cursorPlayerIndex].name;
        }
        targetText.setString(targetInfo);
        targetText.setCharacterSize(12);
        targetText.setFillColor(sf::Color::Yellow);
        
        // Center the target text
        sf::FloatRect targetBounds = targetText.getLocalBounds();
        targetText.setPosition(targetPanel.getPosition().x + (panelWidth - targetBounds.width) / 2,
                              targetPanel.getPosition().y + 45);
        m_game->window().draw(targetText);
        m_game->window().draw(targetText);
    } catch (const std::exception& e) {
        // Ignore font errors
    }
}

// Empty stub implementations for all the complex battle system methods
void Scene_Battle::processPlayerTurn() {}
void Scene_Battle::processEnemyTurn() {}
void Scene_Battle::executeActions() {}
void Scene_Battle::checkBattleEnd() {}
void Scene_Battle::initializeSpells() {}
void Scene_Battle::queuePlayerAction(BattleAction) {}
void Scene_Battle::queuePlayerSpell(const std::string&) {}
void Scene_Battle::handleMenuNavigation(const Action&) {}
void Scene_Battle::handleSpellSelection(const Action&) {}
void Scene_Battle::handleTargetSelection(const Action&) {}
int Scene_Battle::calculateDamage(const BattleCharacter&, const BattleCharacter&) { return 0; }
int Scene_Battle::calculateSpellDamage(const BattleCharacter&, const std::string&) { return 0; }
bool Scene_Battle::isPlayerTeamAlive() { return true; }
bool Scene_Battle::isEnemyTeamAlive() { return true; }
std::vector<Scene_Battle::BattleCharacter*> Scene_Battle::getAliveCharacters(bool) { return {}; }
void Scene_Battle::setupBattlePositions() {}
void Scene_Battle::setupPlayerParty() {}
void Scene_Battle::addPlayerCharacter(const std::string&, int, int, int, int, int) {}
void Scene_Battle::addEnemy(const std::string&, int, int, int, int) {}
void Scene_Battle::startBattle() {}
void Scene_Battle::updateCharacterPositions() {}
void Scene_Battle::renderCharacters() {}
void Scene_Battle::renderStatusBars() {}
void Scene_Battle::renderUI() {}
void Scene_Battle::renderActionMenu() {}
void Scene_Battle::renderSpellMenu() {}
Scene_Battle::BattleCharacter* Scene_Battle::getAlivePlayerCharacter(int) { return nullptr; }
Scene_Battle::BattleCharacter* Scene_Battle::getAliveEnemyCharacter(int) { return nullptr; }
