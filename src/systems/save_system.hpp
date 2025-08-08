#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <ctime>

// Forward declarations
class GameEngine;
class Scene_Play;

struct SaveData {
    // Game state
    std::string currentLevel;
    float playerX, playerY;
    int playerHealth;
    
    // Game progress
    std::map<std::string, bool> flags; // Story flags, completed events, etc.
    std::map<std::string, int> variables; // Numeric game variables
    std::vector<std::string> inventory; // Player inventory items
    
    // Metadata
    std::string saveName;
    std::string levelName;
    std::time_t saveTime;
    int playTimeSeconds;
    
    SaveData() : playerX(0), playerY(0), playerHealth(100), playTimeSeconds(0) {
        saveTime = std::time(nullptr);
    }
};

struct SaveSlot {
    int slotNumber;
    bool isEmpty;
    SaveData data;
    std::string displayName;
    std::string timeString;
    
    SaveSlot(int slot) : slotNumber(slot), isEmpty(true) {}
};

class SaveSystem {
public:
    static const int AUTO_SAVE_SLOT = 0;
    static const int MAX_MANUAL_SAVE_SLOTS = 99;
    static const int TOTAL_SAVE_SLOTS = 100; // 0 (auto) + 1-99 (manual)
    static const std::string SAVE_DIRECTORY;
    
    SaveSystem();
    ~SaveSystem();
    
    // Save operations
    bool saveGame(int slotNumber, const SaveData& data);
    bool loadGame(int slotNumber, SaveData& data);
    bool deleteSave(int slotNumber);
    
    // Slot management
    std::vector<SaveSlot> getAllSaveSlots();
    SaveSlot getSaveSlot(int slotNumber);
    bool isSlotEmpty(int slotNumber);
    
    // Auto-save functionality (uses slot 0)
    void autoSave(const SaveData& data);
    bool hasAutoSave();
    SaveData loadAutoSave();
    
    // Manual save validation
    bool isValidManualSlot(int slotNumber);
    
    // Utility functions
    std::string formatSaveTime(std::time_t time);
    std::string formatPlayTime(int seconds);
    bool createSaveDirectory();
    
private:
    std::string getSaveFilePath(int slotNumber);
    std::string getAutoSaveFilePath();
    bool saveToFile(const std::string& filepath, const SaveData& data);
    bool loadFromFile(const std::string& filepath, SaveData& data);
};
