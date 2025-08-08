#include "character_factory.hpp"
#include "../game_engine.hpp"

CharacterFactory::CharacterFactory(GameEngine* game, EntityManager* entityManager, const Vec2& tileSize)
    : m_game(game), m_entityManager(entityManager), m_tileSize(tileSize) {
    setupDefaultNPCDialogues();
}

std::unique_ptr<Player> CharacterFactory::createPlayer(const Vec2& position) {
    auto player = std::make_unique<Player>(m_game, m_entityManager, m_tileSize);
    player->init(position);
    return player;
}

std::unique_ptr<NPC> CharacterFactory::createNPC(const std::string& name, const std::string& textureName, 
                                                  const Vec2& position, const std::string& dialogueFile) {
    // Use provided dialogue file or look up default
    std::string actualDialogueFile = dialogueFile;
    if (actualDialogueFile.empty()) {
        actualDialogueFile = getNPCDialogueFile(textureName);
    }
    
    auto npc = std::make_unique<NPC>(m_game, m_entityManager, name, textureName, m_tileSize, actualDialogueFile);
    npc->init(position);
    return npc;
}

void CharacterFactory::registerNPCDialogue(const std::string& npcName, const std::string& dialogueFile) {
    m_npcDialogues[npcName] = dialogueFile;
}

std::string CharacterFactory::getNPCDialogueFile(const std::string& npcName) const {
    auto it = m_npcDialogues.find(npcName);
    return (it != m_npcDialogues.end()) ? it->second : "";
}

void CharacterFactory::setupDefaultNPCDialogues() {
    // Register default dialogue files for different NPC types
    registerNPCDialogue("Dummy", "metadata/dialogues/sample_dialogue.txt");
    
    // Add more NPCs as needed
    // registerNPCDialogue("Merchant", "metadata/dialogues/merchant_dialogue.txt");
    // registerNPCDialogue("Guard", "metadata/dialogues/guard_dialogue.txt");
}
