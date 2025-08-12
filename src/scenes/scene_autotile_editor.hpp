#pragma once

#include "scene.hpp"
#include "../systems/auto_tiling_manager.hpp"
#include "../ui/command_overlay.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <map>

class Scene_AutoTileEditor : public Scene {
private:
    // Auto-tiling system
    std::unique_ptr<AutoTilingManager> m_autoTilingManager;
    
    // Map data
    std::vector<std::vector<std::string>> m_tileMap;
    int m_mapWidth;
    int m_mapHeight;
    
    // Rendering
    sf::View m_mapView;
    sf::RectangleShape m_gridLine;
    std::map<std::string, sf::Texture> m_tileTextures;
    std::vector<sf::Sprite> m_tileSprites; // Rendered tiles
    
    // UI Elements
    sf::Text m_titleText;
    sf::Text m_toolText;
    sf::RectangleShape m_uiBackground;
    sf::RectangleShape m_tilePalette;
    
    // Tile palette
    std::vector<std::string> m_availableTileTypes;
    std::vector<sf::Sprite> m_paletteSprites;
    int m_selectedTileType;
    sf::RectangleShape m_paletteSelector;
    
    // Editor state
    sf::Vector2i m_selectedTile;
    sf::Vector2i m_lastMouseTile;
    bool m_showGrid;
    bool m_autoTileMode;
    bool m_isPainting;
    bool m_isErasing;
    
    // Camera controls
    sf::Vector2f m_cameraPosition;
    float m_zoomLevel;
    
    // Constants - responsive sizing
    int m_uiHeight;
    int m_paletteHeight;
    static const float ZOOM_SPEED;
    static const float CAMERA_SPEED;
    
    // Minimum sizes for usability
    static const int MIN_UI_HEIGHT = 80;
    static const int MIN_PALETTE_HEIGHT = 50;
    
public:
    Scene_AutoTileEditor(GameEngine* gameEngine);
    
    void init() override;
    void onEnd() override;
    void update() override;
    void sDoAction(const Action& action) override;
    void sRender() override;
    
private:
    // Initialization
    void setupUI();
    void calculateResponsiveLayout();
    void setupTilePalette();
    void loadTileTextures();
    void initializeMap(int width, int height);
    
    // Map operations
    void placeTile(int x, int y, const std::string& tileType);
    void eraseTile(int x, int y);
    void applyAutoTiling();
    void applyAutoTilingAt(int x, int y);
    void updateTileSprites();
    
    // Input handling
    void handleMouseInput();
    void handleKeyboardInput();
    sf::Vector2i getMouseTilePosition();
    sf::Vector2i screenToTileCoords(sf::Vector2i screenPos);
    
    // Rendering
    void renderMap();
    void renderGrid();
    void renderUI();
    void renderTilePalette();
    void renderCursor();
    
    // Camera controls
    void updateCamera();
    void zoomIn();
    void zoomOut();
    void moveCamera(sf::Vector2f direction);
    
    // File operations
    void saveMap();
    void loadMap();
    void newMap();
    
    // Utility
    bool isValidTilePosition(int x, int y) const;
    std::string getCurrentTileType() const;
    void selectTileType(int index);
    void cycleTileType(int direction);
    
    // Auto-tiling helpers
    void refreshAutoTilingAroundPosition(int x, int y);
    sf::IntRect getAutoTileRect(const std::string& tileType, int x, int y);
};
