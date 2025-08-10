# Scene Renaming Changes

## Overview
Renamed scene classes and files to be more specific about their functionality, preparing for future addition of different map types.

## Files Renamed

### Map Editor → Grid Map Editor
- **Files**:
  - `src/scenes/scene_map_editor.hpp` → `src/scenes/scene_grid_map_editor.hpp`
  - `src/scenes/scene_map_editor.cpp` → `src/scenes/scene_grid_map_editor.cpp`
- **Class**: `Scene_MapEditor` → `Scene_GridMapEditor`

### Play Scene → Play Grid Scene  
- **Files**:
  - `src/scenes/scene_play.hpp` → `src/scenes/scene_play_grid.hpp`
  - `src/scenes/scene_play.cpp` → `src/scenes/scene_play_grid.cpp`
- **Class**: `Scene_Play` → `Scene_PlayGrid`

## Updated References

### Files Modified for Map Editor Renaming:
- `src/scenes/scene_loading.cpp` - Updated include and class instantiation
- `src/scenes/scene_menu.cpp` - Updated include and references

### Files Modified for Play Scene Renaming:
- `src/scenes/scene_dialogue.cpp` - Updated include and references
- `src/scenes/scene_level_selector.cpp` - Updated include and references  
- `src/scenes/scene_save_load.cpp` - Updated include and references
- `src/scenes/scene_loading.cpp` - Updated include and references
- `src/scenes/scene_menu.cpp` - Updated include and references
- `src/scenes/scene_battle.cpp` - Updated include and references
- `src/systems/save_system.hpp` - Updated class references

## Changes Made

### Include Statements
```cpp
// Before
#include "scene_map_editor.hpp"
#include "scene_play.hpp"

// After  
#include "scene_grid_map_editor.hpp"
#include "scene_play_grid.hpp"
```

### Class Declarations
```cpp
// Before
class Scene_MapEditor : public Scene
class Scene_Play : public Scene

// After
class Scene_GridMapEditor : public Scene  
class Scene_PlayGrid : public Scene
```

### Class Instantiations
```cpp
// Before
std::make_shared<Scene_MapEditor>(game)
std::make_shared<Scene_Play>(game, levelPath)

// After
std::make_shared<Scene_GridMapEditor>(game)
std::make_shared<Scene_PlayGrid>(game, levelPath)
```

## Rationale

### Grid Map Editor
- Renamed to `Scene_GridMapEditor` to distinguish from future map editor types
- Prepares for potential addition of:
  - Hex map editors
  - Isometric map editors  
  - Free-form map editors
  - 3D map editors

### Play Grid Scene
- Renamed to `Scene_PlayGrid` to indicate it handles grid-based gameplay
- Prepares for potential addition of:
  - Different gameplay modes
  - Non-grid-based play scenes
  - Specialized play scene variants

## Build System
- Makefile uses wildcards, so no changes needed
- Build system automatically detects renamed files
- All dependencies properly updated

## Testing
- ✅ Build successful after renaming
- ✅ All includes properly updated
- ✅ All class references updated
- ✅ No broken dependencies

## Future Extensibility
This renaming provides a clear foundation for adding new scene types:
- `Scene_HexMapEditor` for hexagonal grid editing
- `Scene_PlayHex` for hexagonal grid gameplay  
- `Scene_IsometricMapEditor` for isometric map editing
- `Scene_PlayIsometric` for isometric gameplay

The naming convention now clearly indicates the specific type of scene functionality.
