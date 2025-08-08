#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <map>

class GameEngine;

class CommandOverlay
{
private:
    GameEngine* m_game;
    sf::RectangleShape m_background;
    sf::Text m_text;
    std::vector<std::pair<std::string, std::string>> m_commands; // key, description
    
    // Filter settings - hide individual movement keys, show simplified versions
    std::vector<std::string> m_hiddenActions = {
        "UP", "DOWN", "LEFT", "RIGHT",  // Movement keys (shown as WASD/Arrows)
        "INTERACT", "SELECT"  // Will be shown as standardized versions
    };
    
    std::map<std::string, std::string> m_actionMappings = {
        // Standard controls across all scenes
        {"SELECT", "SPACE: Confirm/Select"},
        {"CONFIRM", "SPACE: Confirm"},
        {"BACK", "C: Back/Cancel"},
        {"CANCEL", "C: Cancel"},
        {"QUIT", "C: Exit"},
        {"PAUSE", "ESC: Pause Menu"},
        {"INTERACT", "E: Interact"},
        
        // Special scene controls
        {"SAVE", "F: Save"},
        {"LOAD", "L: Load"},
        {"PREV_ASSET", "Q: Previous Asset"},
        {"NEXT_ASSET", "E: Next Asset"},
        {"PREV_TYPE", "Z: Previous Type"},
        {"NEXT_TYPE", "V: Next Type"},
        
        // Debug controls
        {"TOGGLE_TEXTURE", "T: Toggle Textures"},
        {"TOGGLE_COLLISION", "C: Toggle Collision"},
        {"TOGGLE_GRID", "G: Toggle Grid"}
    };

public:
    CommandOverlay(GameEngine* game);
    void update(const std::map<std::string, std::vector<int>>& registeredActions);
    void render();
    void setVisible(bool visible) { m_visible = visible; }
    
private:
    bool m_visible = true;
    void setupUI();
    std::string getKeyName(int keyCode);
    bool shouldShowAction(const std::string& action);
    std::string getActionDescription(const std::string& action);
};
