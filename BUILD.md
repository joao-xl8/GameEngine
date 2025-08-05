# GameEngine - Cross-Platform Build Guide

This document provides comprehensive instructions for building the GameEngine project on Windows, macOS, and Linux.

## Prerequisites

### All Platforms
- C++20 compatible compiler (GCC 10+, Clang 10+, or MSVC 2019+)
- SFML 2.5+ development libraries
- Make (for Makefile builds) or CMake 3.16+ (for CMake builds)

## Quick Start

### Using Makefile (Recommended)
```bash
# Build and run
make run

# Or just build
make

# Debug build
make debug

# Clean
make clean

# Show help
make help
```

### Using CMake
```bash
mkdir build && cd build
cmake ..
make
./bin/GameEngine
```

## Platform-Specific Instructions

### Windows

#### Option 1: MSYS2 (Recommended)
1. Install [MSYS2](https://www.msys2.org/)
2. Open MSYS2 terminal and install dependencies:
   ```bash
   pacman -S mingw-w64-x86_64-gcc
   pacman -S mingw-w64-x86_64-sfml
   pacman -S mingw-w64-x86_64-cmake
   pacman -S make
   ```
3. Build:
   ```bash
   make
   # or
   ./build_windows.bat
   ```

#### Option 2: Manual SFML Installation
1. Download SFML from [sfml-dev.org](https://www.sfml-dev.org/download.php)
2. Extract to `C:\SFML`
3. Install MinGW-w64
4. Build:
   ```bash
   make
   # or
   ./build_windows.bat
   ```

#### Option 3: Visual Studio
1. Install Visual Studio 2019+ with C++ support
2. Install SFML via vcpkg:
   ```cmd
   vcpkg install sfml:x64-windows
   ```
3. Use CMake:
   ```cmd
   mkdir build && cd build
   cmake .. -DCMAKE_TOOLCHAIN_FILE=[vcpkg root]/scripts/buildsystems/vcpkg.cmake
   cmake --build .
   ```

### macOS

#### Using Homebrew (Recommended)
```bash
# Install dependencies
brew install sfml@2
# or
make setup-deps

# Build
make
```

#### Using MacPorts
```bash
sudo port install sfml +universal
make
```

#### Manual Installation
1. Download SFML from [sfml-dev.org](https://www.sfml-dev.org/download.php)
2. Install to `/usr/local/`
3. Build with `make`

### Linux

#### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install build-essential libsfml-dev cmake
make
```

#### CentOS/RHEL/Fedora
```bash
# CentOS/RHEL
sudo yum install gcc-c++ SFML-devel cmake make
# or Fedora
sudo dnf install gcc-c++ SFML-devel cmake make

make
```

#### Arch Linux
```bash
sudo pacman -S gcc sfml cmake make
make
```

#### openSUSE
```bash
sudo zypper install gcc-c++ sfml2-devel cmake make
make
```

#### Using the Linux build script
```bash
./build_linux.sh
```

## Build Options

### Makefile Targets
- `make` or `make all` - Build release version
- `make debug` - Build debug version
- `make run` - Build and run
- `make clean` - Clean build files
- `make setup-deps` - Show dependency installation commands
- `make install` - Install to system (Unix-like only)
- `make help` - Show help
- `make config` - Show build configuration

### CMake Options
```bash
# Debug build
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Release build
cmake -DCMAKE_BUILD_TYPE=Release ..

# Specify SFML location (if needed)
cmake -DSFML_ROOT=/path/to/sfml ..
```

## Troubleshooting

### SFML Not Found
- **Windows**: Ensure SFML is in `C:\SFML` or use MSYS2
- **macOS**: Install via Homebrew: `brew install sfml@2`
- **Linux**: Install dev packages: `sudo apt-get install libsfml-dev`

### Compiler Errors
- Ensure you have a C++20 compatible compiler
- Check that all source files are present
- Verify SFML version compatibility (2.5+)

### Runtime Errors
- Ensure SFML libraries are in your PATH/LD_LIBRARY_PATH
- Check that assets and config files are in the correct location
- On Windows, SFML DLLs must be accessible

### Build System Issues
- **Make not found**: Install build tools for your platform
- **CMake not found**: Install CMake 3.16+
- **Permission denied**: Ensure build scripts are executable (`chmod +x`)

## Project Structure
```
GameEngine/
├── src/                 # Source files
├── include/imgui/       # ImGui library
├── assets/             # Game assets
├── metadata/           # Game metadata
├── build/              # Build output (created)
├── Makefile            # Cross-platform Makefile
├── CMakeLists.txt      # CMake configuration
├── build_windows.bat   # Windows build script
├── build_linux.sh      # Linux build script
└── BUILD.md           # This file
```

## Advanced Usage

### Custom SFML Location
```bash
# Makefile
make SFML_DIR=/custom/path/to/sfml

# CMake
cmake -DSFML_ROOT=/custom/path/to/sfml ..
```

### Static Linking (Windows)
The Windows build automatically uses static linking for the C++ runtime. For fully static builds:
```bash
make CXXFLAGS="-static"
```

### Cross-Compilation
The build system supports cross-compilation. Set the appropriate compiler:
```bash
make CXX=x86_64-w64-mingw32-g++  # Windows from Linux
```

## Contributing

When adding new source files:
1. Add `.cpp` files to the `src/` directory
2. The Makefile automatically detects new files
3. For CMake, add files to the `SOURCES` list in `CMakeLists.txt`

## Support

If you encounter build issues:
1. Check this guide for your platform
2. Ensure all dependencies are installed
3. Try the platform-specific build scripts
4. Use `make help` or `make config` for diagnostics
