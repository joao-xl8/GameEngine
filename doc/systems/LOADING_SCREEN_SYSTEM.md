# Loading Screen System

A comprehensive asset preloading system that ensures smooth scene transitions by loading all required assets before the scene starts.

## Overview

The Loading Screen System provides:
- **Asset Preloading**: Loads textures and sounds before scene initialization
- **Visual Progress**: Shows loading progress with progress bar and percentage
- **Smooth Transitions**: Eliminates loading delays during gameplay
- **Configurable Assets**: Scene-specific asset configuration via file
- **Error Handling**: Graceful handling of missing assets

## Architecture

### Core Components

**Scene_Loading**
- Main loading screen scene
- Handles asset preloading and progress display
- Manages transition to target scene

**Asset Configuration**
- `metadata/scene_assets.txt` - Defines scene-specific assets
- Automatic asset discovery from configuration
- Fallback to manual asset lists

**Visual Elements**
- Progress bar with percentage display
- Loading animation (rotating dots)
- Dark themed background
- Centered layout

## Usage

### Basic Scene Transition
```cpp
// Instead of direct scene change:
// m_game->changeScene("Play", std::make_shared<Scene_Play>(game, levelPath));

// Use loading screen:
Scene_Loading::loadPlayScene(m_game, "metadata/levels/level1.txt");
```

### Custom Asset Loading
```cpp
std::vector<std::string> textures = {"Ground", "Wall", "Player"};
std::vector<std::string> sounds = {"walk", "jump"};

auto sceneFactory = [game]() {
    return std::make_shared<Scene_CustomGame>(game);
};

auto loadingScene = std::make_shared<Scene_Loading>(
    game, "CustomGame", sceneFactory, textures, sounds
);
game->changeScene("Loading", loadingScene);
```

## Asset Configuration File

### Format
```
# metadata/scene_assets.txt
# Format: SceneName AssetType AssetName

Play texture Ground
Play texture Wall
Play sound walk
MapEditor texture Bush
```

### Asset Types
- **texture** - Image assets loaded via `getTexture()`
- **sound** - Audio assets loaded via `getSoundBuffer()`

### Benefits
- **Centralized Configuration** - All scene assets in one file
- **Easy Maintenance** - Add/remove assets without code changes
- **Automatic Discovery** - Loading screen reads configuration automatically
- **No Duplication** - Prevents loading same asset multiple times

## Implementation Details

### Loading Process
1. **Initialize Loading Screen** - Set up visual elements
2. **Read Configuration** - Load scene-specific assets from config file
3. **Asset Loading Loop** - Load one asset per frame to avoid blocking
4. **Progress Updates** - Update progress bar and percentage
5. **Scene Creation** - Create target scene when loading complete
6. **Transition** - Switch to target scene after minimum display time

### Visual Features
- **Progress Bar** - 400px wide green progress bar
- **Percentage Display** - Shows completion percentage
- **Asset Counter** - "Loading assets... X/Y"
- **Loading Animation** - Rotating dots for visual feedback
- **Dark Theme** - Professional dark blue background

### Performance Optimization
- **One Asset Per Frame** - Prevents UI blocking
- **Minimum Display Time** - Ensures loading screen is visible (1 second)
- **Memory Efficient** - Assets cached in main Assets manager
- **Error Resilient** - Continues loading even if individual assets fail

## Code Examples

### Scene Transition with Loading
```cpp
// In Scene_Menu button handler
if (selectedOption == "Play") {
    Scene_Loading::loadPlayScene(m_game, "metadata/levels/level1.txt");
}
```

### Custom Loading Screen
```cpp
// Define assets to preload
std::vector<std::string> battleAssets = {
    "BattleBackground", "PlayerBattle", "EnemySprite"
};
std::vector<std::string> battleSounds = {
    "battleMusic", "swordClash", "victory"
};

// Create scene factory
auto battleSceneFactory = [game, playerData]() {
    return std::make_shared<Scene_Battle>(game, playerData);
};

// Create and show loading screen
auto loadingScene = std::make_shared<Scene_Loading>(
    game, "Battle", battleSceneFactory, battleAssets, battleSounds
);
game->changeScene("Loading", loadingScene);
```

### Asset Configuration Management
```cpp
// Add new scene assets to metadata/scene_assets.txt
Battle texture BattleBackground
Battle texture PlayerBattle
Battle texture EnemySprite
Battle sound battleMusic
Battle sound swordClash
Battle sound victory
```

## Integration Points

### Scene Transitions
All major scene transitions now use loading screens:
- **Menu → Play**: `Scene_Loading::loadPlayScene()`
- **Menu → Map Editor**: `Scene_Loading::loadMapEditorScene()`
- **Play → Menu**: `Scene_Loading::loadMenuScene()`
- **Map Editor → Menu**: `Scene_Loading::loadMenuScene()`

### Asset Management
- **Texture Loading**: Uses `Assets::getTexture()` to cache textures
- **Sound Loading**: Uses `Assets::getSoundBuffer()` to cache sounds
- **Error Handling**: Logs failed loads but continues operation
- **Memory Management**: Assets remain cached for scene lifetime

## User Experience

### Loading Flow
1. **User Action** - Player selects menu option or presses key
2. **Loading Screen** - Smooth transition to loading screen
3. **Progress Display** - Visual feedback with progress bar
4. **Asset Loading** - Background loading of required assets
5. **Scene Ready** - Automatic transition when complete
6. **Instant Gameplay** - No loading delays in target scene

### Visual Feedback
- **Immediate Response** - Loading screen appears instantly
- **Progress Indication** - Clear progress bar and percentage
- **Loading Animation** - Rotating dots show system is active
- **Professional Look** - Clean, dark themed interface

## Technical Benefits

### Performance
- **Eliminates Stuttering** - No mid-gameplay asset loading
- **Smooth Transitions** - All assets ready before scene starts
- **Memory Efficiency** - Assets loaded once and cached
- **Non-Blocking** - One asset per frame prevents UI freezing

### Maintainability
- **Centralized Configuration** - Easy to modify scene assets
- **Modular Design** - Easy to add new scenes
- **Error Resilience** - Graceful handling of missing assets
- **Debug Information** - Comprehensive logging of loading process

### Scalability
- **Configurable Assets** - Easy to add new asset types
- **Scene-Specific Loading** - Each scene loads only what it needs
- **Extensible Design** - Easy to add new loading features
- **Future-Proof** - Ready for more complex asset types

## Future Enhancements

### Planned Features
- **Async Loading** - Background thread asset loading
- **Asset Streaming** - Load assets as needed during gameplay
- **Loading Priorities** - Critical assets loaded first
- **Memory Management** - Automatic asset unloading
- **Loading Hints** - Tips and information during loading
- **Custom Animations** - Scene-specific loading animations

### Advanced Configuration
- **Asset Dependencies** - Define asset loading order
- **Conditional Loading** - Load assets based on game state
- **Asset Variants** - Different assets for different quality settings
- **Preload Strategies** - Different loading strategies per scene

The Loading Screen System provides a professional, smooth user experience while ensuring optimal performance through intelligent asset preloading.
