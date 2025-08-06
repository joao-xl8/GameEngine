@echo off
REM Windows build script for GameEngine with C++ standard detection
REM Requires MinGW/MSYS2 with SFML installed

echo Building GameEngine for Windows...

REM Check g++ version and determine C++ standard support
echo Checking g++ version...
g++ --version > nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: g++ not found! Please install MinGW or MSYS2.
    echo.
    echo Installation options:
    echo 1. Install MSYS2 from https://www.msys2.org/
    echo 2. Install MinGW-w64 from https://www.mingw-w64.org/
    echo 3. Install through package manager like Chocolatey: choco install mingw
    pause
    exit /b 1
)

REM Test C++ standard support
echo Testing C++ standard support...
echo int main(){return 0;} > temp_test.cpp

REM Try C++20 first
g++ -std=c++20 temp_test.cpp -o temp_test.exe > nul 2>&1
if %ERRORLEVEL% EQU 0 (
    set CPP_STD=c++20
    echo Using C++20 standard
) else (
    REM Try C++17
    g++ -std=c++17 temp_test.cpp -o temp_test.exe > nul 2>&1
    if %ERRORLEVEL% EQU 0 (
        set CPP_STD=c++17
        echo Using C++17 standard
    ) else (
        REM Try C++14
        g++ -std=c++14 temp_test.cpp -o temp_test.exe > nul 2>&1
        if %ERRORLEVEL% EQU 0 (
            set CPP_STD=c++14
            echo Using C++14 standard
        ) else (
            REM Fallback to C++11
            set CPP_STD=c++11
            echo Using C++11 standard (fallback)
        )
    )
)

REM Clean up test files
del temp_test.cpp > nul 2>&1
del temp_test.exe > nul 2>&1

REM Set SFML path (modify as needed)
set SFML_DIR=C:\SFML

REM Check for MSYS2 SFML installation
if exist "C:\msys64\mingw64\include\SFML" (
    set SFML_DIR=C:\msys64\mingw64
    echo Using MSYS2 SFML installation at %SFML_DIR%
) else if exist "C:\msys64\ucrt64\include\SFML" (
    set SFML_DIR=C:\msys64\ucrt64
    echo Using MSYS2 UCRT64 SFML installation at %SFML_DIR%
) else if exist "%SFML_DIR%\include\SFML" (
    echo Using SFML from %SFML_DIR%
) else (
    echo WARNING: SFML not found at expected locations!
    echo Please install SFML:
    echo.
    echo Option 1 - MSYS2 (Recommended):
    echo   1. Install MSYS2 from https://www.msys2.org/
    echo   2. Open MSYS2 terminal and run:
    echo      pacman -S mingw-w64-x86_64-gcc
    echo      pacman -S mingw-w64-x86_64-sfml
    echo   3. Add C:\msys64\mingw64\bin to your PATH
    echo.
    echo Option 2 - Manual SFML:
    echo   1. Download SFML from https://www.sfml-dev.org/download.php
    echo   2. Extract to C:\SFML
    echo   3. Make sure you have MinGW-w64 installed
    echo.
    pause
    exit /b 1
)

REM Create build directory
if not exist build mkdir build
if not exist build\release mkdir build\release

echo.
echo Compiling with %CPP_STD% standard...
echo SFML Directory: %SFML_DIR%
echo.

REM Compile with detected C++ standard
g++ -std=%CPP_STD% -Wall -O2 ^
    -I%SFML_DIR%\include -Iinclude -Isrc ^
    main.cpp src\*.cpp ^
    -L%SFML_DIR%\lib ^
    -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio ^
    -lopengl32 -lgdi32 -lwinmm ^
    -static-libgcc -static-libstdc++ ^
    -o build\release\GameEngine.exe

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ================================
    echo Build successful!
    echo ================================
    echo Executable: build\release\GameEngine.exe
    echo C++ Standard: %CPP_STD%
    echo SFML Path: %SFML_DIR%
    echo.
    echo To run the game:
    echo   cd build\release
    echo   GameEngine.exe
    echo.
) else (
    echo.
    echo ================================
    echo Build failed!
    echo ================================
    echo.
    echo Common solutions:
    echo 1. Make sure SFML is properly installed
    echo 2. Check that all .dll files are in the same directory as the .exe
    echo 3. Verify g++ compiler is working: g++ --version
    echo 4. Try installing SFML via MSYS2:
    echo    pacman -S mingw-w64-x86_64-sfml
    echo.
    echo If using manual SFML installation, make sure you have:
    echo - SFML include files in %SFML_DIR%\include\
    echo - SFML library files in %SFML_DIR%\lib\
    echo - SFML DLL files copied to your executable directory
    echo.
)

pause
