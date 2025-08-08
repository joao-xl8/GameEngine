#pragma once
#include "engine_components.hpp"
#include <string>
#include <vector>
#include <functional>

// NPC dialogue state and management
class CNPCDialogue : public Component {
public:
    enum class DialogueState {
        AVAILABLE,      // Can start dialogue
        IN_PROGRESS,    // Currently in dialogue
        COMPLETED,      // Dialogue finished
        LOCKED          // Dialogue not available (quest requirements, etc.)
    };
    
    DialogueState state = DialogueState::AVAILABLE;
    std::string currentDialogueFile;
    std::string defaultDialogueFile;
    
    // Dialogue context and flags
    std::vector<std::string> dialogueFlags; // Story flags that affect dialogue
    std::string lastDialogueId;
    int dialogueCount = 0; // How many times player has talked to this NPC
    
    // Quest-related dialogue
    std::string questDialogueFile;
    bool hasQuest = false;
    bool questCompleted = false;
    
    CNPCDialogue(const std::string& defaultFile = "") 
        : defaultDialogueFile(defaultFile), currentDialogueFile(defaultFile) {}
    
    void setDialogueFile(const std::string& filename) {
        currentDialogueFile = filename;
    }
    
    void addDialogueFlag(const std::string& flag) {
        if (std::find(dialogueFlags.begin(), dialogueFlags.end(), flag) == dialogueFlags.end()) {
            dialogueFlags.push_back(flag);
        }
    }
    
    bool hasDialogueFlag(const std::string& flag) const {
        return std::find(dialogueFlags.begin(), dialogueFlags.end(), flag) != dialogueFlags.end();
    }
    
    void incrementDialogueCount() {
        dialogueCount++;
    }
    
    std::string getActiveDialogueFile() const {
        if (hasQuest && !questCompleted && !questDialogueFile.empty()) {
            return questDialogueFile;
        }
        return currentDialogueFile;
    }
};

// NPC interaction rules and requirements
class CNPCInteraction : public Component {
public:
    bool canInteract = true;
    float interactionRange = 80.0f;
    std::string interactionPrompt = "Press E to talk";
    
    // Interaction requirements
    std::vector<std::string> requiredItems;
    std::vector<std::string> requiredFlags;
    int requiredLevel = 0;
    
    // Interaction callbacks
    std::function<bool()> canInteractCallback;
    std::function<void()> onInteractionStart;
    std::function<void()> onInteractionEnd;
    
    // Interaction state
    bool isPlayerNearby = false;
    bool isInteracting = false;
    float lastInteractionTime = 0.0f;
    
    CNPCInteraction(float range = 80.0f) : interactionRange(range) {}
    
    bool checkRequirements() const {
        // This would check against player state, inventory, etc.
        // For now, just return true
        return canInteract;
    }
    
    void setInteractionCallback(std::function<bool()> callback) {
        canInteractCallback = callback;
    }
    
    void setInteractionPrompt(const std::string& prompt) {
        interactionPrompt = prompt;
    }
};

