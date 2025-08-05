COMP4300 - (https://www.youtube.com/@DaveChurchill/videos) 
Credits to Dave Churchill

# GameEngine - Cross-Platform 2D Game Engine

A modern C++20 game engine built with Entity-Component-System (ECS) architecture, featuring SFML graphics and ImGui integration.

## 🚀 Cross-Platform Build System

This GameEngine supports **Windows**, **macOS**, and **Linux** with multiple build systems and automatic platform detection.

### ✅ Quick Start

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

### 🔧 Build System Status

Check your system compatibility:
```bash
python3 build.py --status
```

**Example output:**
```
GameEngine Build System Status
========================================
Platform: Macos
SFML: ✓ Found at /opt/homebrew/opt/sfml@2
Dependencies: ✓ All required tools available
Available builds: Makefile (Release), Makefile (Debug), CMake
```

### 🛠️ Multiple Build Systems

#### 1. **Makefile (Recommended)**
Cross-platform with automatic OS detection:
```bash
make              # Release build
make debug        # Debug build with symbols
make run          # Build and run
make clean        # Clean build files
make config       # Show build configuration
make setup-deps   # Show dependency installation
```

#### 2. **CMake (Modern)**
For advanced users and IDEs:
```bash
mkdir build && cd build
cmake ..          # Configure
make              # Build
./bin/GameEngine  # Run
```

#### 3. **Python Helper (Smart)**
Intelligent build assistant:
```bash
python3 build.py --build make    # Build with Makefile
python3 build.py --build cmake   # Build with CMake
python3 build.py --debug --run   # Debug build and run
python3 build.py --status        # System status
```

#### 4. **Platform-Specific Scripts**
Direct platform builds:
```bash
./build_linux.sh      # Linux
./build_windows.bat    # Windows
make                   # macOS (or any Unix-like)
```

### 🌍 Platform Setup Instructions

#### **macOS**
```bash
# Install Homebrew (if not installed)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install sfml@2
make run
```

#### **Windows**
**Option 1: MSYS2 (Recommended)**
```bash
# 1. Install MSYS2 from https://www.msys2.org/
# 2. Open MSYS2 terminal and run:
pacman -S mingw-w64-x86_64-gcc
pacman -S mingw-w64-x86_64-sfml
pacman -S mingw-w64-x86_64-cmake
pacman -S make

# 3. Build
make run
```

**Option 2: Visual Studio + vcpkg**
```cmd
# Install SFML via vcpkg
vcpkg install sfml:x64-windows

# Use CMake
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=[vcpkg root]/scripts/buildsystems/vcpkg.cmake
cmake --build .
```

#### **Linux**
**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install build-essential libsfml-dev cmake
make run
```

**CentOS/RHEL/Fedora:**
```bash
# CentOS/RHEL
sudo yum install gcc-c++ SFML-devel cmake make
# Fedora
sudo dnf install gcc-c++ SFML-devel cmake make

make run
```

**Arch Linux:**
```bash
sudo pacman -S gcc sfml cmake make
make run
```

**openSUSE:**
```bash
sudo zypper install gcc-c++ sfml2-devel cmake make
make run
```

### 📁 Project Structure

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
├── build/                  # Build outputs
│   ├── release/GameEngine  # Release executable
│   └── debug/GameEngine    # Debug executable
├── cmake_build/            # CMake build directory
├── Makefile               # Cross-platform Makefile
├── CMakeLists.txt         # CMake configuration
├── build.py               # Python build helper
├── BUILD.md               # Detailed build guide
├── build_windows.bat      # Windows build script
├── build_linux.sh         # Linux build script
└── README.md              # This file
```

### 🎯 Build System Features

- **🔍 Automatic Platform Detection** - Detects macOS/Windows/Linux automatically
- **📦 Smart SFML Detection** - Finds SFML via Homebrew, pkg-config, or manual paths
- **✅ Dependency Checking** - Verifies all required tools are available
- **🔧 Multiple Build Systems** - Choose Makefile, CMake, or platform scripts
- **📂 Asset Management** - Automatically copies game assets to build directory
- **🌐 Cross-Compilation Ready** - Can build for different target platforms
- **🐛 Debug Support** - Full debug builds with symbols and assertions
- **⚡ Optimized Builds** - Release builds with `-O3` optimization

### 🔧 Advanced Usage

**Custom SFML Location:**
```bash
make SFML_DIR=/custom/path/to/sfml
# or
cmake -DSFML_ROOT=/custom/path/to/sfml ..
```

**Static Linking (Windows):**
```bash
make CXXFLAGS="-static"
```

**Cross-Compilation:**
```bash
make CXX=x86_64-w64-mingw32-g++  # Windows from Linux
```

**Verbose Build:**
```bash
make VERBOSE=1
```

### 🚨 Troubleshooting

**SFML Not Found:**
- **macOS**: `brew install sfml@2`
- **Linux**: `sudo apt-get install libsfml-dev`
- **Windows**: Use MSYS2 or download from [sfml-dev.org](https://www.sfml-dev.org/)

**Compiler Errors:**
- Ensure C++20 compatible compiler (GCC 10+, Clang 10+, MSVC 2019+)
- Check that all source files are present
- Verify SFML version compatibility (2.5+)

**Runtime Errors:**
- Ensure SFML libraries are in PATH/LD_LIBRARY_PATH
- Check that assets and config files are accessible
- On Windows, SFML DLLs must be in the same directory as executable

**Build System Issues:**
- Run `python3 build.py --status` for diagnostics
- Use `make config` to see current build configuration
- Check `BUILD.md` for detailed troubleshooting guide

### 📚 Documentation

- **BUILD.md** - Comprehensive build guide with platform-specific instructions
- **CMakeLists.txt** - CMake configuration with comments
- **Makefile** - Cross-platform Makefile with help system
- **build.py** - Python helper with status checking and smart building

---

## Game Programming Concepts

### ECS - Entities, Components, Systems   
Software design paradigm for game architecture:

- **Entity**: Any object in the game (e.g. player, platform, tile, bullet, enemy)
- **Components**: Properties attached to entities (e.g. position, texture, animation, health, gravity) - PURE DATA
- **Systems**: Code/logic that drives behavior (e.g. movement, rendering, sound, physics)

### Engine Architecture:

```
Game
 > Scene
  > Systems
  > EntityManager
   > Entity
    > Component
```

**OOP for ECS design**

### RAII (Resource Acquisition is Initialization)
Binds the life cycle of a resource that must be acquired before use to lifetime of an object
-> New analogy: **EAR** (Encapsulate, Acquire, Release)

Can be cumbersome to create our own RAII classes for every data type we want to store a pointer to.
Use `std::shared_ptr<T>`, which handles RAII for a given data type,
and inc/dec every function it holds a pointer to. Making safe the end of parallel scopes ending in async time.

### Shared Pointer
Reference Counted Pointer:
- Internal counter set to 1 in constructor
- Every time the shared_ptr is copied, the counter increases by 1
- Every time a shared_ptr destructs, the counter is decremented by 1
- When count reaches 0, resource deallocated

### What is a Component?

An ECS Component is just DATA!
- Maybe some logic in the constructor (e.g. convert data types in constructor call)
- No helper functionality within components, don't do game logic
- A component class has some intuitive meaning to an Entity which contains it
- Ex: Position, Gravity, Health, etc.

### Data Management

One of the most important concepts: separate logic & data
- Create data structures which manage data so your logic code doesn't worry about it
- Complex algorithm shouldn't have to know whether set, vector, map implementation

### Entity Manager - Factory Design Pattern
(Will trade some memory for functionality)

Handle the creation, storage, and lifetime of ALL Entity objects:
- Functions like: add, remove, storage, bookkeeping
- Factory should somehow ensure that user cannot create its own Entities
- **Entity Tags**: We may want to group Entities by functionality (e.g. Tile, Player, Bullet, Enemy or an int for optimization)
- Entity Manager should have functionality for quickly getting Entities of a given tag
- Will also store separate data structures (vectors) of Entity objects by their tag for quick retrieval
- We store double the number of pointers to save on computation in our engine

**Be careful with Iterator Invalidation**

Caused by modifying the contents of a collection as we are iterating through it. In games this happens
all the time (e.g. spawn explosion entity, remove entities that died). One way to avoid iterator invalidation is to delay the effects of actions that modify collections until it is safe to do so.

### 2D Math

2D Games - Every game's graphics are drawn to a screen, screens are a plane (2D). Screens are made of small,
discrete elements: pixels, which are (usually) squares and have RGB elements.

**Color Space** - RGB, RGBA, CMYK, Grayscale, Monochrome

**2D Positions** - Systems display: pixel grid. Grid is in two dimensions (x,y) and we can use Euclidean plane to
visualize individual positions in which each integer position in a display pixel. Most APIs have (0,0)
in top-left

Basic notes:
- Point a: (4,2)
- Point b: (6,7)
- **Subtraction**: destination - origin = distance
- **Addition**: origin + distance = destination

**SOH CAH TOA**
- sin Ø = opposite / hypotenuse
- cos Ø = adjacent / hypotenuse  
- tan Ø = opposite / adjacent

**Unit Circle**
- Opposite = Y
- Adjacent = X
- Hypotenuse = R
- sin(Ø) = Y/R = Y
- cos(Ø) = X/R = X
- tan(Ø) = Y/X = R

**Vector Normalization**
- Length: `L = sqrtf(x*x + y*y);`
- Normalized: `Vec2(V.x/L, V.y/L)`
