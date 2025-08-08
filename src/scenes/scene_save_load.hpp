#pragma once

#include "scene.hpp"
#include "../systems/save_system.hpp"
#include "../ui/command_overlay.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

class Scene_SaveLoad : public Scene
{
public:
    enum Mode { SAVE_MODE, LOAD_MODE };
    
    Scene_SaveLoad(GameEngine* gameEngine, Mode mode, const SaveData& currentGameData = SaveData());
    
    void init() override;
    void update() override;
    void sDoAction(const Action& action) override;
    void sRender() override;
    void onEnd() override;

private:
    Mode m_mode;
    SaveSystem m_saveSystem;
    std::vector<SaveSlot> m_saveSlots;
    SaveData m_currentGameData;
    
    // Overwrite confirmation dialog
    bool m_showOverwriteDialog;
    int m_overwriteSlotNumber;
    sf::Text m_overwriteTitle;
    sf::Text m_overwriteMessage;
    sf::Text m_overwriteOptions;
    sf::RectangleShape m_overwriteBackground;
    sf::RectangleShape m_overwriteBorder;
    bool m_overwriteConfirm; // true = Yes, false = No
    
    // UI elements
    sf::Text m_titleText;
    std::vector<sf::Text> m_slotTexts;
    std::vector<sf::Text> m_slotSecondaryTexts; // Second line of slot info
    std::vector<sf::RectangleShape> m_slotBackgrounds;
    sf::RectangleShape m_selectionHighlight;
    
    // Navigation
    int m_selectedSlot;
    int m_scrollOffset;
    int m_maxVisibleSlots; // Calculated dynamically based on screen size
    static const int SLOT_HEIGHT = 90; // Bigger slots for better readability
    
    // Input handling
    float m_inputTimer;
    float m_inputDelay;
    
    // UI layout
    void setupUI();
    void updateSlotDisplay();
    void renderSlots();
    void renderSlotInfo(const SaveSlot& slot, int displayIndex);
    
    // Actions
    void selectSlot();
    void deleteSlot();
    void performSave(int slotNumber);
    void goBack();
    void scrollUp();
    void scrollDown();
    
    // Overwrite dialog
    void showOverwriteDialog(int slotNumber);
    void hideOverwriteDialog();
    void handleOverwriteInput(const Action& action);
    void setupOverwriteDialog();
    void renderOverwriteDialog();
    
    // Utility
    std::string getSlotDisplayText(const SaveSlot& slot);
    std::string getSlotSecondaryText(const SaveSlot& slot);
    std::string getModeTitle();
    sf::Color getSlotColor(const SaveSlot& slot);
};
