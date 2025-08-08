#pragma once
#include "character.hpp"

class NPC : public Character {
private:
    std::string m_textureName;
    std::string m_dialogueFile;
    bool m_canInteract;
    
public:
    NPC(GameEngine* game, EntityManager* entityManager, const std::string& name, const std::string& textureName, 
        const Vec2& tileSize, const std::string& dialogueFile = "");
    
    // Override base class methods
    void init(const Vec2& position) override;
    void update(float deltaTime) override;
    
    // NPC-specific methods
    void setupComponents(const Vec2& position);
    void setupAnimations();
    
    // Interaction methods
    bool canInteract() const { return m_canInteract && !m_dialogueFile.empty(); }
    const std::string& getDialogueFile() const { return m_dialogueFile; }
    void setDialogueFile(const std::string& dialogueFile) { m_dialogueFile = dialogueFile; }
    
    // Distance check for interaction
    bool isNearPlayer(const Vec2& playerPos, float interactionRange) const;
};
