#include "scene_play.hpp"
#include "../components.hpp"
#include "scene_menu.hpp"
#include "scene_loading.hpp"
#include "scene_dialogue.hpp"
#include "scene_battle.hpp"
#include "scene_save_load.hpp"
#include "../game_engine.hpp"
#include <fstream>
#include <sstream>
#include <chrono>

void Scene_Play::init(const std::string &levelPath)
{
    // Game controls
    registerAction(sf::Keyboard::Escape, "PAUSE");
    registerAction(sf::Keyboard::C, "RESUME");
    registerAction(sf::Keyboard::T, "TOGGLE_TEXTURE");
    registerAction(sf::Keyboard::K, "TOGGLE_COLLISION");
    registerAction(sf::Keyboard::G, "TOGGLE_GRID");
    
    // Battle trigger for testing
    registerAction(sf::Keyboard::B, "BATTLE");
    
    // Interaction controls
    registerAction(sf::Keyboard::E, "INTERACT");
    
    // Player movement controls
    registerAction(sf::Keyboard::W, "UP");
    registerAction(sf::Keyboard::A, "LEFT");
    registerAction(sf::Keyboard::S, "DOWN");
    registerAction(sf::Keyboard::D, "RIGHT");
    
    // Standard confirm control
    registerAction(sf::Keyboard::Space, "SELECT");

    m_tileText.setCharacterSize(18);  // Increased from 16 to 18
    m_tileText.setFont(m_game->getAssets().getFont("ShareTech"));
    m_tileText.setFillColor(sf::Color::White);
    
    // Initialize interaction prompt text
    m_interactionPrompt.setCharacterSize(16);
    m_interactionPrompt.setFont(m_game->getAssets().getFont("ShareTech"));
    m_interactionPrompt.setFillColor(sf::Color::Yellow);
    m_interactionPrompt.setString("Press E to talk");

    // Initialize local sound manager for scene-specific sounds
    m_soundManager = std::make_shared<CSound>();
    
    // Load scene-specific sound effects (not background music)
    m_soundManager->addSound("walk", "assets/sounds/tap.wav");
    m_soundManager->addSound("hurt", "assets/sounds/hurt.wav");
    m_soundManager->addSound("jump", "assets/sounds/jump.wav");
    m_soundManager->addSound("coin", "assets/sounds/coin.wav");
    m_soundManager->addSound("power_up", "assets/sounds/power_up.wav");
    m_soundManager->addSound("explosion", "assets/sounds/explosion.wav");
    
    // Background music is handled by global sound manager - no need to start it here
    std::printf("Scene_Play sound effects loaded (background music handled globally)\n");

    // load levels
    // Tile Ground 0 0
    // Tile Ground 1 0
    // Tile Ground 2 0
    // Tile Ground 3 0
    // Dec Bushing 0 1
    std::printf("Loading level: %s\n", levelPath.c_str());
    std::ifstream file(levelPath);
    if (!file.is_open())
    {
        std::cerr << "Failed to open level file: " << levelPath << std::endl;
        return;
    }
    std::string line;
    while (std::getline(file, line))
    {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        std::stringstream ss(line);
        std::string layerStr;
        std::string spriteName;
        int x, y;
        std::string scriptName = ""; // Optional script name for Script Tiles
        
        ss >> layerStr >> spriteName >> x >> y;
        
        // Parse optional script name for Script Tiles
        if (ss >> scriptName) {
            // Script name was provided
        }
        
        // Parse layer number
        int layerNum = -1;
        try {
            layerNum = std::stoi(layerStr);
        } catch (const std::exception& e) {
            std::printf("Invalid layer number '%s' in level file, skipping line\n", layerStr.c_str());
            continue;
        }
        
        // Validate layer number
        if (layerNum < 0 || layerNum > 4) {
            std::printf("Layer number %d out of range (0-4), skipping line\n", layerNum);
            continue;
        }
        
        // Create entity based on layer
        CLayer::LayerType layer = static_cast<CLayer::LayerType>(layerNum);
        auto e = m_entityManager.addEntity("LayeredTile");
        
        // Add basic components
        e->addComponent<CTransform>(std::make_shared<CTransform>(Vec2{x * m_tileSize.x, y * m_tileSize.y}));
        e->addComponent<CSprite>(std::make_shared<CSprite>(spriteName, m_game->getAssets().getTexture(spriteName)));
        e->addComponent<CLayer>(std::make_shared<CLayer>(layer));
        
        // Add collision for decoration layers (1-3)
        if (layer >= CLayer::DECORATION_1 && layer <= CLayer::DECORATION_3) {
            e->addComponent<CBoundingBox>(std::make_shared<CBoundingBox>(m_tileSize));
        }
        
        // Handle special entity layer objects (layer 4)
        if (layer == CLayer::ENTITY) {
            // Handle PlayerSpawn
            if (spriteName == "PlayerSpawn") {
                m_levelSpawnPosition = Vec2{x * m_tileSize.x, y * m_tileSize.y};
                m_hasLevelSpawn = true;
                std::printf("Found PlayerSpawn at position (%d, %d) -> world pos (%.1f, %.1f)\n", 
                           x, y, m_levelSpawnPosition.x, m_levelSpawnPosition.y);
                
                // Add visual indicator for spawn point
                auto animationComponent = std::make_shared<CAnimation>(Vec2{static_cast<float>(m_gameScale), static_cast<float>(m_gameScale)});
                animationComponent->addAnimation("spawn", "PlayerSpawn", 1, 1.0f, true, 0);
                animationComponent->play("spawn");
                e->addComponent<CAnimation>(animationComponent);
            }
            // Handle SavePoint
            else if (spriteName == "SavePoint") {
                e->addComponent<CSave>(std::make_shared<CSave>("SavePoint_" + std::to_string(x) + "_" + std::to_string(y), "Save Game"));
                
                // Add animation to save point
                auto animationComponent = std::make_shared<CAnimation>(Vec2{static_cast<float>(m_gameScale), static_cast<float>(m_gameScale)});
                animationComponent->addAnimation("pulse", "SavePoint", 1, 0.8f, true, 0);
                animationComponent->play("pulse");
                e->addComponent<CAnimation>(animationComponent);
                
                std::printf("Created save point at position (%d, %d)\n", x, y);
            }
            // Handle NPCs
            else if (spriteName == "Dummy") {
                // Change entity tag to NPC for easier identification
                e = m_entityManager.addEntity("NPC");
                e->addComponent<CTransform>(std::make_shared<CTransform>(Vec2{x * m_tileSize.x, y * m_tileSize.y}));
                e->addComponent<CSprite>(std::make_shared<CSprite>(spriteName, m_game->getAssets().getTexture(spriteName)));
                e->addComponent<CLayer>(std::make_shared<CLayer>(layer));
                
                // Add animation for NPCs
                auto animationComponent = std::make_shared<CAnimation>(Vec2{static_cast<float>(m_gameScale), static_cast<float>(m_gameScale)});
                animationComponent->addAnimation("idle", "Dummy", 1, 1.0f, true, 0);
                animationComponent->play("idle");
                e->addComponent<CAnimation>(animationComponent);
                
                std::printf("Loading NPC: %s at position (%d, %d)\n", spriteName.c_str(), x, y);
            }
            // Handle Script Tiles
            else if (!scriptName.empty()) {
                // This is a Script Tile with a script to execute
                e->addComponent<CScriptTile>(std::make_shared<CScriptTile>(scriptName, CScriptTile::ON_ENTER, true));
                std::printf("Created Script Tile '%s' with script '%s' at position (%d, %d)\n", 
                           spriteName.c_str(), scriptName.c_str(), x, y);
            }
        }
        
        std::printf("Loaded %s '%s' on layer %d at position (%d, %d)\n", 
                   CLayer::getLayerName(layer), spriteName.c_str(), layerNum, x, y);
    }
    file.close();
    std::printf("Level loaded\n");
    
    // Create player entity
    spawnPlayer();
}

void Scene_Play::init()
{
    init(m_levelPath);
}

void Scene_Play::onEnd()
{
}

void Scene_Play::sAnimation()
{
    for (auto &entity : m_entityManager.getEntities())
    {
        if (entity->hasComponent<CAnimation>() && entity->hasComponent<CSprite>())
        {
            auto animation = entity->getComponent<CAnimation>();
            auto sprite = entity->getComponent<CSprite>();
            
            // Let the animation component handle its own logic
            animation->update(m_deltaTime, sprite->sprite);
        }
    }
}

void Scene_Play::sCamera()
{
    // Update camera to follow player
    if (m_player && m_player->hasComponent<CCamera>() && m_player->hasComponent<CTransform>())
    {
        auto camera = m_player->getComponent<CCamera>();
        auto transform = m_player->getComponent<CTransform>();
        
        if (camera && transform) {
            // Store previous camera position for debugging
            Vec2 prevCameraPos = camera->position;
            
            // Update camera to follow player position
            camera->followTarget(transform->pos, m_deltaTime);
            
            // Update the game view to match camera position
            sf::View& gameView = m_game->getGameView();
            
            // Simple camera centering - no coordinate conversion needed
            gameView.setCenter(camera->position.x, camera->position.y);
            
            // Enhanced debug output (reduced frequency)
            static int debugCounter = 0;
            if (debugCounter++ % 60 == 0) { // Print every 60 frames (roughly 1 second)
                Vec2 offset = transform->pos - camera->position;
                std::printf("Player: (%.1f, %.1f) | Camera: (%.1f, %.1f) | Offset: (%.1f, %.1f) | Following: %s | CameraMoved: %s\n", 
                           transform->pos.x, transform->pos.y,
                           camera->position.x, camera->position.y,
                           offset.x, offset.y,
                           camera->isFollowing ? "Yes" : "No",
                           (prevCameraPos.x != camera->position.x || prevCameraPos.y != camera->position.y) ? "Yes" : "No");
            }
        }
    }
}

void Scene_Play::sCollision()
{
    // Skip collision handling for grid-based movement
    // Grid movement handles collisions during movement planning
    if (m_player && m_player->hasComponent<CGridMovement>()) {
        return;
    }
    
    // Player collision with tiles (for non-grid movement)
    if (m_player && m_player->hasComponent<CTransform>() && m_player->hasComponent<CBoundingBox>())
    {
        auto playerTransform = m_player->getComponent<CTransform>();
        auto playerBBox = m_player->getComponent<CBoundingBox>();
        
        // Store original position
        Vec2 originalPos = playerTransform->pos;
        
        // Check collision with all tile entities
        auto& tileEntities = m_entityManager.getEntities("Tile");
        for (auto &entity : tileEntities)
        {
            // Safety check: ensure entity is valid and active
            if (!entity || !entity->isActive()) {
                continue;
            }
            
            if (entity->hasComponent<CTransform>() && entity->hasComponent<CBoundingBox>())
            {
                auto tileTransform = entity->getComponent<CTransform>();
                auto tileBBox = entity->getComponent<CBoundingBox>();
                
                // Additional safety checks
                if (!tileTransform || !tileBBox) {
                    continue;
                }
                
                // Check if player overlaps with tile
                if (isColliding(playerTransform->pos, playerBBox->size, 
                               tileTransform->pos, tileBBox->size))
                {
                    // Play collision sound (when sound files are available)
                    if (m_player->hasComponent<CSound>())
                    {
                        auto sound = m_player->getComponent<CSound>();
                        // Uncomment when you have sound files:
                        // sound->playSound("collision");
                    }
                    
                    // Calculate overlap amounts
                    float overlapX = std::min(playerTransform->pos.x + playerBBox->size.x - tileTransform->pos.x,
                                            tileTransform->pos.x + tileBBox->size.x - playerTransform->pos.x);
                    float overlapY = std::min(playerTransform->pos.y + playerBBox->size.y - tileTransform->pos.y,
                                            tileTransform->pos.y + tileBBox->size.y - playerTransform->pos.y);
                    
                    // Resolve collision by moving player out of tile
                    if (overlapX < overlapY)
                    {
                        // Horizontal collision
                        if (playerTransform->pos.x < tileTransform->pos.x)
                        {
                            playerTransform->pos.x = tileTransform->pos.x - playerBBox->size.x;
                        }
                        else
                        {
                            playerTransform->pos.x = tileTransform->pos.x + tileBBox->size.x;
                        }
                        playerTransform->velocity.x = 0;
                    }
                    else
                    {
                        // Vertical collision
                        if (playerTransform->pos.y < tileTransform->pos.y)
                        {
                            playerTransform->pos.y = tileTransform->pos.y - playerBBox->size.y;
                        }
                        else
                        {
                            playerTransform->pos.y = tileTransform->pos.y + tileBBox->size.y;
                        }
                        playerTransform->velocity.y = 0;
                    }
                }
            }
        }
        
        // Keep player within window bounds
        float windowWidth = m_game->window().getSize().x;
        float windowHeight = m_game->window().getSize().y;
        
        if (playerTransform->pos.x < 0)
        {
            playerTransform->pos.x = 0;
            playerTransform->velocity.x = 0;
        }
        if (playerTransform->pos.x + playerBBox->size.x > windowWidth)
        {
            playerTransform->pos.x = windowWidth - playerBBox->size.x;
            playerTransform->velocity.x = 0;
        }
        if (playerTransform->pos.y < 0)
        {
            playerTransform->pos.y = 0;
            playerTransform->velocity.y = 0;
        }
        if (playerTransform->pos.y + playerBBox->size.y > windowHeight)
        {
            playerTransform->pos.y = windowHeight - playerBBox->size.y;
            playerTransform->velocity.y = 0;
        }
    }
}

void Scene_Play::sEnemySpawner()
{
}

void Scene_Play::sMovement()
{
    // Update grid movement timer
    if (m_gridMoveTimer > 0.0f) {
        m_gridMoveTimer -= m_deltaTime;
    }
    
    // Handle player grid movement
    if (m_player && m_player->hasComponent<CInput>() && m_player->hasComponent<CTransform>() && m_player->hasComponent<CGridMovement>())
    {
        auto input = m_player->getComponent<CInput>();
        auto transform = m_player->getComponent<CTransform>();
        auto gridMovement = m_player->getComponent<CGridMovement>();
        auto animation = m_player->getComponent<CAnimation>();
        auto boundingBox = m_player->getComponent<CBoundingBox>();
        auto sound = m_player->getComponent<CSound>();  // Get sound component
        
        // Create collision check function
        auto collisionCheck = [this](Vec2 pos, Vec2 size) -> bool {
            return wouldCollideAtPosition(pos, size);
        };
        
        // Handle grid movement based on key presses (not held keys)
        // Only allow movement if cooldown timer has expired
        bool moved = false;
        if (m_gridMoveTimer <= 0.0f && !gridMovement->isMoving) {
            if (input->upPressed) 
            {
                std::cout << "UP pressed - Current grid pos: " << gridMovement->gridPos.x << ", " << gridMovement->gridPos.y << std::endl;
                // W key = UP = Move towards top of screen = Decrease Y coordinate (SFML/Level system)
                if (gridMovement->startMoveWithCollisionCheck(Vec2{0, -1}, transform->pos, boundingBox->size, collisionCheck)) {
                    if (animation) animation->play("walk_up");
                    // Play walking sound only if sound is enabled
                    if (sound && m_game->isSoundEnabled()) {
                        float volume = m_game->getMasterVolume() * m_game->getEffectsVolume() * 70.0f;
                        sound->playSound("footstep", volume);
                    }
                    moved = true;
                    m_gridMoveTimer = m_changeGridSleep; // Start cooldown
                    std::cout << "UP movement successful - New grid pos: " << gridMovement->gridPos.x << ", " << gridMovement->gridPos.y << std::endl;
                }
            }
            else if (input->downPressed) 
            {
                // S key = DOWN = Move towards bottom of screen = Increase Y coordinate (SFML/Level system)
                if (gridMovement->startMoveWithCollisionCheck(Vec2{0, 1}, transform->pos, boundingBox->size, collisionCheck)) {
                    if (animation) animation->play("walk_down");
                    // Play walking sound only if sound is enabled
                    if (sound && m_game->isSoundEnabled()) {
                        float volume = m_game->getMasterVolume() * m_game->getEffectsVolume() * 70.0f;
                        sound->playSound("footstep", volume);
                    }
                    moved = true;
                    m_gridMoveTimer = m_changeGridSleep; // Start cooldown
                }
            }
            else if (input->leftPressed) 
            {
                if (gridMovement->startMoveWithCollisionCheck(Vec2{-1, 0}, transform->pos, boundingBox->size, collisionCheck)) {
                    if (animation) animation->play("walk_left");
                    // Play walking sound only if sound is enabled
                    if (sound && m_game->isSoundEnabled()) {
                        float volume = m_game->getMasterVolume() * m_game->getEffectsVolume() * 70.0f;
                        sound->playSound("footstep", volume);
                    }
                    moved = true;
                    m_gridMoveTimer = m_changeGridSleep; // Start cooldown
                }
            }
            else if (input->rightPressed) 
            {
                if (gridMovement->startMoveWithCollisionCheck(Vec2{1, 0}, transform->pos, boundingBox->size, collisionCheck)) {
                    if (animation) animation->play("walk_right");
                    // Play walking sound only if sound is enabled
                    if (sound && m_game->isSoundEnabled()) {
                        float volume = m_game->getMasterVolume() * m_game->getEffectsVolume() * 70.0f;
                        sound->playSound("footstep", volume);
                    }
                    moved = true;
                    m_gridMoveTimer = m_changeGridSleep; // Start cooldown
                }
            }
        }
        
        // Update position based on grid movement
        Vec2 gridWorldPos = gridMovement->updateMovement(m_deltaTime, transform->pos);
        
        // Direct positioning - no centering needed since player and tile are same size (64px)
        transform->pos = gridWorldPos;
        
        // Set idle animation if not moving
        if (!gridMovement->isMoving && !moved && animation)
        {
            animation->play("idle");
        }
        
        // Reset input press flags
        input->resetPressFlags();
        
        // Play movement sound if moving (when sound files are available)
        if (moved && m_player->hasComponent<CSound>())
        {
            auto sound = m_player->getComponent<CSound>();
            // Uncomment when you have sound files:
            // sound->playSound("footstep");
        }
    }
    
    // Handle other entity movement
    auto& allEntities = m_entityManager.getEntities();
    for (auto &entity : allEntities)
    {
        // Safety check: ensure entity is valid and active
        if (!entity || !entity->isActive()) {
            continue;
        }
        
        if (entity->hasComponent<CTransform>())
        {
            auto transform = entity->getComponent<CTransform>();
            if (!transform) {
                continue;
            }
            // Apply any general movement logic here
        }
    }
}

void Scene_Play::sRender()
{
    // GameEngine now handles window clearing and viewport setup
    // Set background color by drawing a full-screen rectangle
    sf::RectangleShape background;
    background.setSize(sf::Vector2f(m_game->window().getSize().x, m_game->window().getSize().y));
    background.setFillColor(sf::Color::Magenta);
    background.setPosition(0, 0);
    m_game->window().draw(background);
    
    if(m_drawTextures){
        // Collect all renderable entities with layer information
        std::vector<std::shared_ptr<Entity>> renderableEntities;
        
        for (auto &entity : m_entityManager.getEntities())
        {
            if (entity->hasComponent<CSprite>() && entity->hasComponent<CTransform>())
            {
                renderableEntities.push_back(entity);
            }
        }
        
        // Sort entities by layer order (0 -> 1 -> 2 -> 3 -> 4)
        std::sort(renderableEntities.begin(), renderableEntities.end(), 
            [](const std::shared_ptr<Entity>& a, const std::shared_ptr<Entity>& b) {
                // Get layer components (default to layer 0 if not present)
                int orderA = 0;
                int orderB = 0;
                
                if (a->hasComponent<CLayer>()) {
                    orderA = a->getComponent<CLayer>()->getRenderOrder();
                }
                if (b->hasComponent<CLayer>()) {
                    orderB = b->getComponent<CLayer>()->getRenderOrder();
                }
                
                return orderA < orderB;
            });
        
        // Render entities in layer order
        for (auto &entity : renderableEntities) {
            auto sprite = entity->getComponent<CSprite>();
            auto transform = entity->getComponent<CTransform>();
            
            // Use consistent top-down coordinate system (no Y-axis flip)
            float posX = transform->pos.x;
            float posY = transform->pos.y;
            
            sprite->sprite.setPosition(posX, posY);
            m_game->window().draw(sprite->sprite);
        }
    }
    if (m_drawGrid)
    {
        // Use consistent coordinate system (same as sprites)
        for (size_t i = 0; i < 30; i++)
        {
            for (size_t j = 0; j < 30; j++)
            {
                float posX = j * m_tileSize.x;
                float posY = i * m_tileSize.y; // Use normal Y-axis (same as sprites)

                m_tileText.setPosition(posX, posY);
                m_tileText.setString("(" + std::to_string(j) + ", " + std::to_string(i) + ")");
                m_game->window().draw(m_tileText);

                // draw the transparent square with white border for the tile
                sf::RectangleShape rect;
                rect.setSize({m_tileSize.x, m_tileSize.y});
                rect.setPosition(posX, posY);
                rect.setFillColor(sf::Color::Transparent);
                rect.setOutlineColor(sf::Color::White);
                rect.setOutlineThickness(1);
                m_game->window().draw(rect);
            }
        }
    }
    if(m_drawCollision){
        for (auto &entity : m_entityManager.getEntities())
        {
            if (entity->hasComponent<CBoundingBox>() && entity->hasComponent<CTransform>())
            {
                auto boundingBox = entity->getComponent<CBoundingBox>();
                auto transform = entity->getComponent<CTransform>();
                float posX = transform->pos.x;
                float posY = transform->pos.y; // Use normal Y-axis (same as sprites)
                sf::RectangleShape rect;
                rect.setSize({boundingBox->size.x, boundingBox->size.y});
                rect.setPosition(posX, posY);
                rect.setFillColor(sf::Color::Transparent);
                rect.setOutlineColor(sf::Color::Red);
                rect.setOutlineThickness(1);
                m_game->window().draw(rect);
            }
        }
    }
    
    // Draw interaction prompt if near an NPC
    if (m_showInteractionPrompt && m_nearbyNPC && m_nearbyNPC->hasComponent<CTransform>()) {
        auto npcTransform = m_nearbyNPC->getComponent<CTransform>();
        
        // Position the prompt above the NPC
        float promptX = npcTransform->pos.x;
        float promptY = npcTransform->pos.y - 30.0f;  // 30 pixels above the NPC
        
        m_interactionPrompt.setPosition(promptX, promptY);
        m_game->window().draw(m_interactionPrompt);
    }
    
    // Draw save prompt if near a save point
    if (m_showSavePrompt && m_nearbySavePoint) {
        m_game->window().draw(m_savePrompt);
    }
    
    // Draw command overlay
    renderCommandOverlay();
    
    // Draw pause menu on top of everything
    renderPauseMenu();
}

void Scene_Play::sDoAction(const Action &action)
{
    if (action.getType() == "START")
    {
        std::printf("Start action: %s\n", action.getName().c_str());
        
        // Handle pause menu input first
        if (m_showPauseMenu) {
            std::cout << "Pause menu is active, handling input: " << action.getName() << std::endl;
            handlePauseMenuInput(action);
            return;
        }
        
        if (action.getName() == "PAUSE")
        {
            std::cout << "PAUSE action triggered, showing pause menu" << std::endl;
            showPauseMenu();
        }
        else if (action.getName() == "TOGGLE_TEXTURE")
        {
            m_drawTextures = !m_drawTextures;
        }
        else if (action.getName() == "TOGGLE_COLLISION")
        {
            m_drawCollision = !m_drawCollision;
        }
        else if (action.getName() == "TOGGLE_GRID")
        {
            m_drawGrid = !m_drawGrid;
        }
        else if (action.getName() == "BATTLE")
        {
            // Trigger battle scene with configuration
            std::cout << "Battle triggered! Starting configured battle scene..." << std::endl;
            
            // Create battle scene with level 1 enemies
            std::vector<std::string> enemyTypes = {"GOBLIN", "SLIME"}; // Example encounter
            auto battleScene = std::make_shared<Scene_Battle>(m_game, enemyTypes);
            m_game->pushScene("Battle", battleScene);
        }
        else if (action.getName() == "INTERACT")
        {
            // Handle dialogue interaction with NPCs
            if (m_nearbyNPC != nullptr) {
                startDialogue(m_nearbyNPC);
            }
            // Handle save interaction with save points
            else if (m_nearbySavePoint != nullptr) {
                openSaveMenu();
            }
        }
        // Player movement input
        else if (m_player && m_player->hasComponent<CInput>())
        {
            auto input = m_player->getComponent<CInput>();
            if (action.getName() == "UP") {
                input->up = true;
                input->upPressed = true;
            }
            else if (action.getName() == "DOWN") {
                input->down = true;
                input->downPressed = true;
            }
            else if (action.getName() == "LEFT") {
                input->left = true;
                input->leftPressed = true;
            }
            else if (action.getName() == "RIGHT") {
                input->right = true;
                input->rightPressed = true;
            }
        }
    }
    else if (action.getType() == "END")
    {
        std::printf("End action: %s\n", action.getName().c_str());
        // Handle key release for player movement
        if (m_player && m_player->hasComponent<CInput>())
        {
            auto input = m_player->getComponent<CInput>();
            if (action.getName() == "UP") input->up = false;
            else if (action.getName() == "DOWN") input->down = false;
            else if (action.getName() == "LEFT") input->left = false;
            else if (action.getName() == "RIGHT") input->right = false;
        }
    }
}

void Scene_Play::sDebug()
{
    // if (m_showGrid)
    // {
    //     // drawGrid();
    // }
}

Vec2 Scene_Play::gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity)
{
    if (entity->hasComponent<CTransform>() && entity->hasComponent<CBoundingBox>())
    {
        Vec2 pos = entity->getComponent<CTransform>()->pos;
        Vec2 scale = entity->getComponent<CTransform>()->scale;
        Vec2 size = entity->getComponent<CBoundingBox>()->size;
        return Vec2{gridX * m_tileSize.x + m_tileSize.x / 2 - size.x * scale.x / 2 + pos.x, gridY * m_tileSize.y + m_tileSize.y / 2 - size.y * scale.y / 2 + pos.y};
    }

    return Vec2{0, 0};
}

Scene_Play::Scene_Play(GameEngine *game, const std::string &levelPath)
    : Scene(game), m_levelPath(levelPath)
{
    // Initialize game start time for play time tracking
    m_gameStartTime = std::chrono::steady_clock::now();
}

void Scene_Play::update()
{
    // Calculate delta time
    m_deltaTime = m_deltaClock.restart().asSeconds();
    
    // Don't update game systems when paused, but still render
    if (!m_paused) {
        m_entityManager.update();
        sMovement();
        sCollision();
        sInteraction();  // Check for NPC interactions
        sSaveSystem();   // Check for save point interactions
        sEnemySpawner();
        sAnimation();
        sCamera();  // Update camera system
    }
    
    // Always render (so we can see the pause menu)
    sRender();
    sDebug();
}

void Scene_Play::setCustomSpawnPosition(const Vec2& position)
{
    m_useDefaultSpawn = false;
    m_customSpawnPosition = position;
    std::cout << "Set custom spawn position: " << position.x << ", " << position.y << std::endl;
}

void Scene_Play::spawnPlayer()
{
    m_player = m_entityManager.addEntity("Player");
    
    // Determine spawn position priority:
    // 1. Custom spawn position (from save data) - highest priority
    // 2. Level spawn position (from PlayerSpawn tile) - medium priority  
    // 3. Default hardcoded position - lowest priority (fallback)
    Vec2 startPos;
    if (!m_useDefaultSpawn) {
        // Use custom spawn position from save data
        startPos = m_customSpawnPosition;
        std::cout << "Using custom spawn position from save: " << startPos.x << ", " << startPos.y << std::endl;
    } else if (m_hasLevelSpawn) {
        // Use spawn position from level file (PlayerSpawn tile)
        startPos = m_levelSpawnPosition;
        std::cout << "Using level spawn position from PlayerSpawn tile: " << startPos.x << ", " << startPos.y << std::endl;
    } else {
        // Fallback to default hardcoded position
        startPos = {10 * m_tileSize.x, 7 * m_tileSize.y};
        std::cout << "Using fallback default spawn position: " << startPos.x << ", " << startPos.y << std::endl;
        std::cout << "Warning: No PlayerSpawn tile found in level, using hardcoded position" << std::endl;
    }
    
    m_player->addComponent<CTransform>(std::make_shared<CTransform>(startPos));
    
    // Add grid movement component and initialize it properly
    auto gridMovement = std::make_shared<CGridMovement>(m_tileSize.x, 4.0f, true);
    
    // CRITICAL FIX: Initialize grid position based on actual spawn position
    gridMovement->snapToGrid(startPos);
    std::cout << "Initialized grid position to: " << gridMovement->gridPos.x << ", " << gridMovement->gridPos.y << std::endl;
    
    m_player->addComponent<CGridMovement>(gridMovement);
    
    // Add sprite component with player texture
    auto& playerTexture = m_game->getAssets().getTexture("Player");
    m_player->addComponent<CSprite>(std::make_shared<CSprite>("Player", playerTexture));
    
    // Set up sprite sheet frame (64x64 pixel frames based on the sprite sheet)
    auto sprite = m_player->getComponent<CSprite>();
    sprite->sprite.setTextureRect(sf::IntRect(0, 0, m_playerScale, m_playerScale)); // First frame
    
    // Add animation component with flexible animation definitions (using player scale)
    auto animationComponent = std::make_shared<CAnimation>(Vec2{static_cast<float>(m_playerScale), static_cast<float>(m_playerScale)});

    // Define animations for single-frame texture - all use the same frame
    animationComponent->addAnimation("idle", "Player", 1, 1.0f, true, 0);        // 1 frame, 1s duration, loop
    animationComponent->addAnimation("walk_down", "Player", 1, 0.5f, true, 0);   // 1 frame, 0.5s duration, loop
    animationComponent->addAnimation("walk_up", "Player", 1, 0.5f, true, 0);     // 1 frame, 0.5s duration, loop
    animationComponent->addAnimation("walk_right", "Player", 1, 0.5f, true, 0);  // 1 frame, 0.5s duration, loop
    animationComponent->addAnimation("walk_left", "Player", 1, 0.5f, true, 0);   // 1 frame, 0.5s duration, loop

    animationComponent->play("idle"); // Start with idle animation
    m_player->addComponent<CAnimation>(animationComponent);
    
    // Grid movement component is already added above with proper initialization
    
    // Add bounding box (using player scale for accurate collision)
    m_player->addComponent<CBoundingBox>(std::make_shared<CBoundingBox>(Vec2{static_cast<float>(m_playerScale), static_cast<float>(m_playerScale)}));
    
    // Add input component
    m_player->addComponent<CInput>(std::make_shared<CInput>());
    
    // Add camera component with 1-tile dead zone
    Vec2 deadZoneSize = {static_cast<float>(m_gameScale), static_cast<float>(m_gameScale)}; // 1 tile dead zone
    auto cameraComponent = std::make_shared<CCamera>(startPos, deadZoneSize, 3.0f); // Follow speed of 3.0
    
    // Ensure camera starts exactly at player position
    cameraComponent->setPosition(startPos);  // This sets both position and targetPosition
    
    m_player->addComponent<CCamera>(cameraComponent);
    
    // Initialize game view to center player in middle of screen
    sf::View& gameView = m_game->getGameView();
    
    // Set camera center directly to player position (no coordinate conversion needed)
    gameView.setCenter(startPos.x, startPos.y);
    
    std::printf("Camera initialized at position: %f, %f (player position)\n", startPos.x, startPos.y);
    
    // Add sound component
    auto soundComponent = std::make_shared<CSound>();
    // Load player-specific sounds
    soundComponent->addSound("footstep", "assets/sounds/tap.wav");
    soundComponent->addSound("hurt", "assets/sounds/hurt.wav");
    soundComponent->addSound("jump", "assets/sounds/jump.wav");
    m_player->addComponent<CSound>(soundComponent);
    
    std::printf("Player spawned at position: %f, %f\n", startPos.x, startPos.y);
}

bool Scene_Play::isColliding(const Vec2& pos1, const Vec2& size1, const Vec2& pos2, const Vec2& size2)
{
    return (pos1.x < pos2.x + size2.x &&
            pos1.x + size1.x > pos2.x &&
            pos1.y < pos2.y + size2.y &&
            pos1.y + size1.y > pos2.y);
}

bool Scene_Play::wouldCollideAtPosition(const Vec2& position, const Vec2& size)
{
    // Check window boundaries
    float windowWidth = m_game->window().getSize().x;
    float windowHeight = m_game->window().getSize().y;
    
    if (position.x < 0 || position.y < 0 || 
        position.x + size.x > windowWidth || 
        position.y + size.y > windowHeight) {
        return true; // Would be outside window bounds
    }
    
    // Check collision with entities that have collision (decoration layers 1-3)
    for (auto &entity : m_entityManager.getEntities())
    {
        // Safety check: ensure entity is valid and active
        if (!entity || !entity->isActive()) {
            continue;
        }
        
        // Only check collision with entities that have layer component and collision
        if (entity->hasComponent<CLayer>() && entity->hasComponent<CTransform>() && entity->hasComponent<CBoundingBox>())
        {
            auto layer = entity->getComponent<CLayer>();
            auto tileTransform = entity->getComponent<CTransform>();
            auto tileBBox = entity->getComponent<CBoundingBox>();
            
            // Additional safety checks
            if (!layer || !tileTransform || !tileBBox) {
                continue;
            }
            
            // Only check collision with tiles that have collision enabled
            auto collision = entity->getComponent<CCollision>();
            if (collision && collision->isCollidable())
            {
                if (isColliding(position, size, tileTransform->pos, tileBBox->size))
                {
                    return true; // Would collide with this tile
                }
            }
        }
        
        // Also check collision with old-style Tile entities for backward compatibility
        else if (entity->tag() == "Tile" && entity->hasComponent<CTransform>() && entity->hasComponent<CBoundingBox>())
        {
            auto tileTransform = entity->getComponent<CTransform>();
            auto tileBBox = entity->getComponent<CBoundingBox>();
            
            if (!tileTransform || !tileBBox) {
                continue;
            }
            
            if (isColliding(position, size, tileTransform->pos, tileBBox->size))
            {
                return true; // Would collide with this tile
            }
        }
    }
    
    return false; // No collision detected
}

// Dialogue interaction system implementation
void Scene_Play::sInteraction()
{
    if (!m_player || !m_player->hasComponent<CTransform>()) {
        return;
    }
    
    auto playerTransform = m_player->getComponent<CTransform>();
    Vec2 playerPos = playerTransform->pos;
    
    // Reset nearby NPC
    m_nearbyNPC = nullptr;
    m_showInteractionPrompt = false;
    
    // Check all NPC entities for interaction
    for (auto& entity : m_entityManager.getEntities("NPC")) {
        if (!entity->hasComponent<CTransform>() || !entity->hasComponent<CSprite>()) {
            continue;
        }
        
        auto entityTransform = entity->getComponent<CTransform>();
        auto entitySprite = entity->getComponent<CSprite>();
        
        Vec2 npcPos = entityTransform->pos;
        
        // Calculate distance between player and NPC
        float distance = sqrt(pow(playerPos.x - npcPos.x, 2) + pow(playerPos.y - npcPos.y, 2));
        
        if (distance <= m_interactionRange) {
            m_nearbyNPC = entity;
            m_showInteractionPrompt = true;
            break;  // Found nearby NPC, no need to check others
        }
    }
}

void Scene_Play::startDialogue(std::shared_ptr<Entity> npc)
{
    if (!npc || !npc->hasComponent<CSprite>()) {
        return;
    }
    
    auto npcSprite = npc->getComponent<CSprite>();
    std::string dialogueFile = getNPCDialogueFile(npcSprite->name);
    
    if (!dialogueFile.empty()) {
        std::cout << "Starting dialogue with NPC: " << npcSprite->name << std::endl;
        std::cout << "Using dialogue file: " << dialogueFile << std::endl;
        
        // Preserve current game state
        Vec2 currentPlayerPos = m_player->getComponent<CTransform>()->pos;
        int currentHealth = 100; // TODO: Get from player health component when implemented
        int currentPlayTime = static_cast<int>(std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::steady_clock::now() - m_gameStartTime).count());
        
        std::cout << "Preserving game state:" << std::endl;
        std::cout << "  Level: " << m_levelPath << std::endl;
        std::cout << "  Player Position: (" << currentPlayerPos.x << ", " << currentPlayerPos.y << ")" << std::endl;
        std::cout << "  Health: " << currentHealth << std::endl;
        std::cout << "  Play Time: " << currentPlayTime << " seconds" << std::endl;
        
        try {
            // Create dialogue scene with preserved game state
            auto dialogueScene = std::make_shared<Scene_Dialogue>(
                m_game, dialogueFile, m_levelPath, currentPlayerPos, currentHealth, currentPlayTime
            );
            m_game->pushScene("Dialogue", dialogueScene);
        } catch (const std::exception& e) {
            std::cout << "Error starting dialogue: " << e.what() << std::endl;
        }
    } else {
        std::cout << "No dialogue file found for NPC: " << npcSprite->name << std::endl;
    }
}

std::string Scene_Play::getNPCDialogueFile(const std::string& npcName)
{
    // Map NPC sprite names to their dialogue files
    if (npcName == "Dummy") {
        return "metadata/dialogues/npcs/dummy/enhanced_greeting.txt";
    }
    
    // Add more NPCs here as needed
    // if (npcName == "Merchant") {
    //     return "metadata/dialogues/npcs/merchant/shop_intro.txt";
    // }
    // if (npcName == "Guard") {
    //     return "metadata/dialogues/npcs/guard/patrol_chat.txt";
    // }
    
    return "";  // No dialogue file found
}

// Save System Implementation

void Scene_Play::sSaveSystem()
{
    if (!m_player) return;
    
    Vec2 playerPos = m_player->getComponent<CTransform>()->pos;
    m_nearbySavePoint = nullptr;
    m_showSavePrompt = false;
    
    // Check for nearby save points
    for (auto& entity : m_entityManager.getEntities()) {
        if (entity->hasComponent<CSave>()) {
            Vec2 savePos = entity->getComponent<CTransform>()->pos;
            float distance = sqrt(pow(playerPos.x - savePos.x, 2) + pow(playerPos.y - savePos.y, 2));
            
            if (distance <= m_interactionRange) {
                m_nearbySavePoint = entity;
                m_showSavePrompt = true;
                
                // Setup save prompt text
                try {
                    m_savePrompt.setFont(m_game->getAssets().getFont("ShareTech"));
                    m_savePrompt.setCharacterSize(16);
                    m_savePrompt.setFillColor(sf::Color::Yellow);
                    m_savePrompt.setString("Press E to Save Game");
                    
                    // Position above the save point
                    m_savePrompt.setPosition(savePos.x - 60, savePos.y - 40);
                } catch (const std::exception& e) {
                    std::cout << "Warning: Could not set save prompt font: " << e.what() << std::endl;
                }
                break;
            }
        }
    }
}

void Scene_Play::openSaveMenu()
{
    // Store current player position before opening save menu
    if (m_player) {
        m_playerPositionBeforeSave = m_player->getComponent<CTransform>()->pos;
        std::cout << "Stored player position before save: " << m_playerPositionBeforeSave.x << ", " << m_playerPositionBeforeSave.y << std::endl;
    }
    
    SaveData currentData = getCurrentGameData();
    auto saveScene = std::make_shared<Scene_SaveLoad>(m_game, Scene_SaveLoad::SAVE_MODE, currentData);
    m_game->changeScene("SaveLoad", saveScene);
}

SaveData Scene_Play::getCurrentGameData()
{
    SaveData data;
    
    // Basic game state
    data.currentLevel = m_levelPath;
    data.levelName = "Level 1"; // TODO: Get actual level name
    
    if (m_player) {
        Vec2 playerPos = m_player->getComponent<CTransform>()->pos;
        data.playerX = playerPos.x;
        data.playerY = playerPos.y;
        data.playerHealth = 100; // TODO: Get actual player health
    }
    
    // Calculate play time
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - m_gameStartTime);
    data.playTimeSeconds = static_cast<int>(duration.count());
    
    // TODO: Add game flags, variables, inventory
    
    return data;
}

void Scene_Play::applyLoadedGameData(const SaveData& data)
{
    std::cout << "Applying loaded game data..." << std::endl;
    std::cout << "Setting player spawn position to: " << data.playerX << ", " << data.playerY << std::endl;
    
    // Set custom spawn position BEFORE the level is loaded
    setCustomSpawnPosition(Vec2(data.playerX, data.playerY));
    
    // If player already exists, update position immediately
    if (m_player) {
        m_player->getComponent<CTransform>()->pos = Vec2(data.playerX, data.playerY);
        std::cout << "Updated existing player position" << std::endl;
    }
    
    // TODO: Apply other game state
    // - Set player health
    // - Apply game flags and variables
    // - Load inventory
    
    std::cout << "Applied loaded game data" << std::endl;
}

void Scene_Play::autoSaveGame()
{
    SaveData data = getCurrentGameData();
    m_saveSystem.autoSave(data);
}

// Pause Menu Implementation

void Scene_Play::showPauseMenu()
{
    m_showPauseMenu = true;
    m_pauseMenuSelection = 0; // Default to Resume
    setPaused(true); // Pause the game
    setupPauseMenu();
    std::cout << "Game paused - pause menu shown" << std::endl;
}

void Scene_Play::hidePauseMenu()
{
    m_showPauseMenu = false;
    setPaused(false); // Unpause the game
    std::cout << "Game resumed - pause menu hidden" << std::endl;
}

void Scene_Play::handlePauseMenuInput(const Action& action)
{
    std::cout << "Pause menu input: " << action.getName() << std::endl;
    
    if (action.getName() == "UP" || action.getName() == "DOWN" || 
        action.getName() == "LEFT" || action.getName() == "RIGHT") {
        // Toggle between Resume (0) and Main Menu (1)
        m_pauseMenuSelection = (m_pauseMenuSelection == 0) ? 1 : 0;
        std::cout << "Selected option: " << (m_pauseMenuSelection == 0 ? "Resume" : "Main Menu") << std::endl;
        setupPauseMenu(); // Update display
    } else if (action.getName() == "SELECT") {
        std::cout << "Confirming selection: " << (m_pauseMenuSelection == 0 ? "Resume" : "Main Menu") << std::endl;
        if (m_pauseMenuSelection == 0) {
            // Resume game
            std::cout << "Resuming game..." << std::endl;
            hidePauseMenu();
        } else {
            // Go to main menu
            std::cout << "Going to main menu..." << std::endl;
            hidePauseMenu();
            Scene_Loading::loadMenuScene(m_game);
        }
    } else if (action.getName() == "PAUSE") {
        // ESC to resume
        std::cout << "ESC pressed - resuming game" << std::endl;
        hidePauseMenu();
    } else if (action.getName() == "RESUME") {
        // C to resume (same as ESC)
        std::cout << "C pressed - resuming game" << std::endl;
        hidePauseMenu();
    } else {
        std::cout << "Unhandled pause menu input: " << action.getName() << std::endl;
    }
}

void Scene_Play::setupPauseMenu()
{
    // Use window dimensions for UI positioning (not game view)
    sf::Vector2u windowSize = m_game->window().getSize();
    float windowWidth = static_cast<float>(windowSize.x);
    float windowHeight = static_cast<float>(windowSize.y);
    
    std::cout << "Setting up pause menu for window size: " << windowWidth << "x" << windowHeight << std::endl;
    
    // Dialog dimensions
    float dialogWidth = 400.0f;
    float dialogHeight = 250.0f;
    float dialogX = (windowWidth - dialogWidth) / 2;
    float dialogY = (windowHeight - dialogHeight) / 2;
    
    std::cout << "Pause menu position: " << dialogX << ", " << dialogY << std::endl;
    
    try {
        // Background
        m_pauseBackground.setSize(sf::Vector2f(dialogWidth, dialogHeight));
        m_pauseBackground.setPosition(dialogX, dialogY);
        m_pauseBackground.setFillColor(sf::Color(20, 20, 30, 240));
        
        // Border
        m_pauseBorder.setSize(sf::Vector2f(dialogWidth, dialogHeight));
        m_pauseBorder.setPosition(dialogX, dialogY);
        m_pauseBorder.setFillColor(sf::Color::Transparent);
        m_pauseBorder.setOutlineColor(sf::Color::Yellow);
        m_pauseBorder.setOutlineThickness(2.0f);
        
        // Title
        m_pauseTitle.setFont(m_game->getAssets().getFont("ShareTech"));
        m_pauseTitle.setCharacterSize(28);
        m_pauseTitle.setFillColor(sf::Color::Yellow);
        m_pauseTitle.setString("Game Paused");
        sf::FloatRect titleBounds = m_pauseTitle.getLocalBounds();
        m_pauseTitle.setPosition(dialogX + (dialogWidth - titleBounds.width) / 2, dialogY + 30);
        
        // Options
        m_pauseOptions.setFont(m_game->getAssets().getFont("ShareTech"));
        m_pauseOptions.setCharacterSize(20);
        m_pauseOptions.setFillColor(sf::Color::White);
        
        std::string optionsText;
        if (m_pauseMenuSelection == 0) {
            optionsText = "> Resume <\n\n  Main Menu";
        } else {
            optionsText = "  Resume\n\n> Main Menu <";
        }
        m_pauseOptions.setString(optionsText);
        sf::FloatRect optionsBounds = m_pauseOptions.getLocalBounds();
        m_pauseOptions.setPosition(dialogX + (dialogWidth - optionsBounds.width) / 2, dialogY + 100);
        
    } catch (const std::exception& e) {
        std::cout << "Warning: Could not setup pause menu: " << e.what() << std::endl;
    }
}

void Scene_Play::renderPauseMenu()
{
    if (!m_showPauseMenu) return;
    
    // Switch to default view for UI rendering
    sf::View originalView = m_game->window().getView();
    m_game->window().setView(m_game->window().getDefaultView());
    
    // Draw pause menu elements
    m_game->window().draw(m_pauseBackground);
    m_game->window().draw(m_pauseBorder);
    m_game->window().draw(m_pauseTitle);
    m_game->window().draw(m_pauseOptions);
    
    // Restore original view
    m_game->window().setView(originalView);
}
