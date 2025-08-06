#include "scene_play.hpp"
#include "scene_menu.hpp"
#include "game_engine.hpp"
#include "components.hpp"
#include <sstream>

void Scene_Play::init(const std::string &levelPath)
{
    registerAction(sf::Keyboard::P, "PAUSE");
    registerAction(sf::Keyboard::Escape, "PAUSE");
    registerAction(sf::Keyboard::T, "TOGGLE_TEXTURE");
    registerAction(sf::Keyboard::C, "TOGGLE_COLLISION");
    registerAction(sf::Keyboard::G, "TOGGLE_GRID");
    
    // Player movement controls
    registerAction(sf::Keyboard::W, "UP");
    registerAction(sf::Keyboard::A, "LEFT");
    registerAction(sf::Keyboard::S, "DOWN");
    registerAction(sf::Keyboard::D, "RIGHT");

    m_tileText.setCharacterSize(16);
    m_tileText.setFont(m_game->getAssets().getFont("ShareTech"));
    m_tileText.setFillColor(sf::Color::White);

    // Initialize sound manager
    m_soundManager = std::make_shared<CSound>();
    
    // Load background music
    m_soundManager->addMusic("background", "assets/music/time_for_adventure.mp3");
    
    // Load sound effects
    m_soundManager->addSound("walk", "assets/sounds/tap.wav");
    m_soundManager->addSound("hurt", "assets/sounds/hurt.wav");
    m_soundManager->addSound("jump", "assets/sounds/jump.wav");
    m_soundManager->addSound("coin", "assets/sounds/coin.wav");
    m_soundManager->addSound("power_up", "assets/sounds/power_up.wav");
    m_soundManager->addSound("explosion", "assets/sounds/explosion.wav");
    
    // Start background music
    m_soundManager->playMusic("background", true, 30.0f); // Loop, 30% volume
    
    std::printf("Sound system initialized with background music and sound effects\n");

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
        std::stringstream ss(line);
        std::string type;
        std::string spriteName;
        int x, y;
        ss >> type >> spriteName >> x >> y;
        if (type == "Tile")
        {
            auto e = m_entityManager.addEntity(type);
            e->addComponent<CTransform>(std::make_shared<CTransform>(Vec2{x * m_tileSize.x, y * m_tileSize.y}));
            e->addComponent<CSprite>(std::make_shared<CSprite>(spriteName, m_game->getAssets().getTexture(spriteName)));
            e->addComponent<CBoundingBox>(std::make_shared<CBoundingBox>(m_tileSize));
        }
        else if (type == "Dec")
        {
            auto e = m_entityManager.addEntity(type);
            e->addComponent<CTransform>(std::make_shared<CTransform>(Vec2{x * m_tileSize.x, y * m_tileSize.y}));
            e->addComponent<CSprite>(std::make_shared<CSprite>(spriteName, m_game->getAssets().getTexture(spriteName)));
            // DONT HAVE COLLISION
        }
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
            
            // Update the game view to match camera position (account for coordinate conversion)
            sf::View& gameView = m_game->getGameView();
            sf::Vector2f viewSize = gameView.getSize();
            float screenCenterY = viewSize.y / 2.0f;
            gameView.setCenter(camera->position.x, (viewSize.y - 128.0f) - camera->position.y);
            
            // Enhanced debug output
            static int debugCounter = 0;
            if (debugCounter++ % 30 == 0) { // Print every 30 frames (roughly 0.5 seconds)
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
                if (gridMovement->startMoveWithCollisionCheck(Vec2{0, 1}, transform->pos, boundingBox->size, collisionCheck)) {
                    if (animation) animation->play("walk_up");
                    if (sound) sound->playSound("footstep", 70.0f);  // Play walking sound
                    moved = true;
                    m_gridMoveTimer = m_changeGridSleep; // Start cooldown
                }
            }
            else if (input->downPressed) 
            {
                if (gridMovement->startMoveWithCollisionCheck(Vec2{0, -1}, transform->pos, boundingBox->size, collisionCheck)) {
                    if (animation) animation->play("walk_down");
                    if (sound) sound->playSound("footstep", 70.0f);  // Play walking sound
                    moved = true;
                    m_gridMoveTimer = m_changeGridSleep; // Start cooldown
                }
            }
            else if (input->leftPressed) 
            {
                if (gridMovement->startMoveWithCollisionCheck(Vec2{-1, 0}, transform->pos, boundingBox->size, collisionCheck)) {
                    if (animation) animation->play("walk_left");
                    if (sound) sound->playSound("footstep", 70.0f);  // Play walking sound
                    moved = true;
                    m_gridMoveTimer = m_changeGridSleep; // Start cooldown
                }
            }
            else if (input->rightPressed) 
            {
                if (gridMovement->startMoveWithCollisionCheck(Vec2{1, 0}, transform->pos, boundingBox->size, collisionCheck)) {
                    if (animation) animation->play("walk_right");
                    if (sound) sound->playSound("footstep", 70.0f);  // Play walking sound
                    moved = true;
                    m_gridMoveTimer = m_changeGridSleep; // Start cooldown
                }
            }
        }
        
        // Update position based on grid movement
        transform->pos = gridMovement->updateMovement(m_deltaTime, transform->pos);
        
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
    background.setFillColor(sf::Color::Cyan);
    background.setPosition(0, 0);
    m_game->window().draw(background);
    
    if(m_drawTextures){
        for (auto &entity : m_entityManager.getEntities())
        {
            if (entity->hasComponent<CSprite>() && entity->hasComponent<CTransform>())
            {
                auto sprite = entity->getComponent<CSprite>();
                auto transform = entity->getComponent<CTransform>();
                // Calculate the bottom-left coordinate of the sprite
                float posX = transform->pos.x;
                float posY = m_game->window().getSize().y - transform->pos.y - sprite->sprite.getGlobalBounds().height; // Adjust posY
                sprite->sprite.setPosition(posX, posY);
                m_game->window().draw(sprite->sprite);
            }
        }
    }
    if (m_drawGrid)
    {
        // Calculate the bottom-left corner of the grid
        float gridBottom = m_game->window().getSize().y;

        for (size_t i = 0; i < 30; i++)
        {
            for (size_t j = 0; j < 30; j++)
            {
                float posX = j * m_tileSize.x;
                float posY = gridBottom - (i + 1) * m_tileSize.y; // Adjust the Y position

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
                float posY = m_game->window().getSize().y - transform->pos.y - boundingBox->size.y; // Adjust posY
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
}

void Scene_Play::sDoAction(const Action &action)
{
    if (action.getType() == "START")
    {
        std::printf("Start action: %s\n", action.getName().c_str());
        if (action.getName() == "PAUSE")
        {
            m_game->changeScene("Menu", std::make_shared<Scene_Menu>(m_game));
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
{}

void Scene_Play::update()
{
    // Calculate delta time
    m_deltaTime = m_deltaClock.restart().asSeconds();
    
    m_entityManager.update();
    sMovement();
    sCollision();
    sEnemySpawner();
    sAnimation();
    sCamera();  // Update camera system
    sRender();
    sDebug();
}

void Scene_Play::spawnPlayer()
{
    m_player = m_entityManager.addEntity("Player");
    
    // Set player starting position (adjust as needed)
    Vec2 startPos = {100, 100};
    m_player->addComponent<CTransform>(std::make_shared<CTransform>(startPos));
    
    // Add sprite component with player texture
    auto& playerTexture = m_game->getAssets().getTexture("Player");
    m_player->addComponent<CSprite>(std::make_shared<CSprite>("Player", playerTexture));
    
    // Set up sprite sheet frame (32x32 pixel frames based on the sprite sheet)
    auto sprite = m_player->getComponent<CSprite>();
    sprite->sprite.setTextureRect(sf::IntRect(0, 0, m_playerScale, m_playerScale)); // First frame
    
    // Add animation component with flexible animation definitions (using configurable scale)
    auto animationComponent = std::make_shared<CAnimation>(Vec2{static_cast<float>(m_playerScale), static_cast<float>(m_playerScale)});

    // Define animations - you can use the same texture with different rows
    // or different textures for each animation
    animationComponent->addAnimation("idle", "Player", 6, 0.2f, false, 0);        // Row 0, 6 frames, slow
    animationComponent->addAnimation("walk_down", "Player", 6, 0.15f, false, 3);  // Row 3, 6 frames
    animationComponent->addAnimation("walk_up", "Player", 6, 0.15f, false, 2);    // Row 2, 6 frames
    animationComponent->addAnimation("walk_right", "Player", 6, 0.15f, false, 1); // Row 1, 6 frames
    animationComponent->addAnimation("walk_left", "Player", 6, 0.15f, true, 1);   // Row 1, flipped

    // Example of how you could add animations from separate files:
    // animationComponent->addAnimation("attack", "PlayerAttack", 4, 0.1f, false, 0);  // Different texture
    // animationComponent->addAnimation("jump", "PlayerJump", 8, 0.08f, false, 0);     // Different texture
    // animationComponent->addAnimation("death", "PlayerDeath", 10, 0.12f, false, 0);  // Different texture

    animationComponent->play("idle"); // Start with idle animation
    m_player->addComponent<CAnimation>(animationComponent);
    
    // Add grid movement component (using configurable scale)
    auto gridMovement = std::make_shared<CGridMovement>(m_gameScale, 3.0f, true); // Scale-based grid, 3 moves/sec, smooth
    gridMovement->snapToGrid(startPos); // Snap player to nearest grid position
    m_player->addComponent<CGridMovement>(gridMovement);
    
    // Add bounding box (using configurable scale)
    m_player->addComponent<CBoundingBox>(std::make_shared<CBoundingBox>(Vec2{static_cast<float>(m_gameScale), static_cast<float>(m_gameScale)}));
    
    // Add input component
    m_player->addComponent<CInput>(std::make_shared<CInput>());
    
    // Add camera component with 1-tile dead zone
    Vec2 deadZoneSize = {static_cast<float>(m_gameScale), static_cast<float>(m_gameScale)}; // 1 tile dead zone
    auto cameraComponent = std::make_shared<CCamera>(startPos, deadZoneSize, 3.0f); // Follow speed of 3.0
    
    // Ensure camera starts exactly at player position (centered)
    cameraComponent->setPosition(startPos);  // This sets both position and targetPosition
    
    m_player->addComponent<CCamera>(cameraComponent);
    
    // Initialize game view to center player in middle of screen
    sf::View& gameView = m_game->getGameView();
    
    // Get the game view size to calculate proper center
    sf::Vector2f viewSize = gameView.getSize();
    float screenCenterY = viewSize.y / 2.0f;
    
    // Set camera center accounting for coordinate system conversion
    gameView.setCenter(startPos.x, (viewSize.y - 128.0f) - startPos.y);
    
    std::printf("Camera initialized at position: %f, %f (player centered)\n", startPos.x, startPos.y);
    
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
    
    // Check collision with all tile entities at the given position
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
            
            if (isColliding(position, size, tileTransform->pos, tileBBox->size))
            {
                return true; // Would collide with this tile
            }
        }
    }
    return false; // No collision detected
}
