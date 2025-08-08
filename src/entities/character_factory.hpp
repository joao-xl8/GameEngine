#pragma once
#include "player.hpp"
#include "npc.hpp"
#include "../entity_manager.hpp"
#include <memory>
#include <unordered_map>

class GameEngine;

class CharacterFactory {
private:
    GameEngine* m_game;
    EntityManager* m_entityManager;
    Vec2 m_tileSize;
    
    // NPC dialogue file mappings
    std::unordered_map<std::string, std::string> m_npcDialogues;
    
public:
    CharacterFactory(GameEngine* game, EntityManager* entityManager, const Vec2& tileSize);
    
    // Character creation methods
    std::unique_ptr<Player> createPlayer(const Vec2& position);
    std::unique_ptr<NPC> createNPC(const std::string& name, const std::string& textureName, 
                                   const Vec2& position, const std::string& dialogueFile = "");
    
    // NPC dialogue management
    void registerNPCDialogue(const std::string& npcName, const std::string& dialogueFile);
    std::string getNPCDialogueFile(const std::string& npcName) const;
    
    // Utility methods
    void setupDefaultNPCDialogues();
};
