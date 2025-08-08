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
    // Register ALL input actions for dialogue once
    registerAction(sf::Keyboard::Space, "CONFIRM");
    registerAction(sf::Keyboard::Enter, "CONFIRM");
    registerAction(sf::Keyboard::Escape, "BACK");
    
    // Register choice navigation keys (always available)
    registerAction(sf::Keyboard::Up, "CHOICE_UP");
    registerAction(sf::Keyboard::Down, "CHOICE_DOWN");
    registerAction(sf::Keyboard::W, "CHOICE_UP");
    registerAction(sf::Keyboard::S, "CHOICE_DOWN");
    
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
    std::ifstream file(dialogueFile);
    if (!file.is_open()) {
        std::cerr << "Failed to open dialogue file: " << dialogueFile << std::endl;
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
            std::string labelName;
            iss >> labelName;
            m_dialogueConfig.labels[labelName] = m_dialogueConfig.lines.size();
        } else if (command == "LINE") {
            // If we have pending choices, add them first
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
        std::cout << "Action: " << action.getName() << " | Showing choices: " << (m_showingChoices ? "YES" : "NO") << std::endl;
        
        if (m_showingChoices) {
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
            } else if (action.getName() == "BACK") {
                // Return to play scene when user presses ESC
                m_game->changeScene("Play", std::make_shared<Scene_Play>(m_game, "metadata/levels/level1.txt"));
            }
        } else {
            // Handle normal dialogue navigation
            if (action.getName() == "CONFIRM") {
                nextLine();
            } else if (action.getName() == "BACK") {
                // Return to play scene when user presses ESC
                m_game->changeScene("Play", std::make_shared<Scene_Play>(m_game, "metadata/levels/level1.txt"));
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
    auto it = m_dialogueConfig.labels.find(label);
    if (it != m_dialogueConfig.labels.end()) {
        m_currentLineIndex = it->second;
        processCurrentLine();
    } else {
        std::cout << "Warning: Label '" << label << "' not found!" << std::endl;
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
    if (choiceIndex >= 0 && choiceIndex < static_cast<int>(m_currentChoices.size())) {
        m_showingChoices = false;
        jumpToLabel(m_currentChoices[choiceIndex].jumpTarget);
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
