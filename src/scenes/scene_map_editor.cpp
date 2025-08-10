#include "scene_map_editor.hpp"
#include "scene_menu.hpp"
#include "scene_loading.hpp"
#include "../game_engine.hpp"
#include "../action_types.hpp"
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
    // Standard back control
    registerAction(sf::Keyboard::Escape, ActionTypes::BACK);
    
    registerAction(sf::Keyboard::W, ActionTypes::UP);
    registerAction(sf::Keyboard::S, ActionTypes::DOWN);
    registerAction(sf::Keyboard::A, ActionTypes::LEFT);
    registerAction(sf::Keyboard::D, ActionTypes::RIGHT);

    // Standard confirm/cancel controls
    registerAction(sf::Keyboard::Space, ActionTypes::CONFIRM);
    registerAction(sf::Keyboard::C, ActionTypes::CANCEL);
    
    // Asset/Type selection
    registerAction(sf::Keyboard::Q, "PREV_ASSET");
    registerAction(sf::Keyboard::E, "NEXT_ASSET");
    registerAction(sf::Keyboard::Z, "PREV_TYPE");
    registerAction(sf::Keyboard::X, "NEXT_TYPE");
    
    // File operations
    registerAction(sf::Keyboard::F, ActionTypes::SAVE);
    registerAction(sf::Keyboard::L, ActionTypes::LOAD);
    
    // Layer switching keys (1-5)
    registerAction(sf::Keyboard::Num1, "LAYER_0");
    registerAction(sf::Keyboard::Num2, "LAYER_1");
    registerAction(sf::Keyboard::Num3, "LAYER_2");
    registerAction(sf::Keyboard::Num4, "LAYER_3");
    registerAction(sf::Keyboard::Num5, "LAYER_4");
    
    // Register remaining number keys for filename input
    for (int i = sf::Keyboard::Num6; i <= sf::Keyboard::Num9; i++) {
        registerAction(static_cast<sf::Keyboard::Key>(i), "NUMBER_" + std::to_string(i - sf::Keyboard::Num0));
    }
    registerAction(sf::Keyboard::Num0, "NUMBER_0");
    
    // Register backspace for filename editing
    registerAction(sf::Keyboard::Backspace, "BACKSPACE");
    
    // New features
    registerAction(sf::Keyboard::T, "TOGGLE_COLLISION");  // Toggle collision on current cell
    registerAction(sf::Keyboard::R, "ROTATE_ASSET");      // Rotate current asset
    registerAction(sf::Keyboard::V, "SHOW_COLLISION");    // Show/hide collision overlay
    
    
    // Mouse support - use different approach to avoid key conflicts
    // We'll handle mouse input differently in the action system
    
    // Load available assets and types
    loadAvailableAssets();
    loadAssetProperties();
    
    // Set up UI text
    m_uiText.setFont(m_game->getAssets().getFont("ShareTech"));
    m_uiText.setCharacterSize(16);  // Increased from 14 to 16
    m_uiText.setFillColor(sf::Color::White);
    
    // Set up cursor
    m_cursor.setSize(sf::Vector2f(TILE_SIZE, TILE_SIZE));
    m_cursor.setFillColor(sf::Color::Transparent);
    m_cursor.setOutlineColor(sf::Color::Yellow);
    m_cursor.setOutlineThickness(3.0f);
    
    // Initialize camera views
    m_gameView = m_game->window().getDefaultView();
    m_uiView = m_game->window().getDefaultView();
    
    // Asset preview is now created dynamically in drawAssetPreview()
    // No need for static initialization
    
    // Set up level selector
    m_levelSelectorBackground.setFillColor(sf::Color(0, 0, 0, 200));
    m_levelSelectorBackground.setOutlineColor(sf::Color::White);
    m_levelSelectorBackground.setOutlineThickness(2.0f);
    
    m_levelSelectorText.setFont(m_game->getAssets().getFont("ShareTech"));
    m_levelSelectorText.setCharacterSize(18);  // Increased from 16 to 18
    m_levelSelectorText.setFillColor(sf::Color::White);
    
    // Clear infinite grid (it starts empty by default)
    m_infiniteGrid.clear();
    
    std::cout << "Map Editor initialized. Controls:\n";
    std::cout << "WASD Keys: Move cursor\n";
    std::cout << "Space: Place object\n";
    std::cout << "C: Remove object\n";
    std::cout << "Q/E: Change asset\n";
    std::cout << "Z/X: Change type\n";
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
    
    // Add available layer types (0-4)
    m_availableTypes.push_back("0"); // Ground layer (no collision)
    m_availableTypes.push_back("1"); // Decoration layer 1 (collision)
    m_availableTypes.push_back("2"); // Decoration layer 2 (collision)
    m_availableTypes.push_back("3"); // Decoration layer 3 (collision)
    m_availableTypes.push_back("4"); // Entity layer (NPCs, Script Tiles)
    
    // Add available assets (these should match what's in assets.txt)
    m_availableAssets.push_back("Ground");
    m_availableAssets.push_back("Wall");
    m_availableAssets.push_back("Bush");
    m_availableAssets.push_back("SavePoint");
    m_availableAssets.push_back("PlayerSpawn");
    m_availableAssets.push_back("Player");
    m_availableAssets.push_back("Dummy");  // Add Dummy NPC
    m_availableAssets.push_back("Misc1");  // L-shaped asset (2x3)
    m_availableAssets.push_back("Misc2");  // L-shaped asset (3x2)
    m_availableAssets.push_back("LargeCastle");  // 5x5 test asset
    m_availableAssets.push_back("WideWall");     // 5x1 test asset
    m_availableAssets.push_back("TallTower");    // 1x5 test asset
    
    // Set defaults
    if (!m_availableAssets.empty()) {
        m_currentAsset = m_availableAssets[0];
    }
    if (!m_availableTypes.empty()) {
        m_currentType = m_availableTypes[0];
    }
}

void Scene_MapEditor::loadAssetProperties()
{
    // Load asset properties from configuration file
    std::ifstream file("metadata/asset_properties.txt");
    if (!file.is_open()) {
        std::cout << "Warning: Could not open asset_properties.txt, using defaults" << std::endl;
        return;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        std::stringstream ss(line);
        std::string assetName;
        int width, height, collision;
        float rotation;
        
        if (ss >> assetName >> width >> height >> collision >> rotation) {
            AssetProperties props;
            props.width = width;
            props.height = height;
            props.defaultCollision = (collision == 1);
            props.defaultRotation = rotation;
            
            m_assetProperties[assetName] = props;
            std::cout << "Loaded properties for " << assetName << ": " 
                      << width << "x" << height << ", collision=" << collision 
                      << ", rotation=" << rotation << std::endl;
        }
    }
    
    file.close();
    std::cout << "Loaded " << m_assetProperties.size() << " asset property definitions" << std::endl;
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
        // Handle exit confirmation dialog input
        if (m_showExitConfirmDialog) {
            handleExitConfirmDialogInput(action);
            return;
        }
        
        // Handle save dialog input
        if (m_showSaveDialog) {
            handleSaveDialogInput(action);
            return;
        }
        
        // Handle overwrite dialog input
        if (m_showOverwriteDialog) {
            handleOverwriteDialogInput(action);
            return;
        }
        
        // If level selector is open, handle its input
        if (m_showLevelSelector) {
            handleLevelSelectorInput(action);
            return;
        }
        
        if (action.getName() == "BACK") {
            // Check for unsaved changes before exiting
            if (m_hasUnsavedChanges) {
                m_showExitConfirmDialog = true;
            } else {
                Scene_Loading::loadMenuScene(m_game);
            }
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
        else if (action.getName() == "CONFIRM") {
            placeObject();
        }
        else if (action.getName() == "CANCEL") {
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
        // Layer switching - only if save dialog is not open
        else if (action.getName() == "LAYER_0") {
            if (!m_showSaveDialog) {
                m_currentLayer = 0;
                std::cout << "Switched to Layer 0 (Ground - No Collision)" << std::endl;
            }
        }
        else if (action.getName() == "LAYER_1") {
            if (!m_showSaveDialog) {
                m_currentLayer = 1;
                std::cout << "Switched to Layer 1 (Decoration 1 - Collision)" << std::endl;
            }
        }
        else if (action.getName() == "LAYER_2") {
            if (!m_showSaveDialog) {
                m_currentLayer = 2;
                std::cout << "Switched to Layer 2 (Decoration 2 - Collision)" << std::endl;
            }
        }
        else if (action.getName() == "LAYER_3") {
            if (!m_showSaveDialog) {
                m_currentLayer = 3;
                std::cout << "Switched to Layer 3 (Decoration 3 - Collision)" << std::endl;
            }
        }
        else if (action.getName() == "LAYER_4") {
            if (!m_showSaveDialog) {
                m_currentLayer = 4;
                std::cout << "Switched to Layer 4 (Entity - Interactive)" << std::endl;
            }
        }
        // New features
        else if (action.getName() == "TOGGLE_COLLISION") {
            toggleCollision();
        }
        else if (action.getName() == "ROTATE_ASSET") {
            rotateAsset();
        }
        else if (action.getName() == "SHOW_COLLISION") {
            m_showCollision = !m_showCollision;
            std::cout << "Collision overlay: " << (m_showCollision ? "ON" : "OFF") << std::endl;
        }
        else if (action.getName() == "SAVE") {
            if (!m_showSaveDialog && !m_showOverwriteDialog && !m_showLevelSelector) {
                m_showSaveDialog = true;
                m_inputFileName = "";
                m_isInputMode = true;
            }
        }
        else if (action.getName() == "LOAD") {
            if (!m_showSaveDialog && !m_showOverwriteDialog && !m_showLevelSelector) {
                // Show level selector instead of directly loading level_1.txt
                scanAvailableLevels();
                m_showLevelSelector = true;
            }
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
    auto posKey = std::make_pair(x, y);
    auto posIt = m_infiniteGrid.find(posKey);
    if (posIt != m_infiniteGrid.end()) {
        auto layerIt = posIt->second.find(m_currentLayer);
        if (layerIt != posIt->second.end()) {
            return &layerIt->second;
        }
    }
    return nullptr;
}

void Scene_MapEditor::setGridCell(int x, int y, const GridCell& cell)
{
    auto posKey = std::make_pair(x, y);
    if (cell.occupied) {
        m_infiniteGrid[posKey][m_currentLayer] = cell;
    } else {
        // Remove the cell from the current layer
        auto posIt = m_infiniteGrid.find(posKey);
        if (posIt != m_infiniteGrid.end()) {
            posIt->second.erase(m_currentLayer);
            // If no layers remain at this position, remove the position entirely
            if (posIt->second.empty()) {
                m_infiniteGrid.erase(posIt);
            }
        }
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
    int cursorX = static_cast<int>(m_cursorPos.x);
    int cursorY = static_cast<int>(m_cursorPos.y);
    
    // Get asset properties
    AssetProperties props = getAssetProperties(m_currentAsset);
    
    // Calculate the actual placement position using ORIGINAL dimensions
    // The function will handle the rotation internally
    Vec2 placement = calculateRotatedPlacement(cursorX, cursorY, props.width, props.height, m_currentRotation);
    int x = static_cast<int>(std::round(placement.x));
    int y = static_cast<int>(std::round(placement.y));
    
    // Apply rotation to dimensions for placement validation and cell creation
    int width = props.width;
    int height = props.height;
    if (m_currentRotation == 90.0f || m_currentRotation == 270.0f) {
        std::swap(width, height);
    }
    
    // Debug output for placement calculation
    std::cout << "Placement calculation: " << m_currentAsset << " (" << props.width << "x" << props.height 
              << ") at rotation " << m_currentRotation << "°" << std::endl;
    std::cout << "  Cursor: (" << cursorX << ", " << cursorY << ")" << std::endl;
    std::cout << "  Calculated placement: (" << x << ", " << y << ")" << std::endl;
    std::cout << "  Final dimensions: " << width << "x" << height << std::endl;
    
    // Check if we can place the asset at the calculated position
    if (!canPlaceAsset(x, y, width, height)) {
        std::cout << "Cannot place " << m_currentAsset << " at calculated position (" << x << ", " << y 
                  << ") - area occupied or insufficient space" << std::endl;
        return;
    }
    
    // Clear any existing multi-cell area first (in case we're overwriting)
    clearMultiCellArea(x, y, width, height);
    
    // Place the asset on all required cells
    for (int dx = 0; dx < width; dx++) {
        for (int dy = 0; dy < height; dy++) {
            int placeX = x + dx;
            int placeY = y + dy;
            
            GridCell cell;
            cell.type = std::to_string(m_currentLayer);
            cell.asset = m_currentAsset;
            cell.occupied = true;
            cell.hasCollision = props.defaultCollision;
            cell.rotation = m_currentRotation;
            cell.width = props.width;
            cell.height = props.height;
            
            // Store the origin coordinates for this multi-cell asset
            cell.originX = x;  // Origin X coordinate
            cell.originY = y;  // Origin Y coordinate
            
            setGridCell(placeX, placeY, cell);
        }
    }
    
    std::cout << "Placed Layer " << m_currentLayer << " " << m_currentAsset 
              << " (" << width << "x" << height << ") at (" << x << ", " << y 
              << ") rotation=" << m_currentRotation << "° collision=" 
              << (props.defaultCollision ? "ON" : "OFF") << " (cursor at " << cursorX << "," << cursorY << ")" << std::endl;
    
    // Mark that we have unsaved changes
    markUnsavedChanges();
}

void Scene_MapEditor::removeObject()
{
    int x = static_cast<int>(m_cursorPos.x);
    int y = static_cast<int>(m_cursorPos.y);
    
    // Check if there's an object at this position on the current layer
    auto cellKey = std::make_pair(x, y);
    if (m_infiniteGrid.find(cellKey) != m_infiniteGrid.end() && 
        m_infiniteGrid[cellKey].find(m_currentLayer) != m_infiniteGrid[cellKey].end()) {
        
        GridCell& cell = m_infiniteGrid[cellKey][m_currentLayer];
        
        // If it's a multi-cell asset, remove all cells of this asset instance
        if (cell.width > 1 || cell.height > 1) {
            // Apply rotation to dimensions
            int width = cell.width;
            int height = cell.height;
            if (cell.rotation == 90.0f || cell.rotation == 270.0f) {
                std::swap(width, height);
            }
            
            // Use the stored origin coordinates to remove the entire asset
            int originX = cell.originX;
            int originY = cell.originY;
            
            clearMultiCellArea(originX, originY, width, height);
            
            std::cout << "Removed multi-cell " << cell.asset << " (" << width << "x" << height 
                      << ") with origin at (" << originX << ", " << originY << ")" << std::endl;
        } else {
            // Single cell asset - remove just this cell
            m_infiniteGrid[cellKey].erase(m_currentLayer);
            
            // If no layers remain for this cell, remove the cell entirely
            if (m_infiniteGrid[cellKey].empty()) {
                m_infiniteGrid.erase(cellKey);
            }
            
            std::cout << "Removed " << cell.asset << " at (" << x << ", " << y << ")" << std::endl;
        }
        
        // Mark that we have unsaved changes
        markUnsavedChanges();
    } else {
        std::cout << "No object to remove at (" << x << ", " << y << ") on layer " << m_currentLayer << std::endl;
    }
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
        m_availableLevels.push_back("level_1.txt");
        m_availableLevels.push_back("demo_level.txt");
        m_availableLevels.push_back("infinite_test.txt");
    }
    
    // Sort levels alphabetically
    std::sort(m_availableLevels.begin(), m_availableLevels.end());
    
    std::cout << "Found " << m_availableLevels.size() << " level files in metadata/levels/" << std::endl;
}

void Scene_MapEditor::handleLevelSelectorInput(const Action& action)
{
    if (action.getName() == ActionTypes::BACK || action.getName() == ActionTypes::CANCEL) {
        m_showLevelSelector = false;
    }
    else if (action.getName() == ActionTypes::UP) {
        if (m_selectedLevelIndex > 0) {
            m_selectedLevelIndex--;
        } else {
            m_selectedLevelIndex = m_availableLevels.size() - 1;
        }
    }
    else if (action.getName() == "DOWN") {
        m_selectedLevelIndex = (m_selectedLevelIndex + 1) % m_availableLevels.size();
    }
    else if (action.getName() == ActionTypes::CONFIRM || action.getName() == "LOAD") {
        // Load selected level from levels directory
        if (!m_availableLevels.empty()) {
            std::string selectedFile = "metadata/levels/" + m_availableLevels[m_selectedLevelIndex];
            loadLevel(selectedFile);
        }
        m_showLevelSelector = false;
    }
}

void Scene_MapEditor::handleSaveDialogInput(const Action& action)
{
    if (action.getName() == ActionTypes::BACK || action.getName() == ActionTypes::CANCEL) {
        // Cancel save dialog
        m_showSaveDialog = false;
        m_inputFileName = "";
        m_isInputMode = false;
    }
    else if (action.getName() == ActionTypes::CONFIRM || action.getName() == "SAVE") {
        // Confirm filename
        if (!m_inputFileName.empty()) {
            std::string filename = "level_" + m_inputFileName;
            std::string fullPath = "metadata/levels/" + filename + ".txt";
            
            if (fileExists(fullPath)) {
                // File exists, show overwrite dialog
                m_saveFileName = fullPath;
                m_showSaveDialog = false;
                m_showOverwriteDialog = true;
            } else {
                // File doesn't exist, save directly
                saveLevel(fullPath);
                m_showSaveDialog = false;
                m_isInputMode = false;
            }
        }
    }
    else if (action.getName() == "BACKSPACE") {
        // Remove last character (backspace still works for editing)
        if (!m_inputFileName.empty()) {
            m_inputFileName.pop_back();
        }
    }
    // Handle layer keys 1-5 as numbers for filename input when save dialog is open
    else if (action.getName() == "LAYER_0") {
        if (m_inputFileName.length() < 10) {
            m_inputFileName += "1";
        }
    }
    else if (action.getName() == "LAYER_1") {
        if (m_inputFileName.length() < 10) {
            m_inputFileName += "2";
        }
    }
    else if (action.getName() == "LAYER_2") {
        if (m_inputFileName.length() < 10) {
            m_inputFileName += "3";
        }
    }
    else if (action.getName() == "LAYER_3") {
        if (m_inputFileName.length() < 10) {
            m_inputFileName += "4";
        }
    }
    else if (action.getName() == "LAYER_4") {
        if (m_inputFileName.length() < 10) {
            m_inputFileName += "5";
        }
    }
    else if (action.getName().substr(0, 7) == "NUMBER_") {
        // Add number to filename (keys 6-9 and 0)
        int numberIndex = std::stoi(action.getName().substr(7));
        char number = '0' + numberIndex;
        if (m_inputFileName.length() < 10) {
            m_inputFileName += number;
        }
    }
}

void Scene_MapEditor::handleOverwriteDialogInput(const Action& action)
{
    if (action.getName() == ActionTypes::BACK || action.getName() == ActionTypes::CANCEL) {
        // Cancel and go back to save dialog
        m_showOverwriteDialog = false;
        m_showSaveDialog = true;
    }
    else if (action.getName() == ActionTypes::UP || action.getName() == ActionTypes::DOWN) {
        // Toggle between Yes/No (we'll implement this in the draw method)
    }
    else if (action.getName() == ActionTypes::CONFIRM || action.getName() == "SAVE") {
        // Confirm overwrite
        saveLevel(m_saveFileName);
        m_showOverwriteDialog = false;
        m_isInputMode = false;
    }
}

bool Scene_MapEditor::fileExists(const std::string& filename)
{
    std::ifstream file(filename);
    return file.good();
}

std::string Scene_MapEditor::sanitizeFileName(const std::string& input)
{
    std::string result;
    for (char c : input) {
        if (std::isdigit(c)) {
            result += c;
        }
    }
    return result;
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
    
    // Save all placed objects from infinite grid (multi-layer)
    for (const auto& posPair : m_infiniteGrid) {
        int x = posPair.first.first;
        int y = posPair.first.second;
        const auto& layerMap = posPair.second;
        
        // Save all layers at this position
        for (const auto& layerPair : layerMap) {
            const GridCell& cell = layerPair.second;
            
            if (cell.occupied) {
                file << cell.type << " " << cell.asset 
                     << " " << x << " " << y << "\n";
            }
        }
    }
    
    file.close();
    std::cout << "Level saved to " << filename << " (" << m_infiniteGrid.size() << " objects)" << std::endl;
    m_currentFileName = filename;
}

void Scene_MapEditor::saveLevel(const std::string& filename)
{
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cout << "Failed to save level to " << filename << std::endl;
        return;
    }
    
    file << "# Level created with Map Editor\n";
    file << "# Enhanced Format: Layer SpriteName X Y [Collision] [Rotation] [Width] [Height] [OriginX] [OriginY]\n";
    file << "# Collision: 0=false, 1=true | Rotation: degrees | Width/Height: grid cells | OriginX/Y: multi-cell origin\n\n";
    
    // Save all placed objects from infinite grid
    int objectCount = 0;
    for (const auto& posPair : m_infiniteGrid) {
        int x = posPair.first.first;
        int y = posPair.first.second;
        const auto& layerMap = posPair.second;
        
        // Save all layers at this position
        for (const auto& layerPair : layerMap) {
            const GridCell& cell = layerPair.second;
            
            if (cell.occupied) {
                // Basic format: Layer SpriteName X Y
                file << cell.type << " " << cell.asset 
                     << " " << x << " " << y;
                
                // Extended format: add collision, rotation, size, and origin if non-default
                bool hasExtendedData = cell.hasCollision || 
                                     cell.rotation != 0.0f || 
                                     cell.width != 1 || 
                                     cell.height != 1 ||
                                     cell.originX != x ||
                                     cell.originY != y;
                
                if (hasExtendedData) {
                    file << " " << (cell.hasCollision ? 1 : 0)
                         << " " << static_cast<int>(cell.rotation)
                         << " " << cell.width
                         << " " << cell.height
                         << " " << cell.originX
                         << " " << cell.originY;
                }
                
                file << "\n";
                objectCount++;
            }
        }
    }
    
    file.close();
    std::cout << "Level saved to " << filename << " (" << objectCount << " objects)" << std::endl;
    m_currentFileName = filename;
    
    // Mark changes as saved
    markChangesSaved();
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
        int collision = 0;
        int rotation = 0;
        int width = 1;
        int height = 1;
        int originX = -1; // Use -1 to indicate not set
        int originY = -1; // Use -1 to indicate not set
        
        // Read basic format: Layer SpriteName X Y
        if (iss >> type >> asset >> x >> y) {
            GridCell cell;
            cell.type = type;
            cell.asset = asset;
            cell.occupied = true;
            
            // Try to read extended format: Collision Rotation Width Height OriginX OriginY
            if (iss >> collision >> rotation >> width >> height) {
                cell.hasCollision = (collision == 1);
                cell.rotation = static_cast<float>(rotation);
                cell.width = width;
                cell.height = height;
                
                // Try to read origin coordinates (new format)
                if (iss >> originX >> originY) {
                    cell.originX = originX;
                    cell.originY = originY;
                } else {
                    // Old format without origin coordinates - use current position as origin
                    cell.originX = x;
                    cell.originY = y;
                }
            } else {
                // Use default values or asset properties
                AssetProperties props = getAssetProperties(asset);
                cell.hasCollision = props.defaultCollision;
                cell.rotation = props.defaultRotation;
                cell.width = props.width;
                cell.height = props.height;
                cell.originX = x; // Default origin to current position
                cell.originY = y;
            }
            
            // Parse layer from type (should be 0-4)
            int layer = 0;
            try {
                layer = std::stoi(type);
                if (layer < 0 || layer > 4) {
                    layer = 0; // Default to ground layer for invalid layers
                }
            } catch (...) {
                // If type is not a number, try to map old format
                if (type == "Tile") layer = 1;      // Old tiles go to decoration layer 1
                else if (type == "Dec") layer = 4;  // Old decorations go to entity layer
                else if (type == "NPC") layer = 4;  // NPCs go to entity layer
                else layer = 0;                     // Default to ground layer
            }
            
            auto posKey = std::make_pair(x, y);
            m_infiniteGrid[posKey][layer] = cell;
            objectCount++;
            
            std::cout << "Loaded " << asset << " at (" << x << ", " << y 
                      << ") Layer " << layer << " collision=" << (cell.hasCollision ? "ON" : "OFF")
                      << " rotation=" << cell.rotation << "° size=" << cell.width << "x" << cell.height
                      << " origin=(" << cell.originX << "," << cell.originY << ")" << std::endl;
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
    
    // Draw collision overlay if enabled
    if (m_showCollision) {
        drawCollisionOverlay();
    }
    
    // Draw asset size preview
    drawAssetSizePreview();
    
    // Draw cursor on top of everything in game view (always visible)
    m_cursor.setPosition(m_cursorPos.x * TILE_SIZE, m_cursorPos.y * TILE_SIZE);
    m_game->window().draw(m_cursor);
    
    // Set UI view for interface rendering
    m_game->window().setView(m_uiView);
    
    drawUI();
    drawAssetPreview();
    
    // Draw dialogs on top of everything if active
    drawLevelSelector();
    drawSaveDialog();
    drawOverwriteDialog();
    drawExitConfirmDialog();
    
    // Draw command overlay (always on top)
    renderCommandOverlay();
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
    
    // Keep track of multi-cell assets we've already drawn to avoid duplicates
    std::set<std::string> drawnAssets; // Use unique identifier for each asset instance
    
    // Render layers in order (0-4) for proper layering
    for (int layer = 0; layer <= 4; layer++) {
        // Only draw objects in visible area for performance
        for (int x = static_cast<int>(gridMin.x); x <= static_cast<int>(gridMax.x); x++) {
            for (int y = static_cast<int>(gridMin.y); y <= static_cast<int>(gridMax.y); y++) {
                auto posKey = std::make_pair(x, y);
                auto posIt = m_infiniteGrid.find(posKey);
                if (posIt != m_infiniteGrid.end()) {
                    auto layerIt = posIt->second.find(layer);
                    if (layerIt != posIt->second.end() && layerIt->second.occupied) {
                        GridCell& cell = layerIt->second;
                        
                        // For multi-cell assets, only draw once at the origin cell
                        AssetProperties props = getAssetProperties(cell.asset);
                        bool isMultiCell = (props.width > 1 || props.height > 1);
                        
                        if (isMultiCell) {
                            // Check if this is the origin cell
                            bool isOriginCell = (x == cell.originX && y == cell.originY);
                            
                            if (!isOriginCell) continue; // Skip non-origin cells
                            
                            // Create unique identifier for this asset instance
                            std::string assetId = std::to_string(layer) + "_" + 
                                                std::to_string(cell.originX) + "_" + 
                                                std::to_string(cell.originY) + "_" + 
                                                cell.asset;
                            
                            // Check if we've already drawn this asset instance
                            if (drawnAssets.find(assetId) != drawnAssets.end()) continue;
                            drawnAssets.insert(assetId);
                        }
                        
                        // Try to get the texture for this asset
                        try {
                            const sf::Texture& texture = m_game->getAssets().getTexture(cell.asset);
                            sf::Sprite sprite(texture);
                            
                            if (isMultiCell) {
                                // For multi-cell assets, scale to fit the entire asset area
                                sf::Vector2u textureSize = texture.getSize();
                                
                                // Calculate the actual occupied area dimensions after rotation
                                int occupiedWidth = props.width;
                                int occupiedHeight = props.height;
                                if (cell.rotation == 90.0f || cell.rotation == 270.0f) {
                                    std::swap(occupiedWidth, occupiedHeight);
                                }
                                
                                // For rotated assets, we need to handle scaling differently
                                if (cell.rotation == 90.0f || cell.rotation == 270.0f) {
                                    // For 90/270 degree rotations, swap the scaling
                                    float scaleX = static_cast<float>(occupiedWidth * TILE_SIZE) / textureSize.y;
                                    float scaleY = static_cast<float>(occupiedHeight * TILE_SIZE) / textureSize.x;
                                    sprite.setScale(scaleX, scaleY);
                                } else {
                                    // For 0/180 degree rotations, use normal scaling
                                    float scaleX = static_cast<float>(occupiedWidth * TILE_SIZE) / textureSize.x;
                                    float scaleY = static_cast<float>(occupiedHeight * TILE_SIZE) / textureSize.y;
                                    sprite.setScale(scaleX, scaleY);
                                }
                                
                                // Set rotation and origin for proper rotation around center
                                if (cell.rotation != 0.0f) {
                                    // Set origin to center of the texture
                                    sprite.setOrigin(textureSize.x / 2.0f, textureSize.y / 2.0f);
                                    sprite.setRotation(cell.rotation);
                                    
                                    // Position at the center of the occupied area
                                    float centerX = x * TILE_SIZE + (occupiedWidth * TILE_SIZE) / 2.0f;
                                    float centerY = y * TILE_SIZE + (occupiedHeight * TILE_SIZE) / 2.0f;
                                    sprite.setPosition(centerX, centerY);
                                } else {
                                    // No rotation - position at top-left
                                    sprite.setPosition(x * TILE_SIZE, y * TILE_SIZE);
                                }
                            } else {
                                // For single-cell assets, scale to fit tile size
                                sf::Vector2u textureSize = texture.getSize();
                                float scaleX = static_cast<float>(TILE_SIZE) / textureSize.x;
                                float scaleY = static_cast<float>(TILE_SIZE) / textureSize.y;
                                sprite.setScale(scaleX, scaleY);
                                
                                // Apply rotation for single-cell assets
                                if (cell.rotation != 0.0f) {
                                    sprite.setOrigin(textureSize.x / 2.0f, textureSize.y / 2.0f);
                                    sprite.setRotation(cell.rotation);
                                    sprite.setPosition(x * TILE_SIZE + TILE_SIZE / 2.0f, y * TILE_SIZE + TILE_SIZE / 2.0f);
                                } else {
                                    sprite.setPosition(x * TILE_SIZE, y * TILE_SIZE);
                                }
                            }
                            
                            // Add slight transparency to non-current layers for visual feedback
                            if (layer != m_currentLayer) {
                                sprite.setColor(sf::Color(255, 255, 255, 180));
                            }
                            
                            m_game->window().draw(sprite);
                        } catch (...) {
                            // If texture not found, draw a colored rectangle
                            if (isMultiCell) {
                                // Draw rectangle covering the entire multi-cell area
                                sf::RectangleShape rect(sf::Vector2f(props.width * TILE_SIZE, props.height * TILE_SIZE));
                                rect.setPosition(x * TILE_SIZE, y * TILE_SIZE);
                                rect.setFillColor(sf::Color::Magenta); // Indicates missing texture
                                
                                if (layer != m_currentLayer) {
                                    rect.setFillColor(sf::Color(255, 0, 255, 180));
                                }
                                
                                m_game->window().draw(rect);
                            } else {
                                // Single cell fallback
                                sf::RectangleShape rect(sf::Vector2f(TILE_SIZE, TILE_SIZE));
                                rect.setPosition(x * TILE_SIZE, y * TILE_SIZE);
                                rect.setFillColor(sf::Color::Magenta);
                                
                                if (layer != m_currentLayer) {
                                    rect.setFillColor(sf::Color(255, 0, 255, 180));
                                }
                                
                                m_game->window().draw(rect);
                            }
                        }
                    }
                }
            }
        }
        
        // Clear drawn assets set for next layer
        drawnAssets.clear();
    }
}

void Scene_MapEditor::drawUI()
{
    // Draw UI background - increased height by 150px, decreased width by 30px
    sf::RectangleShape uiBackground(sf::Vector2f(320, 600));
    uiBackground.setPosition(10, 10);
    uiBackground.setFillColor(sf::Color(0, 0, 0, 180));
    uiBackground.setOutlineColor(sf::Color::White);
    uiBackground.setOutlineThickness(1.0f);
    m_game->window().draw(uiBackground);
    
    // Draw UI text with smaller font size to fit more content
    std::ostringstream oss;
    oss << "MAP EDITOR \n";
    
    // Show current loaded map info
    if (!m_currentFileName.empty()) {
        // Extract just the filename from the full path
        std::string displayName = m_currentFileName;
        size_t lastSlash = displayName.find_last_of("/\\");
        if (lastSlash != std::string::npos) {
            displayName = displayName.substr(lastSlash + 1);
        }
        oss << "Current Map: " << displayName;
        if (m_hasUnsavedChanges) {
            oss << " *"; // Asterisk indicates unsaved changes
        }
        oss << "\n";
    } else {
        oss << "Current Map: <new map>";
        if (m_hasUnsavedChanges) {
            oss << " *"; // Asterisk indicates unsaved changes
        }
        oss << "\n";
    }
    
    // Show current layer information
    oss << "Current Layer: " << m_currentLayer;
    if (m_currentLayer == 0) oss << " (Ground)";
    else if (m_currentLayer == 1) oss << " (Decoration 1)";
    else if (m_currentLayer == 2) oss << " (Decoration 2)";
    else if (m_currentLayer == 3) oss << " (Decoration 3)";
    else if (m_currentLayer == 4) oss << " (Entity)";
    oss << "\n";
    
    // Show current asset and its properties
    oss << "Current Asset: " << m_currentAsset;
    if (!m_currentAsset.empty()) {
        AssetProperties props = getAssetProperties(m_currentAsset);
        oss << " (" << props.width << "x" << props.height << ")";
    }
    oss << "\n";
    
    // Show current rotation
    oss << "Rotation: " << static_cast<int>(m_currentRotation) << "°\n";
    
    // Show cursor position
    oss << "Cursor: (" << static_cast<int>(m_cursorPos.x) << ", " << static_cast<int>(m_cursorPos.y) << ")\n";
    
    // Show collision overlay status
    oss << "Collision Overlay: " << (m_showCollision ? "ON" : "OFF") << "\n";
    
    // Count total objects across all layers
    int totalObjects = 0;
    for (const auto& pos : m_infiniteGrid) {
        totalObjects += pos.second.size();
    }
    oss << "Total Objects: " << totalObjects << "\n";
    oss << "Save to: metadata/levels/\n";
    
    // Add division line
    oss << "-----------------------\n";
    
    // Add cursor tile information for current layer
    int cursorX = static_cast<int>(m_cursorPos.x);
    int cursorY = static_cast<int>(m_cursorPos.y);
    GridCell* cursorCell = getGridCell(cursorX, cursorY);
    
    oss << "CURSOR TILE INFO (Layer " << m_currentLayer << "):\n";
    if (cursorCell && cursorCell->occupied) {
        oss << "Layer: " << cursorCell->type;
        if (cursorCell->type == "0") oss << " (Ground)";
        else if (cursorCell->type == "1") oss << " (Decoration 1)";
        else if (cursorCell->type == "2") oss << " (Decoration 2)";
        else if (cursorCell->type == "3") oss << " (Decoration 3)";
        else if (cursorCell->type == "4") oss << " (Entity)";
        oss << "\n";
        oss << "Asset: " << cursorCell->asset << "\n";
        oss << "Size: " << cursorCell->width << "x" << cursorCell->height << "\n";
        oss << "Rotation: " << static_cast<int>(cursorCell->rotation) << "°\n";
        oss << "Collision: " << (cursorCell->hasCollision ? "ON" : "OFF") << "\n";
        oss << "Position: (" << cursorX << ", " << cursorY << ")\n";
        oss << "Status: OCCUPIED";
    } else {
        oss << "Position: (" << cursorX << ", " << cursorY << ")\n";
        oss << "Status: EMPTY\n";
        oss << "Ready to place: Layer " << m_currentLayer << " " << m_currentAsset;
    }
    
    // Add controls section
    oss << "\n-----------------------\n";
    oss << "CONTROLS:\n";
    oss << "WASD: Move cursor\n";
    oss << "Mouse: Click to place/remove\n";
    oss << "1-5: Switch layers\n";
    oss << "Q/E: Change asset\n";
    oss << "R: Rotate asset (" << static_cast<int>(m_currentRotation) << "°)\n";
    oss << "T: Toggle collision on cell\n";
    oss << "V: Show collision overlay\n";
    oss << "F: Save level\n";
    oss << "L: Load level\n";
    oss << "ESC: Back to menu";
    
    // Use clean font size for display
    m_uiText.setCharacterSize(16);  // Increased from 14 to 16
    m_uiText.setString(oss.str());
    m_uiText.setPosition(20, 20);
    m_game->window().draw(m_uiText);
}

void Scene_MapEditor::drawAssetPreview()
{
    // Get asset properties to determine preview size
    AssetProperties props = getAssetProperties(m_currentAsset);
    
    // Apply rotation to dimensions for preview sizing (clockwise rotation)
    int displayWidth = props.width;
    int displayHeight = props.height;
    if (m_currentRotation == 90.0f || m_currentRotation == 270.0f) {
        std::swap(displayWidth, displayHeight);
    }
    
    // Preview sizing constants
    const float MAX_PREVIEW_DIMENSION = 200.0f; // Maximum width or height for preview
    const float MIN_PREVIEW_DIMENSION = 80.0f;   // Minimum width or height for preview
    const float TILE_PREVIEW_SIZE = 40.0f;       // Size per tile in preview
    
    // Calculate base preview dimensions using rotated dimensions (tiles * size per tile)
    float baseWidth = static_cast<float>(displayWidth) * TILE_PREVIEW_SIZE;
    float baseHeight = static_cast<float>(displayHeight) * TILE_PREVIEW_SIZE;
    
    // Calculate scaling factor to fit within max dimensions while preserving aspect ratio
    float scaleX = MAX_PREVIEW_DIMENSION / baseWidth;
    float scaleY = MAX_PREVIEW_DIMENSION / baseHeight;
    float scale = std::min(scaleX, scaleY); // Use the smaller scale to fit both dimensions
    
    // Apply scaling but ensure minimum size
    float previewWidth = std::max(MIN_PREVIEW_DIMENSION, baseWidth * scale);
    float previewHeight = std::max(MIN_PREVIEW_DIMENSION, baseHeight * scale);
    
    // If we had to enforce minimum size, recalculate to maintain aspect ratio
    float actualAspectRatio = static_cast<float>(displayWidth) / static_cast<float>(displayHeight);
    
    if (baseWidth * scale < MIN_PREVIEW_DIMENSION) {
        previewWidth = MIN_PREVIEW_DIMENSION;
        previewHeight = previewWidth / actualAspectRatio;
    } else if (baseHeight * scale < MIN_PREVIEW_DIMENSION) {
        previewHeight = MIN_PREVIEW_DIMENSION;
        previewWidth = previewHeight * actualAspectRatio;
    }
    
    // Position preview on the right side of the screen with proper spacing
    sf::Vector2u windowSize = m_game->window().getSize();
    float margin = 20.0f;
    float previewX = windowSize.x - previewWidth - margin;
    float previewY = margin;
    
    // Create and draw preview background (larger than the preview for padding)
    sf::RectangleShape background;
    float backgroundPadding = 8.0f;
    background.setSize(sf::Vector2f(previewWidth + backgroundPadding * 2, previewHeight + backgroundPadding * 2));
    background.setPosition(previewX - backgroundPadding, previewY - backgroundPadding);
    background.setFillColor(sf::Color(40, 40, 40, 220)); // Dark semi-transparent background
    background.setOutlineColor(sf::Color(100, 100, 100, 255)); // Gray border
    background.setOutlineThickness(2.0f);
    m_game->window().draw(background);
    
    // Create and draw preview border (exact size of preview)
    sf::RectangleShape border;
    border.setSize(sf::Vector2f(previewWidth, previewHeight));
    border.setPosition(previewX, previewY);
    border.setFillColor(sf::Color::Transparent);
    border.setOutlineColor(sf::Color(255, 255, 0, 200)); // Yellow border with slight transparency
    border.setOutlineThickness(2.0f);
    m_game->window().draw(border);
    
    // Draw the asset texture with proper rotation
    try {
        const sf::Texture& texture = m_game->getAssets().getTexture(m_currentAsset);
        sf::Sprite sprite(texture);
        sf::Vector2u textureSize = texture.getSize();
        
        // Always set origin to center for consistent rotation
        sprite.setOrigin(textureSize.x / 2.0f, textureSize.y / 2.0f);
        
        // Apply rotation
        sprite.setRotation(m_currentRotation);
        
        // Position at the center of the preview area
        sprite.setPosition(previewX + previewWidth / 2.0f, previewY + previewHeight / 2.0f);
        
        // Scale sprite to fit the preview dimensions
        // For rotated sprites, we need to map texture dimensions correctly to preview dimensions
        float spriteScaleX, spriteScaleY;
        
        if (m_currentRotation == 90.0f || m_currentRotation == 270.0f) {
            // For 90°/270° rotations, texture width maps to preview height and vice versa
            spriteScaleX = previewWidth / static_cast<float>(textureSize.y);
            spriteScaleY = previewHeight / static_cast<float>(textureSize.x);
        } else {
            // For 0°/180° rotations, normal mapping
            spriteScaleX = previewWidth / static_cast<float>(textureSize.x);
            spriteScaleY = previewHeight / static_cast<float>(textureSize.y);
        }
        
        sprite.setScale(spriteScaleX, spriteScaleY);
        
        m_game->window().draw(sprite);
    } catch (...) {
        // If texture not found, draw a colored rectangle with error pattern
        sf::RectangleShape errorRect(sf::Vector2f(previewWidth, previewHeight));
        errorRect.setPosition(previewX, previewY);
        errorRect.setFillColor(sf::Color(255, 0, 255, 180)); // Magenta indicates missing texture
        m_game->window().draw(errorRect);
        
        // Draw an X pattern to indicate error
        sf::RectangleShape line1(sf::Vector2f(std::sqrt(previewWidth*previewWidth + previewHeight*previewHeight), 2.0f));
        line1.setPosition(previewX, previewY);
        line1.setFillColor(sf::Color::Black);
        line1.setRotation(std::atan2(previewHeight, previewWidth) * 180.0f / 3.14159f);
        m_game->window().draw(line1);
        
        sf::RectangleShape line2(sf::Vector2f(std::sqrt(previewWidth*previewWidth + previewHeight*previewHeight), 2.0f));
        line2.setPosition(previewX, previewY + previewHeight);
        line2.setFillColor(sf::Color::Black);
        line2.setRotation(-std::atan2(previewHeight, previewWidth) * 180.0f / 3.14159f);
        m_game->window().draw(line2);
    }
    
    // Draw asset information below the preview
    sf::Text infoText;
    infoText.setFont(m_game->getAssets().getFont("ShareTech"));
    infoText.setCharacterSize(16); // Slightly larger text
    infoText.setFillColor(sf::Color::White);
    
    // Create detailed asset information showing both original and rotated dimensions
    std::string assetInfo = "Layer: " + m_currentType + "\n" +
                           "Asset: " + m_currentAsset + "\n";
    
    if (m_currentRotation != 0.0f) {
        assetInfo += "Original: " + std::to_string(props.width) + "x" + std::to_string(props.height) + "\n";
        assetInfo += "Rotated: " + std::to_string(displayWidth) + "x" + std::to_string(displayHeight) + "\n";
        assetInfo += "Rotation: " + std::to_string(static_cast<int>(m_currentRotation)) + "°\n";
    } else {
        assetInfo += "Size: " + std::to_string(props.width) + "x" + std::to_string(props.height) + "\n";
    }
    
    assetInfo += "Collision: " + std::string(props.defaultCollision ? "ON" : "OFF");
    
    infoText.setString(assetInfo);
    
    // Position text below preview with margin
    float textMargin = 15.0f;
    float textY = previewY + previewHeight + backgroundPadding + textMargin;
    
    // Calculate text bounds for background sizing
    sf::FloatRect textBounds = infoText.getLocalBounds();
    
    // Create text background
    sf::RectangleShape textBackground;
    float textPadding = 6.0f;
    float textBackgroundWidth = std::max(previewWidth + backgroundPadding * 2, textBounds.width + textPadding * 2);
    float textBackgroundHeight = textBounds.height + textPadding * 2;
    
    textBackground.setSize(sf::Vector2f(textBackgroundWidth, textBackgroundHeight));
    textBackground.setPosition(previewX - backgroundPadding, textY - textPadding);
    textBackground.setFillColor(sf::Color(20, 20, 20, 200)); // Dark background for text
    textBackground.setOutlineColor(sf::Color(80, 80, 80, 255));
    textBackground.setOutlineThickness(1.0f);
    m_game->window().draw(textBackground);
    
    // Center text horizontally within the background
    float centeredTextX = previewX - backgroundPadding + (textBackgroundWidth - textBounds.width) / 2.0f;
    infoText.setPosition(centeredTextX, textY);
    
    // Draw the text
    m_game->window().draw(infoText);
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

void Scene_MapEditor::drawSaveDialog()
{
    if (!m_showSaveDialog) return;
    
    // Calculate dialog size and position
    float dialogWidth = 500;
    float dialogHeight = 200;
    sf::Vector2u windowSize = m_game->window().getSize();
    float dialogX = (windowSize.x - dialogWidth) / 2;
    float dialogY = (windowSize.y - dialogHeight) / 2;
    
    // Draw background
    sf::RectangleShape background;
    background.setSize(sf::Vector2f(dialogWidth, dialogHeight));
    background.setPosition(dialogX, dialogY);
    background.setFillColor(sf::Color(40, 40, 60));
    background.setOutlineColor(sf::Color::White);
    background.setOutlineThickness(2);
    m_game->window().draw(background);
    
    // Draw dialog text
    sf::Text dialogText;
    dialogText.setFont(m_game->getAssets().getFont("ShareTech"));
    dialogText.setCharacterSize(18);  // Increased from 16 to 18
    dialogText.setFillColor(sf::Color::White);
    
    std::ostringstream oss;
    oss << "SAVE LEVEL\n\n";
    oss << "Enter level number (numbers only):\n";
    oss << "Filename will be: level_" << m_inputFileName << ".txt\n\n";
    oss << "Current input: " << m_inputFileName << "_\n\n";
    oss << "Use number keys (0-9), Backspace to edit\n";
    oss << "Press SPACE to confirm, C to cancel";
    
    dialogText.setString(oss.str());
    dialogText.setPosition(dialogX + 20, dialogY + 20);
    m_game->window().draw(dialogText);
}

void Scene_MapEditor::drawOverwriteDialog()
{
    if (!m_showOverwriteDialog) return;
    
    // Calculate dialog size and position
    float dialogWidth = 450;
    float dialogHeight = 170;
    sf::Vector2u windowSize = m_game->window().getSize();
    float dialogX = (windowSize.x - dialogWidth) / 2;
    float dialogY = (windowSize.y - dialogHeight) / 2;
    
    // Draw background
    sf::RectangleShape background;
    background.setSize(sf::Vector2f(dialogWidth, dialogHeight));
    background.setPosition(dialogX, dialogY);
    background.setFillColor(sf::Color(60, 40, 40));
    background.setOutlineColor(sf::Color::Red);
    background.setOutlineThickness(2);
    m_game->window().draw(background);
    
    // Draw dialog text
    sf::Text dialogText;
    dialogText.setFont(m_game->getAssets().getFont("ShareTech"));
    dialogText.setCharacterSize(18);  // Increased from 16 to 18
    dialogText.setFillColor(sf::Color::White);
    
    std::ostringstream oss;
    oss << "FILE ALREADY EXISTS\n\n";
    oss << "The file already exists:\n";
    oss << m_saveFileName << "\n\n";
    oss << "Do you want to overwrite it?\n";
    oss << "Press SPACE to overwrite, C to cancel";
    
    dialogText.setString(oss.str());
    dialogText.setPosition(dialogX + 20, dialogY + 20);
    m_game->window().draw(dialogText);
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

void Scene_MapEditor::toggleCollision()
{
    // Toggle collision on the current cell
    int x = static_cast<int>(m_cursorPos.x);
    int y = static_cast<int>(m_cursorPos.y);
    
    auto cellKey = std::make_pair(x, y);
    if (m_infiniteGrid.find(cellKey) != m_infiniteGrid.end() && 
        m_infiniteGrid[cellKey].find(m_currentLayer) != m_infiniteGrid[cellKey].end()) {
        
        GridCell& cell = m_infiniteGrid[cellKey][m_currentLayer];
        cell.hasCollision = !cell.hasCollision;
        
        std::cout << "Toggled collision at (" << x << ", " << y << ") Layer " << m_currentLayer 
                  << ": " << (cell.hasCollision ? "ON" : "OFF") << std::endl;
        
        // Mark that we have unsaved changes
        markUnsavedChanges();
    } else {
        std::cout << "No object at (" << x << ", " << y << ") Layer " << m_currentLayer << " to toggle collision" << std::endl;
    }
}

Vec2 Scene_MapEditor::calculateRotatedPlacement(int cursorX, int cursorY, int width, int height, float rotation)
{
    Vec2 placement;
    
    // For corner-based rotation, we need to consider the rotated dimensions
    // to calculate the correct offset from the cursor position
    
    if (rotation == 0.0f) {
        // No rotation - place with top-left corner at cursor
        placement.x = cursorX;
        placement.y = cursorY;
    }
    else if (rotation == 90.0f) {
        // 90° clockwise rotation
        // After rotation: width becomes height, height becomes width
        // The cursor should be at the top-right corner of the rotated asset
        // So we need to offset by the new width (which is the original height)
        int rotatedWidth = height;  // After 90° rotation
        placement.x = cursorX - (rotatedWidth - 1);
        placement.y = cursorY;
    }
    else if (rotation == 180.0f) {
        // 180° rotation - dimensions stay the same
        // Cursor should be at bottom-right corner
        placement.x = cursorX - (width - 1);
        placement.y = cursorY - (height - 1);
    }
    else if (rotation == 270.0f) {
        // 270° clockwise rotation
        // After rotation: width becomes height, height becomes width
        // The cursor should be at the bottom-left corner of the rotated asset
        // So we need to offset by the new height (which is the original width)
        int rotatedHeight = width;  // After 270° rotation
        placement.x = cursorX;
        placement.y = cursorY - (rotatedHeight - 1);
    }
    
    return placement;
}

void Scene_MapEditor::rotateAsset()
{
    // Rotate current asset selection
    m_currentRotation += 90.0f;
    if (m_currentRotation >= 360.0f) {
        m_currentRotation = 0.0f;
    }
    
    // Debug output for rotation
    AssetProperties props = getAssetProperties(m_currentAsset);
    int rotatedWidth = props.width;
    int rotatedHeight = props.height;
    if (m_currentRotation == 90.0f || m_currentRotation == 270.0f) {
        std::swap(rotatedWidth, rotatedHeight);
    }
    
    std::cout << "Asset rotation: " << m_currentRotation << "° - " << m_currentAsset 
              << " (" << props.width << "x" << props.height << ") -> (" 
              << rotatedWidth << "x" << rotatedHeight << ")" << std::endl;
}

Scene_MapEditor::AssetProperties Scene_MapEditor::getAssetProperties(const std::string& assetName)
{
    auto it = m_assetProperties.find(assetName);
    if (it != m_assetProperties.end()) {
        return it->second;
    }
    
    // Return default properties if not found
    AssetProperties defaultProps;
    defaultProps.width = 1;
    defaultProps.height = 1;
    defaultProps.defaultCollision = false;
    defaultProps.defaultRotation = 0.0f;
    return defaultProps;
}

bool Scene_MapEditor::canPlaceAsset(int x, int y, int width, int height)
{
    // Check if all cells in the area are free on the current layer
    for (int dx = 0; dx < width; dx++) {
        for (int dy = 0; dy < height; dy++) {
            int checkX = x + dx;
            int checkY = y + dy;
            
            auto cellKey = std::make_pair(checkX, checkY);
            if (m_infiniteGrid.find(cellKey) != m_infiniteGrid.end() && 
                m_infiniteGrid[cellKey].find(m_currentLayer) != m_infiniteGrid[cellKey].end()) {
                // Cell is occupied on this layer
                return false;
            }
        }
    }
    return true;
}

void Scene_MapEditor::clearMultiCellArea(int x, int y, int width, int height)
{
    // Clear all cells in the area on the current layer
    for (int dx = 0; dx < width; dx++) {
        for (int dy = 0; dy < height; dy++) {
            int clearX = x + dx;
            int clearY = y + dy;
            
            auto cellKey = std::make_pair(clearX, clearY);
            if (m_infiniteGrid.find(cellKey) != m_infiniteGrid.end()) {
                m_infiniteGrid[cellKey].erase(m_currentLayer);
                
                // If no layers remain for this cell, remove the cell entirely
                if (m_infiniteGrid[cellKey].empty()) {
                    m_infiniteGrid.erase(cellKey);
                }
            }
        }
    }
}

void Scene_MapEditor::drawCollisionOverlay()
{
    // Draw collision indicators for all objects with collision
    Vec2 gridMin = getVisibleGridMin();
    Vec2 gridMax = getVisibleGridMax();
    
    for (int x = static_cast<int>(gridMin.x); x <= static_cast<int>(gridMax.x); x++) {
        for (int y = static_cast<int>(gridMin.y); y <= static_cast<int>(gridMax.y); y++) {
            auto cellKey = std::make_pair(x, y);
            if (m_infiniteGrid.find(cellKey) != m_infiniteGrid.end()) {
                for (auto& layerPair : m_infiniteGrid[cellKey]) {
                    const GridCell& cell = layerPair.second;
                    
                    if (cell.hasCollision && cell.occupied) {
                        // Draw red overlay for collision
                        sf::RectangleShape collisionOverlay;
                        collisionOverlay.setSize(sf::Vector2f(TILE_SIZE - 2, TILE_SIZE - 2));
                        collisionOverlay.setPosition(x * TILE_SIZE + 1, y * TILE_SIZE + 1);
                        collisionOverlay.setFillColor(sf::Color(255, 0, 0, 100)); // Semi-transparent red
                        collisionOverlay.setOutlineColor(sf::Color::Red);
                        collisionOverlay.setOutlineThickness(1);
                        
                        m_game->window().draw(collisionOverlay);
                    }
                }
            }
        }
    }
}

void Scene_MapEditor::drawAssetSizePreview()
{
    // Draw preview of asset size at cursor position
    if (m_currentAsset.empty()) return;
    
    AssetProperties props = getAssetProperties(m_currentAsset);
    
    int cursorX = static_cast<int>(m_cursorPos.x);
    int cursorY = static_cast<int>(m_cursorPos.y);
    
    // Calculate the actual placement position using ORIGINAL dimensions
    // The function will handle the rotation internally
    Vec2 placement = calculateRotatedPlacement(cursorX, cursorY, props.width, props.height, m_currentRotation);
    int previewX = static_cast<int>(std::round(placement.x));
    int previewY = static_cast<int>(std::round(placement.y));
    
    // Apply rotation to dimensions for preview rendering
    int width = props.width;
    int height = props.height;
    if (m_currentRotation == 90.0f || m_currentRotation == 270.0f) {
        std::swap(width, height);
    }
    
    // Check if this asset can be placed at the calculated position
    bool canPlace = canPlaceAsset(previewX, previewY, width, height);
    
    // First, draw the actual asset sprite (same logic as placed assets)
    try {
        const sf::Texture& texture = m_game->getAssets().getTexture(m_currentAsset);
        sf::Sprite sprite(texture);
        sf::Vector2u textureSize = texture.getSize();
        
        // Calculate the actual occupied area dimensions after rotation
        int occupiedWidth = props.width;
        int occupiedHeight = props.height;
        if (m_currentRotation == 90.0f || m_currentRotation == 270.0f) {
            std::swap(occupiedWidth, occupiedHeight);
        }
        
        // For rotated assets, we need to handle scaling differently
        if (m_currentRotation == 90.0f || m_currentRotation == 270.0f) {
            // For 90/270 degree rotations, swap the scaling
            float scaleX = static_cast<float>(occupiedWidth * TILE_SIZE) / textureSize.y;
            float scaleY = static_cast<float>(occupiedHeight * TILE_SIZE) / textureSize.x;
            sprite.setScale(scaleX, scaleY);
        } else {
            // For 0/180 degree rotations, use normal scaling
            float scaleX = static_cast<float>(occupiedWidth * TILE_SIZE) / textureSize.x;
            float scaleY = static_cast<float>(occupiedHeight * TILE_SIZE) / textureSize.y;
            sprite.setScale(scaleX, scaleY);
        }
        
        // Set rotation and origin for proper rotation around center
        sprite.setOrigin(textureSize.x / 2.0f, textureSize.y / 2.0f);
        sprite.setRotation(m_currentRotation);
        
        // Position at the center of the occupied area
        float centerX = previewX * TILE_SIZE + (occupiedWidth * TILE_SIZE) / 2.0f;
        float centerY = previewY * TILE_SIZE + (occupiedHeight * TILE_SIZE) / 2.0f;
        sprite.setPosition(centerX, centerY);
        
        // Make it semi-transparent to show it's a preview
        sprite.setColor(sf::Color(255, 255, 255, 180));
        
        m_game->window().draw(sprite);
    } catch (...) {
        // If texture not found, we'll just show the colored rectangles
    }
    
    // Then draw preview rectangles on top for each cell the asset would occupy
    for (int dx = 0; dx < width; dx++) {
        for (int dy = 0; dy < height; dy++) {
            int cellX = previewX + dx;
            int cellY = previewY + dy;
            
            sf::RectangleShape preview;
            preview.setSize(sf::Vector2f(TILE_SIZE - 6, TILE_SIZE - 6)); // Slightly smaller for better visibility
            preview.setPosition(cellX * TILE_SIZE + 3, cellY * TILE_SIZE + 3);
            
            if (canPlace) {
                // Green preview for valid placement - more transparent since asset is underneath
                preview.setFillColor(sf::Color(0, 255, 0, 60)); // Less opaque
                preview.setOutlineColor(sf::Color(0, 200, 0, 255)); // Slightly darker green outline
            } else {
                // Red preview for invalid placement - more transparent since asset is underneath
                preview.setFillColor(sf::Color(255, 0, 0, 60)); // Less opaque
                preview.setOutlineColor(sf::Color(200, 0, 0, 255)); // Slightly darker red outline
            }
            
            preview.setOutlineThickness(2);
            m_game->window().draw(preview);
        }
    }
    
    // For larger assets (3x3 or bigger), also draw a connecting outline around the entire area
    if (width >= 3 || height >= 3) {
        sf::RectangleShape overallOutline;
        overallOutline.setSize(sf::Vector2f(width * TILE_SIZE - 2, height * TILE_SIZE - 2));
        overallOutline.setPosition(previewX * TILE_SIZE + 1, previewY * TILE_SIZE + 1);
        overallOutline.setFillColor(sf::Color::Transparent);
        
        if (canPlace) {
            overallOutline.setOutlineColor(sf::Color(0, 255, 0, 200)); // Green for valid
        } else {
            overallOutline.setOutlineColor(sf::Color(255, 0, 0, 200)); // Red for invalid
        }
        
        overallOutline.setOutlineThickness(3);
        m_game->window().draw(overallOutline);
    }
    
    // Draw cursor indicator at the actual cursor position
    sf::CircleShape cursorIndicator(8);
    cursorIndicator.setPosition(cursorX * TILE_SIZE + TILE_SIZE/2 - 8, cursorY * TILE_SIZE + TILE_SIZE/2 - 8);
    cursorIndicator.setFillColor(sf::Color(255, 255, 0, 150)); // Semi-transparent yellow
    cursorIndicator.setOutlineColor(sf::Color::Yellow);
    cursorIndicator.setOutlineThickness(2);
    m_game->window().draw(cursorIndicator);
    
    // Draw rotation indicator
    if (m_currentRotation != 0.0f) {
        sf::Text rotationText;
        rotationText.setFont(m_game->getAssets().getFont("ShareTech"));
        rotationText.setCharacterSize(16);
        rotationText.setFillColor(sf::Color::Yellow);
        rotationText.setString(std::to_string(static_cast<int>(m_currentRotation)) + "°");
        rotationText.setPosition(cursorX * TILE_SIZE + 5, cursorY * TILE_SIZE + 5);
        
        m_game->window().draw(rotationText);
    }
}

void Scene_MapEditor::markUnsavedChanges()
{
    m_hasUnsavedChanges = true;
}

void Scene_MapEditor::markChangesSaved()
{
    m_hasUnsavedChanges = false;
}

void Scene_MapEditor::confirmExit()
{
    Scene_Loading::loadMenuScene(m_game);
}

void Scene_MapEditor::handleExitConfirmDialogInput(const Action& action)
{
    if (action.getName() == ActionTypes::BACK || action.getName() == ActionTypes::CANCEL) {
        // Cancel exit, go back to editing
        m_showExitConfirmDialog = false;
    }
    else if (action.getName() == "SAVE") {
        // Save first, then exit
        m_showExitConfirmDialog = false;
        m_showSaveDialog = true;
        m_inputFileName = "";
        m_isInputMode = true;
        // Note: After saving, we'll need to exit - we'll handle this in the save completion
    }
    else if (action.getName() == "CONFIRM") {
        // Exit without saving
        confirmExit();
    }
}

void Scene_MapEditor::drawExitConfirmDialog()
{
    if (!m_showExitConfirmDialog) return;
    
    // Calculate dialog size and position - increased height by 50px
    float dialogWidth = 500;
    float dialogHeight = 250;
    sf::Vector2u windowSize = m_game->window().getSize();
    float dialogX = (windowSize.x - dialogWidth) / 2;
    float dialogY = (windowSize.y - dialogHeight) / 2;
    
    // Draw background
    sf::RectangleShape background;
    background.setSize(sf::Vector2f(dialogWidth, dialogHeight));
    background.setPosition(dialogX, dialogY);
    background.setFillColor(sf::Color(60, 60, 40)); // Yellow-ish tint for warning
    background.setOutlineColor(sf::Color::Yellow);
    background.setOutlineThickness(2);
    m_game->window().draw(background);
    
    // Draw dialog text
    sf::Text dialogText;
    dialogText.setFont(m_game->getAssets().getFont("ShareTech"));
    dialogText.setCharacterSize(18);
    dialogText.setFillColor(sf::Color::White);
    
    std::ostringstream oss;
    oss << "UNSAVED CHANGES\n\n";
    oss << "You have unsaved changes in your level.\n";
    oss << "Are you sure you want to exit without saving?\n\n";
    oss << "Your changes will be lost!\n\n";
    oss << "Press SPACE to exit without saving\n";
    oss << "Press ESC to cancel and continue editing\n";
    oss << "Press F to save first, then exit";
    
    dialogText.setString(oss.str());
    dialogText.setPosition(dialogX + 20, dialogY + 20);
    m_game->window().draw(dialogText);
}
