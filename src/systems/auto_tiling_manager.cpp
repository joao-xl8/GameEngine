#include "auto_tiling_manager.hpp"
#include "../game_engine.hpp"
#include "../constants/tile_constants.hpp"
#include <iostream>
#include <filesystem>

AutoTilingManager::AutoTilingManager(GameEngine* game) : m_game(game) {
    createDefaultConfigs();
}

bool AutoTilingManager::loadConfig(const std::string& tileType, const std::string& configPath) {
    AutoTilingConfig config;
    if (config.loadFromFile(configPath)) {
        m_configs[tileType] = config;
        
        // Also load the tileset if specified
        if (!config.tilesetPath.empty()) {
            loadTileset(tileType, config.tilesetPath);
        }
        
        std::cout << "Loaded auto-tiling config for " << tileType << std::endl;
        return true;
    }
    
    std::cout << "Failed to load auto-tiling config: " << configPath << std::endl;
    return false;
}

bool AutoTilingManager::saveConfig(const std::string& tileType, const std::string& configPath) {
    auto it = m_configs.find(tileType);
    if (it != m_configs.end()) {
        return it->second.saveToFile(configPath);
    }
    return false;
}

AutoTilingConfig* AutoTilingManager::getConfig(const std::string& tileType) {
    auto it = m_configs.find(tileType);
    return (it != m_configs.end()) ? &it->second : nullptr;
}

bool AutoTilingManager::loadTileset(const std::string& tileType, const std::string& texturePath) {
    sf::Texture texture;
    if (texture.loadFromFile(texturePath)) {
        m_tilesets[tileType] = std::move(texture);
        std::cout << "Loaded tileset for " << tileType << ": " << texturePath << std::endl;
        return true;
    }
    
    std::cout << "Failed to load tileset: " << texturePath << std::endl;
    return false;
}

sf::Texture* AutoTilingManager::getTileset(const std::string& tileType) {
    auto it = m_tilesets.find(tileType);
    return (it != m_tilesets.end()) ? &it->second : nullptr;
}

sf::IntRect AutoTilingManager::getAutoTile(const std::string& tileType, int x, int y,
                                          const std::vector<std::vector<std::string>>& tileMap) {
    auto* config = getConfig(tileType);
    if (!config) {
        // Return default tile rect if no config
        return sf::IntRect(0, 0, TileConstants::TILE_SIZE, TileConstants::TILE_SIZE);
    }
    
    // Get neighbors
    std::vector<bool> neighbors = getNeighbors(tileType, x, y, tileMap);
    
    // Find matching rule
    const AutoTilingRule* rule = config->findMatchingRule(neighbors);
    if (rule) {
        return rule->spriteRect;
    }
    
    // Return default tile if no rule matches
    return config->defaultTile;
}

void AutoTilingManager::applyAutoTiling(std::vector<std::vector<std::string>>& tileMap) {
    // This method would update tile indices based on auto-tiling rules
    // For now, it's a placeholder that could be extended to modify the tile map
    std::cout << "Applying auto-tiling to map of size " << tileMap.size() << "x" 
              << (tileMap.empty() ? 0 : tileMap[0].size()) << std::endl;
}

std::vector<bool> AutoTilingManager::getNeighbors(const std::string& tileType, int x, int y,
                                                  const std::vector<std::vector<std::string>>& tileMap) {
    std::vector<bool> neighbors(9, false);
    
    if (tileMap.empty()) return neighbors;
    
    int mapHeight = static_cast<int>(tileMap.size());
    int mapWidth = static_cast<int>(tileMap[0].size());
    
    // Check 3x3 grid around position (x, y)
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            int neighborX = x + dx;
            int neighborY = y + dy;
            int index = (dy + 1) * 3 + (dx + 1); // Convert to 0-8 index
            
            // Check bounds
            if (neighborX >= 0 && neighborX < mapWidth && 
                neighborY >= 0 && neighborY < mapHeight) {
                // Check if neighbor is same tile type
                neighbors[index] = (tileMap[neighborY][neighborX] == tileType);
            } else {
                // Out of bounds - treat as empty
                neighbors[index] = false;
            }
        }
    }
    
    return neighbors;
}

void AutoTilingManager::createDefaultConfigs() {
    // Create default Wall configuration
    AutoTilingConfig wallConfig;
    wallConfig.tileType = "Wall";
    wallConfig.tilesetPath = "assets/imgs/autotiles/wall_64px.png"; // Use our new 64px tileset
    wallConfig.tileSize = TileConstants::TILE_SIZE_VEC;
    wallConfig.defaultTile = sf::IntRect(0, 0, 64, 64); // 64px tiles
    
    // Add some basic wall rules (simplified 4-directional)
    AutoTilingRule wallRule;
    
    // Single wall tile (no neighbors) - use isolated tile
    wallRule.name = "Single";
    wallRule.priority = 1;
    wallRule.spriteRect = sf::IntRect(3 * 64, 3 * 64, 64, 64); // Bottom-right tile (isolated)
    wallRule.conditions = {
        RuleCondition::DIFFERENT, RuleCondition::DIFFERENT, RuleCondition::DIFFERENT,
        RuleCondition::DIFFERENT, RuleCondition::IGNORE,    RuleCondition::DIFFERENT,
        RuleCondition::DIFFERENT, RuleCondition::DIFFERENT, RuleCondition::DIFFERENT
    };
    wallConfig.rules.push_back(wallRule);
    
    // Interior wall (surrounded by walls)
    wallRule.name = "Interior";
    wallRule.priority = 10;
    wallRule.spriteRect = sf::IntRect(0, 0, 64, 64); // Top-left tile (interior)
    wallRule.conditions = {
        RuleCondition::SAME, RuleCondition::SAME, RuleCondition::SAME,
        RuleCondition::SAME, RuleCondition::IGNORE, RuleCondition::SAME,
        RuleCondition::SAME, RuleCondition::SAME, RuleCondition::SAME
    };
    wallConfig.rules.push_back(wallRule);
    
    // Top edge
    wallRule.name = "Top Edge";
    wallRule.priority = 8;
    wallRule.spriteRect = sf::IntRect(1 * 64, 0, 64, 64); // Top edge tile
    wallRule.conditions = {
        RuleCondition::DIFFERENT, RuleCondition::DIFFERENT, RuleCondition::DIFFERENT,
        RuleCondition::SAME,      RuleCondition::IGNORE,    RuleCondition::SAME,
        RuleCondition::SAME,      RuleCondition::SAME,      RuleCondition::SAME
    };
    wallConfig.rules.push_back(wallRule);
    
    // Bottom edge
    wallRule.name = "Bottom Edge";
    wallRule.priority = 8;
    wallRule.spriteRect = sf::IntRect(3 * 64, 0, 64, 64); // Bottom edge tile
    wallRule.conditions = {
        RuleCondition::SAME,      RuleCondition::SAME,      RuleCondition::SAME,
        RuleCondition::SAME,      RuleCondition::IGNORE,    RuleCondition::SAME,
        RuleCondition::DIFFERENT, RuleCondition::DIFFERENT, RuleCondition::DIFFERENT
    };
    wallConfig.rules.push_back(wallRule);
    
    // Left edge
    wallRule.name = "Left Edge";
    wallRule.priority = 8;
    wallRule.spriteRect = sf::IntRect(0, 1 * 64, 64, 64); // Left edge tile
    wallRule.conditions = {
        RuleCondition::DIFFERENT, RuleCondition::SAME, RuleCondition::SAME,
        RuleCondition::DIFFERENT, RuleCondition::IGNORE, RuleCondition::SAME,
        RuleCondition::DIFFERENT, RuleCondition::SAME, RuleCondition::SAME
    };
    wallConfig.rules.push_back(wallRule);
    
    // Right edge
    wallRule.name = "Right Edge";
    wallRule.priority = 8;
    wallRule.spriteRect = sf::IntRect(2 * 64, 0, 64, 64); // Right edge tile
    wallRule.conditions = {
        RuleCondition::SAME, RuleCondition::SAME, RuleCondition::DIFFERENT,
        RuleCondition::SAME, RuleCondition::IGNORE, RuleCondition::DIFFERENT,
        RuleCondition::SAME, RuleCondition::SAME, RuleCondition::DIFFERENT
    };
    wallConfig.rules.push_back(wallRule);
    
    m_configs["Wall"] = wallConfig;
    
    // Load the tileset
    loadTileset("Wall", wallConfig.tilesetPath);
    
    // Create default Ground configuration
    AutoTilingConfig groundConfig;
    groundConfig.tileType = "Ground";
    groundConfig.tilesetPath = "assets/imgs/autotiles/ground_64px.png"; // Use our new 64px tileset
    groundConfig.tileSize = TileConstants::TILE_SIZE_VEC;
    groundConfig.defaultTile = sf::IntRect(0, 0, 64, 64); // 64px tiles
    
    // Basic ground rule - full grass
    AutoTilingRule groundRule;
    groundRule.name = "Grass";
    groundRule.priority = 1;
    groundRule.spriteRect = sf::IntRect(0, 0, 64, 64); // Full grass tile
    groundRule.conditions = {
        RuleCondition::IGNORE, RuleCondition::IGNORE, RuleCondition::IGNORE,
        RuleCondition::IGNORE, RuleCondition::IGNORE, RuleCondition::IGNORE,
        RuleCondition::IGNORE, RuleCondition::IGNORE, RuleCondition::IGNORE
    };
    groundConfig.rules.push_back(groundRule);
    
    m_configs["Ground"] = groundConfig;
    
    // Load the tileset
    loadTileset("Ground", groundConfig.tilesetPath);
    
    // Create default Water configuration
    AutoTilingConfig waterConfig;
    waterConfig.tileType = "Water";
    waterConfig.tilesetPath = "assets/imgs/autotiles/water_64px.png"; // Use our new 64px tileset
    waterConfig.tileSize = TileConstants::TILE_SIZE_VEC;
    waterConfig.defaultTile = sf::IntRect(0, 0, 64, 64); // 64px tiles
    
    // Basic water rule - full water
    AutoTilingRule waterRule;
    waterRule.name = "Water";
    waterRule.priority = 1;
    waterRule.spriteRect = sf::IntRect(0, 0, 64, 64); // Full water tile
    waterRule.conditions = {
        RuleCondition::IGNORE, RuleCondition::IGNORE, RuleCondition::IGNORE,
        RuleCondition::IGNORE, RuleCondition::IGNORE, RuleCondition::IGNORE,
        RuleCondition::IGNORE, RuleCondition::IGNORE, RuleCondition::IGNORE
    };
    waterConfig.rules.push_back(waterRule);
    
    m_configs["Water"] = waterConfig;
    
    // Load the tileset
    loadTileset("Water", waterConfig.tilesetPath);
    
    std::cout << "Created default auto-tiling configurations with 64px tilesets" << std::endl;
}

std::vector<std::string> AutoTilingManager::getAvailableTileTypes() const {
    std::vector<std::string> types;
    for (const auto& [type, config] : m_configs) {
        types.push_back(type);
    }
    return types;
}
