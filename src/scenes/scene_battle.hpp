#pragma once
#include "scene.hpp"
#include "../components.hpp"
#include <vector>
#include <memory>
#include <vector>

class Scene_Battle : public Scene {
public:
  enum class BattleState {
    ENTERING,    // Battle transition/intro
    PLAYER_TURN, // Player selecting actions
    ENEMY_TURN,  // Enemies acting
    EXECUTING,   // Actions being executed
    VICTORY,     // Player won
    DEFEAT,      // Player lost
    FLEEING      // Player attempting to flee
  };

  enum class BattleAction { ATTACK, DEFEND, ITEM, SKILL, FLEE };

  struct BattleCharacter {
    std::shared_ptr<Entity> entity;
    std::string name;
    int currentHP;
    int maxHP;
    int attack;
    int defense;
    int speed;
    bool isPlayer;
    bool isAlive;
    Vec2 battlePosition;

    BattleCharacter(const std::string &n, int hp, int att, int def, int spd, bool player = false)
        : name(n), currentHP(hp), maxHP(hp), attack(att), defense(def),
          speed(spd), isPlayer(player), isAlive(true), battlePosition(0, 0) {}
  };

  struct ActionCommand {
    BattleCharacter *actor;
    BattleCharacter *target;
    BattleAction action;
    std::string skillName;
    int damage;
    bool executed;

    ActionCommand(BattleCharacter *a, BattleCharacter *t, BattleAction act)
        : actor(a), target(t), action(act), damage(0), executed(false) {}
  };

protected:
  // Battle state
  BattleState m_battleState;
  std::vector<BattleCharacter> m_playerParty;
  std::vector<BattleCharacter> m_enemies;
  std::vector<ActionCommand> m_actionQueue;

  // UI and positioning
  Vec2 m_playerAreaTopLeft; // Top-left corner of player area (bottom-left of
                            // screen)
  Vec2 m_playerAreaSize;    // Size of player positioning area
  Vec2
      m_enemyAreaTopLeft; // Top-left corner of enemy area (top-right of screen)
  Vec2 m_enemyAreaSize;   // Size of enemy positioning area
  Vec2 m_uiAreaTopLeft;   // Top-left corner of UI area (bottom 1/3 of screen)
  Vec2 m_uiAreaSize;      // Size of UI area

  // Current selections
  int m_selectedPlayerIndex; // Which player character is acting
  int m_selectedActionIndex; // Which action is selected
  int m_selectedTargetIndex; // Which target is selected
  bool m_selectingTarget;    // Whether we're in target selection mode

  // Visual elements
  std::shared_ptr<Entity> m_background;
  std::shared_ptr<Entity> m_uiPanel;
  sf::Text m_actionText;
  sf::Text m_statusText;
  sf::Text m_damageText;
  sf::Font m_font;

  // Timing
  sf::Clock m_deltaClock;
  float m_deltaTime;
  float m_actionTimer;  // Timer for action animations
  float m_messageTimer; // Timer for battle messages

  // Battle flow methods
  void init() override;
  void onEnd() override;
  void update() override;
  void sDoAction(const Action &action) override;
  void sRender() override;

  // Battle system methods
  void setupBattlePositions();
  void setupPlayerParty();
  void setupEnemies();
  void processPlayerTurn();
  void processEnemyTurn();
  void executeActions();
  void checkBattleEnd();

  // Action processing
  void queuePlayerAction(BattleAction action);
  void generateEnemyActions();
  void sortActionsBySpeed();
  void executeAction(ActionCommand &command);
  int calculateDamage(const BattleCharacter &attacker,
                      const BattleCharacter &defender);

  // UI methods
  void renderBattleField();
  void renderCharacters();
  void renderUI();
  void renderActionMenu();
  void renderTargetSelection();
  void renderStatusBars();
  void updateCharacterPositions();

  // Input handling
  void handleMenuNavigation(const Action &action);
  void handleTargetSelection(const Action &action);

  // Utility methods
  BattleCharacter *getAlivePlayerCharacter(int index);
  BattleCharacter *getAliveEnemyCharacter(int index);
  std::vector<BattleCharacter *> getAliveCharacters(bool playerTeam);
  bool isPlayerTeamAlive();
  bool isEnemyTeamAlive();

public:
  Scene_Battle(GameEngine *game);
  Scene_Battle(GameEngine *game, const std::vector<std::string> &enemyTypes);

  // Public interface for starting battles
  void addPlayerCharacter(const std::string &name, int hp, int attack,
                          int defense, int speed);
  void addEnemy(const std::string &name, int hp, int attack, int defense,
                int speed);
  void startBattle();
};
