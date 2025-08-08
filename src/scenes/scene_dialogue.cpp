#include "scene_dialogue.hpp"
#include "scene_menu.hpp"
#include "scene_play.hpp"
#include "../game_engine.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

Scene_Dialogue::Scene_Dialogue(GameEngine* game, const std::string& dialogueFile)
    : Scene(game)
{
    std::cout << "DEBUG: Scene_Dialogue constructor called" << std::endl;
    loadDialogueConfig(dialogueFile);
    std::cout << "DEBUG: Scene_Dialogue constructor completed" << std::endl;
}

void Scene_Dialogue::init()
{
    // Register input actions
    registerAction(sf::Keyboard::Space, "ADVANCE");
    registerAction(sf::Keyboard::Enter, "ADVANCE");
    registerAction(sf::Keyboard::Escape, "BACK");
    
    // Initialize sound manager
    m_soundManager = std::make_shared<CSound>();
    
    // Load dialogue sounds
    if (!m_dialogueConfig.textSound.empty()) {
        m_soundManager->addSound("text", "assets/sounds/" + m_dialogueConfig.textSound);
    }
    if (!m_dialogueConfig.backgroundSound.empty()) {
        m_soundManager->addSound("background", "assets/sounds/" + m_dialogueConfig.backgroundSound);
        m_soundManager->playSound("background", 30.0f); // Low volume background
    }
    
    setupUI();
    displayCurrentLine();
    
    std::cout << "Dialogue scene initialized with " << m_dialogueConfig.lines.size() << " lines" << std::endl;
}

void Scene_Dialogue::loadDialogueConfig(const std::string& dialogueFile)
{
    std::ifstream file(dialogueFile);
    if (!file.is_open()) {
        std::cerr << "Failed to open dialogue file: " << dialogueFile << std::endl;
        return;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        
        std::istringstream iss(line);
        std::string command;
        iss >> command;
        
        if (command == "BACKGROUND_IMAGE") {
            iss >> m_dialogueConfig.backgroundImage;
        }
        else if (command == "BACKGROUND_SOUND") {
            iss >> m_dialogueConfig.backgroundSound;
        }
        else if (command == "TEXT_SOUND") {
            iss >> m_dialogueConfig.textSound;
        }
        else if (command == "LINE") {
            DialogueLine dialogueLine;
            iss >> dialogueLine.actor >> dialogueLine.portrait;
            
            // Read the rest of the line as text (may contain spaces)
            std::string remainingLine;
            std::getline(iss, remainingLine);
            dialogueLine.text = remainingLine.substr(1); // Remove leading space
            
            m_dialogueConfig.lines.push_back(dialogueLine);
        }
    }
    
    file.close();
    std::cout << "Loaded dialogue config: " << m_dialogueConfig.lines.size() << " lines" << std::endl;
}

void Scene_Dialogue::setupUI()
{
    // Get window size directly for simpler positioning
    sf::Vector2u windowSize = m_game->window().getSize();
    float windowWidth = static_cast<float>(windowSize.x);
    float windowHeight = static_cast<float>(windowSize.y);
    
    // Setup dialogue box (positioned in lower part of screen)
    float dialogueBoxY = windowHeight - DIALOGUE_BOX_HEIGHT - BOX_MARGIN;
    m_dialogueBox.setSize(sf::Vector2f(windowWidth - 2 * BOX_MARGIN, DIALOGUE_BOX_HEIGHT));
    m_dialogueBox.setPosition(BOX_MARGIN, dialogueBoxY);
    m_dialogueBox.setFillColor(sf::Color(0, 0, 0, 200)); // Semi-transparent black
    m_dialogueBox.setOutlineColor(sf::Color::White);
    m_dialogueBox.setOutlineThickness(2.0f);
    
    // Setup portrait frames (above dialogue box)
    float portraitY = dialogueBoxY - PORTRAIT_SIZE - 10.0f;
    
    // Left portrait frame
    m_leftPortraitFrame.setSize(sf::Vector2f(PORTRAIT_SIZE, PORTRAIT_SIZE));
    m_leftPortraitFrame.setPosition(BOX_MARGIN + 20.0f, portraitY);
    m_leftPortraitFrame.setFillColor(sf::Color(50, 50, 50, 200));
    m_leftPortraitFrame.setOutlineColor(sf::Color::White);
    m_leftPortraitFrame.setOutlineThickness(2.0f);
    
    // Right portrait frame
    m_rightPortraitFrame.setSize(sf::Vector2f(PORTRAIT_SIZE, PORTRAIT_SIZE));
    m_rightPortraitFrame.setPosition(windowWidth - PORTRAIT_SIZE - BOX_MARGIN - 20.0f, portraitY);
    m_rightPortraitFrame.setFillColor(sf::Color(50, 50, 50, 200));
    m_rightPortraitFrame.setOutlineColor(sf::Color::White);
    m_rightPortraitFrame.setOutlineThickness(2.0f);
    
    // Setup text elements
    try {
        // Dialogue text
        m_dialogueText.setFont(m_game->getAssets().getFont("ShareTech"));
        m_dialogueText.setCharacterSize(16);
        m_dialogueText.setFillColor(sf::Color::White);
        m_dialogueText.setPosition(m_dialogueBox.getPosition().x + TEXT_PADDING, 
                                  m_dialogueBox.getPosition().y + TEXT_PADDING + 25.0f);
        
        // Actor name text
        m_actorNameText.setFont(m_game->getAssets().getFont("ShareTech"));
        m_actorNameText.setCharacterSize(18);
        m_actorNameText.setFillColor(sf::Color::Yellow);
        m_actorNameText.setStyle(sf::Text::Bold);
        m_actorNameText.setPosition(m_dialogueBox.getPosition().x + TEXT_PADDING, 
                                   m_dialogueBox.getPosition().y + TEXT_PADDING);
        
        // Instruction text
        m_instructionText.setFont(m_game->getAssets().getFont("ShareTech"));
        m_instructionText.setCharacterSize(14);
        m_instructionText.setFillColor(sf::Color(200, 200, 200));
        m_instructionText.setString("Press SPACE or ENTER to continue, ESC to exit");
        m_instructionText.setPosition(m_dialogueBox.getPosition().x + TEXT_PADDING, 
                                    m_dialogueBox.getPosition().y + DIALOGUE_BOX_HEIGHT - 25.0f);
    } catch (const std::exception& e) {
        std::cout << "Warning: Could not set font for dialogue text: " << e.what() << std::endl;
    }
    
    // Load background image if specified
    if (!m_dialogueConfig.backgroundImage.empty()) {
        try {
            auto& bgTexture = m_game->getAssets().getTexture("DialogueBackground");
            m_backgroundSprite.setTexture(bgTexture);
            
            // Scale background to fit window
            sf::Vector2u bgSize = bgTexture.getSize();
            float scaleX = windowWidth / static_cast<float>(bgSize.x);
            float scaleY = windowHeight / static_cast<float>(bgSize.y);
            m_backgroundSprite.setScale(scaleX, scaleY);
            m_backgroundSprite.setPosition(0, 0);
        } catch (const std::exception& e) {
            std::cout << "Warning: Could not load dialogue background: " << e.what() << std::endl;
        }
    }
    
    std::cout << "Dialogue UI setup complete. Window size: (" << windowWidth << ", " << windowHeight << ")" << std::endl;
    std::cout << "Dialogue box position: (" << m_dialogueBox.getPosition().x << ", " << m_dialogueBox.getPosition().y << ")" << std::endl;
}

void Scene_Dialogue::displayCurrentLine()
{
    if (m_currentLineIndex >= m_dialogueConfig.lines.size()) {
        m_dialogueComplete = true;
        return;
    }
    
    const DialogueLine& currentLine = m_dialogueConfig.lines[m_currentLineIndex];
    
    // Set actor name (position already set in setupUI)
    m_actorNameText.setString(currentLine.actor + ":");
    
    // Setup typewriter effect
    m_fullText = currentLine.text;
    m_displayedText = "";
    m_typewriterTimer = 0.0f;
    m_textComplete = false;
    
    // Determine which side to show portrait based on actor
    // Toriel on left, Ace on right (you can customize this logic)
    bool useLeftSide = (currentLine.actor == "Toriel");
    
    // Clear both portraits first
    m_leftPortrait.setTexture(sf::Texture{});
    m_rightPortrait.setTexture(sf::Texture{});
    
    // Load appropriate portrait
    loadPortrait(currentLine.actor, currentLine.portrait, useLeftSide);
    
    // Highlight the active portrait frame
    if (useLeftSide) {
        m_leftPortraitFrame.setOutlineColor(sf::Color::Yellow);
        m_rightPortraitFrame.setOutlineColor(sf::Color::White);
    } else {
        m_leftPortraitFrame.setOutlineColor(sf::Color::White);
        m_rightPortraitFrame.setOutlineColor(sf::Color::Yellow);
    }
    
    std::cout << "Displaying line " << (m_currentLineIndex + 1) << "/" << m_dialogueConfig.lines.size() 
              << ": " << currentLine.actor << " - " << currentLine.text << std::endl;
}

void Scene_Dialogue::loadPortrait(const std::string& actor, const std::string& portrait, bool isLeft)
{
    try {
        // Convert portrait name to asset name format
        std::string assetName = actor + "_" + portrait;
        
        // Try to get the texture
        auto& portraitTexture = m_game->getAssets().getTexture(assetName);
        
        if (isLeft) {
            m_leftPortrait.setTexture(portraitTexture);
            m_leftPortrait.setPosition(m_leftPortraitFrame.getPosition());
            
            // Scale portrait to fit frame
            sf::Vector2u textureSize = portraitTexture.getSize();
            float scaleX = PORTRAIT_SIZE / static_cast<float>(textureSize.x);
            float scaleY = PORTRAIT_SIZE / static_cast<float>(textureSize.y);
            float scale = std::min(scaleX, scaleY);
            m_leftPortrait.setScale(scale, scale);
            
            // Center the portrait in the frame
            sf::FloatRect bounds = m_leftPortrait.getLocalBounds();
            float offsetX = (PORTRAIT_SIZE - bounds.width * scale) / 2.0f;
            float offsetY = (PORTRAIT_SIZE - bounds.height * scale) / 2.0f;
            m_leftPortrait.setPosition(m_leftPortraitFrame.getPosition().x + offsetX, 
                                     m_leftPortraitFrame.getPosition().y + offsetY);
        } else {
            m_rightPortrait.setTexture(portraitTexture);
            m_rightPortrait.setPosition(m_rightPortraitFrame.getPosition());
            
            // Scale portrait to fit frame
            sf::Vector2u textureSize = portraitTexture.getSize();
            float scaleX = PORTRAIT_SIZE / static_cast<float>(textureSize.x);
            float scaleY = PORTRAIT_SIZE / static_cast<float>(textureSize.y);
            float scale = std::min(scaleX, scaleY);
            m_rightPortrait.setScale(scale, scale);
            
            // Center the portrait in the frame
            sf::FloatRect bounds = m_rightPortrait.getLocalBounds();
            float offsetX = (PORTRAIT_SIZE - bounds.width * scale) / 2.0f;
            float offsetY = (PORTRAIT_SIZE - bounds.height * scale) / 2.0f;
            m_rightPortrait.setPosition(m_rightPortraitFrame.getPosition().x + offsetX, 
                                      m_rightPortraitFrame.getPosition().y + offsetY);
        }
        
        std::cout << "Loaded portrait: " << assetName << (isLeft ? " (left)" : " (right)") << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "Warning: Could not load portrait " << actor << "_" << portrait << ": " << e.what() << std::endl;
    }
}

std::string Scene_Dialogue::getPortraitPath(const std::string& actor, const std::string& portrait)
{
    // Convert portrait name format (e.g., "Normal1" or "Side_Eye1")
    std::string fileName = portrait;
    
    // Replace underscores with spaces for file names
    size_t pos = 0;
    while ((pos = fileName.find('_', pos)) != std::string::npos) {
        fileName.replace(pos, 1, " ");
        pos += 1;
    }
    
    return "assets/imgs/dialogue_portraits/" + actor + "/" + fileName + ".png";
}

void Scene_Dialogue::updateTypewriter()
{
    if (m_textComplete) return;
    
    m_typewriterTimer += m_game->getDeltaTime();
    
    if (m_typewriterTimer >= m_typewriterSpeed) {
        if (m_displayedText.length() < m_fullText.length()) {
            m_displayedText += m_fullText[m_displayedText.length()];
            m_dialogueText.setString(m_displayedText);
            
            // Play text sound for each character
            playTextSound();
            
            m_typewriterTimer = 0.0f;
        } else {
            m_textComplete = true;
        }
    }
}

void Scene_Dialogue::playTextSound()
{
    if (m_soundManager) {
        m_soundManager->playSound("text", 20.0f); // Low volume text sound
    }
}

void Scene_Dialogue::nextLine()
{
    if (!m_textComplete) {
        // Skip typewriter effect and show full text
        m_displayedText = m_fullText;
        m_dialogueText.setString(m_displayedText);
        m_textComplete = true;
        return;
    }
    
    m_currentLineIndex++;
    if (m_currentLineIndex >= m_dialogueConfig.lines.size()) {
        m_dialogueComplete = true;
        std::cout << "Dialogue complete!" << std::endl;
    } else {
        displayCurrentLine();
    }
}

void Scene_Dialogue::update()
{
    // Update input delay timer using proper delta time
    if (m_inputTimer < m_inputDelay) {
        m_inputTimer += m_game->getDeltaTime();
    }
    
    updateTypewriter();
    
    if (m_dialogueComplete) {
        // Return to play scene when dialogue is complete
        m_game->changeScene("Play", std::make_shared<Scene_Play>(m_game, "metadata/levels/level1.txt"));
    }
    
    // Render the dialogue scene
    sRender();
}

void Scene_Dialogue::sDoAction(const Action& action)
{
    // Ignore input during the initial delay period
    if (m_inputTimer < m_inputDelay) {
        return;
    }
              
    if (action.getType() == "START") {
        if (action.getName() == "ADVANCE") {
            nextLine();
        } else if (action.getName() == "BACK") {
            // Return to play scene when user presses ESC
            m_game->changeScene("Play", std::make_shared<Scene_Play>(m_game, "metadata/levels/level1.txt"));
        }
    }
}

void Scene_Dialogue::sRender()
{
    // Use default view for simpler positioning
    m_game->window().setView(m_game->window().getDefaultView());
    
    // Get window size for background
    sf::Vector2u windowSize = m_game->window().getSize();
    float windowWidth = static_cast<float>(windowSize.x);
    float windowHeight = static_cast<float>(windowSize.y);
    
    // Draw background
    sf::RectangleShape background;
    background.setSize(sf::Vector2f(windowWidth, windowHeight));
    background.setPosition(0, 0);
    background.setFillColor(sf::Color(15, 15, 30)); // Dark blue background
    m_game->window().draw(background);
    
    // Draw custom background image if available
    if (m_backgroundSprite.getTexture()) {
        m_game->window().draw(m_backgroundSprite);
    }
    
    // Draw portrait frames
    m_game->window().draw(m_leftPortraitFrame);
    m_game->window().draw(m_rightPortraitFrame);
    
    // Draw portraits
    if (m_leftPortrait.getTexture()) {
        m_game->window().draw(m_leftPortrait);
    }
    if (m_rightPortrait.getTexture()) {
        m_game->window().draw(m_rightPortrait);
    }
    
    // Draw dialogue box
    m_game->window().draw(m_dialogueBox);
    
    // Draw text elements
    m_game->window().draw(m_actorNameText);
    m_game->window().draw(m_dialogueText);
    m_game->window().draw(m_instructionText);
}

void Scene_Dialogue::onEnd()
{
    // Stop background sound
    if (m_soundManager) {
        m_soundManager->stopSound("background");
    }
    
    std::cout << "Dialogue scene ended" << std::endl;
}
