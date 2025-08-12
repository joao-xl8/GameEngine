#include "scene_autotile_editor.hpp"
#include "../game_engine.hpp"
#include "../action_types.hpp"
#include "../constants/tile_constants.hpp"
#include "scene_loading.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>

const float Scene_AutoTileEditor::ZOOM_SPEED = 0.1f;
const float Scene_AutoTileEditor::CAMERA_SPEED = 200.0f;

Scene_AutoTileEditor::Scene_AutoTileEditor(GameEngine* gameEngine) 
    : Scene(gameEngine), m_mapWidth(50), m_mapHeight(50), m_selectedTileType(0),
      m_showGrid(true), m_autoTileMode(true), m_isPainting(false), m_isErasing(false),
      m_cameraPosition(0, 0), m_zoomLevel(1.0f) {
    
    m_autoTilingManager = std::make_unique<AutoTilingManager>(gameEngine);
    init();
}

void Scene_AutoTileEditor::init() {
    // Calculate responsive layout first
    calculateResponsiveLayout();
    
    // Register controls
    registerAction(sf::Keyboard::Escape, ActionTypes::BACK);
    registerAction(sf::Keyboard::S, ActionTypes::SAVE);
    registerAction(sf::Keyboard::L, ActionTypes::LOAD);
    registerAction(sf::Keyboard::N, "NEW_MAP");
    registerAction(sf::Keyboard::G, "TOGGLE_GRID");
    registerAction(sf::Keyboard::A, "TOGGLE_AUTO_TILE");
    registerAction(sf::Keyboard::Tab, "CYCLE_TILE_TYPE");
    registerAction(sf::Keyboard::R, "REFRESH_AUTO_TILE");
    
    // Camera controls
    registerAction(sf::Keyboard::W, "CAMERA_UP");
    registerAction(sf::Keyboard::S, "CAMERA_DOWN");
    registerAction(sf::Keyboard::A, "CAMERA_LEFT");
    registerAction(sf::Keyboard::D, "CAMERA_RIGHT");
    registerAction(sf::Keyboard::Equal, "ZOOM_IN");
    registerAction(sf::Keyboard::Hyphen, "ZOOM_OUT");
    
    setupUI();
    setupTilePalette();
    loadTileTextures();
    initializeMap(m_mapWidth, m_mapHeight);
    
    // Setup camera view with 50px bottom margin
    sf::Vector2u windowSize = m_game->window().getSize();
    m_mapView.setSize(static_cast<float>(windowSize.x), static_cast<float>(windowSize.y - m_uiHeight - 50));
    m_mapView.setCenter(m_mapWidth * TileConstants::TILE_SIZE_F / 2.0f, m_mapHeight * TileConstants::TILE_SIZE_F / 2.0f);
    
    std::cout << "Auto-Tile Map Editor initialized for " << windowSize.x << "x" << windowSize.y << std::endl;
}

void Scene_AutoTileEditor::calculateResponsiveLayout() {
    sf::Vector2u windowSize = m_game->window().getSize();
    
    if (windowSize.y >= 700) {
        // Large screen - use full UI
        m_uiHeight = 120;
        m_paletteHeight = 80;
    } else if (windowSize.y >= 600) {
        // Medium screen - compact UI
        m_uiHeight = 100;
        m_paletteHeight = 60;
    } else {
        // Small screen - minimal UI
        m_uiHeight = std::max(MIN_UI_HEIGHT, static_cast<int>(windowSize.y * 0.15f));
        m_paletteHeight = std::max(MIN_PALETTE_HEIGHT, static_cast<int>(windowSize.y * 0.1f));
    }
    
    std::cout << "Auto-tile editor responsive layout: " << windowSize.x << "x" << windowSize.y 
              << " -> UI: " << m_uiHeight << ", Palette: " << m_paletteHeight << std::endl;
}

void Scene_AutoTileEditor::onEnd() {
    std::cout << "Auto-Tile Map Editor ended" << std::endl;
}

void Scene_AutoTileEditor::setupUI() {
    // Title text
    m_titleText.setFont(m_game->getAssets().getFont("ShareTech"));
    m_titleText.setString("Auto-Tile Map Editor");
    m_titleText.setCharacterSize(std::max(14, m_uiHeight / 6)); // Scale with UI height
    m_titleText.setFillColor(sf::Color::White);
    m_titleText.setPosition(10, 5);
    
    // Tool text
    m_toolText.setFont(m_game->getAssets().getFont("ShareTech"));
    m_toolText.setCharacterSize(std::max(10, m_uiHeight / 8));
    m_toolText.setFillColor(sf::Color::Yellow);
    m_toolText.setPosition(10, 25);
    
    // UI background
    sf::Vector2u windowSize = m_game->window().getSize();
    m_uiBackground.setSize(sf::Vector2f(static_cast<float>(windowSize.x), m_uiHeight));
    m_uiBackground.setPosition(0, 0);
    m_uiBackground.setFillColor(sf::Color(40, 40, 50, 200));
    
    // Tile palette background
    m_tilePalette.setSize(sf::Vector2f(static_cast<float>(windowSize.x), m_paletteHeight));
    m_tilePalette.setPosition(0, m_uiHeight - m_paletteHeight);
    m_tilePalette.setFillColor(sf::Color(60, 60, 70, 200));
    m_tilePalette.setOutlineThickness(1);
    m_tilePalette.setOutlineColor(sf::Color::White);
}

void Scene_AutoTileEditor::setupTilePalette() {
    m_availableTileTypes = m_autoTilingManager->getAvailableTileTypes();
    if (m_availableTileTypes.empty()) {
        m_availableTileTypes = {"Wall", "Ground", "Bush"}; // Default types
    }
    
    // Calculate tile size for palette based on available space
    int maxTileSize = std::min(TileConstants::TILE_SIZE, m_paletteHeight - 20);
    
    // Setup palette selector
    m_paletteSelector.setSize(sf::Vector2f(maxTileSize + 4, maxTileSize + 4));
    m_paletteSelector.setFillColor(sf::Color::Transparent);
    m_paletteSelector.setOutlineThickness(2);
    m_paletteSelector.setOutlineColor(sf::Color::Yellow);
    m_paletteSelector.setPosition(8, m_uiHeight - m_paletteHeight + 8);
}

void Scene_AutoTileEditor::loadTileTextures() {
    // Load textures for each tile type
    for (const auto& tileType : m_availableTileTypes) {
        sf::Texture texture;
        std::string texturePath;
        
        // Map tile types to texture files
        if (tileType == "Wall") {
            texturePath = "assets/imgs/wall.png";
        } else if (tileType == "Ground") {
            texturePath = "assets/imgs/ground.png";
        } else if (tileType == "Bush") {
            texturePath = "assets/imgs/bush.png";
        } else {
            texturePath = "assets/imgs/ground.png"; // Default
        }
        
        if (texture.loadFromFile(texturePath)) {
            m_tileTextures[tileType] = std::move(texture);
            std::cout << "Loaded texture for " << tileType << ": " << texturePath << std::endl;
        } else {
            std::cout << "Failed to load texture for " << tileType << std::endl;
        }
    }
    
    // Setup palette sprites
    m_paletteSprites.clear();
    int maxTileSize = std::min(TileConstants::TILE_SIZE, m_paletteHeight - 20);
    int tileSpacing = std::max(3, maxTileSize / 8);
    
    for (size_t i = 0; i < m_availableTileTypes.size(); i++) {
        const std::string& tileType = m_availableTileTypes[i];
        auto it = m_tileTextures.find(tileType);
        if (it != m_tileTextures.end()) {
            sf::Sprite sprite;
            sprite.setTexture(it->second);
            sprite.setPosition(10 + i * (maxTileSize + tileSpacing), m_uiHeight - m_paletteHeight + 10);
            sprite.setScale(static_cast<float>(maxTileSize) / sprite.getTexture()->getSize().x,
                           static_cast<float>(maxTileSize) / sprite.getTexture()->getSize().y);
            m_paletteSprites.push_back(sprite);
        }
    }
}

void Scene_AutoTileEditor::initializeMap(int width, int height) {
    m_mapWidth = width;
    m_mapHeight = height;
    
    // Initialize empty map
    m_tileMap.clear();
    m_tileMap.resize(height);
    for (auto& row : m_tileMap) {
        row.resize(width, ""); // Empty tiles
    }
    
    updateTileSprites();
    std::cout << "Initialized map: " << width << "x" << height << std::endl;
}

void Scene_AutoTileEditor::update() {
    handleMouseInput();
    handleKeyboardInput();
    updateCamera();
    sRender();
}

void Scene_AutoTileEditor::sDoAction(const Action& action) {
    if (action.getType() == "START") {
        if (action.getName() == ActionTypes::BACK) {
            Scene_Loading::loadMenuScene(m_game);
        }
        else if (action.getName() == ActionTypes::SAVE) {
            saveMap();
        }
        else if (action.getName() == ActionTypes::LOAD) {
            loadMap();
        }
        else if (action.getName() == "NEW_MAP") {
            newMap();
        }
        else if (action.getName() == "TOGGLE_GRID") {
            m_showGrid = !m_showGrid;
            std::cout << "Grid " << (m_showGrid ? "enabled" : "disabled") << std::endl;
        }
        else if (action.getName() == "TOGGLE_AUTO_TILE") {
            m_autoTileMode = !m_autoTileMode;
            std::cout << "Auto-tiling " << (m_autoTileMode ? "enabled" : "disabled") << std::endl;
        }
        else if (action.getName() == "CYCLE_TILE_TYPE") {
            cycleTileType(1);
        }
        else if (action.getName() == "REFRESH_AUTO_TILE") {
            applyAutoTiling();
        }
        else if (action.getName() == "ZOOM_IN") {
            zoomIn();
        }
        else if (action.getName() == "ZOOM_OUT") {
            zoomOut();
        }
    }
    
    // Handle continuous camera movement
    sf::Vector2f cameraMovement(0, 0);
    if (action.getName() == "CAMERA_UP") cameraMovement.y -= CAMERA_SPEED * m_game->getDeltaTime();
    if (action.getName() == "CAMERA_DOWN") cameraMovement.y += CAMERA_SPEED * m_game->getDeltaTime();
    if (action.getName() == "CAMERA_LEFT") cameraMovement.x -= CAMERA_SPEED * m_game->getDeltaTime();
    if (action.getName() == "CAMERA_RIGHT") cameraMovement.x += CAMERA_SPEED * m_game->getDeltaTime();
    
    if (cameraMovement.x != 0 || cameraMovement.y != 0) {
        moveCamera(cameraMovement);
    }
}

void Scene_AutoTileEditor::handleMouseInput() {
    sf::Vector2i mousePos = sf::Mouse::getPosition(m_game->window());
    
    // Check if mouse is in palette area
    if (mousePos.y >= m_uiHeight - m_paletteHeight && mousePos.y < m_uiHeight) {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            // Select tile type from palette
            int maxTileSize = std::min(TileConstants::TILE_SIZE, m_paletteHeight - 20);
            int tileSpacing = std::max(3, maxTileSize / 8);
            int paletteIndex = (mousePos.x - 10) / (maxTileSize + tileSpacing);
            if (paletteIndex >= 0 && paletteIndex < static_cast<int>(m_availableTileTypes.size())) {
                selectTileType(paletteIndex);
            }
        }
        return;
    }
    
    // Handle map painting
    if (mousePos.y > m_uiHeight) {
        sf::Vector2i tilePos = getMouseTilePosition();
        
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            if (!m_isPainting || tilePos != m_lastMouseTile) {
                placeTile(tilePos.x, tilePos.y, getCurrentTileType());
                m_isPainting = true;
                m_lastMouseTile = tilePos;
            }
        } else {
            m_isPainting = false;
        }
        
        if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
            if (!m_isErasing || tilePos != m_lastMouseTile) {
                eraseTile(tilePos.x, tilePos.y);
                m_isErasing = true;
                m_lastMouseTile = tilePos;
            }
        } else {
            m_isErasing = false;
        }
        
        m_selectedTile = tilePos;
    }
}

void Scene_AutoTileEditor::handleKeyboardInput() {
    // Additional keyboard handling if needed
}

sf::Vector2i Scene_AutoTileEditor::getMouseTilePosition() {
    sf::Vector2i mousePos = sf::Mouse::getPosition(m_game->window());
    sf::Vector2f worldPos = m_game->window().mapPixelToCoords(mousePos, m_mapView);
    return TileConstants::pixelToTile(worldPos);
}

void Scene_AutoTileEditor::placeTile(int x, int y, const std::string& tileType) {
    if (!isValidTilePosition(x, y)) return;
    
    m_tileMap[y][x] = tileType;
    
    if (m_autoTileMode) {
        refreshAutoTilingAroundPosition(x, y);
    } else {
        updateTileSprites();
    }
    
    std::cout << "Placed " << tileType << " at (" << x << ", " << y << ")" << std::endl;
}

void Scene_AutoTileEditor::eraseTile(int x, int y) {
    if (!isValidTilePosition(x, y)) return;
    
    m_tileMap[y][x] = "";
    
    if (m_autoTileMode) {
        refreshAutoTilingAroundPosition(x, y);
    } else {
        updateTileSprites();
    }
    
    std::cout << "Erased tile at (" << x << ", " << y << ")" << std::endl;
}

void Scene_AutoTileEditor::refreshAutoTilingAroundPosition(int x, int y) {
    // Refresh auto-tiling for the changed tile and its neighbors
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            int nx = x + dx;
            int ny = y + dy;
            if (isValidTilePosition(nx, ny)) {
                applyAutoTilingAt(nx, ny);
            }
        }
    }
    updateTileSprites();
}

void Scene_AutoTileEditor::applyAutoTiling() {
    for (int y = 0; y < m_mapHeight; y++) {
        for (int x = 0; x < m_mapWidth; x++) {
            applyAutoTilingAt(x, y);
        }
    }
    updateTileSprites();
    std::cout << "Applied auto-tiling to entire map" << std::endl;
}

void Scene_AutoTileEditor::applyAutoTilingAt(int x, int y) {
    if (!isValidTilePosition(x, y) || m_tileMap[y][x].empty()) return;
    
    // Auto-tiling logic would go here
    // For now, this is a placeholder
}

void Scene_AutoTileEditor::updateTileSprites() {
    m_tileSprites.clear();
    
    for (int y = 0; y < m_mapHeight; y++) {
        for (int x = 0; x < m_mapWidth; x++) {
            const std::string& tileType = m_tileMap[y][x];
            if (tileType.empty()) continue;
            
            auto it = m_tileTextures.find(tileType);
            if (it != m_tileTextures.end()) {
                sf::Sprite sprite;
                sprite.setTexture(it->second);
                sprite.setPosition(TileConstants::tileToPixel(x, y));
                
                // Scale sprite to tile size
                sf::Vector2u textureSize = it->second.getSize();
                sprite.setScale(static_cast<float>(TileConstants::TILE_SIZE) / textureSize.x,
                               static_cast<float>(TileConstants::TILE_SIZE) / textureSize.y);
                
                // Apply auto-tiling if enabled, otherwise use full texture
                if (m_autoTileMode) {
                    sf::IntRect autoTileRect = getAutoTileRect(tileType, x, y);
                    sprite.setTextureRect(autoTileRect);
                } else {
                    // Use full texture when auto-tiling is disabled
                    sprite.setTextureRect(sf::IntRect(0, 0, textureSize.x, textureSize.y));
                }
                
                m_tileSprites.push_back(sprite);
            }
        }
    }
}

sf::IntRect Scene_AutoTileEditor::getAutoTileRect(const std::string& tileType, int x, int y) {
    return m_autoTilingManager->getAutoTile(tileType, x, y, m_tileMap);
}

void Scene_AutoTileEditor::updateCamera() {
    m_mapView.setCenter(m_cameraPosition);
    m_mapView.setSize(m_game->window().getSize().x / m_zoomLevel, 
                     (m_game->window().getSize().y - m_uiHeight - 50) / m_zoomLevel); // 50px bottom margin
}

void Scene_AutoTileEditor::zoomIn() {
    m_zoomLevel = std::min(m_zoomLevel + ZOOM_SPEED, 3.0f);
    updateCamera();
}

void Scene_AutoTileEditor::zoomOut() {
    m_zoomLevel = std::max(m_zoomLevel - ZOOM_SPEED, 0.2f);
    updateCamera();
}

void Scene_AutoTileEditor::moveCamera(sf::Vector2f direction) {
    m_cameraPosition += direction / m_zoomLevel;
    updateCamera();
}

bool Scene_AutoTileEditor::isValidTilePosition(int x, int y) const {
    return x >= 0 && x < m_mapWidth && y >= 0 && y < m_mapHeight;
}

std::string Scene_AutoTileEditor::getCurrentTileType() const {
    if (m_selectedTileType >= 0 && m_selectedTileType < static_cast<int>(m_availableTileTypes.size())) {
        return m_availableTileTypes[m_selectedTileType];
    }
    return "Wall"; // Default
}

void Scene_AutoTileEditor::selectTileType(int index) {
    if (index >= 0 && index < static_cast<int>(m_availableTileTypes.size())) {
        m_selectedTileType = index;
        
        // Update palette selector position
        int maxTileSize = std::min(TileConstants::TILE_SIZE, m_paletteHeight - 20);
        int tileSpacing = std::max(3, maxTileSize / 8);
        m_paletteSelector.setPosition(8 + index * (maxTileSize + tileSpacing), m_uiHeight - m_paletteHeight + 8);
        
        std::cout << "Selected tile type: " << getCurrentTileType() << std::endl;
    }
}

void Scene_AutoTileEditor::cycleTileType(int direction) {
    int newIndex = (m_selectedTileType + direction + m_availableTileTypes.size()) % m_availableTileTypes.size();
    selectTileType(newIndex);
}

void Scene_AutoTileEditor::saveMap() {
    std::string filename = "maps/autotile_map.txt";
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cout << "Failed to save map: " << filename << std::endl;
        return;
    }
    
    file << m_mapWidth << " " << m_mapHeight << std::endl;
    for (const auto& row : m_tileMap) {
        for (const auto& tile : row) {
            file << (tile.empty() ? "." : tile) << " ";
        }
        file << std::endl;
    }
    
    file.close();
    std::cout << "Saved map: " << filename << std::endl;
}

void Scene_AutoTileEditor::loadMap() {
    std::string filename = "maps/autotile_map.txt";
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Failed to load map: " << filename << std::endl;
        return;
    }
    
    int width, height;
    file >> width >> height;
    
    initializeMap(width, height);
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            std::string tile;
            file >> tile;
            m_tileMap[y][x] = (tile == ".") ? "" : tile;
        }
    }
    
    file.close();
    updateTileSprites();
    std::cout << "Loaded map: " << filename << std::endl;
}

void Scene_AutoTileEditor::newMap() {
    initializeMap(50, 50); // Default size
    std::cout << "Created new map" << std::endl;
}

void Scene_AutoTileEditor::sRender() {
    // Set map view for rendering the map
    m_game->window().setView(m_mapView);
    
    // Render map
    renderMap();
    
    if (m_showGrid) {
        renderGrid();
    }
    
    renderCursor();
    
    // Reset to default view for UI
    m_game->window().setView(m_game->window().getDefaultView());
    
    // Render UI
    renderUI();
    renderTilePalette();
    
    // Always show command overlay
    renderCommandOverlay();
}

void Scene_AutoTileEditor::renderMap() {
    for (const auto& sprite : m_tileSprites) {
        m_game->window().draw(sprite);
    }
}

void Scene_AutoTileEditor::renderGrid() {
    sf::View currentView = m_game->window().getView();
    sf::FloatRect viewBounds(currentView.getCenter() - currentView.getSize() / 2.0f, currentView.getSize());
    
    // Calculate visible tile range
    int startX = std::max(0, static_cast<int>(viewBounds.left / TileConstants::TILE_SIZE_F) - 1);
    int endX = std::min(m_mapWidth, static_cast<int>((viewBounds.left + viewBounds.width) / TileConstants::TILE_SIZE_F) + 1);
    int startY = std::max(0, static_cast<int>(viewBounds.top / TileConstants::TILE_SIZE_F) - 1);
    int endY = std::min(m_mapHeight, static_cast<int>((viewBounds.top + viewBounds.height) / TileConstants::TILE_SIZE_F) + 1);
    
    m_gridLine.setFillColor(sf::Color(100, 100, 100, 100));
    
    // Draw vertical lines
    for (int x = startX; x <= endX; x++) {
        m_gridLine.setSize(sf::Vector2f(1, (endY - startY) * TileConstants::TILE_SIZE_F));
        m_gridLine.setPosition(x * TileConstants::TILE_SIZE_F, startY * TileConstants::TILE_SIZE_F);
        m_game->window().draw(m_gridLine);
    }
    
    // Draw horizontal lines
    for (int y = startY; y <= endY; y++) {
        m_gridLine.setSize(sf::Vector2f((endX - startX) * TileConstants::TILE_SIZE_F, 1));
        m_gridLine.setPosition(startX * TileConstants::TILE_SIZE_F, y * TileConstants::TILE_SIZE_F);
        m_game->window().draw(m_gridLine);
    }
}

void Scene_AutoTileEditor::renderCursor() {
    if (isValidTilePosition(m_selectedTile.x, m_selectedTile.y)) {
        sf::RectangleShape cursor;
        cursor.setSize(sf::Vector2f(TileConstants::TILE_SIZE_F, TileConstants::TILE_SIZE_F));
        cursor.setPosition(TileConstants::tileToPixel(m_selectedTile));
        cursor.setFillColor(sf::Color::Transparent);
        cursor.setOutlineThickness(2);
        cursor.setOutlineColor(sf::Color::White);
        m_game->window().draw(cursor);
    }
}

void Scene_AutoTileEditor::renderUI() {
    // Draw UI background
    m_game->window().draw(m_uiBackground);
    
    // Draw title
    m_game->window().draw(m_titleText);
    
    // Draw essential info only
    std::string tools = getCurrentTileType() + 
                       " | Grid: " + (m_showGrid ? "ON" : "OFF") +
                       " | Auto-Tile: " + (m_autoTileMode ? "ON" : "OFF");
    m_toolText.setString(tools);
    m_game->window().draw(m_toolText);
}

void Scene_AutoTileEditor::renderTilePalette() {
    // Draw palette background
    m_game->window().draw(m_tilePalette);
    
    // Draw palette sprites
    for (const auto& sprite : m_paletteSprites) {
        m_game->window().draw(sprite);
    }
    
    // Draw selector
    m_game->window().draw(m_paletteSelector);
    
    // Draw tile type labels
    int maxTileSize = std::min(TileConstants::TILE_SIZE, m_paletteHeight - 20);
    int tileSpacing = std::max(3, maxTileSize / 8);
    int labelSize = std::max(8, m_paletteHeight / 8);
    
    for (size_t i = 0; i < m_availableTileTypes.size(); i++) {
        sf::Text label;
        label.setFont(m_game->getAssets().getFont("ShareTech"));
        label.setString(m_availableTileTypes[i]);
        label.setCharacterSize(labelSize);
        label.setFillColor(sf::Color::White);
        label.setPosition(10 + i * (maxTileSize + tileSpacing), m_uiHeight - 15);
        m_game->window().draw(label);
    }
}
