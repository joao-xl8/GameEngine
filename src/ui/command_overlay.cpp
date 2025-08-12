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
    // Setup background - make it solid (opacity 255 instead of 180)
    m_background.setFillColor(sf::Color(0, 0, 0, 255));
    m_background.setOutlineColor(sf::Color(100, 100, 100));
    m_background.setOutlineThickness(1.0f);
    
    // Setup text - check if font is available
    try {
        m_text.setFont(m_game->getAssets().getFont("ShareTech"));
        m_text.setCharacterSize(18);  // Increased from 14 to 18 (+4 more)
        m_text.setFillColor(sf::Color::White);
    } catch (const std::exception& e) {
        std::cout << "Warning: Could not set font for command overlay: " << e.what() << std::endl;
        // Continue without font - SFML will use default
    }
}

void CommandOverlay::update(const std::map<std::string, std::vector<int>>& registeredActions)
{
    m_commands.clear();
    
    for (const auto& pair : registeredActions) {
        const std::string& action = pair.first;
        
        if (action == "QUIT") {
            m_commands.push_back({"ESC", "Exit Game"});
        } else if (action == "PAUSE") {
            m_commands.push_back({"ESC", "Pause Menu"});
        } else if (action == "INTERACT") {
            m_commands.push_back({"E", "Interact"});
        } else if (shouldShowAction(action)) {
            // Show other specific actions
            std::string keyName = getKeyName(pair.second[0]);
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
    
    // Get current view for proper positioning
    sf::View currentView = m_game->window().getView();
    sf::Vector2f viewSize = currentView.getSize();
    sf::Vector2f viewCenter = currentView.getCenter();
    
    // Calculate view bounds (bottom-right corner)
    float viewRight = viewCenter.x + viewSize.x / 2;
    float viewBottom = viewCenter.y + viewSize.y / 2;
    
    // Calculate maximum width (half the window width, with margins)
    float margin = 8.0f;
    float maxWidth = (viewSize.x / 2.0f) - (margin * 2);
    
    // First, try to fit everything in a single line
    std::ostringstream singleLineTest;
    for (size_t i = 0; i < m_commands.size(); i++) {
        if (i > 0) singleLineTest << "  |  ";
        singleLineTest << m_commands[i].first << ": " << m_commands[i].second;
    }
    
    // Test if single line fits within max width
    sf::Text testText;
    if (m_text.getFont()) {
        testText.setFont(*m_text.getFont());
    }
    testText.setCharacterSize(m_text.getCharacterSize());
    testText.setString(singleLineTest.str());
    float singleLineWidth = testText.getLocalBounds().width;
    
    // Build command strings - use single line if it fits, otherwise wrap
    std::vector<std::string> lines;
    
    if (singleLineWidth <= maxWidth) {
        // Single line fits within half window width - use it
        lines.push_back(singleLineTest.str());
    } else {
        // Need to wrap to multiple lines
        std::ostringstream currentLine;
        
        for (size_t i = 0; i < m_commands.size(); i++) {
            std::string commandStr = m_commands[i].first + ": " + m_commands[i].second;
            std::string separator = (i > 0) ? "  |  " : "";
            
            // Test if adding this command would exceed max width
            std::string testLine = currentLine.str() + separator + commandStr;
            testText.setString(testLine);
            float testWidth = testText.getLocalBounds().width;
            
            if (testWidth > maxWidth && !currentLine.str().empty()) {
                // Current line would be too wide, start a new line
                lines.push_back(currentLine.str());
                currentLine.str("");
                currentLine.clear();
                currentLine << commandStr;
            } else {
                // Add to current line
                currentLine << separator << commandStr;
            }
        }
        
        // Add the last line if it has content
        if (!currentLine.str().empty()) {
            lines.push_back(currentLine.str());
        }
    }
    
    // Calculate total dimensions
    float padding = 6.0f;
    float lineHeight = m_text.getCharacterSize() + 2.0f; // Small spacing between lines
    float totalHeight = lines.size() * lineHeight + padding * 2;
    
    // Find the widest line for background width
    float maxLineWidth = 0.0f;
    for (const std::string& line : lines) {
        sf::Text tempText;
        if (m_text.getFont()) {
            tempText.setFont(*m_text.getFont());
        }
        tempText.setCharacterSize(m_text.getCharacterSize());
        tempText.setString(line);
        float lineWidth = tempText.getLocalBounds().width;
        maxLineWidth = std::max(maxLineWidth, lineWidth);
    }
    
    float bgWidth = maxLineWidth + padding * 2;
    
    // Position in bottom-right corner of current view
    float posX = viewRight - bgWidth - margin;
    float posY = viewBottom - totalHeight - margin;
    
    // Set background
    m_background.setSize(sf::Vector2f(bgWidth, totalHeight));
    m_background.setPosition(posX, posY);
    
    // Draw background
    try {
        m_game->window().draw(m_background);
        
        // Draw each line of text
        for (size_t i = 0; i < lines.size(); i++) {
            m_text.setString(lines[i]);
            float textY = posY + padding + (i * lineHeight);
            m_text.setPosition(posX + padding, textY);
            m_game->window().draw(m_text);
        }
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
        case sf::Keyboard::Tab: return "TAB";
        case sf::Keyboard::Up: return "UP";
        case sf::Keyboard::Down: return "DOWN";
        case sf::Keyboard::Left: return "LEFT";
        case sf::Keyboard::Right: return "RIGHT";
        case sf::Keyboard::W: return "W";
        case sf::Keyboard::A: return "A";
        case sf::Keyboard::S: return "S";
        case sf::Keyboard::D: return "D";
        case sf::Keyboard::F: return "F";
        case sf::Keyboard::L: return "L";
        case sf::Keyboard::Q: return "Q";
        case sf::Keyboard::E: return "E";
        case sf::Keyboard::Z: return "Z";
        case sf::Keyboard::V: return "V";
        case sf::Keyboard::C: return "C";
        case sf::Keyboard::X: return "X";
        case sf::Keyboard::T: return "T";
        case sf::Keyboard::R: return "R";
        case sf::Keyboard::N: return "N";
        case sf::Keyboard::I: return "I";
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
    if (
        action == "UP" || 
        action == "DOWN" || 
        action == "LEFT" || 
        action == "RIGHT" ||
        action == "CHOICE_UP" ||
        action == "CHOICE_DOWN" ||
        action == "CONFIRM" || 
        action == "PLACE" || 
        action == "SELECT" ||
        action == "CANCEL" || 
        action == "REMOVE" || 
        action == "BACK" ||
        action.substr(0, 7) == "NUMBER_"
    ) {
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
    if (action == "PAUSE") return "Pause Menu";
    if (action == "QUIT") return "Exit";
    if (action == "SHOW_LOG") return "Show Log";
    if (action == "TOGGLE_TEXTURE") return "Toggle Textures";
    if (action == "TOGGLE_COLLISION") return "Toggle Collision";
    if (action == "TOGGLE_GRID") return "Toggle Grid";
    
    // Default: capitalize first letter and replace underscores
    std::string result = action;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    if (!result.empty()) {
        result[0] = std::toupper(result[0]);
    }
    std::replace(result.begin(), result.end(), '_', ' ');
    
    return result;
}
