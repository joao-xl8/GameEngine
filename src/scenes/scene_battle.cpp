#include "scene_battle.hpp"
#include "../game_engine.hpp"
#include <algorithm>
#include <random>
#include <iostream>

Scene_Battle::Scene_Battle(GameEngine* game) 
    : Scene(game)
    , m_battleState(BattleState::ENTERING)
    , m_playerAreaTopLeft(0, 0)
    , m_playerAreaSize(0, 0)
    , m_enemyAreaTopLeft(0, 0)
    , m_enemyAreaSize(0, 0)
    , m_uiAreaTopLeft(0, 0)
    , m_uiAreaSize(0, 0)
    , m_selectedPlayerIndex(0)
    , m_selectedActionIndex(0)
    , m_selectedTargetIndex(0)
    , m_selectingTarget(false)
    , m_deltaTime(0.0f)
    , m_actionTimer(0.0f)
    , m_messageTimer(0.0f)
{
    init();
}

Scene_Battle::Scene_Battle(GameEngine* game, const std::vector<std::string>& enemyTypes)
    : Scene_Battle(game)
{
    // Add enemies based on the provided types
    for (const auto& enemyType : enemyTypes) {
        if (enemyType == "Goblin") {
            addEnemy("Goblin", 30, 12, 5, 8);
        } else if (enemyType == "Orc") {
            addEnemy("Orc", 50, 18, 8, 6);
        } else if (enemyType == "Dragon") {
            addEnemy("Dragon", 120, 35, 15, 10);
        } else {
            // Default enemy
            addEnemy(enemyType, 25, 10, 4, 7);
        }
    }
}

void Scene_Battle::init() {
    // Register input actions
    registerAction(sf::Keyboard::W, "UP");
    registerAction(sf::Keyboard::S, "DOWN");
    registerAction(sf::Keyboard::A, "LEFT");
    registerAction(sf::Keyboard::D, "RIGHT");
    registerAction(sf::Keyboard::Enter, "SELECT");
    registerAction(sf::Keyboard::Escape, "BACK");
    registerAction(sf::Keyboard::Space, "CONFIRM");
    
    // Calculate screen areas
    float screenWidth = static_cast<float>(width());
    float screenHeight = static_cast<float>(height());
    
    // UI area takes bottom 1/3 of screen
    m_uiAreaSize = Vec2(screenWidth, screenHeight / 3.0f);
    m_uiAreaTopLeft = Vec2(0, screenHeight - m_uiAreaSize.y);
    
    // Battle area is top 2/3 of screen
    float battleAreaHeight = screenHeight - m_uiAreaSize.y;
    
    // Player area: left half of battle area
    m_playerAreaSize = Vec2(screenWidth / 2.0f, battleAreaHeight);
    m_playerAreaTopLeft = Vec2(0, 0);
    
    // Enemy area: right half of battle area  
    m_enemyAreaSize = Vec2(screenWidth / 2.0f, battleAreaHeight);
    m_enemyAreaTopLeft = Vec2(screenWidth / 2.0f, 0);
    
    // Load font for UI text
    if (!m_font.loadFromFile("assets/fonts/arial.ttf")) {
        // Fallback to system font if custom font not available
        std::cout << "Warning: Could not load custom font, using default\n";
    }
    
    // Setup text objects
    m_actionText.setFont(m_font);
    m_actionText.setCharacterSize(24);
    m_actionText.setFillColor(sf::Color::White);
    
    m_statusText.setFont(m_font);
    m_statusText.setCharacterSize(18);
    m_statusText.setFillColor(sf::Color::Yellow);
    
    m_damageText.setFont(m_font);
    m_damageText.setCharacterSize(20);
    m_damageText.setFillColor(sf::Color::Red);
    
    // Create background
    m_background = m_entityManager.addEntity("Background");
    m_background->addComponent<CTransform>(std::make_shared<CTransform>(Vec2(screenWidth/2, screenHeight/2)));
    
    // Create UI panel
    m_uiPanel = m_entityManager.addEntity("UIPanel");
    m_uiPanel->addComponent<CTransform>(std::make_shared<CTransform>(
        Vec2(m_uiAreaTopLeft.x + m_uiAreaSize.x/2, m_uiAreaTopLeft.y + m_uiAreaSize.y/2)
    ));
    
    // Setup default player party if none exists
    if (m_playerParty.empty()) {
        setupPlayerParty();
    }
    
    // Setup default enemies if none exist
    if (m_enemies.empty()) {
        setupEnemies();
    }
    
    setupBattlePositions();
    startBattle();
}

void Scene_Battle::setupPlayerParty() {
    // Default player party
    addPlayerCharacter("Hero", 80, 20, 12, 10);
    addPlayerCharacter("Mage", 50, 25, 8, 12);
    addPlayerCharacter("Warrior", 100, 18, 15, 8);
}

void Scene_Battle::setupEnemies() {
    // Default enemy encounter
    addEnemy("Goblin", 30, 12, 5, 8);
    addEnemy("Orc", 50, 18, 8, 6);
}

void Scene_Battle::setupBattlePositions() {
    // Position player characters in left area
    for (size_t i = 0; i < m_playerParty.size(); ++i) {
        float x = m_playerAreaTopLeft.x + m_playerAreaSize.x * 0.7f; // Towards right side of left area
        float y = m_playerAreaTopLeft.y + (i + 1) * (m_playerAreaSize.y / (m_playerParty.size() + 1));
        m_playerParty[i].battlePosition = Vec2(x, y);
        
        // Create visual entity for player
        m_playerParty[i].entity = m_entityManager.addEntity("Player_" + std::to_string(i));
        m_playerParty[i].entity->addComponent<CTransform>(std::make_shared<CTransform>(m_playerParty[i].battlePosition));
        
        // Add sprite if texture exists
        auto& texture = m_game->getAssets().getTexture("Player");
        m_playerParty[i].entity->addComponent<CSprite>(std::make_shared<CSprite>("Player", texture));
    }
    
    // Position enemies in right area
    for (size_t i = 0; i < m_enemies.size(); ++i) {
        float x = m_enemyAreaTopLeft.x + m_enemyAreaSize.x * 0.3f; // Towards left side of right area
        float y = m_enemyAreaTopLeft.y + (i + 1) * (m_enemyAreaSize.y / (m_enemies.size() + 1));
        m_enemies[i].battlePosition = Vec2(x, y);
        
        // Create visual entity for enemy
        m_enemies[i].entity = m_entityManager.addEntity("Enemy_" + std::to_string(i));
        m_enemies[i].entity->addComponent<CTransform>(std::make_shared<CTransform>(m_enemies[i].battlePosition));
        
        // Add sprite if texture exists
        auto& texture = m_game->getAssets().getTexture("Enemy");
        m_enemies[i].entity->addComponent<CSprite>(std::make_shared<CSprite>("Enemy", texture));
    }
}

void Scene_Battle::addPlayerCharacter(const std::string& name, int hp, int attack, int defense, int speed) {
    m_playerParty.emplace_back(name, hp, attack, defense, speed, true);
}

void Scene_Battle::addEnemy(const std::string& name, int hp, int attack, int defense, int speed) {
    m_enemies.emplace_back(name, hp, attack, defense, speed, false);
}

void Scene_Battle::startBattle() {
    m_battleState = BattleState::PLAYER_TURN;
    m_selectedPlayerIndex = 0;
    m_selectedActionIndex = 0;
    m_selectedTargetIndex = 0;
    m_selectingTarget = false;
    
    // Find first alive player character
    for (size_t i = 0; i < m_playerParty.size(); ++i) {
        if (m_playerParty[i].isAlive) {
            m_selectedPlayerIndex = static_cast<int>(i);
            break;
        }
    }
}

void Scene_Battle::update() {
    m_deltaTime = m_deltaClock.restart().asSeconds();
    m_actionTimer += m_deltaTime;
    m_messageTimer += m_deltaTime;
    
    switch (m_battleState) {
        case BattleState::ENTERING:
            // Battle intro animation/transition
            if (m_actionTimer > 1.0f) {
                m_battleState = BattleState::PLAYER_TURN;
                m_actionTimer = 0.0f;
            }
            break;
            
        case BattleState::PLAYER_TURN:
            processPlayerTurn();
            break;
            
        case BattleState::ENEMY_TURN:
            processEnemyTurn();
            break;
            
        case BattleState::EXECUTING:
            executeActions();
            break;
            
        case BattleState::VICTORY:
        case BattleState::DEFEAT:
            // Handle end of battle
            if (m_actionTimer > 3.0f) {
                onEnd();
            }
            break;
            
        case BattleState::FLEEING:
            // Handle flee attempt
            if (m_actionTimer > 1.0f) {
                onEnd();
            }
            break;
    }
    
    updateCharacterPositions();
    checkBattleEnd();
}

void Scene_Battle::processPlayerTurn() {
    // Player turn logic is handled by input in sDoAction
    // This method can be used for AI players or turn timers
}

void Scene_Battle::processEnemyTurn() {
    if (m_actionTimer > 1.0f) { // Give 1 second for enemy "thinking"
        generateEnemyActions();
        m_battleState = BattleState::EXECUTING;
        m_actionTimer = 0.0f;
    }
}

void Scene_Battle::generateEnemyActions() {
    std::random_device rd;
    std::mt19937 gen(rd());
    
    for (auto& enemy : m_enemies) {
        if (!enemy.isAlive) continue;
        
        // Simple AI: attack random alive player
        auto aliveTargets = getAliveCharacters(true); // Get alive players
        if (!aliveTargets.empty()) {
            std::uniform_int_distribution<> targetDist(0, static_cast<int>(aliveTargets.size() - 1));
            BattleCharacter* target = aliveTargets[targetDist(gen)];
            
            ActionCommand command(&enemy, target, BattleAction::ATTACK);
            m_actionQueue.push_back(command);
        }
    }
}

void Scene_Battle::executeActions() {
    if (m_actionQueue.empty()) {
        // All actions executed, return to player turn
        m_battleState = BattleState::PLAYER_TURN;
        m_actionTimer = 0.0f;
        return;
    }
    
    if (m_actionTimer > 0.5f) { // Execute one action every 0.5 seconds
        if (!m_actionQueue.empty()) {
            executeAction(m_actionQueue.front());
            m_actionQueue.erase(m_actionQueue.begin());
            m_actionTimer = 0.0f;
        }
    }
}

void Scene_Battle::executeAction(ActionCommand& command) {
    if (!command.actor->isAlive || !command.target->isAlive) {
        return;
    }
    
    switch (command.action) {
        case BattleAction::ATTACK: {
            int damage = calculateDamage(*command.actor, *command.target);
            command.target->currentHP = std::max(0, command.target->currentHP - damage);
            command.damage = damage;
            
            if (command.target->currentHP <= 0) {
                command.target->isAlive = false;
            }
            
            std::cout << command.actor->name << " attacks " << command.target->name 
                      << " for " << damage << " damage!\n";
            break;
        }
        case BattleAction::DEFEND:
            // Reduce incoming damage for this turn
            std::cout << command.actor->name << " defends!\n";
            break;
        case BattleAction::FLEE:
            // Attempt to flee battle
            m_battleState = BattleState::FLEEING;
            break;
        default:
            break;
    }
    
    command.executed = true;
}

int Scene_Battle::calculateDamage(const BattleCharacter& attacker, const BattleCharacter& defender) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> variance(0.8, 1.2); // ±20% damage variance
    
    int baseDamage = std::max(1, attacker.attack - defender.defense / 2);
    return static_cast<int>(baseDamage * variance(gen));
}

void Scene_Battle::sDoAction(const Action& action) {
    if (m_battleState != BattleState::PLAYER_TURN) return;
    
    if (m_selectingTarget) {
        handleTargetSelection(action);
    } else {
        handleMenuNavigation(action);
    }
}

void Scene_Battle::handleMenuNavigation(const Action& action) {
    if (action.getType() == "START") {
        if (action.getName() == "UP") {
            // Play menu navigation sound (only if sound is enabled)
            if (m_game->isSoundEnabled()) {
                if (auto globalSound = m_game->getGlobalSoundManager()) {
                    float volume = m_game->getMasterVolume() * m_game->getEffectsVolume() * 60.0f;
                    globalSound->playSound("menu_select", volume);
                }
            }
            m_selectedActionIndex = std::max(0, m_selectedActionIndex - 1);
        } else if (action.getName() == "DOWN") {
            // Play menu navigation sound (only if sound is enabled)
            if (m_game->isSoundEnabled()) {
                if (auto globalSound = m_game->getGlobalSoundManager()) {
                    float volume = m_game->getMasterVolume() * m_game->getEffectsVolume() * 60.0f;
                    globalSound->playSound("menu_select", volume);
                }
            }
            m_selectedActionIndex = std::min(4, m_selectedActionIndex + 1); // 5 actions: Attack, Defend, Item, Skill, Flee
        } else if (action.getName() == "SELECT" || action.getName() == "CONFIRM") {
            // Play menu confirm sound (only if sound is enabled)
            if (m_game->isSoundEnabled()) {
                if (auto globalSound = m_game->getGlobalSoundManager()) {
                    float volume = m_game->getMasterVolume() * m_game->getEffectsVolume() * 80.0f;
                    globalSound->playSound("menu_confirm", volume);
                }
            }
            
            BattleAction selectedAction = static_cast<BattleAction>(m_selectedActionIndex);
            
            if (selectedAction == BattleAction::ATTACK || selectedAction == BattleAction::SKILL) {
                // Need to select target
                m_selectingTarget = true;
                m_selectedTargetIndex = 0;
            } else {
                // Execute action immediately
                queuePlayerAction(selectedAction);
            }
        }
    }
}

void Scene_Battle::handleTargetSelection(const Action& action) {
    auto aliveEnemies = getAliveCharacters(false);
    
    if (action.getType() == "START") {
        if (action.getName() == "UP") {
            // Play menu navigation sound (only if sound is enabled)
            if (m_game->isSoundEnabled()) {
                if (auto globalSound = m_game->getGlobalSoundManager()) {
                    float volume = m_game->getMasterVolume() * m_game->getEffectsVolume() * 60.0f;
                    globalSound->playSound("menu_select", volume);
                }
            }
            m_selectedTargetIndex = std::max(0, m_selectedTargetIndex - 1);
        } else if (action.getName() == "DOWN") {
            // Play menu navigation sound (only if sound is enabled)
            if (m_game->isSoundEnabled()) {
                if (auto globalSound = m_game->getGlobalSoundManager()) {
                    float volume = m_game->getMasterVolume() * m_game->getEffectsVolume() * 60.0f;
                    globalSound->playSound("menu_select", volume);
                }
            }
            m_selectedTargetIndex = std::min(static_cast<int>(aliveEnemies.size() - 1), m_selectedTargetIndex + 1);
        } else if (action.getName() == "SELECT" || action.getName() == "CONFIRM") {
            // Play menu confirm sound (only if sound is enabled)
            if (m_game->isSoundEnabled()) {
                if (auto globalSound = m_game->getGlobalSoundManager()) {
                    float volume = m_game->getMasterVolume() * m_game->getEffectsVolume() * 80.0f;
                    globalSound->playSound("menu_confirm", volume);
                }
            }
            
            if (m_selectedTargetIndex < static_cast<int>(aliveEnemies.size())) {
                BattleAction selectedAction = static_cast<BattleAction>(m_selectedActionIndex);
                
                // Create action command
                BattleCharacter* actor = &m_playerParty[m_selectedPlayerIndex];
                BattleCharacter* target = aliveEnemies[m_selectedTargetIndex];
                
                ActionCommand command(actor, target, selectedAction);
                m_actionQueue.push_back(command);
                
                m_selectingTarget = false;
                m_battleState = BattleState::ENEMY_TURN;
                m_actionTimer = 0.0f;
            }
        }
    } else if (action.getName() == "BACK") {
        // Play menu back sound (only if sound is enabled)
        if (m_game->isSoundEnabled()) {
            if (auto globalSound = m_game->getGlobalSoundManager()) {
                float volume = m_game->getMasterVolume() * m_game->getEffectsVolume() * 50.0f;
                globalSound->playSound("menu_select", volume);
            }
        }
        
        m_selectingTarget = false;
    }
}

void Scene_Battle::queuePlayerAction(BattleAction action) {
    BattleCharacter* actor = &m_playerParty[m_selectedPlayerIndex];
    
    if (action == BattleAction::DEFEND) {
        ActionCommand command(actor, nullptr, action);
        m_actionQueue.push_back(command);
    } else if (action == BattleAction::FLEE) {
        ActionCommand command(actor, nullptr, action);
        m_actionQueue.push_back(command);
    }
    
    m_battleState = BattleState::ENEMY_TURN;
    m_actionTimer = 0.0f;
}

void Scene_Battle::checkBattleEnd() {
    if (!isPlayerTeamAlive()) {
        m_battleState = BattleState::DEFEAT;
        m_actionTimer = 0.0f;
    } else if (!isEnemyTeamAlive()) {
        m_battleState = BattleState::VICTORY;
        m_actionTimer = 0.0f;
    }
}

bool Scene_Battle::isPlayerTeamAlive() {
    return std::any_of(m_playerParty.begin(), m_playerParty.end(),
                       [](const BattleCharacter& c) { return c.isAlive; });
}

bool Scene_Battle::isEnemyTeamAlive() {
    return std::any_of(m_enemies.begin(), m_enemies.end(),
                       [](const BattleCharacter& c) { return c.isAlive; });
}

std::vector<Scene_Battle::BattleCharacter*> Scene_Battle::getAliveCharacters(bool playerTeam) {
    std::vector<BattleCharacter*> alive;
    
    if (playerTeam) {
        for (auto& character : m_playerParty) {
            if (character.isAlive) {
                alive.push_back(&character);
            }
        }
    } else {
        for (auto& character : m_enemies) {
            if (character.isAlive) {
                alive.push_back(&character);
            }
        }
    }
    
    return alive;
}

void Scene_Battle::updateCharacterPositions() {
    // Update entity positions based on battle positions
    for (auto& character : m_playerParty) {
        if (character.entity && character.entity->hasComponent<CTransform>()) {
            character.entity->getComponent<CTransform>()->pos = character.battlePosition;
        }
    }
    
    for (auto& character : m_enemies) {
        if (character.entity && character.entity->hasComponent<CTransform>()) {
            character.entity->getComponent<CTransform>()->pos = character.battlePosition;
        }
    }
}

void Scene_Battle::sRender() {
    renderBattleField();
    renderCharacters();
    renderUI();
}

void Scene_Battle::renderBattleField() {
    // Render background
    if (m_background && m_background->hasComponent<CSprite>()) {
        auto sprite = m_background->getComponent<CSprite>();
        auto transform = m_background->getComponent<CTransform>();
        sprite->sprite.setPosition(transform->pos.x, transform->pos.y);
        m_game->window().draw(sprite->sprite);
    }
    
    // Draw area dividers for debugging
    sf::RectangleShape playerArea(sf::Vector2f(m_playerAreaSize.x, m_playerAreaSize.y));
    playerArea.setPosition(m_playerAreaTopLeft.x, m_playerAreaTopLeft.y);
    playerArea.setFillColor(sf::Color(0, 100, 0, 50)); // Semi-transparent green
    playerArea.setOutlineColor(sf::Color::Green);
    playerArea.setOutlineThickness(2);
    m_game->window().draw(playerArea);
    
    sf::RectangleShape enemyArea(sf::Vector2f(m_enemyAreaSize.x, m_enemyAreaSize.y));
    enemyArea.setPosition(m_enemyAreaTopLeft.x, m_enemyAreaTopLeft.y);
    enemyArea.setFillColor(sf::Color(100, 0, 0, 50)); // Semi-transparent red
    enemyArea.setOutlineColor(sf::Color::Red);
    enemyArea.setOutlineThickness(2);
    m_game->window().draw(enemyArea);
}

void Scene_Battle::renderCharacters() {
    // Render all entities (characters)
    for (auto& entity : m_entityManager.getEntities()) {
        if (entity->hasComponent<CSprite>() && entity->hasComponent<CTransform>()) {
            auto sprite = entity->getComponent<CSprite>();
            auto transform = entity->getComponent<CTransform>();
            
            sprite->sprite.setPosition(transform->pos.x, transform->pos.y);
            m_game->window().draw(sprite->sprite);
        }
    }
    
    renderStatusBars();
}

void Scene_Battle::renderStatusBars() {
    // Render HP bars for all characters
    for (size_t i = 0; i < m_playerParty.size(); ++i) {
        const auto& character = m_playerParty[i];
        if (!character.isAlive) continue;
        
        Vec2 barPos = character.battlePosition;
        barPos.y += 40; // Below character
        
        // Background bar
        sf::RectangleShape bgBar(sf::Vector2f(60, 8));
        bgBar.setPosition(barPos.x - 30, barPos.y);
        bgBar.setFillColor(sf::Color::Red);
        m_game->window().draw(bgBar);
        
        // HP bar
        float hpRatio = static_cast<float>(character.currentHP) / character.maxHP;
        sf::RectangleShape hpBar(sf::Vector2f(60 * hpRatio, 8));
        hpBar.setPosition(barPos.x - 30, barPos.y);
        hpBar.setFillColor(sf::Color::Green);
        m_game->window().draw(hpBar);
        
        // HP text
        sf::Text hpText;
        hpText.setFont(m_font);
        hpText.setString(std::to_string(character.currentHP) + "/" + std::to_string(character.maxHP));
        hpText.setCharacterSize(12);
        hpText.setFillColor(sf::Color::White);
        hpText.setPosition(barPos.x - 25, barPos.y + 10);
        m_game->window().draw(hpText);
    }
    
    // Same for enemies
    for (size_t i = 0; i < m_enemies.size(); ++i) {
        const auto& character = m_enemies[i];
        if (!character.isAlive) continue;
        
        Vec2 barPos = character.battlePosition;
        barPos.y += 40;
        
        sf::RectangleShape bgBar(sf::Vector2f(60, 8));
        bgBar.setPosition(barPos.x - 30, barPos.y);
        bgBar.setFillColor(sf::Color::Red);
        m_game->window().draw(bgBar);
        
        float hpRatio = static_cast<float>(character.currentHP) / character.maxHP;
        sf::RectangleShape hpBar(sf::Vector2f(60 * hpRatio, 8));
        hpBar.setPosition(barPos.x - 30, barPos.y);
        hpBar.setFillColor(sf::Color::Green);
        m_game->window().draw(hpBar);
        
        sf::Text hpText;
        hpText.setFont(m_font);
        hpText.setString(std::to_string(character.currentHP) + "/" + std::to_string(character.maxHP));
        hpText.setCharacterSize(12);
        hpText.setFillColor(sf::Color::White);
        hpText.setPosition(barPos.x - 25, barPos.y + 10);
        m_game->window().draw(hpText);
    }
}

void Scene_Battle::renderUI() {
    // Draw UI background
    sf::RectangleShape uiBackground(sf::Vector2f(m_uiAreaSize.x, m_uiAreaSize.y));
    uiBackground.setPosition(m_uiAreaTopLeft.x, m_uiAreaTopLeft.y);
    uiBackground.setFillColor(sf::Color(0, 0, 0, 180)); // Semi-transparent black
    uiBackground.setOutlineColor(sf::Color::White);
    uiBackground.setOutlineThickness(2);
    m_game->window().draw(uiBackground);
    
    if (m_battleState == BattleState::PLAYER_TURN) {
        if (m_selectingTarget) {
            renderTargetSelection();
        } else {
            renderActionMenu();
        }
    }
    
    // Render battle state text
    std::string stateText;
    switch (m_battleState) {
        case BattleState::ENTERING: stateText = "Battle begins!"; break;
        case BattleState::PLAYER_TURN: stateText = "Your turn"; break;
        case BattleState::ENEMY_TURN: stateText = "Enemy turn"; break;
        case BattleState::EXECUTING: stateText = "Executing actions..."; break;
        case BattleState::VICTORY: stateText = "Victory!"; break;
        case BattleState::DEFEAT: stateText = "Defeat..."; break;
        case BattleState::FLEEING: stateText = "Fleeing battle!"; break;
    }
    
    m_statusText.setString(stateText);
    m_statusText.setPosition(m_uiAreaTopLeft.x + 10, m_uiAreaTopLeft.y + 10);
    m_game->window().draw(m_statusText);
}

void Scene_Battle::renderActionMenu() {
    std::vector<std::string> actions = {"Attack", "Defend", "Item", "Skill", "Flee"};
    
    for (size_t i = 0; i < actions.size(); ++i) {
        sf::Text actionText;
        actionText.setFont(m_font);
        actionText.setString(actions[i]);
        actionText.setCharacterSize(20);
        
        if (static_cast<int>(i) == m_selectedActionIndex) {
            actionText.setFillColor(sf::Color::Yellow);
            actionText.setString("> " + actions[i]);
        } else {
            actionText.setFillColor(sf::Color::White);
        }
        
        actionText.setPosition(m_uiAreaTopLeft.x + 50, m_uiAreaTopLeft.y + 50 + i * 30);
        m_game->window().draw(actionText);
    }
}

void Scene_Battle::renderTargetSelection() {
    auto aliveEnemies = getAliveCharacters(false);
    
    sf::Text headerText;
    headerText.setFont(m_font);
    headerText.setString("Select Target:");
    headerText.setCharacterSize(20);
    headerText.setFillColor(sf::Color::White);
    headerText.setPosition(m_uiAreaTopLeft.x + 50, m_uiAreaTopLeft.y + 50);
    m_game->window().draw(headerText);
    
    for (size_t i = 0; i < aliveEnemies.size(); ++i) {
        sf::Text targetText;
        targetText.setFont(m_font);
        targetText.setString(aliveEnemies[i]->name);
        targetText.setCharacterSize(18);
        
        if (static_cast<int>(i) == m_selectedTargetIndex) {
            targetText.setFillColor(sf::Color::Yellow);
            targetText.setString("> " + aliveEnemies[i]->name);
        } else {
            targetText.setFillColor(sf::Color::White);
        }
        
        targetText.setPosition(m_uiAreaTopLeft.x + 70, m_uiAreaTopLeft.y + 80 + i * 25);
        m_game->window().draw(targetText);
    }
}

void Scene_Battle::onEnd() {
    // Return to previous scene or game over
    // This would typically transition back to the overworld or show results
    std::cout << "Battle ended!\n";
}
