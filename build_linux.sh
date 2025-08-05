#!/bin/bash
# Linux build script for GameEngine

echo "Building GameEngine for Linux..."

# Check if SFML is installed
if ! pkg-config --exists sfml-all; then
    echo "SFML not found! Please install SFML development libraries:"
    echo ""
    if command -v apt-get >/dev/null 2>&1; then
        echo "Ubuntu/Debian: sudo apt-get install libsfml-dev"
    elif command -v yum >/dev/null 2>&1; then
        echo "CentOS/RHEL: sudo yum install SFML-devel"
    elif command -v pacman >/dev/null 2>&1; then
        echo "Arch Linux: sudo pacman -S sfml"
    elif command -v zypper >/dev/null 2>&1; then
        echo "openSUSE: sudo zypper install sfml2-devel"
    else
        echo "Please install SFML development libraries for your distribution"
    fi
    echo ""
    exit 1
fi

# Create build directory
mkdir -p build

# Get SFML flags
SFML_CFLAGS=$(pkg-config --cflags sfml-all)
SFML_LIBS=$(pkg-config --libs sfml-all)

echo "Using SFML flags: $SFML_CFLAGS"
echo "Using SFML libs: $SFML_LIBS"

# Compile
g++ -std=c++20 -Wall -O2 \
    $SFML_CFLAGS -Iinclude/imgui -Isrc \
    main.cpp src/*.cpp include/imgui/imgui*.cpp \
    $SFML_LIBS -lGL -lX11 -lpthread \
    -o build/GameEngine

if [ $? -eq 0 ]; then
    echo "Build successful! Executable: build/GameEngine"
    echo ""
    echo "To run: ./build/GameEngine"
    
    # Make executable
    chmod +x build/GameEngine
else
    echo "Build failed!"
    exit 1
fi
