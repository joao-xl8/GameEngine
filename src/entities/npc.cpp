#include "npc.hpp"
#include "../game_engine.hpp"
#include "../components/common_components.hpp"
#include <iostream>
#include <cmath>

NPC::NPC(GameEngine* game, EntityManager* entityManager, const std::string& name, const std::string& textureName, 
         const Vec2& tileSize, const std::string& dialogueFile)
    : Character(game, entityManager, name, tileSize), m_textureName(textureName), 
      m_dialogueFile(dialogueFile), m_canInteract(true) {
}

void NPC::init(const Vec2& position) {
    // Create the entity first
    createEntity("NPC"); // NPCs are their own entity type
    
    std::cout << "Initializing NPC '" << m_name << "' at position: " << position.x << ", " << position.y << std::endl;
    
    setupComponents(position);
    
    std::cout << "NPC '" << m_name << "' initialization complete" << std::endl;
}

void NPC::setupComponents(const Vec2& position) {
    addComponent<CTransform>(std::make_shared<CTransform>(position));
    
    auto& npcTexture = m_game->getAssets().getTexture(m_textureName);
    addComponent<CSprite>(std::make_shared<CSprite>(m_textureName, npcTexture));
   
    addComponent<CBoundingBox>(std::make_shared<CBoundingBox>(m_tileSize));
}

void NPC::update(float deltaTime) {
    // NPC-specific update logic can go here
    // For example: patrol behavior, idle animations, etc.
}

bool NPC::isNearPlayer(const Vec2& playerPos, float interactionRange) const {
    Vec2 npcPos = getPosition();
    float distance = sqrt(pow(playerPos.x - npcPos.x, 2) + pow(playerPos.y - npcPos.y, 2));
    return distance <= interactionRange;
}
