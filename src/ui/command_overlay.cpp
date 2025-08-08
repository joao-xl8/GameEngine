#include "command_overlay.hpp"
#include "../game_engine.hpp"
#include <sstream>
#include <algorithm>

CommandOverlay::CommandOverlay(GameEngine* game) : m_game(game)
{
    setupUI();
}

void CommandOverlay::setupUI()
{
    // Setup background
    m_background.setFillColor(sf::Color(0, 0, 0, 180));
    m_background.setOutlineColor(sf::Color(100, 100, 100));
    m_background.setOutlineThickness(1.0f);
    
    // Setup text - check if font is available
    try {
        m_text.setFont(m_game->getAssets().getFont("ShareTech"));
        m_text.setCharacterSize(12);
        m_text.setFillColor(sf::Color::White);
    } catch (const std::exception& e) {
        std::cout << "Warning: Could not set font for command overlay: " << e.what() << std::endl;
        // Continue without font - SFML will use default
    }
}

void CommandOverlay::update(const std::map<std::string, std::vector<int>>& registeredActions)
{
    m_commands.clear();
    
    // Always show basic commands first
    bool hasConfirm = false, hasCancel = false, hasBack = false;
    
    // Check what actions are available
    for (const auto& pair : registeredActions) {
        const std::string& action = pair.first;
        
        if (action == "CONFIRM" || action == "PLACE" || action == "SELECT") {
            hasConfirm = true;
        } else if (action == "CANCEL" || action == "REMOVE") {
            hasCancel = true;
        } else if (action == "BACK") {
            hasBack = true;
        }
    }
    
    // Add standardized commands
    if (hasConfirm) {
        m_commands.push_back({"SPACE", "Confirm"});
    }
    if (hasCancel) {
        m_commands.push_back({"C", "Cancel"});
    }
    if (hasBack) {
        m_commands.push_back({"ESC", "Back"});
    }
    
    // Add specific scene commands (filtered)
    for (const auto& pair : registeredActions) {
        const std::string& action = pair.first;
        
        if (shouldShowAction(action)) {
            std::string keyName = getKeyName(pair.second[0]); // Use first key
            std::string description = getActionDescription(action);
            
            // Avoid duplicates
            bool alreadyAdded = false;
            for (const auto& cmd : m_commands) {
                if (cmd.first == keyName || cmd.second == description) {
                    alreadyAdded = true;
                    break;
                }
            }
            
            if (!alreadyAdded) {
                m_commands.push_back({keyName, description});
            }
        }
    }
}

void CommandOverlay::render()
{
    if (!m_visible || m_commands.empty() || !m_game) {
        return;
    }
    
    // Check if window is available
    if (!m_game->window().isOpen()) {
        return;
    }
    
    // Get window size for positioning
    sf::Vector2u windowSize = m_game->window().getSize();
    
    // Build command string
    std::ostringstream oss;
    for (size_t i = 0; i < m_commands.size(); i++) {
        if (i > 0) oss << "  |  ";
        oss << m_commands[i].first << ": " << m_commands[i].second;
    }
    
    m_text.setString(oss.str());
    
    // Calculate size and position
    sf::FloatRect textBounds = m_text.getLocalBounds();
    float padding = 8.0f;
    float bgWidth = textBounds.width + padding * 2;
    float bgHeight = textBounds.height + padding * 2;
    
    // Position in bottom-right corner
    float posX = windowSize.x - bgWidth - 10;
    float posY = windowSize.y - bgHeight - 10;
    
    // Set background
    m_background.setSize(sf::Vector2f(bgWidth, bgHeight));
    m_background.setPosition(posX, posY);
    
    // Set text position
    m_text.setPosition(posX + padding, posY + padding);
    
    // Draw
    try {
        m_game->window().draw(m_background);
        m_game->window().draw(m_text);
    } catch (const std::exception& e) {
        std::cout << "Warning: Command overlay draw failed: " << e.what() << std::endl;
    }
}

std::string CommandOverlay::getKeyName(int keyCode)
{
    // Convert SFML key codes to readable names
    switch (keyCode) {
        case sf::Keyboard::Space: return "SPACE";
        case sf::Keyboard::Enter: return "ENTER";
        case sf::Keyboard::Escape: return "ESC";
        case sf::Keyboard::Backspace: return "BKSP";
        case sf::Keyboard::F: return "F";
        case sf::Keyboard::L: return "L";
        case sf::Keyboard::Q: return "Q";
        case sf::Keyboard::E: return "E";
        case sf::Keyboard::Z: return "Z";
        case sf::Keyboard::V: return "V";
        case sf::Keyboard::C: return "C";
        case sf::Keyboard::X: return "X";
        case sf::Keyboard::F11: return "F11";
        case sf::Keyboard::Num0: return "0";
        case sf::Keyboard::Num1: return "1";
        case sf::Keyboard::Num2: return "2";
        case sf::Keyboard::Num3: return "3";
        case sf::Keyboard::Num4: return "4";
        case sf::Keyboard::Num5: return "5";
        case sf::Keyboard::Num6: return "6";
        case sf::Keyboard::Num7: return "7";
        case sf::Keyboard::Num8: return "8";
        case sf::Keyboard::Num9: return "9";
        default:
            // For other keys, try to convert to char
            if (keyCode >= sf::Keyboard::A && keyCode <= sf::Keyboard::Z) {
                return std::string(1, 'A' + (keyCode - sf::Keyboard::A));
            }
            return "?";
    }
}

bool CommandOverlay::shouldShowAction(const std::string& action)
{
    // Skip movement keys
    if (action == "UP" || action == "DOWN" || action == "LEFT" || action == "RIGHT") {
        return false;
    }
    
    // Skip actions we handle specially
    if (action == "CONFIRM" || action == "PLACE" || action == "SELECT" ||
        action == "CANCEL" || action == "REMOVE" || action == "BACK") {
        return false;
    }
    
    // Skip number keys (too many for map editor)
    if (action.substr(0, 7) == "NUMBER_") {
        return false;
    }
    
    return true;
}

std::string CommandOverlay::getActionDescription(const std::string& action)
{
    // Map action names to user-friendly descriptions
    if (action == "SAVE") return "Save";
    if (action == "LOAD") return "Load";
    if (action == "PREV_ASSET") return "Prev Asset";
    if (action == "NEXT_ASSET") return "Next Asset";
    if (action == "PREV_TYPE") return "Prev Type";
    if (action == "NEXT_TYPE") return "Next Type";
    if (action == "PAUSE") return "Pause";
    if (action == "QUIT") return "Quit";
    
    // Default: capitalize first letter and replace underscores
    std::string result = action;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    if (!result.empty()) {
        result[0] = std::toupper(result[0]);
    }
    std::replace(result.begin(), result.end(), '_', ' ');
    
    return result;
}
