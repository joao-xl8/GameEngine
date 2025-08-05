@echo off
REM Windows build script for GameEngine
REM Requires MinGW/MSYS2 with SFML installed

echo Building GameEngine for Windows...

REM Set SFML path (modify as needed)
set SFML_DIR=C:\SFML
if exist "C:\msys64\mingw64\include\SFML" (
    set SFML_DIR=C:\msys64\mingw64
    echo Using MSYS2 SFML installation
) else (
    echo Using SFML from %SFML_DIR%
    echo If SFML is not found, please:
    echo 1. Install MSYS2 and run: pacman -S mingw-w64-x86_64-sfml
    echo 2. Or download SFML from https://www.sfml-dev.org/download.php
)

REM Create build directory
if not exist build mkdir build

REM Compile
g++ -std=c++20 -Wall -O2 ^
    -I%SFML_DIR%\include -Iinclude\imgui -Isrc ^
    main.cpp src\*.cpp include\imgui\imgui*.cpp ^
    -L%SFML_DIR%\lib ^
    -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio ^
    -lopengl32 -lgdi32 -lwinmm ^
    -static-libgcc -static-libstdc++ ^
    -o build\GameEngine.exe

if %ERRORLEVEL% EQU 0 (
    echo Build successful! Executable: build\GameEngine.exe
    echo.
    echo To run: build\GameEngine.exe
) else (
    echo Build failed!
    echo Please check that SFML is properly installed.
)

pause
