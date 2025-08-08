#pragma once
#include "scene.hpp"
#include "../vec2.hpp"
#include <fstream>
#include <filesystem>
#include <map>

class Scene_MapEditor : public Scene
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
    };
    
    // Multi-layer grid: position -> layer -> cell
    std::map<std::pair<int, int>, std::map<int, GridCell>> m_infiniteGrid;
    
    // Current layer being edited (0-4)
    int m_currentLayer = 0;
    
    // Camera/view
    sf::View m_gameView;
    sf::View m_uiView;
    Vec2 m_cameraPos;
    float m_cameraSpeed = 5.0f;
    
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
    std::string m_saveFileName;
    std::string m_inputFileName;
    bool m_isInputMode = false;
    
    void init();
    void update();
    void onEnd();
    void sDoAction(const Action &action);
    void loadAvailableAssets();
    void updateCamera();
    void drawInfiniteGrid();
    void drawUI();
    void drawAssetPreview();
    void drawPlacedObjects();
    void drawLevelSelector();
    void drawSaveDialog();
    void drawOverwriteDialog();
    void scanAvailableLevels();
    void handleLevelSelectorInput(const Action& action);
    void handleSaveDialogInput(const Action& action);
    void handleOverwriteDialogInput(const Action& action);
    void placeObject();
    void removeObject();
    void saveLevel();
    void saveLevel(const std::string& filename);
    void loadLevel(const std::string& filename);
    bool fileExists(const std::string& filename);
    std::string sanitizeFileName(const std::string& input);
    Vec2 screenToGrid(const Vec2& screenPos);
    Vec2 gridToScreen(const Vec2& gridPos);
    Vec2 getVisibleGridMin();
    Vec2 getVisibleGridMax();
    Scene_MapEditor::GridCell* getGridCell(int x, int y);
    void setGridCell(int x, int y, const GridCell& cell);
    
public:
    Scene_MapEditor(GameEngine* game);
    void sRender();
};
