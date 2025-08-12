#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

// Neighbor positions for 3x3 grid around a tile
enum class NeighborPosition {
    TOP_LEFT = 0, TOP = 1, TOP_RIGHT = 2,
    LEFT = 3, CENTER = 4, RIGHT = 5,
    BOTTOM_LEFT = 6, BOTTOM = 7, BOTTOM_RIGHT = 8
};

// Rule condition for each neighbor position
enum class RuleCondition {
    IGNORE,     // Don't care about this position
    SAME,       // Must be same tile type
    DIFFERENT,  // Must be different tile type
    EMPTY       // Must be empty/null
};

// Auto-tiling rule that determines which sprite to use
struct AutoTilingRule {
    std::string name;                                    // Rule name for identification
    std::vector<RuleCondition> conditions;              // 9 conditions for 3x3 grid
    sf::IntRect spriteRect;                             // Source rectangle in tileset
    int priority;                                       // Higher priority rules checked first
    
    AutoTilingRule() : conditions(9, RuleCondition::IGNORE), priority(0) {}
    
    // Check if this rule matches the given neighbor pattern
    bool matches(const std::vector<bool>& neighbors) const;
    
    // Convert rule to string for saving/loading
    std::string toString() const;
    
    // Load rule from string
    static AutoTilingRule fromString(const std::string& str);
};

// Auto-tiling configuration for a specific tile type
struct AutoTilingConfig {
    std::string tileType;                               // Tile type name (e.g., "Wall", "Ground")
    std::string tilesetPath;                            // Path to tileset image
    sf::Vector2i tileSize;                              // Size of each tile in pixels
    std::vector<AutoTilingRule> rules;                  // List of tiling rules
    sf::IntRect defaultTile;                            // Default tile when no rules match
    
    AutoTilingConfig() : tileSize(64, 64) {}
    
    // Find the best matching rule for given neighbors
    const AutoTilingRule* findMatchingRule(const std::vector<bool>& neighbors) const;
    
    // Save configuration to file
    bool saveToFile(const std::string& filepath) const;
    
    // Load configuration from file
    bool loadFromFile(const std::string& filepath);
};
