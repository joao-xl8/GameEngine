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
  
  // Turn-based system
  struct TurnEntry {
    bool isPlayer;
    int characterIndex;
    int speed;
    std::string name;
  };
  
  std::vector<TurnEntry> m_turnQueue;
  int m_currentTurnIndex;
  bool m_waitingForPlayerAction;
  float m_turnTimer;
  const float TURN_DELAY = 1.0f; // Delay between turns in seconds
  
  // Cursor navigation system
  bool m_cursorOnPlayerSide;  // true = player side, false = enemy side
  int m_cursorPlayerIndex;    // Which player is selected (0-based)
  int m_cursorEnemyIndex;     // Which enemy is selected (0-based)
  bool m_selectingAction;     // true when selecting action, false when selecting target
  
  // Battle menu system
  enum class BattleMenuState {
    MAIN_MENU,      // Attack, Spells, Items
    SPELL_MENU,     // List of available spells
    ITEM_MENU,      // List of available items
    TARGET_SELECT   // Selecting target for action
  };
  
  BattleMenuState m_menuState;
  int m_selectedMenuIndex;    // Currently selected menu option
  std::vector<std::string> m_currentMenuOptions;
  std::string m_selectedAction; // "ATTACK", "SPELL:FireBall", "ITEM:Potion", etc.
  std::string m_pendingAction;  // Action waiting for target selection
  bool m_targetingEnemies;      // true = targeting enemies, false = targeting party

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
  
  // Turn queue system
  void buildTurnQueue();
  void resetTurnQueue();
  TurnEntry getCurrentTurn();
  bool isCurrentTurnPlayer();
  void advanceTurn();
  void enemyAI(int enemyIndex);
  void updateTurnTimer(float deltaTime);
  
  // Cursor navigation system
  void initializeCursor();
  void moveCursorLeft();
  void moveCursorRight();
  void moveCursorUp();
  void moveCursorDown();
  int getCurrentlySelectedPlayerIndex();
  int getCurrentlySelectedEnemyIndex();
  void highlightCurrentTurnCharacter();
  
  // Battle menu system
  void initializeBattleMenu();
  void showMainMenu();
  void showSpellMenu();
  void showItemMenu();
  void handleMenuNavigation(const std::string& direction);
  void handleMenuSelection();
  void executeSelectedAction();
  void renderBattleMenu();
  
  // Target selection system
  void enterTargetSelection(const std::string& action);
  void handleTargetNavigation(const std::string& direction);
  void executeActionOnTarget();
  bool isOffensiveAction(const std::string& action);
  bool isDefensiveAction(const std::string& action);
  void renderTargetSelection();

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
  void renderStatusBars();
  void updateCharacterPositions();
  
  // Enhanced visual rendering methods
  void renderPartyMembers(const sf::RectangleShape& area);
  void renderEnemies(const sf::RectangleShape& area);
  void renderBattleStatus();
  
  // Interactive demo actions
  void performDemoAttack();
  void performDemoHeal();
  void performDemoSpell();
  void performDemoDefend();

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
  
  // Visual utility methods
  sf::Color getCharacterColor(const std::string& name);
  sf::Color getEnemyColor(const std::string& name);

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
