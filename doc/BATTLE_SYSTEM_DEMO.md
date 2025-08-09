# Battle System Demo and Configuration

## Overview
This document describes the battle system demo and configuration files created for the game engine. The system uses a modular, file-based approach to define party members, enemies, and spells.

## Configuration Files Structure

### 1. Party Member Files
Location: `metadata/characters/party_*.txt`

Each party member has their own configuration file:
- `party_hero.txt` - Main protagonist with balanced stats
- `party_mage.txt` - Elemental spellcaster with high MP
- `party_warrior.txt` - Heavy fighter with high defense
- `party_rogue.txt` - Fast attacker with critical hits

**Format:**
```
NAME Character_Name
HP 120
ATTACK 15
DEFENSE 12
SPEED 14
MP 30
SPRITE_TEXTURE texture_name
DESCRIPTION "Character description"

# Growth stats per level
HP_GROWTH 8
ATTACK_GROWTH 2
DEFENSE_GROWTH 2
SPEED_GROWTH 1
MP_GROWTH 3

# Equipment types
WEAPON_TYPE sword
ARMOR_TYPE light
ACCESSORY_TYPE ring

# Known spells
SPELL spell_id_1
SPELL spell_id_2
```

### 2. Enemy Files by Level
Location: `metadata/characters/enemies_level_*.txt`

Enemies are organized by area/level:
- `enemies_level_1.txt` - Starting area enemies (Goblins, Slimes, etc.)
- `enemies_level_2.txt` - Forest area enemies (Wolves, Bandits, etc.)

**Format:**
```
ENEMY_ID ENEMY_KEY
NAME Enemy_Name
HP 25
ATTACK 8
DEFENSE 5
SPEED 12
MP 10
SPRITE_TEXTURE enemy_texture
DESCRIPTION "Enemy description"

# AI and behavior
AI_TYPE aggressive
AI_SPELL_CHANCE 20

# Rewards
EXP_REWARD 15
GOLD_REWARD 8

# Known spells
SPELL spell_id_1
SPELL spell_id_2
END_ENEMY

# Enemy groups for encounters
GROUP_WEAK GOBLIN
GROUP_MEDIUM GOBLIN GOBLIN
GROUP_STRONG GOBLIN GOBLIN SLIME

# Encounter rates
ENCOUNTER_RATE_WEAK 40
ENCOUNTER_RATE_MEDIUM 35
ENCOUNTER_RATE_STRONG 20
```

### 3. Spell Database
Location: `metadata/characters/spells.txt`

Central database of all spells used by both party members and enemies.

**Format:**
```
SPELL_ID spell_key
NAME Spell Name
MP_COST 10
BASE_DAMAGE 25
SPELL_TYPE fire
TARGET_TYPE enemy
DESCRIPTION "Spell description"
END_SPELL
```

## Battle System Components

### 1. BattleConfigLoader Class
- Loads and parses configuration files
- Creates battle characters from config data
- Manages spell database
- Handles level-based enemy loading

**Key Methods:**
- `loadSpells()` - Load spell database
- `loadPartyMember(memberId)` - Load specific party member
- `loadEnemiesForLevel(level)` - Load enemies for specific level
- `createPartyMember(memberId, level)` - Create scaled party member
- `createEnemy(enemyId)` - Create enemy instance
- `createSpell(spellId)` - Create spell instance

### 2. Scene_Battle_Demo Class
- Demonstrates battle system functionality
- Interactive menu for testing different scenarios
- Integration with existing battle scene framework

**Demo Features:**
- Level 1 and Level 2 battle testing
- Party configuration testing
- Spell system validation
- Configuration file validation

## Current Battle System Stats

### Party Members (Level 1)
1. **Hero** - HP:120 ATK:15 DEF:12 SPD:14 MP:30
   - Spells: heal_minor, sword_strike, guard_stance, flame_slash
   
2. **Lyra (Mage)** - HP:80 ATK:10 DEF:8 SPD:12 MP:60
   - Spells: fireball, ice_shard, lightning_bolt, heal_minor, mana_restore, magic_shield, meteor, freeze
   
3. **Gareth (Warrior)** - HP:150 ATK:20 DEF:18 SPD:10 MP:20
   - Spells: power_strike, defensive_stance, berserker_rage, taunt, armor_break
   
4. **Kira (Rogue)** - HP:100 ATK:18 DEF:10 SPD:16 MP:25
   - Spells: backstab, poison_blade, shadow_step, smoke_bomb, critical_strike, steal

### Level 1 Enemies
- **Goblin** - HP:25 ATK:8 DEF:5 SPD:12 (Basic enemy)
- **Slime** - HP:30 ATK:6 DEF:3 SPD:8 (Defensive, can split)
- **Giant Rat** - HP:20 ATK:10 DEF:4 SPD:14 (Fast, hit-and-run)
- **Cave Spider** - HP:22 ATK:12 DEF:6 SPD:16 (Tactical, poison)
- **Goblin Chief** - HP:60 ATK:15 DEF:10 SPD:10 (Mini-boss)

### Level 2 Enemies
- **Forest Wolf** - HP:45 ATK:18 DEF:12 SPD:18 (Pack hunter)
- **Brown Bear** - HP:80 ATK:22 DEF:16 SPD:12 (Berserker)
- **Bandit** - HP:50 ATK:20 DEF:14 SPD:15 (Opportunist)
- **Young Treant** - HP:70 ATK:16 DEF:20 SPD:8 (Guardian, nature magic)
- **Orc Warrior** - HP:65 ATK:24 DEF:18 SPD:14 (Aggressive)
- **Bandit Leader** - HP:120 ATK:28 DEF:20 SPD:16 (Boss)

## Spell System
Total: **41 spells** across different categories:
- **Healing**: heal_minor, mana_restore, absorb, nature_heal
- **Fire**: fireball, flame_slash, meteor
- **Ice**: ice_shard, freeze
- **Lightning**: lightning_bolt
- **Physical**: sword_strike, power_strike, backstab, bite, claw_swipe
- **Buffs**: guard_stance, defensive_stance, berserker_rage, magic_shield
- **Debuffs**: taunt, intimidate, poison_blade, web_trap
- **Utility**: steal, smoke_bomb, shadow_step

## Testing and Validation

### Simple Configuration Test
Run: `g++ -std=c++17 -o simple_battle_test simple_battle_test.cpp && ./simple_battle_test`

This test validates:
- ✓ Spell database loading (41 spells)
- ✓ Party member configurations (4 members)
- ✓ Enemy configurations (5 level 1, 6 level 2)
- ✓ File format parsing
- ✓ Data integrity

### Integration with Game Engine
The battle system integrates with the existing Scene_Battle framework:
- Uses existing BattleCharacter structure
- Compatible with current battle state management
- Extends existing spell system
- Maintains scene transition compatibility

## Usage Examples

### Loading a Party Member
```cpp
BattleConfigLoader loader;
loader.loadSpells();
loader.loadPartyMember("hero");

auto heroLevel1 = loader.createPartyMember("hero", 1);
auto heroLevel3 = loader.createPartyMember("hero", 3); // Scaled stats
```

### Loading Enemies for an Area
```cpp
loader.loadEnemiesForLevel(1);
auto goblin = loader.createEnemy("GOBLIN");
auto boss = loader.createEnemy("GOBLIN_CHIEF");
```

### Creating Spells
```cpp
auto fireball = loader.createSpell("fireball");
// fireball.name = "Fireball"
// fireball.mpCost = 10
// fireball.baseDamage = 25
```

## Future Enhancements
1. **Equipment System**: Weapon/armor modifiers from config files
2. **Status Effects**: Poison, paralysis, buffs/debuffs
3. **Elemental Weaknesses**: Type effectiveness system
4. **Dynamic Encounters**: Procedural enemy group generation
5. **Loot Tables**: Item drops from enemies
6. **Experience Curves**: Non-linear level progression
7. **Skill Trees**: Unlockable spells per character

## File Organization
```
metadata/characters/
├── spells.txt              # Central spell database
├── party_hero.txt          # Hero configuration
├── party_mage.txt          # Mage configuration  
├── party_warrior.txt       # Warrior configuration
├── party_rogue.txt         # Rogue configuration
├── enemies_level_1.txt     # Starting area enemies
└── enemies_level_2.txt     # Forest area enemies
```

The battle system is now ready for integration into the main game engine and can be easily extended with additional party members, enemies, and spells by creating new configuration files.
