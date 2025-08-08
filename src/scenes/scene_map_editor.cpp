#include "scene_map_editor.hpp"
#include "scene_menu.hpp"
#include "../game_engine.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <filesystem>
#include <algorithm>

Scene_MapEditor::Scene_MapEditor(GameEngine* game)
    : Scene(game), m_cameraPos(0, 0), m_cursorPos(0, 0), m_currentFileName("new_level.txt")
{
    init();
}

void Scene_MapEditor::init()
{
    // Register input actions
    registerAction(sf::Keyboard::Escape, "BACK");
    registerAction(sf::Keyboard::W, "UP");
    registerAction(sf::Keyboard::S, "DOWN");
    registerAction(sf::Keyboard::A, "LEFT");
    registerAction(sf::Keyboard::D, "RIGHT");
    registerAction(sf::Keyboard::Up, "UP");
    registerAction(sf::Keyboard::Down, "DOWN");
    registerAction(sf::Keyboard::Left, "LEFT");
    registerAction(sf::Keyboard::Right, "RIGHT");
    registerAction(sf::Keyboard::Space, "PLACE");
    registerAction(sf::Keyboard::X, "REMOVE");
    registerAction(sf::Keyboard::Q, "PREV_ASSET");
    registerAction(sf::Keyboard::E, "NEXT_ASSET");
    registerAction(sf::Keyboard::Z, "PREV_TYPE");
    registerAction(sf::Keyboard::C, "NEXT_TYPE");
    registerAction(sf::Keyboard::F, "SAVE");
    registerAction(sf::Keyboard::L, "LOAD");
    
    // Mouse support - use different approach to avoid key conflicts
    // We'll handle mouse input differently in the action system
    
    // Load available assets and types
    loadAvailableAssets();
    
    // Set up UI text
    m_uiText.setFont(m_game->getAssets().getFont("ShareTech"));
    m_uiText.setCharacterSize(14);
    m_uiText.setFillColor(sf::Color::White);
    
    // Set up cursor
    m_cursor.setSize(sf::Vector2f(TILE_SIZE, TILE_SIZE));
    m_cursor.setFillColor(sf::Color::Transparent);
    m_cursor.setOutlineColor(sf::Color::Yellow);
    m_cursor.setOutlineThickness(3.0f);
    
    // Initialize camera views
    m_gameView = m_game->window().getDefaultView();
    m_uiView = m_game->window().getDefaultView();
    
    // Set up asset preview
    m_previewBackground.setSize(sf::Vector2f(TILE_SIZE + 10, TILE_SIZE + 10));
    m_previewBackground.setFillColor(sf::Color(50, 50, 50, 200));
    m_previewBackground.setOutlineColor(sf::Color::White);
    m_previewBackground.setOutlineThickness(1.0f);
    
    m_previewBorder.setSize(sf::Vector2f(TILE_SIZE, TILE_SIZE));
    m_previewBorder.setFillColor(sf::Color::Transparent);
    m_previewBorder.setOutlineColor(sf::Color::Yellow);
    m_previewBorder.setOutlineThickness(2.0f);
    
    // Set up level selector
    m_levelSelectorBackground.setFillColor(sf::Color(0, 0, 0, 200));
    m_levelSelectorBackground.setOutlineColor(sf::Color::White);
    m_levelSelectorBackground.setOutlineThickness(2.0f);
    
    m_levelSelectorText.setFont(m_game->getAssets().getFont("ShareTech"));
    m_levelSelectorText.setCharacterSize(16);
    m_levelSelectorText.setFillColor(sf::Color::White);
    
    // Clear infinite grid (it starts empty by default)
    m_infiniteGrid.clear();
    
    std::cout << "Map Editor initialized. Controls:\n";
    std::cout << "WASD/Arrow Keys: Move cursor\n";
    std::cout << "Space: Place object\n";
    std::cout << "X: Remove object\n";
    std::cout << "Left Click: Place object at mouse\n";
    std::cout << "Right Click: Remove object at mouse\n";
    std::cout << "Q/E: Change asset\n";
    std::cout << "Z/C: Change type\n";
    std::cout << "F: Save level (to metadata/levels/)\n";
    std::cout << "L: Open level selector GUI\n";
    std::cout << "Escape: Back to menu\n";
    std::cout << "Camera follows cursor automatically\n";
    std::cout << "Directory structure reorganized!\n";
    std::cout << "Levels: metadata/levels/ | Config: metadata/\n";
}

void Scene_MapEditor::loadAvailableAssets()
{
    // Load from assets.txt to get available textures
    m_availableAssets.clear();
    m_availableTypes.clear();
    
    // Add available asset types
    m_availableTypes.push_back("Tile");
    m_availableTypes.push_back("Dec");
    
    // Add available assets (these should match what's in assets.txt)
    m_availableAssets.push_back("Ground");
    m_availableAssets.push_back("Wall");
    m_availableAssets.push_back("Bush");
    m_availableAssets.push_back("Player");
    
    // Set defaults
    if (!m_availableAssets.empty()) {
        m_currentAsset = m_availableAssets[0];
    }
    if (!m_availableTypes.empty()) {
        m_currentType = m_availableTypes[0];
    }
}

void Scene_MapEditor::update()
{
    // Handle mouse input directly to avoid key conflicts
    static bool leftMousePressed = false;
    static bool rightMousePressed = false;
    
    bool leftMouseDown = sf::Mouse::isButtonPressed(sf::Mouse::Left);
    bool rightMouseDown = sf::Mouse::isButtonPressed(sf::Mouse::Right);
    
    // Handle left mouse click (place object)
    if (leftMouseDown && !leftMousePressed) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(m_game->window());
        sf::Vector2f worldPos = m_game->window().mapPixelToCoords(mousePos, m_gameView);
        Vec2 gridPos = screenToGrid(Vec2(worldPos.x, worldPos.y));
        m_cursorPos = gridPos;
        updateCamera();
        placeObject();
    }
    
    // Handle right mouse click (remove object)
    if (rightMouseDown && !rightMousePressed) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(m_game->window());
        sf::Vector2f worldPos = m_game->window().mapPixelToCoords(mousePos, m_gameView);
        Vec2 gridPos = screenToGrid(Vec2(worldPos.x, worldPos.y));
        m_cursorPos = gridPos;
        updateCamera();
        removeObject();
    }
    
    // Update mouse state for next frame
    leftMousePressed = leftMouseDown;
    rightMousePressed = rightMouseDown;
    
    sRender();
}

void Scene_MapEditor::sDoAction(const Action& action)
{
    if (action.getType() == "START") {
        // If level selector is open, handle its input
        if (m_showLevelSelector) {
            handleLevelSelectorInput(action);
            return;
        }
        
        if (action.getName() == "BACK") {
            m_game->changeScene("Menu", std::make_shared<Scene_Menu>(m_game));
        }
        else if (action.getName() == "UP") {
            m_cursorPos.y--;
            updateCamera();
        }
        else if (action.getName() == "DOWN") {
            m_cursorPos.y++;
            updateCamera();
        }
        else if (action.getName() == "LEFT") {
            m_cursorPos.x--;
            updateCamera();
        }
        else if (action.getName() == "RIGHT") {
            m_cursorPos.x++;
            updateCamera();
        }
        else if (action.getName() == "PLACE") {
            placeObject();
        }
        else if (action.getName() == "REMOVE") {
            removeObject();
        }
        else if (action.getName() == "PREV_ASSET") {
            if (m_assetIndex > 0) {
                m_assetIndex--;
            } else {
                m_assetIndex = m_availableAssets.size() - 1;
            }
            m_currentAsset = m_availableAssets[m_assetIndex];
        }
        else if (action.getName() == "NEXT_ASSET") {
            m_assetIndex = (m_assetIndex + 1) % m_availableAssets.size();
            m_currentAsset = m_availableAssets[m_assetIndex];
        }
        else if (action.getName() == "PREV_TYPE") {
            if (m_typeIndex > 0) {
                m_typeIndex--;
            } else {
                m_typeIndex = m_availableTypes.size() - 1;
            }
            m_currentType = m_availableTypes[m_typeIndex];
        }
        else if (action.getName() == "NEXT_TYPE") {
            m_typeIndex = (m_typeIndex + 1) % m_availableTypes.size();
            m_currentType = m_availableTypes[m_typeIndex];
        }
        else if (action.getName() == "SAVE") {
            saveLevel();
        }
        else if (action.getName() == "LOAD") {
            // Show level selector instead of directly loading level1.txt
            scanAvailableLevels();
            m_showLevelSelector = true;
        }
    }
}

void Scene_MapEditor::updateCamera()
{
    // Center camera on cursor position
    Vec2 targetPos = gridToScreen(m_cursorPos);
    targetPos.x += TILE_SIZE / 2; // Center on tile
    targetPos.y += TILE_SIZE / 2;
    
    // Smooth camera movement
    Vec2 diff = Vec2(targetPos.x - m_cameraPos.x, targetPos.y - m_cameraPos.y);
    m_cameraPos.x += diff.x * 0.1f; // Smooth interpolation
    m_cameraPos.y += diff.y * 0.1f;
    
    // Update game view
    m_gameView.setCenter(m_cameraPos.x, m_cameraPos.y);
}

Scene_MapEditor::GridCell* Scene_MapEditor::getGridCell(int x, int y)
{
    auto key = std::make_pair(x, y);
    auto it = m_infiniteGrid.find(key);
    if (it != m_infiniteGrid.end()) {
        return &it->second;
    }
    return nullptr;
}

void Scene_MapEditor::setGridCell(int x, int y, const GridCell& cell)
{
    auto key = std::make_pair(x, y);
    if (cell.occupied) {
        m_infiniteGrid[key] = cell;
    } else {
        // Remove empty cells to save memory
        m_infiniteGrid.erase(key);
    }
}

Vec2 Scene_MapEditor::getVisibleGridMin()
{
    sf::Vector2f topLeft = m_game->window().mapPixelToCoords(sf::Vector2i(0, 0), m_gameView);
    Vec2 gridMin = screenToGrid(Vec2(topLeft.x, topLeft.y));
    return Vec2(gridMin.x - 1, gridMin.y - 1); // Add buffer
}

Vec2 Scene_MapEditor::getVisibleGridMax()
{
    sf::Vector2u windowSize = m_game->window().getSize();
    sf::Vector2f bottomRight = m_game->window().mapPixelToCoords(
        sf::Vector2i(windowSize.x, windowSize.y), m_gameView);
    Vec2 gridMax = screenToGrid(Vec2(bottomRight.x, bottomRight.y));
    return Vec2(gridMax.x + 1, gridMax.y + 1); // Add buffer
}

void Scene_MapEditor::placeObject()
{
    int x = static_cast<int>(m_cursorPos.x);
    int y = static_cast<int>(m_cursorPos.y);
    
    GridCell cell;
    cell.type = m_currentType;
    cell.asset = m_currentAsset;
    cell.occupied = true;
    
    setGridCell(x, y, cell);
    
    std::cout << "Placed " << m_currentType << " " << m_currentAsset 
              << " at (" << x << ", " << y << ")\n";
}

void Scene_MapEditor::removeObject()
{
    int x = static_cast<int>(m_cursorPos.x);
    int y = static_cast<int>(m_cursorPos.y);
    
    GridCell emptyCell;
    setGridCell(x, y, emptyCell); // This will remove the cell from the map
    
    std::cout << "Removed object at (" << x << ", " << y << ")\n";
}

void Scene_MapEditor::scanAvailableLevels()
{
    m_availableLevels.clear();
    m_selectedLevelIndex = 0;
    
    // Scan metadata/levels directory for .txt files
    std::string levelsDir = "metadata/levels/";
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(levelsDir)) {
            if (entry.is_regular_file() && entry.path().extension() == ".txt") {
                std::string filename = entry.path().filename().string();
                m_availableLevels.push_back(filename);
            }
        }
    } catch (const std::exception& e) {
        std::cout << "Error scanning levels directory: " << e.what() << std::endl;
        // Add some default levels if directory scan fails
        m_availableLevels.push_back("level1.txt");
        m_availableLevels.push_back("demo_level.txt");
        m_availableLevels.push_back("infinite_test.txt");
    }
    
    // Sort levels alphabetically
    std::sort(m_availableLevels.begin(), m_availableLevels.end());
    
    std::cout << "Found " << m_availableLevels.size() << " level files in metadata/levels/" << std::endl;
}

void Scene_MapEditor::handleLevelSelectorInput(const Action& action)
{
    if (action.getName() == "BACK") {
        m_showLevelSelector = false;
    }
    else if (action.getName() == "UP") {
        if (m_selectedLevelIndex > 0) {
            m_selectedLevelIndex--;
        } else {
            m_selectedLevelIndex = m_availableLevels.size() - 1;
        }
    }
    else if (action.getName() == "DOWN") {
        m_selectedLevelIndex = (m_selectedLevelIndex + 1) % m_availableLevels.size();
    }
    else if (action.getName() == "PLACE" || action.getName() == "LOAD") {
        // Load selected level from levels directory
        if (!m_availableLevels.empty()) {
            std::string selectedFile = "metadata/levels/" + m_availableLevels[m_selectedLevelIndex];
            loadLevel(selectedFile);
        }
        m_showLevelSelector = false;
    }
}

void Scene_MapEditor::saveLevel()
{
    // Generate filename with timestamp in levels directory
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    std::ostringstream oss;
    oss << "metadata/levels/level_editor_" << std::put_time(&tm, "%Y%m%d_%H%M%S") << ".txt";
    std::string filename = oss.str();
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cout << "Failed to save level to " << filename << std::endl;
        return;
    }
    
    file << "# Level created with Map Editor\n";
    file << "# Format: Type SpriteName X Y\n\n";
    
    // Save all placed objects from infinite grid
    for (const auto& pair : m_infiniteGrid) {
        int x = pair.first.first;
        int y = pair.first.second;
        const GridCell& cell = pair.second;
        
        if (cell.occupied) {
            file << cell.type << " " << cell.asset 
                 << " " << x << " " << y << "\n";
        }
    }
    
    file.close();
    std::cout << "Level saved to " << filename << " (" << m_infiniteGrid.size() << " objects)" << std::endl;
    m_currentFileName = filename;
}

void Scene_MapEditor::loadLevel(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Failed to load level from " << filename << std::endl;
        return;
    }
    
    // Clear current infinite grid
    m_infiniteGrid.clear();
    
    std::string line;
    int objectCount = 0;
    while (std::getline(file, line)) {
        // Skip comments and empty lines
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        std::istringstream iss(line);
        std::string type, asset;
        int x, y;
        
        if (iss >> type >> asset >> x >> y) {
            GridCell cell;
            cell.type = type;
            cell.asset = asset;
            cell.occupied = true;
            setGridCell(x, y, cell);
            objectCount++;
        }
    }
    
    file.close();
    std::cout << "Level loaded from " << filename << " (" << objectCount << " objects)" << std::endl;
    m_currentFileName = filename;
    
    // Reset cursor to origin and update camera
    m_cursorPos = Vec2(0, 0);
    updateCamera();
}

void Scene_MapEditor::sRender()
{
    // Update camera position
    updateCamera();
    
    // Set the game view for world rendering
    m_game->window().setView(m_gameView);
    
    drawInfiniteGrid();
    drawPlacedObjects();
    
    // Draw cursor on top of everything in game view (always visible)
    m_cursor.setPosition(m_cursorPos.x * TILE_SIZE, m_cursorPos.y * TILE_SIZE);
    m_game->window().draw(m_cursor);
    
    // Set UI view for interface rendering
    m_game->window().setView(m_uiView);
    
    drawUI();
    drawAssetPreview();
    
    // Draw level selector on top of everything if active
    drawLevelSelector();
}

void Scene_MapEditor::drawInfiniteGrid()
{
    Vec2 gridMin = getVisibleGridMin();
    Vec2 gridMax = getVisibleGridMax();
    
    // Draw vertical grid lines
    for (int x = static_cast<int>(gridMin.x); x <= static_cast<int>(gridMax.x); x++) {
        sf::RectangleShape line(sf::Vector2f(1, (gridMax.y - gridMin.y + 1) * TILE_SIZE));
        line.setPosition(x * TILE_SIZE, gridMin.y * TILE_SIZE);
        line.setFillColor(sf::Color(80, 80, 80, 100));
        m_game->window().draw(line);
    }
    
    // Draw horizontal grid lines
    for (int y = static_cast<int>(gridMin.y); y <= static_cast<int>(gridMax.y); y++) {
        sf::RectangleShape line(sf::Vector2f((gridMax.x - gridMin.x + 1) * TILE_SIZE, 1));
        line.setPosition(gridMin.x * TILE_SIZE, y * TILE_SIZE);
        line.setFillColor(sf::Color(80, 80, 80, 100));
        m_game->window().draw(line);
    }
    
    // Cursor is now drawn separately in sRender() to ensure it's always on top
}

void Scene_MapEditor::drawPlacedObjects()
{
    Vec2 gridMin = getVisibleGridMin();
    Vec2 gridMax = getVisibleGridMax();
    
    // Only draw objects in visible area for performance
    for (int x = static_cast<int>(gridMin.x); x <= static_cast<int>(gridMax.x); x++) {
        for (int y = static_cast<int>(gridMin.y); y <= static_cast<int>(gridMax.y); y++) {
            GridCell* cell = getGridCell(x, y);
            if (cell && cell->occupied) {
                // Try to get the texture for this asset
                try {
                    const sf::Texture& texture = m_game->getAssets().getTexture(cell->asset);
                    sf::Sprite sprite(texture);
                    sprite.setPosition(x * TILE_SIZE, y * TILE_SIZE);
                    
                    // Scale sprite to fit tile size if needed
                    sf::Vector2u textureSize = texture.getSize();
                    float scaleX = static_cast<float>(TILE_SIZE) / textureSize.x;
                    float scaleY = static_cast<float>(TILE_SIZE) / textureSize.y;
                    sprite.setScale(scaleX, scaleY);
                    
                    m_game->window().draw(sprite);
                } catch (...) {
                    // If texture not found, draw a colored rectangle
                    sf::RectangleShape rect(sf::Vector2f(TILE_SIZE, TILE_SIZE));
                    rect.setPosition(x * TILE_SIZE, y * TILE_SIZE);
                    rect.setFillColor(sf::Color::Magenta); // Indicates missing texture
                    m_game->window().draw(rect);
                }
            }
        }
    }
}

void Scene_MapEditor::drawUI()
{
    // Draw UI background - increased height to accommodate all text
    sf::RectangleShape uiBackground(sf::Vector2f(320, 280));
    uiBackground.setPosition(10, 10);
    uiBackground.setFillColor(sf::Color(0, 0, 0, 180));
    uiBackground.setOutlineColor(sf::Color::White);
    uiBackground.setOutlineThickness(1.0f);
    m_game->window().draw(uiBackground);
    
    // Draw UI text with smaller font size to fit more content
    std::ostringstream oss;
    oss << "MAP EDITOR (Infinite Grid)\n";
    oss << "Current Type: " << m_currentType << "\n";
    oss << "Current Asset: " << m_currentAsset << "\n";
    oss << "Cursor: (" << static_cast<int>(m_cursorPos.x) << ", " << static_cast<int>(m_cursorPos.y) << ")\n";
    oss << "Objects: " << m_infiniteGrid.size() << "\n";
    oss << "Save to: metadata/levels/\n\n";
    oss << "Controls:\n";
    oss << "WASD/Arrows: Move cursor\n";
    oss << "Space: Place object\n";
    oss << "X: Remove object\n";
    oss << "Left Click: Place at mouse\n";
    oss << "Right Click: Remove at mouse\n";
    oss << "Q/E: Change asset\n";
    oss << "Z/C: Change type\n";
    oss << "F: Save level\n";
    oss << "L: Select level to load\n";
    oss << "ESC: Back to menu";
    
    // Use smaller font size to fit all text
    m_uiText.setCharacterSize(12);
    m_uiText.setString(oss.str());
    m_uiText.setPosition(20, 20);
    m_game->window().draw(m_uiText);
}

void Scene_MapEditor::drawAssetPreview()
{
    // Position preview on the right side of the screen
    sf::Vector2u windowSize = m_game->window().getSize();
    float previewX = windowSize.x - TILE_SIZE - 20;
    float previewY = 20;
    
    // Draw preview background
    m_previewBackground.setPosition(previewX - 5, previewY - 5);
    m_game->window().draw(m_previewBackground);
    
    // Draw preview border
    m_previewBorder.setPosition(previewX, previewY);
    m_game->window().draw(m_previewBorder);
    
    // Draw preview sprite
    try {
        const sf::Texture& texture = m_game->getAssets().getTexture(m_currentAsset);
        m_previewSprite.setTexture(texture);
        m_previewSprite.setPosition(previewX, previewY);
        
        // Scale sprite to fit preview size
        sf::Vector2u textureSize = texture.getSize();
        float scaleX = static_cast<float>(TILE_SIZE) / textureSize.x;
        float scaleY = static_cast<float>(TILE_SIZE) / textureSize.y;
        m_previewSprite.setScale(scaleX, scaleY);
        
        m_game->window().draw(m_previewSprite);
    } catch (...) {
        // If texture not found, draw a colored rectangle
        sf::RectangleShape rect(sf::Vector2f(TILE_SIZE, TILE_SIZE));
        rect.setPosition(previewX, previewY);
        rect.setFillColor(sf::Color::Magenta);
        m_game->window().draw(rect);
    }
    
    // Draw asset name below preview
    sf::Text previewText;
    previewText.setFont(m_game->getAssets().getFont("ShareTech"));
    previewText.setCharacterSize(12);
    previewText.setFillColor(sf::Color::White);
    previewText.setString(m_currentType + "\n" + m_currentAsset);
    previewText.setPosition(previewX, previewY + TILE_SIZE + 5);
    m_game->window().draw(previewText);
}

void Scene_MapEditor::drawLevelSelector()
{
    if (!m_showLevelSelector) return;
    
    // Calculate selector size based on content
    float selectorWidth = 400;
    float selectorHeight = 300;
    sf::Vector2u windowSize = m_game->window().getSize();
    float selectorX = (windowSize.x - selectorWidth) / 2;
    float selectorY = (windowSize.y - selectorHeight) / 2;
    
    // Draw background
    m_levelSelectorBackground.setSize(sf::Vector2f(selectorWidth, selectorHeight));
    m_levelSelectorBackground.setPosition(selectorX, selectorY);
    m_game->window().draw(m_levelSelectorBackground);
    
    // Draw title
    std::ostringstream oss;
    oss << "SELECT LEVEL TO LOAD\n";
    oss << "Use UP/DOWN to navigate, SPACE/ENTER to select, ESC to cancel\n";
    oss << "Loading from: metadata/levels/\n\n";
    
    // Draw level list
    for (size_t i = 0; i < m_availableLevels.size(); i++) {
        if (i == m_selectedLevelIndex) {
            oss << "> " << m_availableLevels[i] << " <\n";
        } else {
            oss << "  " << m_availableLevels[i] << "\n";
        }
    }
    
    if (m_availableLevels.empty()) {
        oss << "No level files found in metadata/levels/ directory\n";
    }
    
    m_levelSelectorText.setString(oss.str());
    m_levelSelectorText.setPosition(selectorX + 20, selectorY + 20);
    m_game->window().draw(m_levelSelectorText);
}

Vec2 Scene_MapEditor::screenToGrid(const Vec2& screenPos)
{
    return Vec2(std::floor(screenPos.x / TILE_SIZE), std::floor(screenPos.y / TILE_SIZE));
}

Vec2 Scene_MapEditor::gridToScreen(const Vec2& gridPos)
{
    return Vec2(gridPos.x * TILE_SIZE, gridPos.y * TILE_SIZE);
}

void Scene_MapEditor::onEnd()
{
    // Cleanup if needed
}
