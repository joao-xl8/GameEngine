#include "save_system.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <iomanip>

const std::string SaveSystem::SAVE_DIRECTORY = "saves/";

SaveSystem::SaveSystem() {
    createSaveDirectory();
}

SaveSystem::~SaveSystem() {}

bool SaveSystem::createSaveDirectory() {
    try {
        if (!std::filesystem::exists(SAVE_DIRECTORY)) {
            std::filesystem::create_directories(SAVE_DIRECTORY);
            std::cout << "Created save directory: " << SAVE_DIRECTORY << std::endl;
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to create save directory: " << e.what() << std::endl;
        return false;
    }
}

std::string SaveSystem::getSaveFilePath(int slotNumber) {
    return SAVE_DIRECTORY + "save_slot_" + std::to_string(slotNumber) + ".sav";
}

std::string SaveSystem::getAutoSaveFilePath() {
    return SAVE_DIRECTORY + "autosave.sav";
}

bool SaveSystem::saveGame(int slotNumber, const SaveData& data) {
    if (slotNumber < 0 || slotNumber > MAX_MANUAL_SAVE_SLOTS) {
        std::cerr << "Invalid save slot number: " << slotNumber << " (valid range: 0-" << MAX_MANUAL_SAVE_SLOTS << ")" << std::endl;
        return false;
    }
    
    std::string filepath = getSaveFilePath(slotNumber);
    return saveToFile(filepath, data);
}

bool SaveSystem::loadGame(int slotNumber, SaveData& data) {
    if (slotNumber < 0 || slotNumber > MAX_MANUAL_SAVE_SLOTS) {
        std::cerr << "Invalid save slot number: " << slotNumber << " (valid range: 0-" << MAX_MANUAL_SAVE_SLOTS << ")" << std::endl;
        return false;
    }
    
    std::string filepath = getSaveFilePath(slotNumber);
    return loadFromFile(filepath, data);
}

bool SaveSystem::deleteSave(int slotNumber) {
    if (slotNumber < 0 || slotNumber > MAX_MANUAL_SAVE_SLOTS) {
        return false;
    }
    
    // Don't allow deleting auto-save slot
    if (slotNumber == AUTO_SAVE_SLOT) {
        std::cout << "Cannot delete auto-save slot" << std::endl;
        return false;
    }
    
    std::string filepath = getSaveFilePath(slotNumber);
    try {
        if (std::filesystem::exists(filepath)) {
            std::filesystem::remove(filepath);
            std::cout << "Deleted save slot " << slotNumber << std::endl;
            return true;
        }
    } catch (const std::exception& e) {
        std::cerr << "Failed to delete save slot " << slotNumber << ": " << e.what() << std::endl;
    }
    return false;
}

bool SaveSystem::saveToFile(const std::string& filepath, const SaveData& data) {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "Failed to open save file for writing: " << filepath << std::endl;
            return false;
        }
        
        // Write save data in a simple format
        file << "SAVE_VERSION 1.0\n";
        file << "SAVE_NAME " << data.saveName << "\n";
        file << "LEVEL_NAME " << data.levelName << "\n";
        file << "CURRENT_LEVEL " << data.currentLevel << "\n";
        file << "PLAYER_X " << data.playerX << "\n";
        file << "PLAYER_Y " << data.playerY << "\n";
        file << "PLAYER_HEALTH " << data.playerHealth << "\n";
        file << "SAVE_TIME " << data.saveTime << "\n";
        file << "PLAY_TIME " << data.playTimeSeconds << "\n";
        
        // Write flags
        file << "FLAGS_START\n";
        for (const auto& flag : data.flags) {
            file << flag.first << " " << (flag.second ? "1" : "0") << "\n";
        }
        file << "FLAGS_END\n";
        
        // Write variables
        file << "VARIABLES_START\n";
        for (const auto& var : data.variables) {
            file << var.first << " " << var.second << "\n";
        }
        file << "VARIABLES_END\n";
        
        // Write inventory
        file << "INVENTORY_START\n";
        for (const auto& item : data.inventory) {
            file << item << "\n";
        }
        file << "INVENTORY_END\n";
        
        file.close();
        std::cout << "Game saved to: " << filepath << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error saving game: " << e.what() << std::endl;
        return false;
    }
}

bool SaveSystem::loadFromFile(const std::string& filepath, SaveData& data) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false; // File doesn't exist or can't be opened
        }
        
        std::string line;
        std::string section = "";
        
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;
            
            std::istringstream iss(line);
            std::string key;
            iss >> key;
            
            if (key == "SAVE_VERSION") {
                // Version check could go here
            } else if (key == "SAVE_NAME") {
                std::getline(iss, data.saveName);
                if (!data.saveName.empty() && data.saveName[0] == ' ') {
                    data.saveName = data.saveName.substr(1);
                }
            } else if (key == "LEVEL_NAME") {
                std::getline(iss, data.levelName);
                if (!data.levelName.empty() && data.levelName[0] == ' ') {
                    data.levelName = data.levelName.substr(1);
                }
            } else if (key == "CURRENT_LEVEL") {
                iss >> data.currentLevel;
            } else if (key == "PLAYER_X") {
                iss >> data.playerX;
            } else if (key == "PLAYER_Y") {
                iss >> data.playerY;
            } else if (key == "PLAYER_HEALTH") {
                iss >> data.playerHealth;
            } else if (key == "SAVE_TIME") {
                iss >> data.saveTime;
            } else if (key == "PLAY_TIME") {
                iss >> data.playTimeSeconds;
            } else if (key == "FLAGS_START") {
                section = "FLAGS";
            } else if (key == "FLAGS_END") {
                section = "";
            } else if (key == "VARIABLES_START") {
                section = "VARIABLES";
            } else if (key == "VARIABLES_END") {
                section = "";
            } else if (key == "INVENTORY_START") {
                section = "INVENTORY";
            } else if (key == "INVENTORY_END") {
                section = "";
            } else if (section == "FLAGS") {
                std::string flagName = key;
                int flagValue;
                iss >> flagValue;
                data.flags[flagName] = (flagValue == 1);
            } else if (section == "VARIABLES") {
                std::string varName = key;
                int varValue;
                iss >> varValue;
                data.variables[varName] = varValue;
            } else if (section == "INVENTORY") {
                data.inventory.push_back(key);
            }
        }
        
        file.close();
        std::cout << "Game loaded from: " << filepath << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error loading game: " << e.what() << std::endl;
        return false;
    }
}

std::vector<SaveSlot> SaveSystem::getAllSaveSlots() {
    std::vector<SaveSlot> slots;
    slots.reserve(TOTAL_SAVE_SLOTS);
    
    std::cout << "Loading all save slots..." << std::endl;
    
    // Add auto-save slot (slot 0) first
    SaveSlot autoSlot(AUTO_SAVE_SLOT);
    if (loadGame(AUTO_SAVE_SLOT, autoSlot.data)) {
        autoSlot.isEmpty = false;
        autoSlot.displayName = "Auto-Save";
        autoSlot.timeString = formatSaveTime(autoSlot.data.saveTime);
        std::cout << "Auto-save slot: OCCUPIED" << std::endl;
    } else {
        autoSlot.isEmpty = true;
        autoSlot.displayName = "Auto-Save (Empty)";
        autoSlot.timeString = "";
        std::cout << "Auto-save slot: EMPTY" << std::endl;
    }
    slots.push_back(autoSlot);
    
    // Add manual save slots (1-99)
    for (int i = 1; i <= MAX_MANUAL_SAVE_SLOTS; i++) {
        SaveSlot slot(i);
        
        if (loadGame(i, slot.data)) {
            slot.isEmpty = false;
            slot.displayName = slot.data.saveName.empty() ? 
                ("Save " + std::to_string(i)) : slot.data.saveName;
            slot.timeString = formatSaveTime(slot.data.saveTime);
            std::cout << "Slot " << i << ": OCCUPIED (" << slot.displayName << ")" << std::endl;
        } else {
            slot.isEmpty = true;
            slot.displayName = "Empty Slot";
            slot.timeString = "";
            std::cout << "Slot " << i << ": EMPTY" << std::endl;
        }
        
        slots.push_back(slot);
    }
    
    std::cout << "Loaded " << slots.size() << " save slots" << std::endl;
    return slots;
}

SaveSlot SaveSystem::getSaveSlot(int slotNumber) {
    SaveSlot slot(slotNumber);
    
    if (slotNumber >= 0 && slotNumber <= MAX_MANUAL_SAVE_SLOTS) {
        if (loadGame(slotNumber, slot.data)) {
            slot.isEmpty = false;
            if (slotNumber == AUTO_SAVE_SLOT) {
                slot.displayName = "Auto-Save";
            } else {
                slot.displayName = slot.data.saveName.empty() ? 
                    ("Save " + std::to_string(slotNumber)) : slot.data.saveName;
            }
            slot.timeString = formatSaveTime(slot.data.saveTime);
        } else {
            slot.isEmpty = true;
            if (slotNumber == AUTO_SAVE_SLOT) {
                slot.displayName = "Auto-Save (Empty)";
            } else {
                slot.displayName = "Empty Slot";
            }
        }
    }
    
    return slot;
}

bool SaveSystem::isSlotEmpty(int slotNumber) {
    if (slotNumber < 0 || slotNumber > MAX_MANUAL_SAVE_SLOTS) {
        return true;
    }
    
    std::string filepath = getSaveFilePath(slotNumber);
    return !std::filesystem::exists(filepath);
}

bool SaveSystem::isValidManualSlot(int slotNumber) {
    return slotNumber >= 1 && slotNumber <= MAX_MANUAL_SAVE_SLOTS;
}

void SaveSystem::autoSave(const SaveData& data) {
    if (saveGame(AUTO_SAVE_SLOT, data)) {
        std::cout << "Auto-saved game to slot " << AUTO_SAVE_SLOT << std::endl;
    } else {
        std::cout << "Failed to auto-save game" << std::endl;
    }
}

bool SaveSystem::hasAutoSave() {
    return !isSlotEmpty(AUTO_SAVE_SLOT);
}

SaveData SaveSystem::loadAutoSave() {
    SaveData data;
    loadGame(AUTO_SAVE_SLOT, data);
    return data;
}

std::string SaveSystem::formatSaveTime(std::time_t time) {
    std::tm* tm = std::localtime(&time);
    std::ostringstream oss;
    oss << std::put_time(tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string SaveSystem::formatPlayTime(int seconds) {
    int hours = seconds / 3600;
    int minutes = (seconds % 3600) / 60;
    int secs = seconds % 60;
    
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << hours << ":"
        << std::setfill('0') << std::setw(2) << minutes << ":"
        << std::setfill('0') << std::setw(2) << secs;
    return oss.str();
}
