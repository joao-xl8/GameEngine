# Project Organization Guide

## 📁 Directory Structure

```
GameEngine/
├── src/
│   ├── entities/                    # Character classes and entity logic
│   │   ├── character.hpp/cpp        # Base character class
│   │   ├── player/                  # Player-specific code
│   │   │   ├── player.hpp/cpp       # Player class
│   │   │   ├── player_config.hpp    # Player configuration
│   │   │   └── player_states.hpp    # Player state management
│   │   ├── npcs/                    # NPC-specific code
│   │   │   ├── npc.hpp/cpp          # Base NPC class
│   │   │   ├── npc_types.hpp        # NPC type definitions
│   │   │   ├── actor_idle.hpp/cpp   # Specific NPC implementations
│   │   │   ├── merchant.hpp/cpp     # Merchant NPC
│   │   │   └── guard.hpp/cpp        # Guard NPC
│   │   └── character_factory.hpp/cpp # Character creation factory
│   ├── components/
│   │   ├── engine_components.hpp    # Core engine components
│   │   ├── common_components.hpp    # Shared components
│   │   ├── player_components.hpp    # Player-specific components
│   │   └── npc_components.hpp       # NPC-specific components
│   └── dialogue/                    # Dialogue system
│       ├── dialogue_manager.hpp/cpp # Dialogue system manager
│       ├── dialogue_parser.hpp/cpp  # Dialogue file parser
│       └── dialogue_types.hpp       # Dialogue data structures
├── assets/
│   ├── characters/                  # Character assets
│   │   ├── player/                  # Player assets
│   │   │   ├── sprites/
│   │   │   │   ├── Player.png       # Player spritesheet
│   │   │   │   ├── Player_attack.png
│   │   │   │   └── Player_special.png
│   │   │   ├── portraits/           # Player portraits for dialogue
│   │   │   │   └── Player_portrait.png
│   │   │   └── sounds/              # Player-specific sounds
│   │   │       ├── footstep.wav
│   │   │       ├── hurt.wav
│   │   │       └── jump.wav
│   │   └── npcs/                    # NPC assets
│   │       ├── actor_idle/
│   │       │   ├── sprites/
│   │       │   │   └── Actor_idle.png
│   │       │   ├── portraits/
│   │       │   │   ├── Actor_neutral.png
│   │       │   │   ├── Actor_happy.png
│   │       │   │   └── Actor_surprised.png
│   │       │   └── sounds/
│   │       │       └── actor_voice.wav
│   │       ├── merchant/
│   │       │   ├── sprites/
│   │       │   │   ├── Merchant_idle.png
│   │       │   │   └── Merchant_working.png
│   │       │   └── portraits/
│   │       │       ├── Merchant_neutral.png
│   │       │       └── Merchant_happy.png
│   │       └── guard/
│   │           ├── sprites/
│   │           │   ├── Guard_idle.png
│   │           │   └── Guard_alert.png
│   │           └── portraits/
│   │               ├── Guard_neutral.png
│   │               └── Guard_stern.png
│   ├── ui/                          # UI assets
│   │   ├── dialogue/
│   │   │   ├── dialogue_box.png
│   │   │   ├── dialogue_arrow.png
│   │   │   └── dialogue_background.png
│   │   └── hud/
│   │       ├── health_bar.png
│   │       └── interaction_prompt.png
│   └── sounds/
│       ├── global/                  # Global sounds
│       │   ├── background_music.ogg
│       │   └── menu_select.wav
│       ├── dialogue/                # Dialogue sounds
│       │   ├── text_beep.wav
│       │   └── dialogue_open.wav
│       └── environment/             # Environment sounds
│           ├── door_open.wav
│           └── chest_open.wav
├── metadata/
│   ├── characters/                  # Character definitions
│   │   ├── player_config.txt        # Player configuration
│   │   └── npc_definitions.txt      # NPC type definitions
│   ├── dialogues/                   # Dialogue files
│   │   ├── npcs/                    # NPC dialogues
│   │   │   ├── actor_idle/
│   │   │   │   ├── greeting.txt
│   │   │   │   ├── quest_intro.txt
│   │   │   │   └── quest_complete.txt
│   │   │   ├── merchant/
│   │   │   │   ├── shop_intro.txt
│   │   │   │   ├── buy_dialogue.txt
│   │   │   │   └── sell_dialogue.txt
│   │   │   └── guard/
│   │   │       ├── patrol_chat.txt
│   │   │       └── warning.txt
│   │   ├── system/                  # System dialogues
│   │   │   ├── tutorial.txt
│   │   │   └── game_over.txt
│   │   └── cutscenes/               # Cutscene dialogues
│   │       ├── intro.txt
│   │       └── ending.txt
│   ├── levels/                      # Level files
│   │   ├── level1.txt
│   │   └── level2.txt
│   └── assets.txt                   # Asset definitions
└── config/                          # Configuration files
    ├── character_animations.json    # Animation definitions
    ├── dialogue_config.json         # Dialogue system config
    └── npc_behaviors.json           # NPC behavior definitions
```

## 🎯 **Asset Organization Strategy**

### **1. Character Assets**
- **Sprites**: Organized by character type and animation purpose
- **Portraits**: Multiple expressions per character for dialogue
- **Sounds**: Character-specific audio (footsteps, voices, etc.)

### **2. Dialogue Assets**
- **Hierarchical**: Organized by character and dialogue type
- **Modular**: Each conversation is a separate file
- **Contextual**: Different dialogues for different game states

### **3. Configuration Files**
- **JSON Format**: For complex configurations
- **TXT Format**: For simple lists and level data
- **Separation**: Different aspects in different files

## 🔧 **Implementation Strategy**

### **Phase 1: Immediate Improvements**
1. Reorganize existing assets into character folders
2. Create NPC-specific component files
3. Implement character factory pattern
4. Create dialogue organization system

### **Phase 2: Advanced Features**
1. JSON-based configuration system
2. Dynamic asset loading
3. Character behavior trees
4. Advanced dialogue system with branching

### **Phase 3: Scalability**
1. Asset streaming system
2. Modular character system
3. Plugin-based NPC behaviors
4. Localization support

## 📋 **File Naming Conventions**

### **Assets**
- **Sprites**: `CharacterName_AnimationType.png` (e.g., `Player_walk.png`)
- **Portraits**: `CharacterName_Expression.png` (e.g., `Actor_happy.png`)
- **Sounds**: `character_action.wav` (e.g., `player_footstep.wav`)

### **Code Files**
- **Classes**: `snake_case.hpp/cpp` (e.g., `actor_idle.cpp`)
- **Components**: `ComponentName.hpp` (e.g., `PlayerMovement.hpp`)
- **Managers**: `system_manager.hpp` (e.g., `dialogue_manager.hpp`)

### **Data Files**
- **Dialogues**: `character_context.txt` (e.g., `actor_greeting.txt`)
- **Configs**: `system_config.json` (e.g., `character_animations.json`)
- **Levels**: `levelN.txt` (e.g., `level1.txt`)

## 🎮 **Character Component Organization**

### **Player Components**
```cpp
// player_components.hpp
class CPlayerMovement : public Component { /* Grid-based movement */ };
class CPlayerInput : public Component { /* Input handling */ };
class CPlayerStats : public Component { /* Health, XP, etc. */ };
class CPlayerInventory : public Component { /* Items, equipment */ };
```

### **NPC Components**
```cpp
// npc_components.hpp
class CNPCBehavior : public Component { /* AI behavior */ };
class CNPCDialogue : public Component { /* Dialogue state */ };
class CNPCInteraction : public Component { /* Interaction rules */ };
class CNPCSchedule : public Component { /* Daily routines */ };
```

## 🗣️ **Dialogue System Organization**

### **Dialogue File Format**
```
# actor_greeting.txt
[CHARACTER]
name=Actor
portrait_base=Actor

[DIALOGUE]
id=greeting_001
speaker=Actor
portrait=neutral
text=Hello there, traveler! Welcome to our village.
next=greeting_002

[DIALOGUE]
id=greeting_002
speaker=Actor
portrait=happy
text=Is there anything I can help you with today?
choices=help_yes,help_no,goodbye

[CHOICE]
id=help_yes
text=Yes, I need some information.
next=info_start

[CHOICE]
id=help_no
text=No, just looking around.
next=casual_chat

[CHOICE]
id=goodbye
text=I have to go now.
next=farewell
```

### **Dialogue Manager Integration**
```cpp
class DialogueManager {
    void loadDialogue(const std::string& characterName, const std::string& context);
    void startDialogue(const std::string& dialogueId);
    void processChoice(int choiceIndex);
    bool isDialogueActive() const;
};
```

## 🚀 **Benefits of This Organization**

1. **Scalability**: Easy to add new characters and dialogues
2. **Maintainability**: Clear separation of concerns
3. **Team Collaboration**: Different team members can work on different areas
4. **Asset Management**: Easy to find and update specific assets
5. **Localization Ready**: Structure supports multiple languages
6. **Performance**: Efficient asset loading and management
7. **Modularity**: Components can be mixed and matched

## 📝 **Next Steps**

1. **Immediate**: Reorganize current assets into character folders
2. **Short-term**: Implement character factory and component separation
3. **Medium-term**: Create JSON-based configuration system
4. **Long-term**: Implement advanced dialogue and behavior systems

This organization will make your game much more maintainable and allow for easy expansion as you add more characters, dialogues, and features.
