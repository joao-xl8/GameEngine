#pragma once
#include "scene.hpp"
#include "../components/engine_components.hpp"
#include <vector>
#include <string>

struct DialogueLine {
    std::string actor;
    std::string portrait;
    std::string text;
};

struct DialogueConfig {
    std::string backgroundImage;
    std::string backgroundSound;
    std::string textSound;
    std::vector<DialogueLine> lines;
};

class Scene_Dialogue : public Scene
{
protected:
    DialogueConfig m_dialogueConfig;
    size_t m_currentLineIndex = 0;
    bool m_dialogueComplete = false;
    
    // UI Elements
    sf::RectangleShape m_dialogueBox;
    sf::RectangleShape m_leftPortraitFrame;
    sf::RectangleShape m_rightPortraitFrame;
    sf::Sprite m_leftPortrait;
    sf::Sprite m_rightPortrait;
    sf::Sprite m_backgroundSprite;
    sf::Text m_dialogueText;
    sf::Text m_actorNameText;
    sf::Text m_instructionText;
    
    // Layout constants
    static constexpr float DIALOGUE_BOX_HEIGHT = 150.0f;
    static constexpr float PORTRAIT_SIZE = 120.0f;
    static constexpr float BOX_MARGIN = 10.0f;
    static constexpr float TEXT_PADDING = 15.0f;
    
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

public:
    Scene_Dialogue(GameEngine* game, const std::string& dialogueFile);
    void init();
    void update();
    void sDoAction(const Action& action);
    void sRender();
    void onEnd();
    
private:
    void loadDialogueConfig(const std::string& dialogueFile);
    void setupUI();
    void displayCurrentLine();
    void nextLine();
    void updateTypewriter();
    void loadPortrait(const std::string& actor, const std::string& portrait, bool isLeft);
    std::string getPortraitPath(const std::string& actor, const std::string& portrait);
    void playTextSound();
};
