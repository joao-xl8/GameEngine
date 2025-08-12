#pragma once

#include <SFML/Graphics.hpp>

namespace TileConstants {
    // Global tile size used throughout the engine
    constexpr int TILE_SIZE = 32;
    constexpr int TILE_WIDTH = TILE_SIZE;
    constexpr int TILE_HEIGHT = TILE_SIZE;
    
    // Tile size as SFML Vector2i for convenience
    const sf::Vector2i TILE_SIZE_VEC(TILE_SIZE, TILE_SIZE);
    
    // Common tile-related calculations
    constexpr float TILE_SIZE_F = static_cast<float>(TILE_SIZE);
    
    // Grid-related constants
    constexpr int GRID_LINE_THICKNESS = 1;
    constexpr int GRID_HIGHLIGHT_THICKNESS = 2;
    
    // Auto-tiling specific constants
    constexpr int AUTOTILE_GRID_SIZE = 3; // 3x3 grid for neighbor checking
    constexpr int AUTOTILE_NEIGHBOR_COUNT = 9; // Total neighbors in 3x3 grid
    constexpr int AUTOTILE_CENTER_INDEX = 4; // Center position in 3x3 grid (0-8)
    
    // Tile palette constants
    constexpr int PALETTE_TILE_SPACING = 5; // Space between tiles in palette
    constexpr int PALETTE_TILE_BORDER = 2; // Border around selected tile
    
    // Map editor constants
    constexpr int DEFAULT_MAP_WIDTH = 50;
    constexpr int DEFAULT_MAP_HEIGHT = 50;
    constexpr int MIN_MAP_SIZE = 10;
    constexpr int MAX_MAP_SIZE = 200;
    
    // Utility functions for tile calculations
    inline sf::Vector2i pixelToTile(const sf::Vector2f& pixelPos) {
        return sf::Vector2i(
            static_cast<int>(pixelPos.x) / TILE_SIZE,
            static_cast<int>(pixelPos.y) / TILE_SIZE
        );
    }
    
    inline sf::Vector2f tileToPixel(const sf::Vector2i& tilePos) {
        return sf::Vector2f(
            static_cast<float>(tilePos.x * TILE_SIZE),
            static_cast<float>(tilePos.y * TILE_SIZE)
        );
    }
    
    inline sf::Vector2f tileToPixel(int tileX, int tileY) {
        return sf::Vector2f(
            static_cast<float>(tileX * TILE_SIZE),
            static_cast<float>(tileY * TILE_SIZE)
        );
    }
    
    inline bool isValidTilePosition(int x, int y, int mapWidth, int mapHeight) {
        return x >= 0 && x < mapWidth && y >= 0 && y < mapHeight;
    }
    
    inline sf::IntRect createTileRect(int tileX, int tileY) {
        return sf::IntRect(tileX * TILE_SIZE, tileY * TILE_SIZE, TILE_SIZE, TILE_SIZE);
    }
    
    inline sf::IntRect createTileRect(const sf::Vector2i& tilePos) {
        return createTileRect(tilePos.x, tilePos.y);
    }
}
