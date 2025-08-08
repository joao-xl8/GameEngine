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
    
    // Filter settings
    std::vector<std::string> m_hiddenActions = {
        "UP", "DOWN", "LEFT", "RIGHT",  // Movement keys
        "CONFIRM", "CANCEL"  // We'll show simplified versions
    };
    
    std::map<std::string, std::string> m_actionMappings = {
        {"CONFIRM", "SPACE: Confirm"},
        {"CANCEL", "C: Cancel"},
        {"BACK", "ESC: Back"}
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
