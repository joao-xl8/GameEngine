#include "scene_dialogue.hpp"
#include "scene_menu.hpp"
#include "scene_play.hpp"
#include "../game_engine.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

Scene_Dialogue::Scene_Dialogue(GameEngine* game, const std::string& dialogueFile)
    : Scene(game), m_showingLog(false), m_logScrollOffset(0)
{
    std::cout << "DEBUG: Scene_Dialogue constructor called" << std::endl;
    
    loadDialogueConfig(dialogueFile);
    
    // Calculate maximum possible log entries and reserve space to avoid mallocs
    size_t maxLogEntries = 0;
    for (const auto& line : m_dialogueConfig.lines) {
        if (line.type == "LINE") {
            maxLogEntries++; // Each dialogue line creates one log entry
        } else if (line.type == "CHOICES") {
            maxLogEntries++; // Each choice selection creates one log entry
        }
    }
    
    // Reserve space for the entire conversation to avoid any runtime allocations
    m_dialogueLog.reserve(maxLogEntries);
    std::cout << "Reserved space for " << maxLogEntries << " dialogue log entries" << std::endl;
    
    std::cout << "DEBUG: Scene_Dialogue constructor completed" << std::endl;
}

Scene_Dialogue::Scene_Dialogue(GameEngine* game, const std::string& dialogueFile, 
                               const std::string& originalLevel, const Vec2& playerPos, 
                               int playerHealth, int playTime)
    : Scene(game), m_showingLog(false), m_logScrollOffset(0),
      m_originalLevel(originalLevel), m_originalPlayerPosition(playerPos),
      m_originalPlayerHealth(playerHealth), m_originalPlayTime(playTime),
      m_hasPreservedState(true)
{
    std::cout << "DEBUG: Scene_Dialogue constructor with preserved state called" << std::endl;
    std::cout << "Preserved state - Level: " << originalLevel << ", Position: (" 
              << playerPos.x << ", " << playerPos.y << "), Health: " << playerHealth << std::endl;
    
    loadDialogueConfig(dialogueFile);
    
    // Calculate maximum possible log entries and reserve space to avoid mallocs
    size_t maxLogEntries = 0;
    for (const auto& line : m_dialogueConfig.lines) {
        if (line.type == "LINE") {
            maxLogEntries++; // Each dialogue line creates one log entry
        } else if (line.type == "CHOICES") {
            maxLogEntries++; // Each choice selection creates one log entry
        }
    }
    
    // Reserve space for the entire conversation to avoid any runtime allocations
    m_dialogueLog.reserve(maxLogEntries);
    std::cout << "Reserved space for " << maxLogEntries << " dialogue log entries" << std::endl;
    
    std::cout << "DEBUG: Scene_Dialogue constructor with preserved state completed" << std::endl;
}

void Scene_Dialogue::init()
{
    // Register ALL input actions for dialogue once
    registerAction(sf::Keyboard::Space, "CONFIRM");
    registerAction(sf::Keyboard::C, "BACK");
    
    // Register choice navigation keys (always available)
    registerAction(sf::Keyboard::Up, "CHOICE_UP");
    registerAction(sf::Keyboard::Down, "CHOICE_DOWN");
    registerAction(sf::Keyboard::W, "CHOICE_UP");
    registerAction(sf::Keyboard::S, "CHOICE_DOWN");
    
    // Register dialogue log key
    registerAction(sf::Keyboard::B, "SHOW_LOG");
    
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
    processCurrentLine();
    
    std::cout << "Dialogue scene initialized with " << m_dialogueConfig.lines.size() << " lines" << std::endl;
}

void Scene_Dialogue::loadDialogueConfig(const std::string& dialogueFile)
{
    // Check if file exists before trying to open it
    std::ifstream testFile(dialogueFile);
    if (!testFile.good()) {
        std::cerr << "ERROR: Dialogue file does not exist or cannot be accessed: " << dialogueFile << std::endl;
        // Initialize with empty config to prevent crashes
        m_dialogueConfig = DialogueConfig{};
        m_dialogueComplete = true;
        return;
    }
    testFile.close();
    
    std::ifstream file(dialogueFile);
    if (!file.is_open()) {
        std::cerr << "Failed to open dialogue file: " << dialogueFile << std::endl;
        m_dialogueConfig = DialogueConfig{};
        m_dialogueComplete = true;
        return;
    }
    
    std::string line;
    bool inAssetsSection = false;
    std::vector<DialogueChoice> pendingChoices;
    
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        
        std::istringstream iss(line);
        std::string command;
        iss >> command;
        
        if (command == "ASSETS_START") {
            inAssetsSection = true;
            continue;
        } else if (command == "ASSETS_END") {
            inAssetsSection = false;
            continue;
        } else if (inAssetsSection) {
            m_dialogueConfig.requiredAssets.push_back(line);
            continue;
        }
        
        if (command == "BACKGROUND_IMAGE") {
            iss >> m_dialogueConfig.backgroundImage;
        } else if (command == "BACKGROUND_SOUND") {
            iss >> m_dialogueConfig.backgroundSound;
        } else if (command == "TEXT_SOUND") {
            iss >> m_dialogueConfig.textSound;
        } else if (command == "PORTRAIT_ASSIGNMENT") {
            std::string actor, side;
            int slot;
            iss >> actor >> side >> slot;
            bool isLeft = (side == "LEFT");
            m_dialogueConfig.portraitAssignments[actor] = std::make_pair(isLeft, slot);
        } else if (command == "LABEL") {
            // If we have pending choices, add them first before processing the label
            if (!pendingChoices.empty()) {
                DialogueLine choicesLine;
                choicesLine.type = "CHOICES";
                // Store choices in a simple format: "text1|target1|text2|target2"
                for (size_t i = 0; i < pendingChoices.size(); i++) {
                    if (i > 0) choicesLine.text += "|";
                    choicesLine.text += pendingChoices[i].text + "|" + pendingChoices[i].jumpTarget;
                }
                m_dialogueConfig.lines.push_back(choicesLine);
                pendingChoices.clear();
            }
            
            std::string labelName;
            iss >> labelName;
            m_dialogueConfig.labels[labelName] = m_dialogueConfig.lines.size();
        } else if (command == "LINE") {
            DialogueLine dialogueLine;
            dialogueLine.type = "LINE";
            iss >> dialogueLine.actor >> dialogueLine.portrait;
            
            std::string remainingLine;
            std::getline(iss, remainingLine);
            dialogueLine.text = remainingLine.substr(1); // Remove leading space
            
            m_dialogueConfig.lines.push_back(dialogueLine);
        } else if (command == "CHOICE") {
            // Parse: CHOICE "Option Text" label_to_jump
            size_t firstQuote = line.find('"');
            size_t secondQuote = line.find('"', firstQuote + 1);
            if (firstQuote != std::string::npos && secondQuote != std::string::npos) {
                std::string choiceText = line.substr(firstQuote + 1, secondQuote - firstQuote - 1);
                std::string remaining = line.substr(secondQuote + 1);
                std::istringstream remainingStream(remaining);
                std::string jumpTarget;
                remainingStream >> jumpTarget;
                
                DialogueChoice choice;
                choice.text = choiceText;
                choice.jumpTarget = jumpTarget;
                pendingChoices.push_back(choice);
            }
        } else if (command == "JUMP") {
            std::string jumpTarget;
            iss >> jumpTarget;
            
            DialogueLine jumpLine;
            jumpLine.type = "JUMP";
            jumpLine.jumpTarget = jumpTarget;
            m_dialogueConfig.lines.push_back(jumpLine);
        }
    }
    
    // Handle any remaining choices at the end
    if (!pendingChoices.empty()) {
        DialogueLine choicesLine;
        choicesLine.type = "CHOICES";
        for (size_t i = 0; i < pendingChoices.size(); i++) {
            if (i > 0) choicesLine.text += "|";
            choicesLine.text += pendingChoices[i].text + "|" + pendingChoices[i].jumpTarget;
        }
        m_dialogueConfig.lines.push_back(choicesLine);
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
    
    // Setup dialogue box (positioned with half margin from command overlay)
    float dialogueBoxY = windowHeight - DIALOGUE_BOX_HEIGHT - 40.0f; // Half the previous 80px margin
    m_dialogueBox.setSize(sf::Vector2f(windowWidth - 2 * BOX_MARGIN, DIALOGUE_BOX_HEIGHT));
    m_dialogueBox.setPosition(BOX_MARGIN, dialogueBoxY);
    m_dialogueBox.setFillColor(sf::Color(0, 0, 0, 200)); // Semi-transparent black
    m_dialogueBox.setOutlineColor(sf::Color::White);
    m_dialogueBox.setOutlineThickness(2.0f);
    
    // Calculate choice box dimensions
    float choiceBoxHeight = 120.0f;
    
    // Setup portrait frames VERTICALLY (bottom-aligned - closest to dialogue box)
    float portraitEndY = dialogueBoxY - 20.0f; // End position (bottom of portrait area)
    
    // Initialize left portrait frames (VERTICAL stack, BOTTOM-ALIGNED)
    m_leftPortraitFrames.clear();
    m_leftPortraits.clear();
    for (int i = 0; i < MAX_PORTRAITS_PER_SIDE; i++) {
        sf::RectangleShape frame;
        frame.setSize(sf::Vector2f(PORTRAIT_SIZE, PORTRAIT_SIZE));
        float xPos = BOX_MARGIN + 20.0f;
        // Position from bottom up: slot 0 is closest to dialogue box
        float yPos = portraitEndY - (i + 1) * (PORTRAIT_SIZE + PORTRAIT_SPACING) + PORTRAIT_SPACING;
        frame.setPosition(xPos, yPos);
        frame.setFillColor(sf::Color(50, 50, 50, 200));
        frame.setOutlineColor(sf::Color::White);
        frame.setOutlineThickness(2.0f);
        m_leftPortraitFrames.push_back(frame);
        
        sf::Sprite portrait;
        portrait.setPosition(xPos, yPos);
        m_leftPortraits.push_back(portrait);
    }
    
    // Initialize right portrait frames (VERTICAL stack, BOTTOM-ALIGNED)
    m_rightPortraitFrames.clear();
    m_rightPortraits.clear();
    for (int i = 0; i < MAX_PORTRAITS_PER_SIDE; i++) {
        sf::RectangleShape frame;
        frame.setSize(sf::Vector2f(PORTRAIT_SIZE, PORTRAIT_SIZE));
        float xPos = windowWidth - BOX_MARGIN - 20.0f - PORTRAIT_SIZE;
        // Position from bottom up: slot 0 is closest to dialogue box
        float yPos = portraitEndY - (i + 1) * (PORTRAIT_SIZE + PORTRAIT_SPACING) + PORTRAIT_SPACING;
        frame.setPosition(xPos, yPos);
        frame.setFillColor(sf::Color(50, 50, 50, 200));
        frame.setOutlineColor(sf::Color::White);
        frame.setOutlineThickness(2.0f);
        m_rightPortraitFrames.push_back(frame);
        
        sf::Sprite portrait;
        portrait.setPosition(xPos, yPos);
        m_rightPortraits.push_back(portrait);
    }
    
    // Setup choice box aligned with portraits, close to dialogue box
    float choiceBoxY = dialogueBoxY - choiceBoxHeight - 10.0f; // Just above dialogue box
    float choiceBoxWidth = windowWidth - 2 * BOX_MARGIN - 2 * (PORTRAIT_SIZE + 40.0f); // Leave space for portraits + margins
    float choiceBoxX = BOX_MARGIN + PORTRAIT_SIZE + 40.0f; // Start after left portrait + margin
    
    m_choiceBox.setSize(sf::Vector2f(choiceBoxWidth, choiceBoxHeight));
    m_choiceBox.setPosition(choiceBoxX, choiceBoxY);
    m_choiceBox.setFillColor(sf::Color(20, 20, 40, 220));
    m_choiceBox.setOutlineColor(sf::Color::Yellow);
    m_choiceBox.setOutlineThickness(2.0f);
    
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
        
        // Initialize choice texts (positioned in the middle choice box)
        m_choiceTexts.clear();
        for (int i = 0; i < 4; i++) { // Support up to 4 choices
            sf::Text choiceText;
            choiceText.setFont(m_game->getAssets().getFont("ShareTech"));
            choiceText.setCharacterSize(16);
            choiceText.setFillColor(sf::Color::White);
            choiceText.setPosition(m_choiceBox.getPosition().x + TEXT_PADDING, 
                                 m_choiceBox.getPosition().y + TEXT_PADDING + i * 25.0f);
            m_choiceTexts.push_back(choiceText);
        }
        
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
    
    setupLogUI();
    
    std::cout << "Dialogue UI setup complete. Window size: (" << windowWidth << ", " << windowHeight << ")" << std::endl;
    std::cout << "Choice box positioned at: (" << m_choiceBox.getPosition().x << ", " << m_choiceBox.getPosition().y << ")" << std::endl;
}

void Scene_Dialogue::displayCurrentLine()
{
    if (m_currentLineIndex >= m_dialogueConfig.lines.size()) {
        m_dialogueComplete = true;
        return;
    }
    
    const DialogueLine& currentLine = m_dialogueConfig.lines[m_currentLineIndex];
    
    // Only display if it's a regular LINE
    if (currentLine.type != "LINE") {
        processCurrentLine();
        return;
    }
    
    // Add to dialogue log
    addToLog(currentLine.actor, currentLine.text);
    
    // Set actor name
    m_actorNameText.setString(currentLine.actor + ":");
    
    // Setup typewriter effect
    m_fullText = currentLine.text;
    m_displayedText = "";
    m_typewriterTimer = 0.0f;
    m_textComplete = false;
    
    // Load and highlight appropriate portrait
    int leftSlot = findPortraitSlot(currentLine.actor, true);
    int rightSlot = findPortraitSlot(currentLine.actor, false);
    
    if (leftSlot >= 0) {
        loadPortrait(currentLine.actor, currentLine.portrait, true, leftSlot);
    } else if (rightSlot >= 0) {
        loadPortrait(currentLine.actor, currentLine.portrait, false, rightSlot);
    }
    
    highlightActivePortrait(currentLine.actor);
    
    std::cout << "Displaying line " << (m_currentLineIndex + 1) << "/" << m_dialogueConfig.lines.size() 
              << ": " << currentLine.actor << " - " << currentLine.text << std::endl;
}

void Scene_Dialogue::loadPortrait(const std::string& actor, const std::string& portrait, bool useLeftSide, int slotIndex)
{
    try {
        // Convert portrait name to asset name format
        std::string assetName = actor + "_" + portrait;
        
        // Try to get the texture
        auto& portraitTexture = m_game->getAssets().getTexture(assetName);
        
        if (useLeftSide && slotIndex < static_cast<int>(m_leftPortraits.size())) {
            m_leftPortraits[slotIndex].setTexture(portraitTexture);
            
            // Scale portrait to fit frame
            sf::Vector2u textureSize = portraitTexture.getSize();
            float scaleX = PORTRAIT_SIZE / static_cast<float>(textureSize.x);
            float scaleY = PORTRAIT_SIZE / static_cast<float>(textureSize.y);
            float scale = std::min(scaleX, scaleY);
            m_leftPortraits[slotIndex].setScale(scale, scale);
            
            // Center the portrait in the frame
            sf::Vector2f framePos = m_leftPortraitFrames[slotIndex].getPosition();
            sf::FloatRect bounds = m_leftPortraits[slotIndex].getLocalBounds();
            float offsetX = (PORTRAIT_SIZE - bounds.width * scale) / 2.0f;
            float offsetY = (PORTRAIT_SIZE - bounds.height * scale) / 2.0f;
            m_leftPortraits[slotIndex].setPosition(framePos.x + offsetX, framePos.y + offsetY);
            
            std::cout << "Loaded portrait: " << assetName << " (left slot " << slotIndex << ")" << std::endl;
        } else if (!useLeftSide && slotIndex < static_cast<int>(m_rightPortraits.size())) {
            m_rightPortraits[slotIndex].setTexture(portraitTexture);
            
            // Scale portrait to fit frame
            sf::Vector2u textureSize = portraitTexture.getSize();
            float scaleX = PORTRAIT_SIZE / static_cast<float>(textureSize.x);
            float scaleY = PORTRAIT_SIZE / static_cast<float>(textureSize.y);
            float scale = std::min(scaleX, scaleY);
            m_rightPortraits[slotIndex].setScale(scale, scale);
            
            // Center the portrait in the frame
            sf::Vector2f framePos = m_rightPortraitFrames[slotIndex].getPosition();
            sf::FloatRect bounds = m_rightPortraits[slotIndex].getLocalBounds();
            float offsetX = (PORTRAIT_SIZE - bounds.width * scale) / 2.0f;
            float offsetY = (PORTRAIT_SIZE - bounds.height * scale) / 2.0f;
            m_rightPortraits[slotIndex].setPosition(framePos.x + offsetX, framePos.y + offsetY);
            
            std::cout << "Loaded portrait: " << assetName << " (right slot " << slotIndex << ")" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cout << "Warning: Could not load portrait " << actor << "_" << portrait << ": " << e.what() << std::endl;
    }
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
        processCurrentLine();
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
        if (m_hasPreservedState) {
            // Create play scene with preserved state
            std::cout << "Returning to play scene with preserved state:" << std::endl;
            std::cout << "  Level: " << m_originalLevel << std::endl;
            std::cout << "  Position: (" << m_originalPlayerPosition.x << ", " << m_originalPlayerPosition.y << ")" << std::endl;
            std::cout << "  Health: " << m_originalPlayerHealth << std::endl;
            
            auto playScene = std::make_shared<Scene_Play>(m_game, m_originalLevel);
            
            // Set the custom spawn position to preserve player location
            playScene->setCustomSpawnPosition(m_originalPlayerPosition);
            
            m_game->changeScene("Play", playScene);
        } else {
            // Fallback to default behavior (shouldn't happen with new system)
            std::cout << "Warning: No preserved state, using default level" << std::endl;
            m_game->changeScene("Play", std::make_shared<Scene_Play>(m_game, "metadata/levels/level1.txt"));
        }
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
        std::cout << "Action: " << action.getName() << " | Showing choices: " << (m_showingChoices ? "YES" : "NO") << " | Showing log: " << (m_showingLog ? "YES" : "NO") << std::endl;
        
        if (m_showingLog) {
            // Handle log navigation
            if (action.getName() == "CHOICE_UP") {
                scrollLog(-1);
            } else if (action.getName() == "CHOICE_DOWN") {
                scrollLog(1);
            } else if (action.getName() == "SHOW_LOG" || action.getName() == "BACK") {
                hideDialogueLog();
            }
        } else if (m_showingChoices) {
            // Handle choice navigation
            if (action.getName() == "CHOICE_UP") {
                std::cout << "Choice UP - Current: " << m_selectedChoice;
                m_selectedChoice = (m_selectedChoice - 1 + m_currentChoices.size()) % m_currentChoices.size();
                std::cout << " -> New: " << m_selectedChoice << std::endl;
                
                // Update choice display directly
                for (size_t i = 0; i < m_currentChoices.size() && i < m_choiceTexts.size(); i++) {
                    bool isSelected = (static_cast<int>(i) == m_selectedChoice);
                    m_choiceTexts[i].setString((isSelected ? "> " : "  ") + m_currentChoices[i].text);
                    m_choiceTexts[i].setFillColor(isSelected ? sf::Color::Yellow : sf::Color::White);
                }
            } else if (action.getName() == "CHOICE_DOWN") {
                std::cout << "Choice DOWN - Current: " << m_selectedChoice;
                m_selectedChoice = (m_selectedChoice + 1) % m_currentChoices.size();
                std::cout << " -> New: " << m_selectedChoice << std::endl;
                
                // Update choice display directly
                for (size_t i = 0; i < m_currentChoices.size() && i < m_choiceTexts.size(); i++) {
                    bool isSelected = (static_cast<int>(i) == m_selectedChoice);
                    m_choiceTexts[i].setString((isSelected ? "> " : "  ") + m_currentChoices[i].text);
                    m_choiceTexts[i].setFillColor(isSelected ? sf::Color::Yellow : sf::Color::White);
                }
            } else if (action.getName() == "CONFIRM") {
                std::cout << "Selecting choice: " << m_selectedChoice << std::endl;
                selectChoice(m_selectedChoice);
            } else if (action.getName() == "SHOW_LOG") {
                showDialogueLog();
            } else if (action.getName() == "BACK") {
                // Set dialogue as complete to use normal completion flow with preserved state
                m_dialogueComplete = true;
            }
        } else {
            // Handle normal dialogue navigation
            if (action.getName() == "CONFIRM") {
                nextLine();
            } else if (action.getName() == "SHOW_LOG") {
                showDialogueLog();
            } else if (action.getName() == "BACK") {
                // Set dialogue as complete to use normal completion flow with preserved state
                m_dialogueComplete = true;
            }
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
    
    // Draw portrait frames and portraits
    for (size_t i = 0; i < m_leftPortraitFrames.size(); i++) {
        m_game->window().draw(m_leftPortraitFrames[i]);
        if (m_leftPortraits[i].getTexture()) {
            m_game->window().draw(m_leftPortraits[i]);
        }
    }
    
    for (size_t i = 0; i < m_rightPortraitFrames.size(); i++) {
        m_game->window().draw(m_rightPortraitFrames[i]);
        if (m_rightPortraits[i].getTexture()) {
            m_game->window().draw(m_rightPortraits[i]);
        }
    }
    
    // Draw dialogue box
    m_game->window().draw(m_dialogueBox);
    
    // Draw choice box if showing choices
    if (m_showingChoices) {
        m_game->window().draw(m_choiceBox);
        for (size_t i = 0; i < m_currentChoices.size() && i < m_choiceTexts.size(); i++) {
            m_game->window().draw(m_choiceTexts[i]);
        }
    }
    
    // Draw text elements
    m_game->window().draw(m_actorNameText);
    m_game->window().draw(m_dialogueText);
    
    // Draw dialogue log if showing
    if (m_showingLog) {
        renderDialogueLog();
    }
    
    // Draw command overlay (from base class)
    renderCommandOverlay();
}

// Enhanced dialogue system methods

void Scene_Dialogue::processCurrentLine()
{
    if (m_currentLineIndex >= m_dialogueConfig.lines.size()) {
        m_dialogueComplete = true;
        return;
    }
    
    const DialogueLine& currentLine = m_dialogueConfig.lines[m_currentLineIndex];
    
    if (currentLine.type == "LINE") {
        displayCurrentLine();
    } else if (currentLine.type == "CHOICES") {
        // Parse choices from the simplified format: "text1|target1|text2|target2"
        m_currentChoices.clear();
        std::string choicesText = currentLine.text;
        
        // Split by | and process pairs
        std::vector<std::string> parts;
        size_t pos = 0;
        while (pos < choicesText.length()) {
            size_t pipePos = choicesText.find('|', pos);
            if (pipePos != std::string::npos) {
                parts.push_back(choicesText.substr(pos, pipePos - pos));
                pos = pipePos + 1;
            } else {
                parts.push_back(choicesText.substr(pos));
                break;
            }
        }
        
        // Process pairs (text, target)
        for (size_t i = 0; i + 1 < parts.size(); i += 2) {
            DialogueChoice choice;
            choice.text = parts[i];
            choice.jumpTarget = parts[i + 1];
            m_currentChoices.push_back(choice);
        }
        
        showChoices(m_currentChoices);
    } else if (currentLine.type == "JUMP") {
        jumpToLabel(currentLine.jumpTarget);
    }
}

void Scene_Dialogue::jumpToLabel(const std::string& label)
{
    std::cout << "jumpToLabel called with: '" << label << "'" << std::endl;
    auto it = m_dialogueConfig.labels.find(label);
    if (it != m_dialogueConfig.labels.end()) {
        std::cout << "Found label '" << label << "' at line index: " << it->second << std::endl;
        m_currentLineIndex = it->second;
        processCurrentLine();
    } else {
        std::cout << "Warning: Label '" << label << "' not found!" << std::endl;
        std::cout << "Available labels:" << std::endl;
        for (const auto& labelPair : m_dialogueConfig.labels) {
            std::cout << "  '" << labelPair.first << "' -> " << labelPair.second << std::endl;
        }
        m_dialogueComplete = true;
    }
}

void Scene_Dialogue::showChoices(const std::vector<DialogueChoice>& choices)
{
    m_showingChoices = true;
    m_selectedChoice = 0; // Reset to first choice
    
    std::cout << "Showing " << choices.size() << " choices:" << std::endl;
    for (size_t i = 0; i < choices.size(); i++) {
        std::cout << "  " << choices[i].text << " -> " << choices[i].jumpTarget << std::endl;
    }
    
    // Set up choice texts with current selection
    for (size_t i = 0; i < choices.size() && i < m_choiceTexts.size(); i++) {
        bool isSelected = (static_cast<int>(i) == m_selectedChoice);
        m_choiceTexts[i].setString((isSelected ? "> " : "  ") + choices[i].text);
        m_choiceTexts[i].setFillColor(isSelected ? sf::Color::Yellow : sf::Color::White);
    }
}

void Scene_Dialogue::selectChoice(int choiceIndex)
{
    std::cout << "selectChoice called with index: " << choiceIndex << " | Current choices size: " << m_currentChoices.size() << std::endl;
    
    if (choiceIndex >= 0 && choiceIndex < static_cast<int>(m_currentChoices.size())) {
        std::cout << "Selecting choice: '" << m_currentChoices[choiceIndex].text << "' -> '" << m_currentChoices[choiceIndex].jumpTarget << "'" << std::endl;
        
        // Add choice selection to log
        addChoiceToLog(m_currentChoices, choiceIndex);
        
        m_showingChoices = false;
        jumpToLabel(m_currentChoices[choiceIndex].jumpTarget);
    } else {
        std::cout << "Invalid choice index: " << choiceIndex << std::endl;
    }
}

void Scene_Dialogue::clearAllPortraits()
{
    for (auto& portrait : m_leftPortraits) {
        portrait.setTexture(sf::Texture{});
    }
    for (auto& portrait : m_rightPortraits) {
        portrait.setTexture(sf::Texture{});
    }
    
    // Reset all frame colors
    for (auto& frame : m_leftPortraitFrames) {
        frame.setOutlineColor(sf::Color::White);
    }
    for (auto& frame : m_rightPortraitFrames) {
        frame.setOutlineColor(sf::Color::White);
    }
}

void Scene_Dialogue::highlightActivePortrait(const std::string& actor)
{
    // Reset all frames to white
    for (auto& frame : m_leftPortraitFrames) {
        frame.setOutlineColor(sf::Color::White);
    }
    for (auto& frame : m_rightPortraitFrames) {
        frame.setOutlineColor(sf::Color::White);
    }
    
    // Find and highlight the active actor's portrait
    int leftSlot = findPortraitSlot(actor, true);
    int rightSlot = findPortraitSlot(actor, false);
    
    if (leftSlot >= 0) {
        m_leftPortraitFrames[leftSlot].setOutlineColor(sf::Color::Yellow);
    } else if (rightSlot >= 0) {
        m_rightPortraitFrames[rightSlot].setOutlineColor(sf::Color::Yellow);
    }
}

int Scene_Dialogue::findPortraitSlot(const std::string& actor, bool leftSide)
{
    // Look up actor in portrait assignments from config
    auto it = m_dialogueConfig.portraitAssignments.find(actor);
    if (it != m_dialogueConfig.portraitAssignments.end()) {
        bool configLeftSide = it->second.first;
        int slot = it->second.second;
        
        // Return slot if the side matches what we're looking for
        if (configLeftSide == leftSide) {
            return slot;
        }
    }
    
    return -1; // Not found or wrong side
}

void Scene_Dialogue::onEnd()
{
    // Stop background sound
    if (m_soundManager) {
        m_soundManager->stopSound("background");
    }
    
    std::cout << "Dialogue scene ended" << std::endl;
}

// Dialogue Log System Implementation

void Scene_Dialogue::addToLog(const std::string& actor, const std::string& text)
{
    if (actor.empty() || text.empty()) {
        std::cout << "Warning: Empty actor or text for dialogue log" << std::endl;
        return;
    }
    
    try {
        m_dialogueLog.emplace_back(actor, text);
        std::cout << "Added to log: " << actor << " - " << text << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Error adding to log: " << e.what() << std::endl;
    }
}

void Scene_Dialogue::addChoiceToLog(const std::vector<DialogueChoice>& choices, int selectedIndex)
{
    if (choices.empty() || selectedIndex < 0 || selectedIndex >= static_cast<int>(choices.size())) {
        std::cout << "Warning: Invalid choice selection for log" << std::endl;
        return;
    }
    
    std::vector<std::string> choiceTexts;
    choiceTexts.reserve(choices.size()); // Reserve space to avoid reallocations
    
    for (const auto& choice : choices) {
        choiceTexts.push_back(choice.text);
    }
    
    try {
        m_dialogueLog.emplace_back(choiceTexts, selectedIndex);
        std::cout << "Added choice to log: selected " << selectedIndex << " of " << choices.size() << " options" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Error adding choice to log: " << e.what() << std::endl;
    }
}

void Scene_Dialogue::showDialogueLog()
{
    m_showingLog = true;
    m_logScrollOffset = 0;
    std::cout << "Showing dialogue log with " << m_dialogueLog.size() << " entries" << std::endl;
}

void Scene_Dialogue::hideDialogueLog()
{
    m_showingLog = false;
    std::cout << "Hiding dialogue log" << std::endl;
}

void Scene_Dialogue::scrollLog(int direction)
{
    int maxOffset = static_cast<int>(m_dialogueLog.size()) - MAX_LOG_LINES_VISIBLE;
    if (maxOffset < 0) maxOffset = 0;
    
    m_logScrollOffset += direction;
    if (m_logScrollOffset < 0) m_logScrollOffset = 0;
    if (m_logScrollOffset > maxOffset) m_logScrollOffset = maxOffset;
    
    std::cout << "Scrolled log: offset = " << m_logScrollOffset << " (max: " << maxOffset << ")" << std::endl;
}

void Scene_Dialogue::setupLogUI()
{
    sf::Vector2u windowSize = m_game->window().getSize();
    float windowWidth = static_cast<float>(windowSize.x);
    float windowHeight = static_cast<float>(windowSize.y);
    
    // Setup log background (avoid command overlay area like dialogue box does)
    float logMargin = 20.0f;
    float commandOverlayHeight = 40.0f; // Same as dialogue box margin
    float logHeight = windowHeight - (2 * logMargin) - commandOverlayHeight;
    
    m_logBackground.setSize(sf::Vector2f(windowWidth - (2 * logMargin), logHeight));
    m_logBackground.setPosition(logMargin, logMargin);
    m_logBackground.setFillColor(sf::Color(0, 0, 0, 240)); // Very dark background
    m_logBackground.setOutlineColor(sf::Color::White);
    m_logBackground.setOutlineThickness(2.0f);
    
    // Setup log title
    try {
        m_logTitle.setFont(m_game->getAssets().getFont("ShareTech"));
        m_logTitle.setCharacterSize(24);
        m_logTitle.setFillColor(sf::Color::Yellow);
        m_logTitle.setStyle(sf::Text::Bold);
        m_logTitle.setString("Dialogue History");
        m_logTitle.setPosition(logMargin + 20.0f, logMargin + 20.0f);
    } catch (const std::exception& e) {
        std::cout << "Warning: Could not set font for log title: " << e.what() << std::endl;
    }
    
    // Initialize log text objects
    m_logTexts.clear();
    m_logTexts.reserve(MAX_LOG_LINES_VISIBLE); // Reserve space to avoid allocations
    
    for (int i = 0; i < MAX_LOG_LINES_VISIBLE; i++) {
        sf::Text logText;
        try {
            logText.setFont(m_game->getAssets().getFont("ShareTech"));
            logText.setCharacterSize(14);
            logText.setFillColor(sf::Color::White);
            logText.setPosition(logMargin + 20.0f, logMargin + 60.0f + i * 20.0f);
        } catch (const std::exception& e) {
            std::cout << "Warning: Could not set font for log text: " << e.what() << std::endl;
        }
        m_logTexts.push_back(logText);
    }
}

void Scene_Dialogue::renderDialogueLog()
{
    // Draw log background
    m_game->window().draw(m_logBackground);
    
    // Draw log title
    m_game->window().draw(m_logTitle);
    
    // Draw log entries
    int displayIndex = 0;
    for (size_t i = m_logScrollOffset; i < m_dialogueLog.size() && displayIndex < MAX_LOG_LINES_VISIBLE; i++, displayIndex++) {
        const DialogueLogEntry& entry = m_dialogueLog[i];
        
        if (entry.type == DialogueLogEntry::LINE) {
            // Regular dialogue line
            std::string displayText = entry.actor + ": " + entry.text;
            m_logTexts[displayIndex].setString(displayText);
            m_logTexts[displayIndex].setFillColor(sf::Color::White);
            m_game->window().draw(m_logTexts[displayIndex]);
        } else if (entry.type == DialogueLogEntry::CHOICE_SELECTION) {
            // Choice selection entry header
            std::string displayText = "Player Choice:";
            m_logTexts[displayIndex].setString(displayText);
            m_logTexts[displayIndex].setFillColor(sf::Color::Yellow);
            m_game->window().draw(m_logTexts[displayIndex]);
            displayIndex++;
            
            // Show available choices
            for (size_t j = 0; j < entry.availableChoices.size() && displayIndex < MAX_LOG_LINES_VISIBLE; j++, displayIndex++) {
                bool wasSelected = (static_cast<int>(j) == entry.selectedChoiceIndex);
                std::string choiceText = "  " + entry.availableChoices[j];
                m_logTexts[displayIndex].setString(choiceText);
                
                // Style like choice menu: selected = yellow, unselected = white
                m_logTexts[displayIndex].setFillColor(wasSelected ? sf::Color::Yellow : sf::Color::White);
                m_game->window().draw(m_logTexts[displayIndex]);
            }
            displayIndex--; // Adjust for the increment in the for loop
        }
    }
    
    // Draw simple scroll indicators (no instructions - command overlay shows controls)
    if (m_logScrollOffset > 0) {
        sf::Text scrollUp;
        try {
            scrollUp.setFont(m_game->getAssets().getFont("ShareTech"));
            scrollUp.setCharacterSize(16);
            scrollUp.setFillColor(sf::Color::Cyan);
            scrollUp.setString("▲ More above");
            scrollUp.setPosition(m_logBackground.getPosition().x + m_logBackground.getSize().x - 150.0f, 
                                m_logBackground.getPosition().y + 10.0f);
            m_game->window().draw(scrollUp);
        } catch (const std::exception& e) {}
    }
    
    int maxOffset = static_cast<int>(m_dialogueLog.size()) - MAX_LOG_LINES_VISIBLE;
    if (maxOffset > 0 && m_logScrollOffset < maxOffset) {
        sf::Text scrollDown;
        try {
            scrollDown.setFont(m_game->getAssets().getFont("ShareTech"));
            scrollDown.setCharacterSize(16);
            scrollDown.setFillColor(sf::Color::Cyan);
            scrollDown.setString("▼ More below");
            scrollDown.setPosition(m_logBackground.getPosition().x + m_logBackground.getSize().x - 150.0f, 
                                  m_logBackground.getPosition().y + m_logBackground.getSize().y - 30.0f);
            m_game->window().draw(scrollDown);
        } catch (const std::exception& e) {}
    }
}
