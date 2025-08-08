# Cross-Platform GameEngine Makefile
# Supports Windows (MinGW/MSYS2), macOS, and Linux

# Project configuration
PROJECT_NAME = GameEngine
TARGET = $(PROJECT_NAME)
BUILD_DIR = build
SRC_DIR = src
INCLUDE_DIR = include
IMGUI_DIR = $(INCLUDE_DIR)/imgui

# Detect operating system
ifeq ($(OS),Windows_NT)
    DETECTED_OS := Windows
    TARGET := $(TARGET).exe
else
    DETECTED_OS := $(shell uname -s)
endif

# Compiler settings
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -O2
DEBUG_FLAGS = -g -DDEBUG -O0
RELEASE_FLAGS = -DNDEBUG -O3

# Source files
SOURCES = main.cpp \
          $(wildcard $(SRC_DIR)/*.cpp) \
          $(wildcard $(SRC_DIR)/scenes/*.cpp) \
          $(wildcard $(SRC_DIR)/ui/*.cpp) \
          $(wildcard $(IMGUI_DIR)/imgui*.cpp)

# Object files
OBJECTS = $(SOURCES:%.cpp=$(BUILD_DIR)/%.o)

# Platform-specific configurations
ifeq ($(DETECTED_OS),Windows)
    # Windows (MinGW/MSYS2)
    SFML_DIR ?= C:/SFML
    INCLUDES = -I$(SFML_DIR)/include -I$(IMGUI_DIR) -I$(SRC_DIR)
    LIBS = -L$(SFML_DIR)/lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
    LIBS += -lopengl32 -lgdi32 -lwinmm
    CXXFLAGS += -static-libgcc -static-libstdc++
    
    # Windows-specific commands
    RM = del /Q
    RMDIR = rmdir /S /Q
    MKDIR = mkdir
    PATH_SEP = \\
    
    # Check for MSYS2/MinGW SFML
    ifneq ($(shell where pacman 2>nul),)
        # MSYS2 detected
        SFML_MSYS2 = $(shell pacman -Ql mingw-w64-x86_64-sfml 2>nul | head -1 | cut -d' ' -f2 | sed 's|/[^/]*$$||')
        ifneq ($(SFML_MSYS2),)
            SFML_DIR = $(SFML_MSYS2)
            INCLUDES = -I$(SFML_DIR)/include -I$(IMGUI_DIR) -I$(SRC_DIR)
            LIBS = -L$(SFML_DIR)/lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
        endif
    endif
    
else ifeq ($(DETECTED_OS),Darwin)
    # macOS
    HOMEBREW_PREFIX = $(shell brew --prefix 2>/dev/null || echo /opt/homebrew)
    SFML_DIR = $(HOMEBREW_PREFIX)/opt/sfml@2
    
    # Fallback to regular SFML if sfml@2 not found
    ifeq ($(wildcard $(SFML_DIR)),)
        SFML_DIR = $(HOMEBREW_PREFIX)/opt/sfml
    endif
    
    INCLUDES = -I$(SFML_DIR)/include -I$(IMGUI_DIR) -I$(SRC_DIR)
    LIBS = -L$(SFML_DIR)/lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
    LIBS += -framework OpenGL -framework Foundation
    
    # macOS-specific commands
    RM = rm -f
    RMDIR = rm -rf
    MKDIR = mkdir -p
    PATH_SEP = /
    
else
    # Linux and other Unix-like systems
    INCLUDES = -I$(IMGUI_DIR) -I$(SRC_DIR)
    
    # Try pkg-config first
    SFML_CFLAGS = $(shell pkg-config --cflags sfml-all 2>/dev/null)
    SFML_LIBS = $(shell pkg-config --libs sfml-all 2>/dev/null)
    
    ifneq ($(SFML_CFLAGS),)
        INCLUDES += $(SFML_CFLAGS)
        LIBS = $(SFML_LIBS)
    else
        # Fallback to standard locations
        LIBS = -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
    endif
    
    LIBS += -lGL -lX11 -lpthread
    
    # Linux-specific commands
    RM = rm -f
    RMDIR = rm -rf
    MKDIR = mkdir -p
    PATH_SEP = /
endif

# Build modes
ifdef DEBUG
    CXXFLAGS += $(DEBUG_FLAGS)
    BUILD_DIR := $(BUILD_DIR)/debug
    TARGET := $(BUILD_DIR)/$(TARGET)
else
    CXXFLAGS += $(RELEASE_FLAGS)
    BUILD_DIR := $(BUILD_DIR)/release
    TARGET := $(BUILD_DIR)/$(TARGET)
endif

# Default target
.PHONY: all clean debug release run test-run install help setup-deps

all: $(TARGET)

# Create build directory
$(BUILD_DIR):
	@echo "Creating build directory..."
	@$(MKDIR) $(BUILD_DIR)$(PATH_SEP)$(SRC_DIR) 2>/dev/null || mkdir -p $(BUILD_DIR)/$(SRC_DIR)
	@$(MKDIR) $(BUILD_DIR)$(PATH_SEP)$(SRC_DIR)$(PATH_SEP)scenes 2>/dev/null || mkdir -p $(BUILD_DIR)/$(SRC_DIR)/scenes
	@$(MKDIR) $(BUILD_DIR)$(PATH_SEP)$(SRC_DIR)$(PATH_SEP)ui 2>/dev/null || mkdir -p $(BUILD_DIR)/$(SRC_DIR)/ui
	@$(MKDIR) $(BUILD_DIR)$(PATH_SEP)$(IMGUI_DIR) 2>/dev/null || mkdir -p $(BUILD_DIR)/$(IMGUI_DIR)

# Compile object files
$(BUILD_DIR)/%.o: %.cpp | $(BUILD_DIR)
	@echo "Compiling $<..."
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Link executable
$(TARGET): $(OBJECTS)
	@echo "Linking $(TARGET)..."
	@$(CXX) $(OBJECTS) $(LIBS) -o $(TARGET)
	@echo "Build complete! Platform: $(DETECTED_OS)"
	@echo "Executable: $(TARGET)"

# Debug build
debug:
	@$(MAKE) DEBUG=1

# Release build
release:
	@$(MAKE)

# Run the executable
run: $(TARGET)
	@echo "Running $(PROJECT_NAME)..."
	@./$(TARGET)

# Test run (safer for automated testing)
test-run: $(TARGET)
	@echo "Testing $(PROJECT_NAME)..."
	@echo "Working directory: $(shell pwd)"
	@echo "Executable: $(TARGET)"
	@echo "Assets check:"
	@ls -la assets/ 2>/dev/null || echo "  Warning: assets/ directory not found"
	@ls -la metadata/ 2>/dev/null || echo "  Warning: metadata/ directory not found"
	@ls -la config.txt 2>/dev/null || echo "  Warning: config.txt not found"
	@echo "Starting game..."
	@./$(TARGET)

# Clean build files
clean:
	@echo "Cleaning build files..."
ifeq ($(DETECTED_OS),Windows)
	@if exist build $(RMDIR) build
else
	@$(RMDIR) build
endif

# Install dependencies (platform-specific)
setup-deps:
	@echo "Setting up dependencies for $(DETECTED_OS)..."
ifeq ($(DETECTED_OS),Windows)
	@echo "Windows: Please install SFML manually or use MSYS2:"
	@echo "  MSYS2: pacman -S mingw-w64-x86_64-sfml"
	@echo "  Manual: Download from https://www.sfml-dev.org/download.php"
else ifeq ($(DETECTED_OS),Darwin)
	@echo "macOS: Installing SFML via Homebrew..."
	@brew install sfml@2 || brew install sfml
else
	@echo "Linux: Installing SFML..."
	@if command -v apt-get >/dev/null 2>&1; then \
		echo "Ubuntu/Debian: sudo apt-get install libsfml-dev"; \
	elif command -v yum >/dev/null 2>&1; then \
		echo "CentOS/RHEL: sudo yum install SFML-devel"; \
	elif command -v pacman >/dev/null 2>&1; then \
		echo "Arch: sudo pacman -S sfml"; \
	elif command -v zypper >/dev/null 2>&1; then \
		echo "openSUSE: sudo zypper install sfml2-devel"; \
	else \
		echo "Please install SFML development libraries for your distribution"; \
	fi
endif

# Install the executable (Unix-like systems)
install: $(TARGET)
ifeq ($(DETECTED_OS),Windows)
	@echo "Windows: Copy $(TARGET) to desired location manually"
else
	@echo "Installing $(PROJECT_NAME)..."
	@install -m 755 $(TARGET) /usr/local/bin/$(PROJECT_NAME)
	@echo "Installed to /usr/local/bin/$(PROJECT_NAME)"
endif

# Help target
help:
	@echo "GameEngine Cross-Platform Build System"
	@echo "======================================"
	@echo ""
	@echo "Detected OS: $(DETECTED_OS)"
	@echo ""
	@echo "Available targets:"
	@echo "  all        - Build the project (default)"
	@echo "  debug      - Build with debug flags"
	@echo "  release    - Build with release optimization"
	@echo "  run        - Build and run the executable"
	@echo "  test-run   - Build and run with diagnostic information"
	@echo "  clean      - Remove build files"
	@echo "  setup-deps - Install/show dependency installation commands"
	@echo "  install    - Install executable to system (Unix-like only)"
	@echo "  help       - Show this help message"
	@echo ""
	@echo "Build modes:"
	@echo "  make DEBUG=1  - Debug build"
	@echo "  make          - Release build"
	@echo ""
	@echo "Platform-specific notes:"
ifeq ($(DETECTED_OS),Windows)
	@echo "  Windows: Requires MinGW/MSYS2 with SFML"
	@echo "  SFML Path: $(SFML_DIR)"
else ifeq ($(DETECTED_OS),Darwin)
	@echo "  macOS: Uses Homebrew SFML installation"
	@echo "  SFML Path: $(SFML_DIR)"
else
	@echo "  Linux: Uses system SFML installation"
	@echo "  Detected via pkg-config: $(if $(SFML_CFLAGS),Yes,No)"
endif

# Show current configuration
config:
	@echo "Build Configuration:"
	@echo "==================="
	@echo "OS: $(DETECTED_OS)"
	@echo "Compiler: $(CXX)"
	@echo "Flags: $(CXXFLAGS)"
	@echo "Includes: $(INCLUDES)"
	@echo "Libraries: $(LIBS)"
	@echo "Target: $(TARGET)"
	@echo "Sources: $(words $(SOURCES)) files"
