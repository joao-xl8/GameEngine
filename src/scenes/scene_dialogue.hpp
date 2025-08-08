#pragma once
#include "scene.hpp"
#include "../components/engine_components.hpp"
#include "../ui/command_overlay.hpp"
#include <vector>
#include <string>
#include <map>

struct DialogueLine {
    std::string actor;
    std::string portrait;
    std::string text;
    std::string type = "LINE"; // LINE, CHOICE, LABEL, JUMP
    std::string jumpTarget = ""; // For CHOICE and JUMP
};

struct DialogueLogEntry {
    enum Type { LINE, CHOICE_SELECTION };
    Type type;
    std::string actor;
    std::string text;
    std::vector<std::string> availableChoices; // For choice entries
    int selectedChoiceIndex; // For choice entries
    
    // Default constructor
    DialogueLogEntry() : type(LINE), selectedChoiceIndex(-1) {}
    
    // Constructor for dialogue lines
    DialogueLogEntry(const std::string& a, const std::string& t) 
        : type(LINE), actor(a), text(t), selectedChoiceIndex(-1) {}
    
    // Constructor for choice selections
    DialogueLogEntry(const std::vector<std::string>& choices, int selected)
        : type(CHOICE_SELECTION), actor("Player"), selectedChoiceIndex(selected) {
        availableChoices = choices;
        if (selected >= 0 && selected < static_cast<int>(choices.size())) {
            text = "Selected: " + choices[selected];
        } else {
            text = "Invalid selection";
        }
    }
};

struct DialogueChoice {
    std::string text;
    std::string jumpTarget;
};

struct DialogueConfig {
    std::string backgroundImage;
    std::string backgroundSound;
    std::string textSound;
    std::vector<DialogueLine> lines;
    std::map<std::string, size_t> labels; // label_name -> line_index
    std::vector<std::string> requiredAssets; // All assets needed for this dialogue
    std::map<std::string, std::pair<bool, int>> portraitAssignments; // actor -> (isLeft, slotIndex)
};

class Scene_Dialogue : public Scene
{
protected:
    DialogueConfig m_dialogueConfig;
    size_t m_currentLineIndex = 0;
    bool m_dialogueComplete = false;
    
    // Choice system
    std::vector<DialogueChoice> m_currentChoices;
    int m_selectedChoice = 0;
    bool m_showingChoices = false;
    
    // UI Elements - Multiple portraits support
    sf::RectangleShape m_dialogueBox;
    
    // Left side portraits (up to 3)
    std::vector<sf::RectangleShape> m_leftPortraitFrames;
    std::vector<sf::Sprite> m_leftPortraits;
    
    // Right side portraits (up to 3)  
    std::vector<sf::RectangleShape> m_rightPortraitFrames;
    std::vector<sf::Sprite> m_rightPortraits;
    
    sf::Sprite m_backgroundSprite;
    sf::Text m_dialogueText;
    sf::Text m_actorNameText;
    sf::Text m_instructionText;
    
    // Choice UI
    std::vector<sf::Text> m_choiceTexts;
    sf::RectangleShape m_choiceBox;
    
    // Dialogue log system
    std::vector<DialogueLogEntry> m_dialogueLog;
    bool m_showingLog = false;
    int m_logScrollOffset = 0;
    sf::RectangleShape m_logBackground;
    sf::Text m_logTitle;
    std::vector<sf::Text> m_logTexts;
    static const int MAX_LOG_LINES_VISIBLE = 20;
    
    // Layout constants
    static constexpr float DIALOGUE_BOX_HEIGHT = 150.0f;
    static constexpr float PORTRAIT_SIZE = 80.0f; // Smaller to fit 3 portraits
    static constexpr float PORTRAIT_SPACING = 10.0f;
    static constexpr float BOX_MARGIN = 10.0f;
    static constexpr float TEXT_PADDING = 15.0f;
    static constexpr int MAX_PORTRAITS_PER_SIDE = 3;
    
    // Typewriter effect
    std::string m_fullText;
    std::string m_displayedText;
    float m_typewriterTimer = 0.0f;
    float m_typewriterSpeed = 0.03f; // seconds per character
    bool m_textComplete = false;
    
    // Input delay to prevent immediate input from menu selection
    float m_inputDelay = 0.3f; // Short delay to prevent accidental input
    float m_inputTimer = 0.0f;
    
    // Sound management
    std::shared_ptr<CSound> m_soundManager;
    
    // Preserved game state for returning to play scene
    std::string m_originalLevel;
    Vec2 m_originalPlayerPosition;
    int m_originalPlayerHealth;
    int m_originalPlayTime;
    bool m_hasPreservedState = false;

    void loadDialogueConfig(const std::string& dialogueFile);
    void setupUI();
    void displayCurrentLine();
    void loadPortrait(const std::string& actor, const std::string& portrait, bool useLeftSide, int slotIndex = 0);
    void updateTypewriter();
    void playTextSound();
    void nextLine();
    void jumpToLabel(const std::string& label);
    void showChoices(const std::vector<DialogueChoice>& choices);
    void selectChoice(int choiceIndex);
    void processCurrentLine();
    
    // Dialogue log system
    void addToLog(const std::string& actor, const std::string& text);
    void addChoiceToLog(const std::vector<DialogueChoice>& choices, int selectedIndex);
    void showDialogueLog();
    void hideDialogueLog();
    void scrollLog(int direction);
    void setupLogUI();
    void renderDialogueLog();
    
    // Portrait management
    void clearAllPortraits();
    void highlightActivePortrait(const std::string& actor);
    int findPortraitSlot(const std::string& actor, bool leftSide);

    void sRender();
    void sDoAction(const Action& action);

public:
    Scene_Dialogue(GameEngine* game, const std::string& dialogueFile);
    Scene_Dialogue(GameEngine* game, const std::string& dialogueFile, 
                   const std::string& originalLevel, const Vec2& playerPos, 
                   int playerHealth, int playTime);
    void init();
    void update();
    void onEnd();
};
