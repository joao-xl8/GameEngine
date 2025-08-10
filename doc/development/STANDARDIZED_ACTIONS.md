# Standardized Action System

## Overview

This document describes the standardized action system implemented to replace hardcoded string literals with centralized constants for action names across all scenes.

## Problem Solved

Previously, action names were hardcoded as string literals throughout the codebase:

```cpp
// Old approach - prone to typos and inconsistencies
registerAction(sf::Keyboard::Space, "CONFIRM");
registerAction(sf::Keyboard::C, "BACK");

if (action.getName() == "CONFIRM") {
    // Handle confirm action
}
```

This approach had several issues:
- **Inconsistency**: Different scenes might use different names for the same action
- **Typos**: String literals are prone to typos that won't be caught at compile time
- **Maintenance**: Changing an action name required updating multiple files
- **No IDE support**: No autocomplete or refactoring support for string literals

## Solution: ActionTypes Namespace

The new system uses a centralized `ActionTypes` namespace with `constexpr` constants:

```cpp
// New approach - consistent and type-safe
#include "../action_types.hpp"

registerAction(sf::Keyboard::Space, ActionTypes::CONFIRM);
registerAction(sf::Keyboard::C, ActionTypes::BACK);

if (action.getName() == ActionTypes::CONFIRM) {
    // Handle confirm action
}
```

## Available Action Types

### Navigation Actions
- `ActionTypes::UP` - Move up/previous item
- `ActionTypes::DOWN` - Move down/next item  
- `ActionTypes::LEFT` - Move left
- `ActionTypes::RIGHT` - Move right

### Confirmation Actions
- `ActionTypes::CONFIRM` - Primary confirmation action
- `ActionTypes::SELECT` - Select an item
- `ActionTypes::ACCEPT` - Accept a choice
- `ActionTypes::YES` - Affirmative response

### Cancellation/Back Actions
- `ActionTypes::BACK` - Go back/cancel
- `ActionTypes::CANCEL` - Cancel current action
- `ActionTypes::NO` - Negative response
- `ActionTypes::ESCAPE` - Escape/exit

### System Actions
- `ActionTypes::QUIT` - Quit the application
- `ActionTypes::PAUSE` - Pause the game
- `ActionTypes::MENU` - Open menu

### Game-Specific Actions
- `ActionTypes::INTERACT` - Interact with objects/NPCs
- `ActionTypes::ATTACK` - Attack action
- `ActionTypes::DEFEND` - Defend action
- `ActionTypes::USE_ITEM` - Use an item
- `ActionTypes::CAST_SPELL` - Cast a spell

### UI-Specific Actions
- `ActionTypes::SHOW_LOG` - Show dialogue/combat log
- `ActionTypes::TOGGLE_INVENTORY` - Toggle inventory
- `ActionTypes::SAVE` - Save game
- `ActionTypes::LOAD` - Load game

## Benefits

1. **Compile-time Safety**: Typos in action names will cause compilation errors
2. **IDE Support**: Full autocomplete and refactoring support
3. **Consistency**: All scenes use the same action names
4. **Maintainability**: Change an action name in one place
5. **Documentation**: Clear, centralized list of all available actions

## Usage Guidelines

### For Scene Developers

1. **Always include the header**:
   ```cpp
   #include "../action_types.hpp"
   ```

2. **Use constants for registration**:
   ```cpp
   registerAction(sf::Keyboard::Space, ActionTypes::CONFIRM);
   ```

3. **Use constants for comparisons**:
   ```cpp
   if (action.getName() == ActionTypes::CONFIRM) {
       // Handle action
   }
   ```

### Adding New Actions

1. Add the constant to `ActionTypes` namespace in `action_types.hpp`
2. If using the enum approach, add to `StandardAction` enum and update `actionToString()`
3. Update this documentation
4. Consider if the action should be standardized across scenes

## Migration Guide

To migrate existing scenes:

1. **Add the include**:
   ```cpp
   #include "../action_types.hpp"
   ```

2. **Replace string literals in registerAction calls**:
   ```cpp
   // Before
   registerAction(sf::Keyboard::Space, "CONFIRM");
   
   // After  
   registerAction(sf::Keyboard::Space, ActionTypes::CONFIRM);
   ```

3. **Replace string literals in action comparisons**:
   ```cpp
   // Before
   if (action.getName() == "CONFIRM") {
   
   // After
   if (action.getName() == ActionTypes::CONFIRM) {
   ```

4. **Test compilation and functionality**

## Alternative Enum Approach

The system also provides an enum-based approach for even stronger type safety:

```cpp
enum class StandardAction {
    CONFIRM, BACK, UP, DOWN, // etc.
};

// Convert enum to string
std::string actionStr = actionToString(StandardAction::CONFIRM);
registerAction(sf::Keyboard::Space, actionStr);
```

This approach provides:
- Stronger type safety
- Better IDE support
- Impossible to use invalid action names

## Migration Script

A Python script `migrate_actions.py` is provided to analyze which files need updating:

```bash
python3 migrate_actions.py
```

This script will:
- Identify all files that need changes
- Show exactly what needs to be updated
- Provide line numbers for each change

## Best Practices

1. **Use semantic names**: Choose action names that describe the intent, not the key
2. **Group related actions**: Use consistent naming patterns (e.g., all confirmation actions)
3. **Document custom actions**: If adding scene-specific actions, document their purpose
4. **Test thoroughly**: Ensure all action handling still works after migration

## Future Enhancements

Potential improvements to consider:

1. **Action Groups**: Group related actions for easier management
2. **Configurable Bindings**: Allow users to customize key bindings
3. **Action Validation**: Runtime validation of registered actions
4. **Action Documentation**: Auto-generate action documentation from code

## Files Modified

The following files were updated as part of this standardization:

- `src/action_types.hpp` - New header with action constants
- `src/scenes/scene_menu.cpp` - Updated to use ActionTypes
- `src/scenes/scene_options.cpp` - Updated to use ActionTypes
- `migrate_actions.py` - Migration analysis script
- `doc/development/STANDARDIZED_ACTIONS.md` - This documentation

## Conclusion

The standardized action system provides a robust foundation for consistent action handling across the entire game engine. It improves code quality, reduces bugs, and makes the codebase more maintainable.

All new scenes should use this system from the start, and existing scenes should be migrated when convenient or when being modified for other reasons.
