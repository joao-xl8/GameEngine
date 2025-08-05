# Makefile for GameEngine
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wunused-result
TARGET = build.out

# Directories
IMGUI_DIR = include/imgui
SFML_PREFIX = /opt/homebrew/opt/sfml@2
GLFW_DIR = include/GLFW
ROOT_DIR = src

# Include paths
INCLUDES = -I$(SFML_PREFIX)/include -I$(IMGUI_DIR) -I$(GLFW_DIR) -I$(ROOT_DIR)

# Library paths and libraries
LIBS = -L$(SFML_PREFIX)/lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -framework OpenGL

# Source files
SOURCES = main.cpp $(wildcard $(IMGUI_DIR)/imgui*.cpp) $(wildcard $(ROOT_DIR)/*.cpp)

# Default target
all: $(TARGET)

$(TARGET): $(SOURCES)
	@echo "Building GameEngine..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SOURCES) $(LIBS) -o $(TARGET)
	@echo "Build complete! Executable: $(TARGET)"

run: $(TARGET)
	@echo "Running GameEngine..."
	./$(TARGET)

clean:
	@echo "Cleaning build files..."
	rm -f $(TARGET)

rebuild: clean all

.PHONY: all run clean rebuild
