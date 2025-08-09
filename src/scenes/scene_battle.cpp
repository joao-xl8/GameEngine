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
        
        startBattle();
    }
}

Scene_Battle::~Scene_Battle() {
    // Destructor implementation needed for unique_ptr with forward declaration
}

void Scene_Battle::init() {
    // Register only basic transition controls
    registerAction(sf::Keyboard::B, "BACK");
    registerAction(sf::Keyboard::Escape, "BACK");
    
    std::cout << "Battle scene initialized - Press B or ESC to return" << std::endl;
}

void Scene_Battle::update() {
    // Nothing to update
}

void Scene_Battle::sDoAction(const Action& action) {
    if (action.getType() == "START" && action.getName() == "BACK") {
        std::cout << "Exiting battle scene" << std::endl;
        // Return to the previous scene that called this battle
        m_game->popScene();
    }
}

void Scene_Battle::sRender() {
    // Set the game view for proper rendering
    m_game->window().setView(m_game->getGameView());
    
    // Get view dimensions
    sf::View gameView = m_game->getGameView();
    sf::Vector2f viewSize = gameView.getSize();
    sf::Vector2f viewCenter = gameView.getCenter();
    
    float viewLeft = viewCenter.x - viewSize.x / 2;
    float viewTop = viewCenter.y - viewSize.y / 2;
    
    // Draw simple background
    sf::RectangleShape background;
    background.setSize(viewSize);
    background.setPosition(viewLeft, viewTop);
    background.setFillColor(sf::Color(40, 20, 60)); // Dark purple
    m_game->window().draw(background);
    
    // Draw simple text
    sf::Text battleText;
    try {
        battleText.setFont(m_game->getAssets().getFont("ShareTech"));
        battleText.setString("BATTLE SCENE\n\nPress B or ESC to return to game");
        battleText.setCharacterSize(24);
        battleText.setFillColor(sf::Color::White);
        battleText.setPosition(viewCenter.x - 150, viewCenter.y - 50);
        m_game->window().draw(battleText);
    } catch (const std::exception& e) {
        std::cout << "Could not render text: " << e.what() << std::endl;
    }
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
void Scene_Battle::renderBattleField() {}
void Scene_Battle::renderCharacters() {}
void Scene_Battle::renderStatusBars() {}
void Scene_Battle::renderUI() {}
void Scene_Battle::renderActionMenu() {}
void Scene_Battle::renderSpellMenu() {}
void Scene_Battle::renderTargetSelection() {}
Scene_Battle::BattleCharacter* Scene_Battle::getAlivePlayerCharacter(int) { return nullptr; }
Scene_Battle::BattleCharacter* Scene_Battle::getAliveEnemyCharacter(int) { return nullptr; }
