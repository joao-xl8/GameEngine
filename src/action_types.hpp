#pragma once
#include <string>

// Standard action types used across all scenes
namespace ActionTypes {
    // Navigation actions
    constexpr const char* UP = "UP";
    constexpr const char* DOWN = "DOWN";
    constexpr const char* LEFT = "LEFT";
    constexpr const char* RIGHT = "RIGHT";
    
    // Confirmation actions
    constexpr const char* CONFIRM = "CONFIRM";
    constexpr const char* SELECT = "SELECT";
    constexpr const char* ACCEPT = "ACCEPT";
    constexpr const char* YES = "YES";
    
    // Cancellation/Back actions
    constexpr const char* BACK = "BACK";
    constexpr const char* CANCEL = "CANCEL";
    constexpr const char* NO = "NO";
    constexpr const char* ESCAPE = "ESCAPE";
    
    // System actions
    constexpr const char* QUIT = "QUIT";
    constexpr const char* PAUSE = "PAUSE";
    constexpr const char* MENU = "MENU";
    
    // Game-specific actions
    constexpr const char* INTERACT = "INTERACT";
    constexpr const char* ATTACK = "ATTACK";
    constexpr const char* DEFEND = "DEFEND";
    constexpr const char* USE_ITEM = "USE_ITEM";
    constexpr const char* CAST_SPELL = "CAST_SPELL";
    
    // UI-specific actions
    constexpr const char* SHOW_LOG = "SHOW_LOG";
    constexpr const char* TOGGLE_INVENTORY = "TOGGLE_INVENTORY";
    constexpr const char* SAVE = "SAVE";
    constexpr const char* LOAD = "LOAD";
}

// Alternative enum-based approach (if you prefer type safety)
enum class StandardAction {
    // Navigation
    UP,
    DOWN,
    LEFT,
    RIGHT,
    
    // Confirmation
    CONFIRM,
    SELECT,
    ACCEPT,
    YES,
    
    // Cancellation
    BACK,
    CANCEL,
    NO,
    ESCAPE,
    
    // System
    QUIT,
    PAUSE,
    MENU,
    
    // Game-specific
    INTERACT,
    ATTACK,
    DEFEND,
    USE_ITEM,
    CAST_SPELL,
    
    // UI-specific
    SHOW_LOG,
    TOGGLE_INVENTORY,
    SAVE,
    LOAD
};

// Utility function to convert enum to string
inline std::string actionToString(StandardAction action) {
    switch (action) {
        case StandardAction::UP: return ActionTypes::UP;
        case StandardAction::DOWN: return ActionTypes::DOWN;
        case StandardAction::LEFT: return ActionTypes::LEFT;
        case StandardAction::RIGHT: return ActionTypes::RIGHT;
        case StandardAction::CONFIRM: return ActionTypes::CONFIRM;
        case StandardAction::SELECT: return ActionTypes::SELECT;
        case StandardAction::ACCEPT: return ActionTypes::ACCEPT;
        case StandardAction::YES: return ActionTypes::YES;
        case StandardAction::BACK: return ActionTypes::BACK;
        case StandardAction::CANCEL: return ActionTypes::CANCEL;
        case StandardAction::NO: return ActionTypes::NO;
        case StandardAction::ESCAPE: return ActionTypes::ESCAPE;
        case StandardAction::QUIT: return ActionTypes::QUIT;
        case StandardAction::PAUSE: return ActionTypes::PAUSE;
        case StandardAction::MENU: return ActionTypes::MENU;
        case StandardAction::INTERACT: return ActionTypes::INTERACT;
        case StandardAction::ATTACK: return ActionTypes::ATTACK;
        case StandardAction::DEFEND: return ActionTypes::DEFEND;
        case StandardAction::USE_ITEM: return ActionTypes::USE_ITEM;
        case StandardAction::CAST_SPELL: return ActionTypes::CAST_SPELL;
        case StandardAction::SHOW_LOG: return ActionTypes::SHOW_LOG;
        case StandardAction::TOGGLE_INVENTORY: return ActionTypes::TOGGLE_INVENTORY;
        case StandardAction::SAVE: return ActionTypes::SAVE;
        case StandardAction::LOAD: return ActionTypes::LOAD;
        default: return "UNKNOWN";
    }
}
