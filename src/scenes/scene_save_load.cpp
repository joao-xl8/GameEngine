#include "scene_save_load.hpp"
#include "scene_menu.hpp"
#include "scene_play.hpp"
#include "../game_engine.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>

Scene_SaveLoad::Scene_SaveLoad(GameEngine* gameEngine, Mode mode, const SaveData& currentGameData)
    : Scene(gameEngine), m_mode(mode), m_currentGameData(currentGameData),
      m_showOverwriteDialog(false), m_overwriteSlotNumber(-1), m_overwriteConfirm(false),
      m_selectedSlot(0), m_scrollOffset(0), m_inputTimer(0.0f), m_inputDelay(0.2f)
{
    std::cout << "Scene_SaveLoad created in " << (mode == SAVE_MODE ? "SAVE" : "LOAD") << " mode" << std::endl;
}

void Scene_SaveLoad::init()
{
    // Standard navigation controls
    registerAction(sf::Keyboard::W, "UP");
    registerAction(sf::Keyboard::S, "DOWN");
    registerAction(sf::Keyboard::A, "LEFT");
    registerAction(sf::Keyboard::D, "RIGHT");
    
    // Standard confirm/cancel controls
    registerAction(sf::Keyboard::Space, "SELECT");
    registerAction(sf::Keyboard::C, "BACK");
    
    // Load save slots
    m_saveSlots = m_saveSystem.getAllSaveSlots();
    
    setupUI();
    updateSlotDisplay();
    
    std::cout << "Scene_SaveLoad initialized with " << m_saveSlots.size() << " slots" << std::endl;
}

void Scene_SaveLoad::setupUI()
{
    sf::Vector2u windowSize = m_game->window().getSize();
    float windowWidth = static_cast<float>(windowSize.x);
    float windowHeight = static_cast<float>(windowSize.y);
    
    // Calculate how many slots can fit on screen
    float availableHeight = windowHeight - 200.0f; // Reserve space for title and command overlay
    m_maxVisibleSlots = static_cast<int>(availableHeight / SLOT_HEIGHT);
    
    // Ensure we have at least 3 slots and don't exceed reasonable limits
    m_maxVisibleSlots = std::max(3, std::min(m_maxVisibleSlots, 15));
    
    std::cout << "Screen size: " << windowSize.x << "x" << windowSize.y 
              << ", Max visible slots: " << m_maxVisibleSlots << std::endl;
    
    try {
        // Title
        m_titleText.setFont(m_game->getAssets().getFont("ShareTech"));
        m_titleText.setCharacterSize(32);
        m_titleText.setFillColor(sf::Color::Yellow);
        m_titleText.setStyle(sf::Text::Bold);
        m_titleText.setString(getModeTitle());
        
        // Center title
        sf::FloatRect titleBounds = m_titleText.getLocalBounds();
        m_titleText.setPosition((windowWidth - titleBounds.width) / 2, 50);
        
        // Selection highlight
        m_selectionHighlight.setSize(sf::Vector2f(windowWidth - 100, SLOT_HEIGHT - 10));
        m_selectionHighlight.setFillColor(sf::Color(100, 100, 100, 100));
        m_selectionHighlight.setOutlineColor(sf::Color::Yellow);
        m_selectionHighlight.setOutlineThickness(2.0f);
        
        // Initialize slot display elements
        m_slotTexts.clear();
        m_slotSecondaryTexts.clear();
        m_slotBackgrounds.clear();
        
        for (int i = 0; i < m_maxVisibleSlots; i++) {
            // Primary slot text (first line)
            sf::Text slotText;
            slotText.setFont(m_game->getAssets().getFont("ShareTech"));
            slotText.setCharacterSize(20); // Larger text for bigger slots
            slotText.setFillColor(sf::Color::White);
            slotText.setPosition(70, 140 + i * SLOT_HEIGHT);
            m_slotTexts.push_back(slotText);
            
            // Secondary slot text (second line)
            sf::Text slotSecondaryText;
            slotSecondaryText.setFont(m_game->getAssets().getFont("ShareTech"));
            slotSecondaryText.setCharacterSize(16); // Slightly larger for better readability
            slotSecondaryText.setFillColor(sf::Color(180, 180, 180)); // Lighter gray
            slotSecondaryText.setPosition(70, 165 + i * SLOT_HEIGHT); // 25px below main text
            m_slotSecondaryTexts.push_back(slotSecondaryText);
            
            // Slot background
            sf::RectangleShape slotBg;
            slotBg.setSize(sf::Vector2f(windowWidth - 100, SLOT_HEIGHT - 10)); // 10px gap between slots
            slotBg.setPosition(50, 130 + i * SLOT_HEIGHT);
            slotBg.setFillColor(sf::Color(20, 20, 20, 150));
            slotBg.setOutlineColor(sf::Color(60, 60, 60));
            slotBg.setOutlineThickness(1.0f);
            m_slotBackgrounds.push_back(slotBg);
        }
        
    } catch (const std::exception& e) {
        std::cout << "Warning: Could not setup save/load UI fonts: " << e.what() << std::endl;
    }
}

void Scene_SaveLoad::update()
{
    // Update input delay timer
    if (m_inputTimer < m_inputDelay) {
        m_inputTimer += m_game->getDeltaTime();
    }
    
    sRender();
}

void Scene_SaveLoad::sDoAction(const Action& action)
{
    // Shorter input delay for better responsiveness
    if (m_inputTimer < 0.1f) {
        return;
    }
    
    if (action.getType() == "START") {
        std::cout << "Action received: " << action.getName() << ", Dialog active: " << (m_showOverwriteDialog ? "YES" : "NO") << std::endl;
        
        // Handle overwrite dialog input first
        if (m_showOverwriteDialog) {
            handleOverwriteInput(action);
            m_inputTimer = 0.0f;
            return;
        }
        
        // Normal navigation
        if (action.getName() == "UP") {
            // Play menu navigation sound (only if sound is enabled)
            if (m_game->isSoundEnabled()) {
                if (auto globalSound = m_game->getGlobalSoundManager()) {
                    float volume = m_game->getMasterVolume() * m_game->getEffectsVolume() * 60.0f;
                    globalSound->playSound("menu_select", volume);
                }
            }
            scrollUp();
        } else if (action.getName() == "DOWN") {
            // Play menu navigation sound (only if sound is enabled)
            if (m_game->isSoundEnabled()) {
                if (auto globalSound = m_game->getGlobalSoundManager()) {
                    float volume = m_game->getMasterVolume() * m_game->getEffectsVolume() * 60.0f;
                    globalSound->playSound("menu_select", volume);
                }
            }
            scrollDown();
        } else if (action.getName() == "SELECT") {
            // Play menu confirm sound (only if sound is enabled)
            if (m_game->isSoundEnabled()) {
                if (auto globalSound = m_game->getGlobalSoundManager()) {
                    float volume = m_game->getMasterVolume() * m_game->getEffectsVolume() * 80.0f;
                    globalSound->playSound("menu_confirm", volume);
                }
            }
            std::cout << "SELECT pressed on slot " << m_selectedSlot << std::endl;
            selectSlot();
        } else if (action.getName() == "BACK") {
            // Play menu back sound (only if sound is enabled)
            if (m_game->isSoundEnabled()) {
                if (auto globalSound = m_game->getGlobalSoundManager()) {
                    float volume = m_game->getMasterVolume() * m_game->getEffectsVolume() * 50.0f;
                    globalSound->playSound("menu_select", volume);
                }
            }
            
            goBack();
        }
        
        m_inputTimer = 0.0f; // Reset input timer
    }
}

void Scene_SaveLoad::scrollUp()
{
    if (m_selectedSlot > 0) {
        m_selectedSlot--;
        updateSlotDisplay(); // Auto-centering will handle scroll offset
        std::cout << "Selected slot: " << (m_selectedSlot + 1) << std::endl;
    }
}

void Scene_SaveLoad::scrollDown()
{
    if (m_selectedSlot < static_cast<int>(m_saveSlots.size()) - 1) {
        m_selectedSlot++;
        updateSlotDisplay(); // Auto-centering will handle scroll offset
        std::cout << "Selected slot: " << (m_selectedSlot + 1) << std::endl;
    }
}

void Scene_SaveLoad::selectSlot()
{
    if (m_selectedSlot < 0 || m_selectedSlot >= static_cast<int>(m_saveSlots.size())) {
        std::cout << "Invalid slot selection: " << m_selectedSlot << std::endl;
        return;
    }
    
    SaveSlot& selectedSlot = m_saveSlots[m_selectedSlot];
    int slotNumber = selectedSlot.slotNumber;
    
    std::cout << "Selected slot " << slotNumber << ", isEmpty: " << (selectedSlot.isEmpty ? "YES" : "NO") << ", Mode: " << (m_mode == SAVE_MODE ? "SAVE" : "LOAD") << std::endl;
    
    if (m_mode == SAVE_MODE) {
        // Don't allow manual saving to auto-save slot
        if (slotNumber == SaveSystem::AUTO_SAVE_SLOT) {
            std::cout << "Cannot manually save to auto-save slot" << std::endl;
            return;
        }
        
        // Check if slot is occupied and show overwrite dialog
        if (!selectedSlot.isEmpty) {
            std::cout << "Slot is occupied, showing overwrite dialog..." << std::endl;
            showOverwriteDialog(slotNumber);
            return;
        }
        
        // Save to empty slot directly
        std::cout << "Slot is empty, saving directly..." << std::endl;
        performSave(slotNumber);
    } else {
        // Load game
        if (!selectedSlot.isEmpty) {
            SaveData loadedData;
            if (m_saveSystem.loadGame(slotNumber, loadedData)) {
                std::cout << "Loading game from slot " << slotNumber << std::endl;
                std::cout << "Loaded player position: " << loadedData.playerX << ", " << loadedData.playerY << std::endl;
                
                // Switch to play scene with loaded data
                auto playScene = std::make_shared<Scene_Play>(m_game, loadedData.currentLevel);
                
                // Set the custom spawn position BEFORE initializing the scene
                playScene->setCustomSpawnPosition(Vec2(loadedData.playerX, loadedData.playerY));
                
                m_game->changeScene("Play", playScene);
            } else {
                std::cout << "Failed to load game from slot " << slotNumber << std::endl;
            }
        } else {
            std::cout << "Cannot load from empty slot " << slotNumber << std::endl;
        }
    }
}

void Scene_SaveLoad::deleteSlot()
{
    // Delete functionality removed - no longer supported
    std::cout << "Delete functionality has been removed" << std::endl;
}

void Scene_SaveLoad::performSave(int slotNumber)
{
    std::cout << "Performing save to slot " << slotNumber << std::endl;
    
    // Create complete new save data - this will COMPLETELY REPLACE the existing slot
    SaveData newSaveData = m_currentGameData; // Start with current game state
    
    // Generate meaningful save name based on current game state
    std::string levelName = "Level";
    if (!newSaveData.levelName.empty()) {
        levelName = newSaveData.levelName;
    }
    
    // Create a descriptive save name
    newSaveData.saveName = levelName + " - Slot " + std::to_string(slotNumber);
    newSaveData.saveTime = std::time(nullptr); // Current timestamp
    
    // Ensure all current game data is included (this completely overwrites the slot)
    std::cout << "Overwriting slot " << slotNumber << " with new data:" << std::endl;
    std::cout << "  Save Name: " << newSaveData.saveName << std::endl;
    std::cout << "  Player Position: (" << newSaveData.playerX << ", " << newSaveData.playerY << ")" << std::endl;
    std::cout << "  Level: " << newSaveData.currentLevel << std::endl;
    std::cout << "  Health: " << newSaveData.playerHealth << std::endl;
    std::cout << "  Play Time: " << newSaveData.playTimeSeconds << " seconds" << std::endl;
    std::cout << "  Timestamp: " << newSaveData.saveTime << std::endl;
    
    // Save the complete new data (this overwrites everything in the slot)
    if (m_saveSystem.saveGame(slotNumber, newSaveData)) {
        std::cout << "✅ Slot " << slotNumber << " completely overwritten with new save data!" << std::endl;
        
        // IMMEDIATELY refresh the slot list from disk to get the new data
        std::cout << "Refreshing save slots from disk..." << std::endl;
        m_saveSlots = m_saveSystem.getAllSaveSlots();
        
        // Verify the slot was updated correctly
        bool slotFound = false;
        for (const auto& slot : m_saveSlots) {
            if (slot.slotNumber == slotNumber) {
                slotFound = true;
                std::cout << "✅ Verified slot " << slotNumber << " updated:" << std::endl;
                std::cout << "  Display Name: " << slot.displayName << std::endl;
                std::cout << "  Time String: " << slot.timeString << std::endl;
                std::cout << "  Is Empty: " << (slot.isEmpty ? "YES" : "NO") << std::endl;
                std::cout << "  Player Pos: (" << slot.data.playerX << ", " << slot.data.playerY << ")" << std::endl;
                break;
            }
        }
        
        if (!slotFound) {
            std::cout << "⚠️ Warning: Could not find updated slot " << slotNumber << " in refreshed list" << std::endl;
        }
        
        // Force UI update
        std::cout << "Updating UI display..." << std::endl;
        updateSlotDisplay();
        
        // Force immediate screen refresh to show the new data
        m_game->window().clear();
        sRender();
        m_game->window().display();
        
        std::cout << "✅ UI updated - slot " << slotNumber << " now shows new save data on screen" << std::endl;
        
    } else {
        std::cout << "❌ Failed to save game to slot " << slotNumber << std::endl;
    }
}

void Scene_SaveLoad::showOverwriteDialog(int slotNumber)
{
    std::cout << "Showing overwrite dialog for slot " << slotNumber << std::endl;
    m_showOverwriteDialog = true;
    m_overwriteSlotNumber = slotNumber;
    m_overwriteConfirm = false; // Default to "No"
    setupOverwriteDialog();
    std::cout << "Overwrite dialog setup complete, m_showOverwriteDialog = " << (m_showOverwriteDialog ? "true" : "false") << std::endl;
}

void Scene_SaveLoad::hideOverwriteDialog()
{
    m_showOverwriteDialog = false;
    m_overwriteSlotNumber = -1;
}

void Scene_SaveLoad::handleOverwriteInput(const Action& action)
{
    std::cout << "Overwrite dialog input: " << action.getName() << std::endl;
    
    if (action.getName() == "LEFT" || action.getName() == "RIGHT" || 
        action.getName() == "UP" || action.getName() == "DOWN") {
        // Toggle between Yes and No
        m_overwriteConfirm = !m_overwriteConfirm;
        std::cout << "Toggled to: " << (m_overwriteConfirm ? "YES" : "NO") << std::endl;
        setupOverwriteDialog(); // Update display
    } else if (action.getName() == "SELECT") {
        std::cout << "SELECT pressed, confirm = " << (m_overwriteConfirm ? "YES" : "NO") << std::endl;
        if (m_overwriteConfirm) {
            // User confirmed overwrite
            std::cout << "Overwriting slot " << m_overwriteSlotNumber << "..." << std::endl;
            
            // CRITICAL FIX: Store slot number BEFORE hiding dialog (which resets it to -1)
            int slotToOverwrite = m_overwriteSlotNumber;
            hideOverwriteDialog();
            
            // Now perform save with the stored slot number
            performSave(slotToOverwrite);
            std::cout << "Slot " << slotToOverwrite << " has been overwritten successfully!" << std::endl;
        } else {
            // User cancelled
            std::cout << "Overwrite cancelled by user" << std::endl;
            hideOverwriteDialog();
        }
    } else if (action.getName() == "BACK") {
        // Cancel overwrite
        std::cout << "Overwrite cancelled (ESC pressed)" << std::endl;
        hideOverwriteDialog();
    } else {
        std::cout << "Unhandled action in overwrite dialog: " << action.getName() << std::endl;
    }
}

void Scene_SaveLoad::setupOverwriteDialog()
{
    sf::Vector2u windowSize = m_game->window().getSize();
    float windowWidth = static_cast<float>(windowSize.x);
    float windowHeight = static_cast<float>(windowSize.y);
    
    // Dialog dimensions
    float dialogWidth = 500.0f;
    float dialogHeight = 200.0f;
    float dialogX = (windowWidth - dialogWidth) / 2;
    float dialogY = (windowHeight - dialogHeight) / 2;
    
    try {
        // Background
        m_overwriteBackground.setSize(sf::Vector2f(dialogWidth, dialogHeight));
        m_overwriteBackground.setPosition(dialogX, dialogY);
        m_overwriteBackground.setFillColor(sf::Color(20, 20, 30, 240));
        
        // Border
        m_overwriteBorder.setSize(sf::Vector2f(dialogWidth, dialogHeight));
        m_overwriteBorder.setPosition(dialogX, dialogY);
        m_overwriteBorder.setFillColor(sf::Color::Transparent);
        m_overwriteBorder.setOutlineColor(sf::Color::Yellow);
        m_overwriteBorder.setOutlineThickness(2.0f);
        
        // Title
        m_overwriteTitle.setFont(m_game->getAssets().getFont("ShareTech"));
        m_overwriteTitle.setCharacterSize(24);
        m_overwriteTitle.setFillColor(sf::Color::Yellow);
        m_overwriteTitle.setString("Overwrite Save?");
        sf::FloatRect titleBounds = m_overwriteTitle.getLocalBounds();
        m_overwriteTitle.setPosition(dialogX + (dialogWidth - titleBounds.width) / 2, dialogY + 20);
        
        // Message
        m_overwriteMessage.setFont(m_game->getAssets().getFont("ShareTech"));
        m_overwriteMessage.setCharacterSize(16);
        m_overwriteMessage.setFillColor(sf::Color::White);
        m_overwriteMessage.setString("Slot " + std::to_string(m_overwriteSlotNumber) + " already contains a save.\nDo you want to overwrite it?");
        sf::FloatRect messageBounds = m_overwriteMessage.getLocalBounds();
        m_overwriteMessage.setPosition(dialogX + (dialogWidth - messageBounds.width) / 2, dialogY + 70);
        
        // Options
        m_overwriteOptions.setFont(m_game->getAssets().getFont("ShareTech"));
        m_overwriteOptions.setCharacterSize(18);
        m_overwriteOptions.setFillColor(sf::Color::White);
        
        std::string optionsText;
        if (m_overwriteConfirm) {
            optionsText = "> YES <    NO";
        } else {
            optionsText = "  YES    > NO <";
        }
        m_overwriteOptions.setString(optionsText);
        sf::FloatRect optionsBounds = m_overwriteOptions.getLocalBounds();
        m_overwriteOptions.setPosition(dialogX + (dialogWidth - optionsBounds.width) / 2, dialogY + 140);
        
    } catch (const std::exception& e) {
        std::cout << "Warning: Could not setup overwrite dialog: " << e.what() << std::endl;
    }
}

void Scene_SaveLoad::renderOverwriteDialog()
{
    if (!m_showOverwriteDialog) return;
    
    // Draw dialog elements
    m_game->window().draw(m_overwriteBackground);
    m_game->window().draw(m_overwriteBorder);
    m_game->window().draw(m_overwriteTitle);
    m_game->window().draw(m_overwriteMessage);
    m_game->window().draw(m_overwriteOptions);
}

void Scene_SaveLoad::goBack()
{
    // Return to the previous scene (play scene when saving from game)
    if (m_mode == SAVE_MODE) {
        // Return to play scene and restore player position
        auto playScene = std::make_shared<Scene_Play>(m_game, m_currentGameData.currentLevel);
        
        // Set the custom spawn position BEFORE initializing the scene
        playScene->setCustomSpawnPosition(Vec2(m_currentGameData.playerX, m_currentGameData.playerY));
        
        m_game->changeScene("Play", playScene);
        std::cout << "Returned to play scene with restored player position: " 
                  << m_currentGameData.playerX << ", " << m_currentGameData.playerY << std::endl;
    } else {
        // Return to main menu when loading
        m_game->changeScene("Menu", std::make_shared<Scene_Menu>(m_game));
    }
}

void Scene_SaveLoad::updateSlotDisplay()
{
    // Auto-scroll to keep selected slot in the middle of the screen
    int middleSlot = m_maxVisibleSlots / 2;
    int desiredOffset = m_selectedSlot - middleSlot;
    
    // Clamp scroll offset to valid range
    int maxOffset = static_cast<int>(m_saveSlots.size()) - m_maxVisibleSlots;
    if (maxOffset < 0) maxOffset = 0;
    
    m_scrollOffset = std::max(0, std::min(desiredOffset, maxOffset));
    
    for (int i = 0; i < m_maxVisibleSlots; i++) {
        int slotIndex = m_scrollOffset + i;
        
        if (slotIndex < static_cast<int>(m_saveSlots.size())) {
            const SaveSlot& slot = m_saveSlots[slotIndex];
            
            // Update primary slot text (first line)
            m_slotTexts[i].setString(getSlotDisplayText(slot));
            m_slotTexts[i].setFillColor(getSlotColor(slot));
            
            // Update secondary slot text (second line)
            m_slotSecondaryTexts[i].setString(getSlotSecondaryText(slot));
            m_slotSecondaryTexts[i].setFillColor(sf::Color(180, 180, 180)); // Consistent gray
            
            // Update background color
            if (slotIndex == m_selectedSlot) {
                m_slotBackgrounds[i].setFillColor(sf::Color(50, 50, 100, 200));
            } else {
                m_slotBackgrounds[i].setFillColor(sf::Color(20, 20, 20, 150));
            }
        } else {
            // Empty display slot
            m_slotTexts[i].setString("");
            m_slotSecondaryTexts[i].setString("");
            m_slotBackgrounds[i].setFillColor(sf::Color::Transparent);
        }
    }
    
    // Update selection highlight position
    int highlightIndex = m_selectedSlot - m_scrollOffset;
    if (highlightIndex >= 0 && highlightIndex < m_maxVisibleSlots) {
        m_selectionHighlight.setPosition(50, 130 + highlightIndex * SLOT_HEIGHT);
    }
}

std::string Scene_SaveLoad::getSlotDisplayText(const SaveSlot& slot)
{
    std::ostringstream oss;
    
    // First line: Slot ID and Save Name
    if (slot.slotNumber == SaveSystem::AUTO_SAVE_SLOT) {
        oss << "[AUTO-SAVE] ";
    } else {
        oss << "SLOT " << std::setfill('0') << std::setw(2) << slot.slotNumber << " ";
    }
    
    if (slot.isEmpty) {
        oss << "Empty Slot";
    } else {
        oss << slot.displayName;
    }
    
    return oss.str();
}

std::string Scene_SaveLoad::getSlotSecondaryText(const SaveSlot& slot)
{
    if (slot.isEmpty) {
        return "";
    }
    
    std::ostringstream oss;
    
    // Second line: Level, Time, and Save Date
    if (!slot.data.levelName.empty()) {
        oss << "Level: " << slot.data.levelName << " | ";
    }
    
    oss << "Time: " << m_saveSystem.formatPlayTime(slot.data.playTimeSeconds);
    
    if (!slot.timeString.empty()) {
        oss << " | Saved: " << slot.timeString;
    }
    
    return oss.str();
}

std::string Scene_SaveLoad::getModeTitle()
{
    return (m_mode == SAVE_MODE) ? "Save Game" : "Load Game";
}

sf::Color Scene_SaveLoad::getSlotColor(const SaveSlot& slot)
{
    if (slot.isEmpty) {
        return sf::Color(150, 150, 150); // Gray for empty slots
    } else if (slot.slotNumber == SaveSystem::AUTO_SAVE_SLOT) {
        return sf::Color::Cyan; // Cyan for auto-save
    } else {
        return sf::Color::White; // White for manual saves
    }
}

void Scene_SaveLoad::sRender()
{
    // Use default view
    m_game->window().setView(m_game->window().getDefaultView());
    
    // Clear with dark background
    sf::Vector2u windowSize = m_game->window().getSize();
    sf::RectangleShape background;
    background.setSize(sf::Vector2f(static_cast<float>(windowSize.x), static_cast<float>(windowSize.y)));
    background.setFillColor(sf::Color(15, 15, 30));
    m_game->window().draw(background);
    
    // Draw title
    m_game->window().draw(m_titleText);
    
    // Draw slot backgrounds
    for (int i = 0; i < m_maxVisibleSlots; i++) {
        int slotIndex = m_scrollOffset + i;
        if (slotIndex < static_cast<int>(m_saveSlots.size())) {
            m_game->window().draw(m_slotBackgrounds[i]);
        }
    }
    
    // Draw selection highlight
    if (m_selectedSlot >= m_scrollOffset && m_selectedSlot < m_scrollOffset + m_maxVisibleSlots) {
        m_game->window().draw(m_selectionHighlight);
    }
    
    // Draw slot texts (both primary and secondary)
    for (int i = 0; i < m_maxVisibleSlots; i++) {
        int slotIndex = m_scrollOffset + i;
        if (slotIndex < static_cast<int>(m_saveSlots.size())) {
            m_game->window().draw(m_slotTexts[i]);
            m_game->window().draw(m_slotSecondaryTexts[i]);
        }
    }
    
    // Draw fade effect at top if there are more slots above
    if (m_scrollOffset > 0) {
        // Create multiple layers for softer gradient effect
        for (int layer = 0; layer < 4; layer++) {
            sf::RectangleShape topFadeOverlay;
            float fadeHeight = 30.0f; // Each layer is 30px
            float fadeStartY = 120.0f + (layer * fadeHeight);
            
            topFadeOverlay.setSize(sf::Vector2f(static_cast<float>(windowSize.x) - 100, fadeHeight));
            topFadeOverlay.setPosition(50, fadeStartY);
            
            // Create softer fade - stronger at top, weaker as it goes down
            int alpha = 150 - (layer * 30); // 150, 120, 90, 60
            sf::Color topFadeColor = sf::Color(15, 15, 30, alpha);
            topFadeOverlay.setFillColor(topFadeColor);
            
            m_game->window().draw(topFadeOverlay);
        }
    }
    
    // Draw fade effect at bottom if there are more slots below
    if (m_scrollOffset + m_maxVisibleSlots < static_cast<int>(m_saveSlots.size())) {
        // Create multiple layers for softer gradient effect
        for (int layer = 0; layer < 4; layer++) {
            sf::RectangleShape bottomFadeOverlay;
            float fadeHeight = 30.0f; // Each layer is 30px
            float totalVisibleHeight = m_maxVisibleSlots * SLOT_HEIGHT;
            float fadeStartY = 120.0f + totalVisibleHeight - (120.0f) + (layer * fadeHeight);
            
            bottomFadeOverlay.setSize(sf::Vector2f(static_cast<float>(windowSize.x) - 100, fadeHeight));
            bottomFadeOverlay.setPosition(50, fadeStartY);
            
            // Create softer fade - weaker at top, stronger at bottom
            int alpha = 60 + (layer * 30); // 60, 90, 120, 150
            sf::Color bottomFadeColor = sf::Color(15, 15, 30, alpha);
            bottomFadeOverlay.setFillColor(bottomFadeColor);
            
            m_game->window().draw(bottomFadeOverlay);
        }
    }
    
    // Draw command overlay
    renderCommandOverlay();
    
    // Draw overwrite dialog on top of everything
    renderOverwriteDialog();
}

void Scene_SaveLoad::onEnd()
{
    std::cout << "Scene_SaveLoad ended" << std::endl;
}
