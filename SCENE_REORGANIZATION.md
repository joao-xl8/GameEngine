# Scene Files Reorganization

## Overview
All scene-related files have been moved from `src/` to `src/scenes/` for better organization.

## Files Moved

### Scene Files Moved to `src/scenes/`:
- `scene.hpp` → `src/scenes/scene.hpp`
- `scene.cpp` → `src/scenes/scene.cpp`
- `scene_menu.hpp` → `src/scenes/scene_menu.hpp`
- `scene_menu.cpp` → `src/scenes/scene_menu.cpp`
- `scene_play.hpp` → `src/scenes/scene_play.hpp`
- `scene_play.cpp` → `src/scenes/scene_play.cpp`
- `scene_options.hpp` → `src/scenes/scene_options.hpp`
- `scene_options.cpp` → `src/scenes/scene_options.cpp`
- `scene_battle.hpp` → `src/scenes/scene_battle.hpp`
- `scene_battle.cpp` → `src/scenes/scene_battle.cpp`

## Import Changes Made

### 1. Files that import scene files:

#### `src/game_engine.cpp`:
```cpp
// Before
#include "scene_menu.hpp"

// After
#include "scenes/scene_menu.hpp"
```

### 2. Scene files importing other files:

#### `src/scenes/scene.hpp`:
```cpp
// Before
#include "action.hpp"
#include "entity_manager.hpp"

// After
#include "../action.hpp"
#include "../entity_manager.hpp"
```

#### `src/scenes/scene.cpp`:
```cpp
// Before
#include "game_engine.hpp"

// After
#include "../game_engine.hpp"
```

#### `src/scenes/scene_play.hpp`:
```cpp
// Before
#include "components/engine_components.hpp"

// After
#include "../components/engine_components.hpp"
```

#### `src/scenes/scene_play.cpp`:
```cpp
// Before
#include "components/common_components.hpp"
#include "game_engine.hpp"

// After
#include "../components/common_components.hpp"
#include "../game_engine.hpp"
```

#### `src/scenes/scene_menu.cpp`:
```cpp
// Before
#include "game_engine.hpp"

// After
#include "../game_engine.hpp"
```

#### `src/scenes/scene_options.cpp`:
```cpp
// Before
#include "game_engine.hpp"

// After
#include "../game_engine.hpp"
```

#### `src/scenes/scene_battle.hpp`:
```cpp
// Before
#include "components.hpp"

// After
#include "../components.hpp"
```

#### `src/scenes/scene_battle.cpp`:
```cpp
// Before
#include "game_engine.hpp"

// After
#include "../game_engine.hpp"
```

## Build System Changes

### Makefile Updates:

#### Source Files:
```makefile
# Before
SOURCES = main.cpp \
          $(wildcard $(SRC_DIR)/*.cpp) \
          $(wildcard $(IMGUI_DIR)/imgui*.cpp)

# After
SOURCES = main.cpp \
          $(wildcard $(SRC_DIR)/*.cpp) \
          $(wildcard $(SRC_DIR)/scenes/*.cpp) \
          $(wildcard $(IMGUI_DIR)/imgui*.cpp)
```

#### Build Directory Creation:
```makefile
# Before
@$(MKDIR) $(BUILD_DIR)$(PATH_SEP)$(SRC_DIR) 2>/dev/null || mkdir -p $(BUILD_DIR)/$(SRC_DIR)

# After
@$(MKDIR) $(BUILD_DIR)$(PATH_SEP)$(SRC_DIR) 2>/dev/null || mkdir -p $(BUILD_DIR)/$(SRC_DIR)
@$(MKDIR) $(BUILD_DIR)$(PATH_SEP)$(SRC_DIR)$(PATH_SEP)scenes 2>/dev/null || mkdir -p $(BUILD_DIR)/$(SRC_DIR)/scenes
```

## New Project Structure

```
src/
├── scenes/
│   ├── scene.hpp
│   ├── scene.cpp
│   ├── scene_menu.hpp
│   ├── scene_menu.cpp
│   ├── scene_play.hpp
│   ├── scene_play.cpp
│   ├── scene_options.hpp
│   ├── scene_options.cpp
│   ├── scene_battle.hpp
│   └── scene_battle.cpp
├── components/
│   ├── base_component.hpp
│   ├── common_components.hpp
│   ├── engine_components.hpp
│   └── game_components.hpp
├── game_engine.hpp
├── game_engine.cpp
├── assets.hpp
├── assets.cpp
├── entity.hpp
├── entity.cpp
├── entity_manager.hpp
├── entity_manager.cpp
├── action.hpp
├── animation.hpp
├── animation.cpp
├── vec2.hpp
├── vec2.cpp
└── components.hpp
```

## Benefits of This Organization

1. **Better Code Organization**: All scene-related files are now grouped together
2. **Clearer Project Structure**: Easier to navigate and understand the codebase
3. **Scalability**: Easy to add new scenes without cluttering the main src directory
4. **Maintainability**: Related files are grouped together, making maintenance easier
5. **Professional Structure**: Follows common C++ project organization patterns

## Build Status

✅ **All files successfully moved**  
✅ **All imports updated and working**  
✅ **Makefile updated to include scenes directory**  
✅ **Project builds successfully**  
✅ **No breaking changes to existing functionality**

## Usage

The reorganization is transparent to users of the GameEngine. All existing functionality remains the same, but the codebase is now better organized for development and maintenance.

To add new scenes, simply:
1. Create the new scene files in `src/scenes/`
2. Use relative imports: `#include "../game_engine.hpp"` for parent directory files
3. The Makefile will automatically include them in the build process
