# GameEngine - Cross-Platform 2D Game Engine

A modern C++20 game engine built with Entity-Component-System (ECS) architecture, featuring SFML graphics, ImGui integration, and a powerful built-in map editor.

**Credits**: COMP4300 - [Dave Churchill](https://www.youtube.com/@DaveChurchill/videos)

---

## 🚀 Quick Start

```bash
# Build and run (recommended)
make run

# Just build
make

# Debug build
make debug

# Clean build files
make clean

# Show all options
make help
```

---

## 📁 Project Structure

```
GameEngine/
├── src/                    # C++ source files
│   ├── game_engine.cpp     # Main engine
│   ├── entity.cpp          # Entity system
│   ├── scene_*.cpp         # Game scenes
│   └── ...
├── include/imgui/          # ImGui library
├── assets/                 # Game assets (textures, sounds)
├── metadata/               # Game configuration files
│   ├── assets.txt          # Asset definitions
│   └── levels/             # Level files directory
│       ├── level1.txt      # Main game level
│       └── *.txt           # Other level files
├── doc/                    # Documentation (organized)
│   ├── build/              # Build and setup guides
│   │   ├── BUILD.md        # Comprehensive build guide
│   │   └── WINDOWS_SETUP_GUIDE.md
│   ├── systems/            # Engine systems documentation
│   │   ├── CAMERA_SYSTEM.md
│   │   ├── SOUND_SYSTEM.md
│   │   ├── VIEWPORT_SYSTEM.md
│   │   ├── CENTERED_VIEWPORT_SYSTEM.md
│   │   └── CONFIGURABLE_SCALE_SYSTEM.md
│   ├── features/           # Game features documentation
│   │   ├── BATTLE_SYSTEM.md
│   │   ├── COLLISION_GRID_MOVEMENT.md
│   │   └── GRID_MOVEMENT_COOLDOWN.md
│   ├── development/        # Development guides
│   │   ├── RPG_DEVELOPMENT_GUIDE.md
│   │   ├── RPG_DEVELOPMENT_ROADMAP.md
│   │   ├── RPG_SCENES_IMPLEMENTATION.md
│   │   ├── STEAM_PUBLISHING_GUIDE.md
│   │   ├── SCENE_REORGANIZATION.md
│   │   └── COMPONENT_ORGANIZATION.md
│   ├── examples/           # Code examples and tutorials
│   │   ├── ANIMATION_EXAMPLES.md
│   │   ├── GRID_MOVEMENT_EXAMPLES.md
│   │   ├── LEVEL_DESIGN_GUIDE.md
│   │   └── dialogue_example.md
│   └── DOCUMENTATION_CONSOLIDATION.md
├── build/                  # Build outputs
│   ├── release/GameEngine  # Release executable
│   └── debug/GameEngine    # Debug executable
├── cmake_build/            # CMake build directory
├── Makefile               # Cross-platform Makefile
├── CMakeLists.txt         # CMake configuration
├── build.py               # Python build helper
├── build_windows.bat      # Windows build script
├── build_linux.sh         # Linux build script
├── validate_level.py      # Level validation script
└── README.md              # This file (main documentation)
```

---

## 🎮 Game Features

### Core Gameplay
- **2D Platformer**: Classic side-scrolling gameplay
- **Grid-Based Movement**: Precise tile-based character movement
- **Physics System**: Gravity, collision detection, and response
- **Sound Integration**: Background music and sound effects
- **Multiple Scenes**: Menu, gameplay, options, and map editor

### Technical Features
- **ECS Architecture**: Entity-Component-System design pattern
- **Cross-Platform**: Windows, macOS, and Linux support
- **Modern C++20**: Latest C++ features and best practices
- **SFML Graphics**: Hardware-accelerated 2D rendering
- **ImGui Integration**: Immediate mode GUI for debugging
- **Asset Management**: Centralized texture and sound loading
- **Global Sound System**: Persistent background music with scene-specific effects

---

## 🛠️ Map Editor - Professional Level Creation Tool

The built-in Map Editor is a powerful level creation tool with modern features and an intuitive interface.

### ✨ Key Features
- **🌐 Infinite Grid**: No size limitations - place objects anywhere!
- **📹 Camera Following**: Camera automatically follows cursor for seamless navigation
- **🖼️ Asset Preview**: Live preview of selected asset with yellow border
- **📂 Level Browser**: GUI-based level selection with file scanning
- **⚡ Performance Optimized**: Only renders visible objects for smooth editing
- **💾 Auto-Save**: Timestamped level files with organized directory structure

### 🎯 Recent Improvements
- **📏 Fixed UI Panel Height**: Commands panel properly displays all controls (320x280px)
- **👁️ Always Visible Cursor**: Yellow cursor now always appears on top of assets
- **📂 Level Selection GUI**: Interactive file browser for loading levels
- **📁 Organized Directory**: Levels separated from config files in `metadata/levels/`

### 🎮 How to Use

#### Accessing the Map Editor
1. Run the game: `make run`
2. From the main menu, select "Map Editor"
3. The editor opens with infinite grid and camera at origin (0,0)

#### Controls

**Navigation**
- **WASD** or **Arrow Keys**: Move cursor (camera follows automatically)
- **Mouse**: Click anywhere to jump cursor to that position

**Object Placement**
- **Space** or **Left Click**: Place the current object at cursor/mouse position
- **X** or **Right Click**: Remove object at cursor/mouse position

**Asset Selection**
- **Q/E**: Cycle through available assets (Ground, Wall, Bush, Player)
- **Z/C**: Cycle through object types (Tile, Dec)

**File Operations**
- **F**: Save the current level (auto-generates filename with timestamp)
- **L**: **NEW!** Open level selection GUI to choose which level to load
- **ESC**: Return to main menu (or close level selector if open)

#### Level Selection GUI
1. Press **L** in the map editor
2. A centered dialog appears showing all available .txt files in `metadata/levels/`
3. Use **UP/DOWN** arrows to navigate through the list
4. Press **SPACE** or **ENTER** to load the selected level
5. Press **ESC** to cancel and return to editing

**Features:**
- **Automatic Scanning**: Finds all .txt files in metadata/levels/ directory
- **Alphabetical Sorting**: Levels are sorted for easy browsing
- **Visual Selection**: Current selection highlighted with > brackets <
- **Error Handling**: Gracefully handles missing directories or files

### 🖥️ User Interface

#### Main UI Panel (Top-Left) - Enhanced!
- **Larger Size**: Now 320x280 pixels to fit all content
- **Optimized Font**: 12pt font for better text density
- **Complete Information**: Current object type and asset, cursor coordinates, object count
- **Directory Info**: Shows "Save to: metadata/levels/"
- **Full Control Reference**: All controls clearly visible

#### Asset Preview (Top-Right)
- **Live Preview**: Real-time display of selected asset
- **Yellow Border**: Clear highlighting for current selection
- **Asset Info**: Type and name display below preview
- **Proper Scaling**: Scales to 64x64 pixel preview size

#### Cursor Visibility - Fixed!
- **Always On Top**: Yellow cursor now renders above all assets
- **Never Hidden**: Cursor remains visible regardless of placed objects
- **Clear Positioning**: Exact grid position always visible

### 🎨 Object Types

#### Tile (Solid Objects)
- **Collision**: Solid objects with collision detection
- **Examples**: Wall, Ground
- **Usage**: Platforms, barriers, walkable surfaces

#### Dec (Decorations)
- **Non-Solid**: Decorative objects without collision
- **Examples**: Bush, Player spawn point
- **Usage**: Visual elements, spawn markers, background details

### 🌐 Infinite Grid System

#### Revolutionary Features
- **No Size Limits**: Place objects at any coordinate (positive or negative)
- **Memory Efficient**: Uses sparse storage - only occupied cells consume memory
- **Performance Optimized**: Only renders objects in visible area
- **Coordinate System**: Standard X,Y grid where (0,0) is the origin

#### Navigation Tips
- Camera smoothly follows cursor movement
- Grid lines extend infinitely in all directions
- Cursor coordinates display current position (can be negative!)
- Yellow cursor outline shows exact placement location

### 📂 File Management - Reorganized!

#### New Directory Structure
```
metadata/
├── assets.txt          # Asset configuration (protected)
└── levels/             # Level files (organized)
    ├── level1.txt          # Main game level
    ├── demo_level.txt      # Demo level
    ├── infinite_test.txt   # Test level
    └── level_editor_*.txt  # Generated levels
```

#### Benefits
- **Clean Separation**: Configuration files protected from accidental editing
- **Organized Levels**: All level files in dedicated directory
- **Cleaner Browser**: Level selector shows only level files
- **Future-Proof**: Ready for additional file types

#### File Format
Saved levels use the standard format with infinite coordinate support:
```
# Comments start with #
Type AssetName X Y

# Examples (coordinates can be negative):
Tile Wall -10 -5
Tile Ground 0 0
Dec Bush 100 50
Dec Player -5 10
```

### 🔄 Workflow Example

1. **Start Editing**: Open Map Editor from main menu
2. **Navigate**: Use WASD to move around the infinite grid
3. **Select Assets**: Use Q/E to choose Wall, Ground, Bush, or Player
4. **Choose Type**: Use Z/C to select Tile (solid) or Dec (decoration)
5. **Place Objects**: Use Space or left-click to place objects
6. **Preview**: Check the asset preview on the right to confirm selection
7. **Save**: Press F to save your level with timestamp to `metadata/levels/`
8. **Load Existing**: Press L to open level browser and load existing levels
9. **Test**: Rename saved file to `level1.txt` to play it in the main game

### ⚡ Performance Features

#### Optimized Rendering
- Only draws objects in visible screen area
- Efficient grid line rendering with automatic culling
- Smooth camera interpolation for comfortable movement
- Memory-efficient sparse grid storage

#### Smart Grid Management
- Automatic cleanup of empty cells
- Fast coordinate-based lookups using `std::map`
- Minimal memory footprint even for very large levels
- Visibility culling for optimal performance

---

## 🎯 Game Programming Concepts

### ECS - Entity-Component-System Architecture
Modern software design paradigm for game architecture:

- **Entity**: Any object in the game (player, platform, tile, bullet, enemy)
- **Components**: Properties attached to entities (position, texture, animation, health, gravity) - **PURE DATA**
- **Systems**: Code/logic that drives behavior (movement, rendering, sound, physics)

### Engine Architecture
```
Game
 > Scene
  > Systems
  > EntityManager
   > Entity
    > Component
```

### RAII (Resource Acquisition is Initialization)
Binds the life cycle of a resource to the lifetime of an object. We use `std::shared_ptr<T>` for automatic memory management and safe resource handling across different scopes.

### Component Design Philosophy
An ECS Component is just **DATA**:
- Maybe some logic in the constructor for data type conversion
- No helper functionality within components
- No game logic in components
- Components have intuitive meaning (Position, Gravity, Health, etc.)

### Data Management Principles
Separate logic from data:
- Create data structures that manage data so logic code doesn't worry about implementation
- Complex algorithms shouldn't need to know about underlying storage (set, vector, map)
- Entity Manager uses Factory Design Pattern for object creation and lifetime management

---

## 🎮 Level Validation System

A comprehensive validation system ensures level files are properly formatted and contain valid data.

### Usage
```bash
# Validate specific level
python3 validate_level.py metadata/levels/level1.txt

# Auto-detection (searches in levels directory)
python3 validate_level.py level1.txt

# Validate infinite grid level
python3 validate_level.py infinite_test.txt
```

### Features
- **Infinite Grid Support**: No coordinate limitations
- **Auto-Detection**: Automatically finds levels in `metadata/levels/`
- **Comprehensive Validation**: Checks format, types, assets, and coordinates
- **Performance Warnings**: Alerts for extremely large coordinates
- **Detailed Statistics**: Shows coordinate bounds and level dimensions

### Validation Rules
- **Valid Types**: `Tile`, `Dec`
- **Known Assets**: `Ground`, `Wall`, `Bush`, `Player`
- **Coordinate Format**: Integer X,Y coordinates (infinite range supported)
- **File Format**: `Type Asset X Y` per line, comments with `#`

---

## 🔧 Technical Implementation Details

### Infinite Grid System
```cpp
// Sparse storage for memory efficiency
std::map<std::pair<int, int>, GridCell> m_infiniteGrid;

// Only occupied cells consume memory
void setGridCell(int x, int y, const GridCell& cell) {
    auto key = std::make_pair(x, y);
    if (cell.occupied) {
        m_infiniteGrid[key] = cell;
    } else {
        m_infiniteGrid.erase(key); // Cleanup empty cells
    }
}
```

### Camera Following System
```cpp
void updateCamera() {
    // Center camera on cursor position
    Vec2 targetPos = gridToScreen(m_cursorPos);
    targetPos.x += TILE_SIZE / 2; // Center on tile
    targetPos.y += TILE_SIZE / 2;
    
    // Smooth camera movement
    Vec2 diff = Vec2(targetPos.x - m_cameraPos.x, targetPos.y - m_cameraPos.y);
    m_cameraPos.x += diff.x * 0.1f; // Smooth interpolation
    m_cameraPos.y += diff.y * 0.1f;
    
    m_gameView.setCenter(m_cameraPos.x, m_cameraPos.y);
}
```

### Level Selection System
```cpp
void scanAvailableLevels() {
    m_availableLevels.clear();
    std::string levelsDir = "metadata/levels/";
    
    for (const auto& entry : std::filesystem::directory_iterator(levelsDir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".txt") {
            m_availableLevels.push_back(entry.path().filename().string());
        }
    }
    std::sort(m_availableLevels.begin(), m_availableLevels.end());
}
```

### Cursor Visibility Fix
```cpp
void sRender() {
    // Draw world objects first
    m_game->window().setView(m_gameView);
    drawInfiniteGrid();
    drawPlacedObjects();
    
    // Draw cursor on top (always visible)
    m_cursor.setPosition(m_cursorPos.x * TILE_SIZE, m_cursorPos.y * TILE_SIZE);
    m_game->window().draw(m_cursor);
    
    // Draw UI last
    m_game->window().setView(m_uiView);
    drawUI();
    drawAssetPreview();
}
```

### Global Sound System
```cpp
// Global sound manager for persistent background music
class GameEngine {
    std::shared_ptr<CSound> m_globalSoundManager;
    
    void init() {
        m_globalSoundManager = std::make_shared<CSound>();
        m_globalSoundManager->addSound("background", "assets/sounds/background.wav");
        m_globalSoundManager->playSound("background", true); // Loop
    }
};

// Scene-specific sound effects
class Scene_Play {
    std::shared_ptr<CSound> m_soundManager;
    
    void init() {
        m_soundManager = std::make_shared<CSound>();
        m_soundManager->addSound("walk", "assets/sounds/tap.wav");
    }
};
```

---

## 🎨 2D Math and Graphics

### 2D Coordinate System
- **Screen Space**: Pixel grid with (0,0) typically at top-left
- **World Space**: Game world coordinates (can be negative in infinite grid)
- **Grid Space**: Tile-based coordinates for level editing

### Vector Mathematics
- **Point Subtraction**: destination - origin = distance
- **Point Addition**: origin + distance = destination
- **Vector Normalization**: `Vec2(V.x/L, V.y/L)` where `L = sqrt(x*x + y*y)`

### Trigonometry (SOH CAH TOA)
- `sin(θ) = opposite / hypotenuse = Y/R`
- `cos(θ) = adjacent / hypotenuse = X/R`
- `tan(θ) = opposite / adjacent = Y/X`

---

## 🔧 Build System

For detailed build instructions, platform-specific setup, and troubleshooting, see [doc/build/BUILD.md](doc/build/BUILD.md).

### Quick Build Commands
```bash
make              # Release build
make debug        # Debug build with symbols
make run          # Build and run
make clean        # Clean build files
make config       # Show build configuration
```

### Platform Support
- **macOS**: Native support with Homebrew SFML
- **Windows**: MSYS2 or Visual Studio + vcpkg (see [doc/build/WINDOWS_SETUP_GUIDE.md](doc/build/WINDOWS_SETUP_GUIDE.md))
- **Linux**: Native package managers (apt, yum, pacman, etc.)

---

## 🚨 Troubleshooting

### Map Editor Issues
- **Missing textures**: Magenta rectangles indicate missing assets in `metadata/assets.txt`
- **Performance**: Large levels render only visible portions automatically
- **Coordinates**: Remember coordinates can be negative in infinite grid
- **Save location**: Files save to `metadata/levels/` directory
- **Level selector empty**: Ensure .txt files exist in `metadata/levels/`

### General Issues
- **Build failures**: Check [doc/build/BUILD.md](doc/build/BUILD.md) for platform-specific instructions
- **Missing SFML**: Install via package manager or build from source
- **Runtime errors**: Ensure SFML libraries are in PATH/LD_LIBRARY_PATH
- **Asset loading**: Verify `metadata/assets.txt` contains all required assets

### Tips
- Use the object counter to track level complexity
- Preview panel shows exactly what will be placed
- Camera follows cursor automatically - no manual control needed
- Grid extends infinitely - explore freely!
- Cursor is always visible on top of all objects
- Level selector scans files automatically

---

## 🎯 Integration with Main Game

Created levels work seamlessly with the main game:

1. **Create Level**: Use Map Editor to create your level
2. **Save Level**: Press F to save with timestamp to `metadata/levels/`
3. **Test Level**: Use L to load and test existing levels in editor
4. **Deploy Level**: Rename your level file to `level1.txt` to make it the default game level
5. **Play**: The game automatically loads and parses the level format

All object types, coordinates, and properties are preserved between editor and game.

---

## 🚀 Future Enhancements

### Map Editor
- Multiple level support with tabbed interface
- Copy/paste functionality for level sections
- Undo/redo system for editing operations
- Layer support for complex level design
- Custom asset loading and management
- Level validation and error checking
- Minimap view for large levels
- Zoom functionality for detailed editing

### Game Engine
- Animation system improvements
- Particle effects system
- Advanced physics (slopes, moving platforms)
- Sound system enhancements
- Scripting system integration
- Multiplayer networking support

---

## 📚 Documentation Structure

This README provides a comprehensive overview of the GameEngine. Additional detailed documentation is organized in the `doc/` folder:

### 🔧 Build & Setup
- **[doc/build/BUILD.md](doc/build/BUILD.md)** - Comprehensive build guide with platform-specific instructions
- **[doc/build/WINDOWS_SETUP_GUIDE.md](doc/build/WINDOWS_SETUP_GUIDE.md)** - Detailed Windows setup instructions

### ⚙️ Engine Systems
- **[doc/systems/CAMERA_SYSTEM.md](doc/systems/CAMERA_SYSTEM.md)** - Camera following with dead zone implementation
- **[doc/systems/SOUND_SYSTEM.md](doc/systems/SOUND_SYSTEM.md)** - Global and scene-specific audio system
- **[doc/systems/VIEWPORT_SYSTEM.md](doc/systems/VIEWPORT_SYSTEM.md)** - Viewport and rendering system
- **[doc/systems/CENTERED_VIEWPORT_SYSTEM.md](doc/systems/CENTERED_VIEWPORT_SYSTEM.md)** - Centered viewport implementation
- **[doc/systems/CONFIGURABLE_SCALE_SYSTEM.md](doc/systems/CONFIGURABLE_SCALE_SYSTEM.md)** - Configurable scaling system

### 🎮 Game Features
- **[doc/features/BATTLE_SYSTEM.md](doc/features/BATTLE_SYSTEM.md)** - Turn-based battle system implementation
- **[doc/features/COLLISION_GRID_MOVEMENT.md](doc/features/COLLISION_GRID_MOVEMENT.md)** - Grid-based movement with collision
- **[doc/features/GRID_MOVEMENT_COOLDOWN.md](doc/features/GRID_MOVEMENT_COOLDOWN.md)** - Movement cooldown system

### 🚀 Development Guides
- **[doc/development/RPG_DEVELOPMENT_GUIDE.md](doc/development/RPG_DEVELOPMENT_GUIDE.md)** - Guide for RPG game development
- **[doc/development/RPG_DEVELOPMENT_ROADMAP.md](doc/development/RPG_DEVELOPMENT_ROADMAP.md)** - RPG development roadmap
- **[doc/development/RPG_SCENES_IMPLEMENTATION.md](doc/development/RPG_SCENES_IMPLEMENTATION.md)** - RPG scene implementation
- **[doc/development/STEAM_PUBLISHING_GUIDE.md](doc/development/STEAM_PUBLISHING_GUIDE.md)** - Guide for publishing to Steam
- **[doc/development/SCENE_REORGANIZATION.md](doc/development/SCENE_REORGANIZATION.md)** - Scene organization patterns
- **[doc/development/COMPONENT_ORGANIZATION.md](doc/development/COMPONENT_ORGANIZATION.md)** - Component organization guide

### 💡 Examples & Tutorials
- **[doc/examples/ANIMATION_EXAMPLES.md](doc/examples/ANIMATION_EXAMPLES.md)** - Animation system examples
- **[doc/examples/GRID_MOVEMENT_EXAMPLES.md](doc/examples/GRID_MOVEMENT_EXAMPLES.md)** - Grid movement implementation examples
- **[doc/examples/LEVEL_DESIGN_GUIDE.md](doc/examples/LEVEL_DESIGN_GUIDE.md)** - Level design best practices
- **[doc/examples/dialogue_example.md](doc/examples/dialogue_example.md)** - Dialogue system example

### 📋 Meta Documentation
- **[doc/DOCUMENTATION_CONSOLIDATION.md](doc/DOCUMENTATION_CONSOLIDATION.md)** - Documentation organization summary

---

## 📚 Learning Resources

This project demonstrates key game development concepts:

- **Entity-Component-System (ECS)** architecture
- **Resource management** with RAII principles
- **2D graphics programming** with SFML
- **Cross-platform development** techniques
- **Modern C++20** features and best practices
- **Game loop** and timing systems
- **Asset management** and loading systems
- **Scene management** and state machines

Perfect for learning game engine development, C++ programming, and software architecture principles.

---

## 🤝 Contributing

This project serves as an educational example of game engine development. Feel free to:

- Study the code structure and architecture
- Experiment with new features and improvements
- Use as a foundation for your own game projects
- Learn from the ECS implementation and design patterns

---

## 📄 License

Educational project based on COMP4300 course materials by Dave Churchill.

---

*This GameEngine provides a comprehensive foundation for 2D game development with modern C++ practices, professional-grade tools, and extensive documentation for learning and development.*
