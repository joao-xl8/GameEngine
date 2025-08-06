# Battle System Documentation

## Overview

The battle system provides a turn-based combat interface with the following layout:
- **Player Party**: Positioned in the bottom-left area of the screen
- **Enemy Party**: Positioned in the top-right area of the screen  
- **UI Panel**: Takes up the bottom 1/3 of the screen for actions and information

## Scene Layout

```
┌─────────────────────────────────────────────────────────┐
│                    BATTLE AREA (2/3 screen)            │
│  ┌─────────────────┐    ┌─────────────────┐            │
│  │   PLAYER AREA   │    │   ENEMY AREA    │            │
│  │                 │    │                 │            │
│  │  Hero    ◄──────┼────┤  ►  Goblin      │            │
│  │  Mage           │    │     Orc         │            │
│  │  Warrior        │    │                 │            │
│  └─────────────────┘    └─────────────────┘            │
├─────────────────────────────────────────────────────────┤
│                    UI AREA (1/3 screen)                │
│  Battle Status: Your Turn                              │
│  ┌─────────────────────────────────────────────────────┐│
│  │ > Attack                                            ││
│  │   Defend                                            ││
│  │   Item                                              ││
│  │   Skill                                             ││
│  │   Flee                                              ││
│  └─────────────────────────────────────────────────────┘│
└─────────────────────────────────────────────────────────┘
```

## Core Classes

### Scene_Battle
Main battle scene class that handles:
- Turn management
- Character positioning
- UI rendering
- Input handling
- Battle flow control

### BattleCharacter
Represents a character in battle with:
```cpp
struct BattleCharacter {
    std::string name;
    int currentHP, maxHP;
    int attack, defense, speed;
    bool isPlayer, isAlive;
    Vec2 battlePosition;
    std::shared_ptr<Entity> entity;
};
```

### ActionCommand
Represents a queued battle action:
```cpp
struct ActionCommand {
    BattleCharacter* actor;
    BattleCharacter* target;
    BattleAction action;
    std::string skillName;
    int damage;
    bool executed;
};
```

## Battle States

1. **ENTERING** - Battle intro/transition
2. **PLAYER_TURN** - Player selecting actions
3. **ENEMY_TURN** - AI generating enemy actions
4. **EXECUTING** - Actions being performed
5. **VICTORY** - Player won
6. **DEFEAT** - Player lost
7. **FLEEING** - Player attempting to escape

## Battle Actions

- **ATTACK** - Basic physical attack
- **DEFEND** - Reduce incoming damage
- **ITEM** - Use consumable items
- **SKILL** - Use special abilities
- **FLEE** - Attempt to escape battle

## Usage Examples

### Basic Battle Setup
```cpp
// Create battle scene
auto battleScene = std::make_shared<Scene_Battle>(game);

// Add player characters
battleScene->addPlayerCharacter("Hero", 100, 25, 15, 12);
battleScene->addPlayerCharacter("Mage", 60, 30, 8, 15);

// Add enemies
battleScene->addEnemy("Goblin", 30, 12, 5, 8);
battleScene->addEnemy("Orc", 50, 18, 8, 6);

// Start battle
game->changeScene("BATTLE", battleScene);
```

### Encounter-Based Battle
```cpp
// From encounter zone
std::vector<std::string> enemyTypes = {"Goblin", "Goblin", "Orc"};
auto battleScene = std::make_shared<Scene_Battle>(game, enemyTypes);
game->changeScene("BATTLE", battleScene);
```

## Controls

- **W/S** - Navigate menu up/down
- **A/D** - Navigate menu left/right (for target selection)
- **Enter/Space** - Select/Confirm action
- **Escape** - Back/Cancel

## Character Positioning

### Player Characters
- Positioned in left 50% of battle area
- Arranged vertically with equal spacing
- Face right towards enemies

### Enemy Characters  
- Positioned in right 50% of battle area
- Arranged vertically with equal spacing
- Face left towards players

### Position Calculation
```cpp
// Player positioning
float x = m_playerAreaTopLeft.x + m_playerAreaSize.x * 0.7f; // 70% across left area
float y = m_playerAreaTopLeft.y + (i + 1) * (m_playerAreaSize.y / (partySize + 1));

// Enemy positioning  
float x = m_enemyAreaTopLeft.x + m_enemyAreaSize.x * 0.3f; // 30% across right area
float y = m_enemyAreaTopLeft.y + (i + 1) * (m_enemyAreaSize.y / (partySize + 1));
```

## UI System

### Action Menu
- Displays available actions for current character
- Highlights selected action
- Shows in bottom 1/3 of screen

### Target Selection
- Shows available targets when needed
- Highlights selected target
- Only shows alive enemies for attack actions

### Status Display
- Shows current battle state
- Displays character HP bars
- Shows damage numbers during combat

## Combat Mechanics

### Damage Calculation
```cpp
int baseDamage = std::max(1, attacker.attack - defender.defense / 2);
int finalDamage = baseDamage * randomVariance(0.8, 1.2); // ±20% variance
```

### Turn Order
- Currently simple: Player turn → Enemy turn → Execute actions
- Can be extended to speed-based initiative system

### AI Behavior
- Enemies randomly target alive player characters
- Simple attack-focused AI
- Can be extended with more complex behaviors

## Asset Requirements

### Textures
- Character battle sprites
- Enemy battle sprites  
- Battle backgrounds
- UI panels and elements
- HP bar graphics

### Audio
- Battle music
- Attack sound effects
- Magic/skill sound effects
- Victory/defeat music

### Fonts
- UI text font (defaults to system font if not found)

## Extension Points

### Custom Actions
Add new battle actions by:
1. Adding to `BattleAction` enum
2. Implementing in `executeAction()`
3. Adding to action menu

### Advanced AI
Extend enemy AI by:
1. Adding behavior patterns to enemies
2. Implementing skill usage
3. Adding conditional logic

### Animation System
Add battle animations by:
1. Integrating with existing CAnimation component
2. Adding animation triggers to actions
3. Implementing damage number animations

### Status Effects
Add status effects by:
1. Creating status effect component
2. Modifying damage calculation
3. Adding status icons to UI

## Integration with Main Game

### Triggering Battles
```cpp
// From encounter zones
if (encounterZone->checkForEncounter()) {
    auto encounter = encounterZone->selectRandomEncounter();
    startBattle(encounter.enemyTypes);
}

// From scripted events
if (playerInteractedWithBoss()) {
    startBossBattle("Dragon");
}
```

### Post-Battle Handling
```cpp
void Scene_Battle::onEnd() {
    if (m_battleState == BattleState::VICTORY) {
        // Give rewards
        giveExperience(calculateExp());
        giveGold(calculateGold());
        giveItems(rollForItems());
    }
    
    // Return to previous scene
    m_game->changeScene("PLAY", previousScene);
}
```

## Performance Considerations

- Battle scenes are self-contained
- Entity cleanup handled automatically
- Minimal memory allocation during battle
- Efficient rendering with culling

## Future Enhancements

1. **Animated Sprites** - Character attack/idle animations
2. **Particle Effects** - Magic spells, impact effects
3. **Formation System** - Different battle formations
4. **Multi-target Actions** - Area of effect abilities
5. **Battle Backgrounds** - Dynamic background selection
6. **Save/Load** - Battle state persistence
7. **Replay System** - Battle action replay
8. **Statistics** - Battle performance tracking
