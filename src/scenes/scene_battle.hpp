#pragma once
#include "scene.hpp"
#include "../components.hpp"
#include <vector>
#include <memory>
#include <queue>

// Forward declaration to avoid circular dependency
class BattleConfigLoader;

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

  enum class BattleAction { ATTACK, DEFEND, ITEM, SPELL, FLEE };

  struct BattleCharacter {
    std::shared_ptr<Entity> entity;
    std::string name;
    int currentHP;
    int maxHP;
    int attack;
    int defense;
    int speed;
    int mp;
    int maxMP;
    bool isPlayer;
    bool isAlive;
    bool isDefending;
    Vec2 battlePosition;
    std::string spriteTexture;

    BattleCharacter(const std::string &n, int hp, int att, int def, int spd, int magic = 20, bool player = false)
        : name(n), currentHP(hp), maxHP(hp), attack(att), defense(def),
          speed(spd), mp(magic), maxMP(magic), isPlayer(player), isAlive(true), 
          isDefending(false), battlePosition(0, 0) {}
  };

  struct ActionCommand {
    BattleCharacter *actor;
    BattleCharacter *target;
    BattleAction action;
    std::string spellName;
    int damage;
    bool executed;

    ActionCommand(BattleCharacter *a, BattleCharacter *t, BattleAction act, const std::string& spell = "")
        : actor(a), target(t), action(act), spellName(spell), damage(0), executed(false) {}
  };

  struct Spell {
    std::string name;
    int mpCost;
    int baseDamage;
    std::string description;
    
    Spell(const std::string& n, int cost, int damage, const std::string& desc)
        : name(n), mpCost(cost), baseDamage(damage), description(desc) {}
  };

protected:
  // Battle configuration loader (loaded on demand)
  std::unique_ptr<BattleConfigLoader> m_configLoader;
  bool m_configLoaded = false;
  
  // Battle state
  BattleState m_battleState;
  std::vector<BattleCharacter> m_playerParty;
  std::vector<BattleCharacter> m_enemies;
  std::queue<ActionCommand> m_actionQueue;
  
  // Available spells
  std::vector<Spell> m_availableSpells;
  bool m_selectingSpell;
  int m_selectedSpellIndex;

  // UI and positioning (using game view)
  Vec2 m_playerAreaTopLeft;
  Vec2 m_playerAreaSize;
  Vec2 m_enemyAreaTopLeft;
  Vec2 m_enemyAreaSize;
  Vec2 m_uiAreaTopLeft;
  Vec2 m_uiAreaSize;

  // Current selections
  int m_selectedPlayerIndex;
  int m_selectedActionIndex;
  int m_selectedTargetIndex;
  bool m_selectingTarget;

  // State persistence (like dialogue scene)
  std::string m_returnLevel;
  Vec2 m_returnPosition;
  int m_returnHealth;
  float m_returnPlayTime;
  bool m_preserveState;

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
  float m_actionTimer;
  float m_messageTimer;

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
  void initializeSpells();

  // Action processing
  void queuePlayerAction(BattleAction action);
  void queuePlayerSpell(const std::string& spellName);
  void generateEnemyActions();
  void sortActionsBySpeed();
  void executeAction(ActionCommand &command);
  int calculateDamage(const BattleCharacter &attacker, const BattleCharacter &defender);
  int calculateSpellDamage(const BattleCharacter &caster, const std::string& spellName);

  // UI methods
  void renderBattleField();
  void renderCharacters();
  void renderUI();
  void renderActionMenu();
  void renderSpellMenu();
  void renderTargetSelection();
  void renderStatusBars();
  void updateCharacterPositions();

  // Input handling
  void handleMenuNavigation(const Action &action);
  void handleSpellSelection(const Action &action);
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
  
  // Constructor with state preservation (like dialogue scene)
  Scene_Battle(GameEngine *game, const std::vector<std::string> &enemyTypes,
               const std::string& returnLevel, const Vec2& returnPos, 
               int returnHealth, float returnPlayTime);
  
  // Destructor must be declared in header for unique_ptr with forward declaration
  ~Scene_Battle();

  // Public interface for starting battles
  void addPlayerCharacter(const std::string &name, int hp, int attack, int defense, int speed, int mp = 20);
  void addEnemy(const std::string &name, int hp, int attack, int defense, int speed);
  void startBattle();
  
  // Configuration loading methods
  bool loadBattleConfigurations();
  void loadPartyFromConfig(const std::vector<std::string>& memberIds, int level = 1);
  void loadEnemiesFromConfig(int level, const std::vector<std::string>& enemyIds = {});
  void loadRandomEncounter(int level);
  BattleConfigLoader* getConfigLoader() { return m_configLoader.get(); }
};
