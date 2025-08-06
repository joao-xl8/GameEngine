# Windows Setup Guide for GameEngine

## The Problem
Your current g++ compiler doesn't support C++20. The error `g++: error: unrecognized command line option '-std=c++20'` indicates you're using an older compiler version.

## Solution Options

### Option 1: MSYS2 (Recommended)

MSYS2 provides the most up-to-date compilers and easy SFML installation.

#### Step 1: Install MSYS2
1. Download MSYS2 from https://www.msys2.org/
2. Run the installer and follow the instructions
3. After installation, open the MSYS2 terminal

#### Step 2: Update MSYS2
```bash
pacman -Syu
```
Close the terminal when prompted, then reopen and run:
```bash
pacman -Su
```

#### Step 3: Install Development Tools
```bash
# Install GCC compiler with C++20 support
pacman -S mingw-w64-x86_64-gcc

# Install SFML
pacman -S mingw-w64-x86_64-sfml

# Install other useful tools
pacman -S mingw-w64-x86_64-make
pacman -S mingw-w64-x86_64-cmake
```

#### Step 4: Add to PATH
Add `C:\msys64\mingw64\bin` to your Windows PATH environment variable:
1. Press Win+R, type `sysdm.cpl`, press Enter
2. Click "Environment Variables"
3. Under "System Variables", find and select "Path", click "Edit"
4. Click "New" and add: `C:\msys64\mingw64\bin`
5. Click OK to save

#### Step 5: Verify Installation
Open a new Command Prompt and run:
```cmd
g++ --version
```
You should see GCC 11+ which supports C++20.

### Option 2: Manual SFML Installation

If you prefer to keep your current setup and just update the compiler:

#### Step 1: Update MinGW
1. Download the latest MinGW-w64 from https://www.mingw-w64.org/downloads/
2. Or use a package manager like Chocolatey:
   ```cmd
   choco install mingw
   ```

#### Step 2: Download SFML
1. Go to https://www.sfml-dev.org/download.php
2. Download "GCC 7.3.0 MinGW (SEH) - 64-bit" or newer
3. Extract to `C:\SFML`

#### Step 3: Copy DLL Files
Copy all `.dll` files from `C:\SFML\bin\` to your project's build directory.

### Option 3: Visual Studio (Alternative)

If you prefer Visual Studio:

#### Step 1: Install Visual Studio
1. Download Visual Studio Community (free)
2. Install with C++ development tools

#### Step 2: Install SFML for Visual Studio
1. Download SFML Visual C++ version
2. Extract to `C:\SFML`

#### Step 3: Use Visual Studio Build Script
Create `build_vs.bat`:
```batch
@echo off
cl /EHsc /std:c++20 /I"C:\SFML\include" main.cpp src\*.cpp /link /LIBPATH:"C:\SFML\lib" sfml-graphics.lib sfml-window.lib sfml-system.lib sfml-audio.lib /OUT:GameEngine.exe
```

## Using the Fixed Build Script

I've created `build_windows_fixed.bat` which:
- Automatically detects your compiler's C++ standard support
- Falls back from C++20 → C++17 → C++14 → C++11
- Provides better error messages
- Checks for SFML in multiple locations

### To use it:
```cmd
cd /path/to/GameEngine
build_windows_fixed.bat
```

## Troubleshooting

### Error: "g++ not found"
- Make sure MinGW/MSYS2 bin directory is in your PATH
- Restart Command Prompt after changing PATH
- Verify with: `g++ --version`

### Error: "SFML headers not found"
- Check that SFML is installed in the expected location
- Verify include path: `C:\SFML\include\SFML\` or `C:\msys64\mingw64\include\SFML\`
- Make sure you downloaded the correct SFML version for your compiler

### Error: "Cannot find -lsfml-graphics"
- Check that SFML libraries are in the lib directory
- Verify lib path: `C:\SFML\lib\` or `C:\msys64\mingw64\lib\`
- Make sure library files exist (`.a` files for MinGW)

### Runtime Error: "DLL not found"
- Copy SFML DLL files to your executable directory
- From MSYS2: Copy from `C:\msys64\mingw64\bin\`
- From manual install: Copy from `C:\SFML\bin\`

Required DLLs:
- `sfml-graphics-2.dll`
- `sfml-window-2.dll`
- `sfml-system-2.dll`
- `sfml-audio-2.dll`

### C++ Standard Issues
The project uses modern C++ features. Minimum requirements:
- **C++17**: Required for most features
- **C++20**: Preferred for full feature support
- **C++14**: May work with minor modifications
- **C++11**: Requires significant code changes

## Quick Fix for Immediate Building

If you need to build right now with your current setup:

1. **Downgrade C++ Standard**: Edit your build script to use `-std=c++17` or `-std=c++14`

2. **Check Compiler Version**:
   ```cmd
   g++ --version
   ```

3. **Update Code if Needed**: Some C++20 features may need to be replaced:
   - Replace `std::format` with `std::stringstream`
   - Replace some modern STL features with older equivalents

## Recommended Development Environment

For the best Windows C++ development experience:

1. **MSYS2** for compiler and libraries
2. **Visual Studio Code** with C++ extensions
3. **Git for Windows** for version control
4. **CMake** for build system (optional)

This setup gives you:
- Latest GCC with full C++20 support
- Easy package management
- Cross-platform compatibility
- Modern development tools

## Testing Your Setup

After installation, test with this simple program:

```cpp
// test_setup.cpp
#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
    std::cout << "Testing SFML setup...\n";
    
    sf::RenderWindow window(sf::VideoMode(200, 200), "Test");
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);
    
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        
        window.clear();
        window.draw(shape);
        window.display();
    }
    
    std::cout << "SFML setup successful!\n";
    return 0;
}
```

Compile with:
```cmd
g++ -std=c++17 test_setup.cpp -lsfml-graphics -lsfml-window -lsfml-system -o test_setup.exe
```

If this works, your setup is ready for the GameEngine project!
