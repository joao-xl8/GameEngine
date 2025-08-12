#pragma once

#include "auto_tiling_rule.hpp"
#include <SFML/Graphics.hpp>
#include <map>
#include <memory>

class GameEngine;

// Manages auto-tiling configurations and applies them to tile maps
class AutoTilingManager {
private:
    GameEngine* m_game;
    std::map<std::string, AutoTilingConfig> m_configs;
    std::map<std::string, sf::Texture> m_tilesets;
    
public:
    AutoTilingManager(GameEngine* game);
    ~AutoTilingManager() = default;
    
    // Configuration management
    bool loadConfig(const std::string& tileType, const std::string& configPath);
    bool saveConfig(const std::string& tileType, const std::string& configPath);
    AutoTilingConfig* getConfig(const std::string& tileType);
    
    // Tileset management
    bool loadTileset(const std::string& tileType, const std::string& texturePath);
    sf::Texture* getTileset(const std::string& tileType);
    
    // Auto-tiling operations
    sf::IntRect getAutoTile(const std::string& tileType, int x, int y, 
                           const std::vector<std::vector<std::string>>& tileMap);
    
    // Apply auto-tiling to entire map
    void applyAutoTiling(std::vector<std::vector<std::string>>& tileMap);
    
    // Get neighbors for a position (returns 9-element vector for 3x3 grid)
    std::vector<bool> getNeighbors(const std::string& tileType, int x, int y,
                                  const std::vector<std::vector<std::string>>& tileMap);
    
    // Create default configurations for common tile types
    void createDefaultConfigs();
    
    // Get list of available tile types
    std::vector<std::string> getAvailableTileTypes() const;
};
