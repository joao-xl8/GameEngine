#include "auto_tiling_rule.hpp"
#include <sstream>
#include <fstream>
#include <algorithm>

bool AutoTilingRule::matches(const std::vector<bool>& neighbors) const {
    if (neighbors.size() != 9) return false;
    
    for (int i = 0; i < 9; i++) {
        if (i == 4) continue; // Skip center position
        
        switch (conditions[i]) {
            case RuleCondition::IGNORE:
                // Don't care about this position
                break;
            case RuleCondition::SAME:
                if (!neighbors[i]) return false;
                break;
            case RuleCondition::DIFFERENT:
                if (neighbors[i]) return false;
                break;
            case RuleCondition::EMPTY:
                if (neighbors[i]) return false;
                break;
        }
    }
    
    return true;
}

std::string AutoTilingRule::toString() const {
    std::stringstream ss;
    ss << name << "|" << priority << "|";
    ss << spriteRect.left << "," << spriteRect.top << "," << spriteRect.width << "," << spriteRect.height << "|";
    
    for (const auto& condition : conditions) {
        ss << static_cast<int>(condition) << ",";
    }
    
    return ss.str();
}

AutoTilingRule AutoTilingRule::fromString(const std::string& str) {
    AutoTilingRule rule;
    std::stringstream ss(str);
    std::string token;
    
    // Parse name
    if (std::getline(ss, token, '|')) {
        rule.name = token;
    }
    
    // Parse priority
    if (std::getline(ss, token, '|')) {
        rule.priority = std::stoi(token);
    }
    
    // Parse sprite rect
    if (std::getline(ss, token, '|')) {
        std::stringstream rectSS(token);
        std::string coord;
        if (std::getline(rectSS, coord, ',')) rule.spriteRect.left = std::stoi(coord);
        if (std::getline(rectSS, coord, ',')) rule.spriteRect.top = std::stoi(coord);
        if (std::getline(rectSS, coord, ',')) rule.spriteRect.width = std::stoi(coord);
        if (std::getline(rectSS, coord, ',')) rule.spriteRect.height = std::stoi(coord);
    }
    
    // Parse conditions
    if (std::getline(ss, token, '|')) {
        std::stringstream condSS(token);
        std::string condStr;
        int i = 0;
        while (std::getline(condSS, condStr, ',') && i < 9) {
            if (!condStr.empty()) {
                rule.conditions[i] = static_cast<RuleCondition>(std::stoi(condStr));
            }
            i++;
        }
    }
    
    return rule;
}

const AutoTilingRule* AutoTilingConfig::findMatchingRule(const std::vector<bool>& neighbors) const {
    // Sort rules by priority (higher first) and find first match
    auto sortedRules = rules;
    std::sort(sortedRules.begin(), sortedRules.end(), 
              [](const AutoTilingRule& a, const AutoTilingRule& b) {
                  return a.priority > b.priority;
              });
    
    for (const auto& rule : sortedRules) {
        if (rule.matches(neighbors)) {
            return &rule;
        }
    }
    
    return nullptr; // No matching rule found
}

bool AutoTilingConfig::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) return false;
    
    file << "# Auto-tiling configuration for " << tileType << std::endl;
    file << "TileType=" << tileType << std::endl;
    file << "TilesetPath=" << tilesetPath << std::endl;
    file << "TileSize=" << tileSize.x << "," << tileSize.y << std::endl;
    file << "DefaultTile=" << defaultTile.left << "," << defaultTile.top << "," 
         << defaultTile.width << "," << defaultTile.height << std::endl;
    file << std::endl;
    
    file << "# Rules (Name|Priority|SpriteRect|Conditions)" << std::endl;
    for (const auto& rule : rules) {
        file << "Rule=" << rule.toString() << std::endl;
    }
    
    file.close();
    return true;
}

bool AutoTilingConfig::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) return false;
    
    std::string line;
    rules.clear();
    
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        
        size_t equalPos = line.find('=');
        if (equalPos == std::string::npos) continue;
        
        std::string key = line.substr(0, equalPos);
        std::string value = line.substr(equalPos + 1);
        
        if (key == "TileType") {
            tileType = value;
        } else if (key == "TilesetPath") {
            tilesetPath = value;
        } else if (key == "TileSize") {
            std::stringstream ss(value);
            std::string coord;
            if (std::getline(ss, coord, ',')) tileSize.x = std::stoi(coord);
            if (std::getline(ss, coord, ',')) tileSize.y = std::stoi(coord);
        } else if (key == "DefaultTile") {
            std::stringstream ss(value);
            std::string coord;
            if (std::getline(ss, coord, ',')) defaultTile.left = std::stoi(coord);
            if (std::getline(ss, coord, ',')) defaultTile.top = std::stoi(coord);
            if (std::getline(ss, coord, ',')) defaultTile.width = std::stoi(coord);
            if (std::getline(ss, coord, ',')) defaultTile.height = std::stoi(coord);
        } else if (key == "Rule") {
            rules.push_back(AutoTilingRule::fromString(value));
        }
    }
    
    file.close();
    return true;
}
