#pragma once
#include "scene.hpp"
#include "../vec2.hpp"
#include <fstream>
#include <filesystem>
#include <map>
#include <set>
#include <algorithm>
#include <cmath>

class Scene_GridMapEditor : public Scene
{
protected:
    // Grid settings - now supports infinite grid
    static const int TILE_SIZE = 64;
    static const int VISIBLE_GRID_WIDTH = 20;
    static const int VISIBLE_GRID_HEIGHT = 15;
    
    // Editor state
    std::string m_currentAsset;
    std::string m_currentType;
    std::vector<std::string> m_availableAssets;
    std::vector<std::string> m_availableTypes;
    size_t m_assetIndex = 0;
    size_t m_typeIndex = 0;
    
    // Infinite grid data - using map for sparse storage with layer support
    struct GridCell {
        std::string type;
        std::string asset;
        bool occupied = false;
        bool hasCollision = false;  // Independent collision property
        float rotation = 0.0f;      // Rotation in degrees
        int width = 1;              // Asset width in cells
        int height = 1;             // Asset height in cells
        int originX = 0;            // Origin X coordinate for multi-cell assets
        int originY = 0;            // Origin Y coordinate for multi-cell assets
    };
    
    // Multi-layer grid: position -> layer -> cell
    std::map<std::pair<int, int>, std::map<int, GridCell>> m_infiniteGrid;
    
    // Asset properties loaded from configuration
    struct AssetProperties {
        int width = 1;
        int height = 1;
        bool defaultCollision = false;
        float defaultRotation = 0.0f;
    };
    std::map<std::string, AssetProperties> m_assetProperties;
    
    // Current layer being edited (0-4)
    int m_currentLayer = 0;
    
    // Current asset rotation (0, 90, 180, 270)
    float m_currentRotation = 0.0f;
    
    // Editor modes
    bool m_showCollision = false;    // Show collision overlay
    bool m_collisionMode = false;    // Toggle collision mode
    
    // Camera/view
    sf::View m_gameView;
    sf::View m_uiView;
    Vec2 m_cameraPos;
    float m_cameraSpeed = 5.0f;
    float m_zoomLevel = 1.0f;         // Current zoom level
    float m_minZoom = 0.25f;          // Minimum zoom (zoomed out)
    float m_maxZoom = 4.0f;           // Maximum zoom (zoomed in)
    float m_zoomStep = 0.1f;          // Zoom increment/decrement
    
    // UI controls
    bool m_showInfoPanel = true;      // Toggle for info panel visibility
    bool m_showAxis = true;          // Toggle for axis display
    
    // UI elements
    sf::Text m_uiText;
    sf::RectangleShape m_cursor;
    Vec2 m_cursorPos;
    
    // Asset preview
    sf::RectangleShape m_previewBackground;
    sf::RectangleShape m_previewBorder;
    sf::Sprite m_previewSprite;
    
    // Level selection GUI
    bool m_showLevelSelector = false;
    std::vector<std::string> m_availableLevels;
    size_t m_selectedLevelIndex = 0;
    sf::RectangleShape m_levelSelectorBackground;
    sf::Text m_levelSelectorText;
    
    // File management
    std::string m_currentFileName;
    bool m_showSaveDialog = false;
    bool m_showOverwriteDialog = false;
    bool m_showExitConfirmDialog = false;  // New exit confirmation dialog
    std::string m_saveFileName;
    std::string m_inputFileName;
    bool m_isInputMode = false;
    bool m_hasUnsavedChanges = false;      // Track unsaved changes
    
    void init();
    void update();
    void onEnd();
    void sDoAction(const Action &action);
    void loadAvailableAssets();
    void loadAssetProperties();
    void updateCamera();
    void drawInfiniteGrid();
    void drawAxis();
    void drawUI();
    void drawAssetPreview();
    void drawPlacedObjects();
    void drawCollisionOverlay();
    void drawAssetSizePreview();
    void drawLevelSelector();
    void drawSaveDialog();
    void drawOverwriteDialog();
    void drawExitConfirmDialog();
    void scanAvailableLevels();
    void handleLevelSelectorInput(const Action& action);
    void handleSaveDialogInput(const Action& action);
    void handleOverwriteDialogInput(const Action& action);
    void handleExitConfirmDialogInput(const Action& action);
    void placeObject();
    void removeObject();
    void toggleCollision();
    void rotateAsset();
    Vec2 calculateRotatedPlacement(int cursorX, int cursorY, int width, int height, float rotation);
    void saveLevel();
    void saveLevel(const std::string& filename);
    void loadLevel(const std::string& filename);
    bool fileExists(const std::string& filename);
    std::string sanitizeFileName(const std::string& input);
    Vec2 screenToGrid(const Vec2& screenPos);
    Vec2 gridToScreen(const Vec2& gridPos);
    Vec2 getVisibleGridMin();
    Vec2 getVisibleGridMax();
    Scene_GridMapEditor::GridCell* getGridCell(int x, int y);
    void setGridCell(int x, int y, const GridCell& cell);
    bool canPlaceAsset(int x, int y, int width, int height);
    void clearMultiCellArea(int x, int y, int width, int height);
    AssetProperties getAssetProperties(const std::string& assetName);
    void markUnsavedChanges();
    void markChangesSaved();
    void confirmExit();
    
public:
    Scene_GridMapEditor(GameEngine* game);
    void sRender();
};
