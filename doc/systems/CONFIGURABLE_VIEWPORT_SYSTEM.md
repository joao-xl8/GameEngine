# Configurable Viewport System

A flexible, maintainable viewport system that removes letterboxing and provides configurable scaling modes for different resolutions and display preferences.

## Overview

The Configurable Viewport System replaces the old fixed letterboxing system with a flexible configuration-based approach that supports:
- **Multiple scaling modes** (Fill Window, Maintain Aspect, Stretch)
- **Runtime resolution changes** without restart
- **Configurable zoom levels** and game view sizes
- **No black bars** by default (FILL_WINDOW mode)
- **Easy integration** with configuration menus

## Architecture

### Core Components

**ViewportConfig Structure**
```cpp
struct ViewportConfig {
    // Resolution settings
    int windowWidth = 800;
    int windowHeight = 640;
    
    // Game view settings
    int gameWidth = 600;
    int gameHeight = 800;
    float zoomFactor = 0.9f;
    
    // Scaling mode
    enum ScalingMode {
        STRETCH,        // Stretch to fill window (no aspect ratio preservation)
        MAINTAIN_ASPECT, // Maintain aspect ratio with letterboxing
        FILL_WINDOW     // Fill entire window (may crop content)
    } scalingMode = FILL_WINDOW;
    
    // Background color (for letterboxing if used)
    sf::Color backgroundColor = sf::Color::Black;
};
```

**GameEngine Integration**
- Centralized viewport configuration management
- Runtime configuration changes
- Automatic viewport recalculation
- Clean separation of concerns

## Scaling Modes

### 1. FILL_WINDOW (Default)
- **Behavior**: Game view fills entire window
- **Pros**: No black bars, uses full screen real estate
- **Cons**: May crop content at different aspect ratios
- **Best for**: Games designed to handle variable aspect ratios

### 2. MAINTAIN_ASPECT
- **Behavior**: Preserves game aspect ratio with letterboxing
- **Pros**: Consistent game view proportions
- **Cons**: Black bars on sides or top/bottom
- **Best for**: Games requiring exact aspect ratios

### 3. STRETCH
- **Behavior**: Stretches game view to fill window
- **Pros**: Uses entire window, no cropping
- **Cons**: May distort graphics
- **Best for**: UI-heavy games or specific design requirements

## Usage

### Basic Configuration
```cpp
// Get current configuration
ViewportConfig& config = gameEngine.getViewportConfig();

// Modify settings
config.windowWidth = 1920;
config.windowHeight = 1080;
config.scalingMode = ViewportConfig::FILL_WINDOW;
config.zoomFactor = 1.0f;

// Apply changes
gameEngine.setViewportConfig(config);
```

### Individual Setting Changes
```cpp
// Change resolution
gameEngine.setResolution(1280, 720);

// Change game view size
gameEngine.setGameViewSize(800, 600);

// Change scaling mode
gameEngine.setScalingMode(ViewportConfig::MAINTAIN_ASPECT);

// Change zoom
gameEngine.setZoom(1.2f);
```

### Runtime Updates
```cpp
// Update viewport after changes
gameEngine.updateViewport();
```

## Configuration Menu Integration

### Scene_ViewportConfig
A complete configuration scene that provides:
- **Resolution selection** from predefined options
- **Scaling mode selection** with live preview
- **Zoom factor adjustment** with real-time feedback
- **Apply/Reset functionality** for testing changes
- **Current configuration display** for reference

### Menu Integration
```cpp
// Add to main menu
m_menuStrings.push_back("Viewport Config");

// Handle selection
if (selectedOption == "Viewport Config") {
    m_game->changeScene("ViewportConfig", 
        std::make_shared<Scene_ViewportConfig>(m_game));
}
```

## Implementation Details

### Viewport Calculation
```cpp
void GameEngine::calculateViewport() {
    // Get window and game dimensions
    sf::Vector2u windowSize = m_window.getSize();
    float scaledWidth = m_viewportConfig.gameWidth * m_viewportConfig.zoomFactor;
    float scaledHeight = m_viewportConfig.gameHeight * m_viewportConfig.zoomFactor;
    
    // Apply scaling mode
    switch (m_viewportConfig.scalingMode) {
        case ViewportConfig::FILL_WINDOW:
            // Fill entire window (no letterboxing)
            m_gameView.setViewport(sf::FloatRect(0.0f, 0.0f, 1.0f, 1.0f));
            break;
            
        case ViewportConfig::MAINTAIN_ASPECT:
            // Calculate letterboxing viewport
            // ... aspect ratio calculations
            break;
            
        case ViewportConfig::STRETCH:
            // Stretch to fill (may distort)
            m_gameView.setViewport(sf::FloatRect(0.0f, 0.0f, 1.0f, 1.0f));
            break;
    }
    
    m_gameView.setSize(scaledWidth, scaledHeight);
    m_gameView.setCenter(scaledWidth / 2.0f, scaledHeight / 2.0f);
}
```

### Render Loop Changes
```cpp
void GameEngine::update() {
    // Clear with configurable background color
    m_window.clear(m_viewportConfig.backgroundColor);
    
    // Set game view (no letterbox drawing needed)
    m_window.setView(m_gameView);
    
    // Render scene content
    currentScene()->update();
    
    // Reset for UI
    m_window.setView(m_window.getDefaultView());
    ImGui::SFML::Render(m_window);
    m_window.display();
}
```

## Benefits Over Old System

### Removed Complexity
- ❌ **No letterbox rectangles** to manage and draw
- ❌ **No complex letterbox positioning** calculations
- ❌ **No separate letterbox rendering** code
- ❌ **No hardcoded aspect ratio** assumptions

### Added Flexibility
- ✅ **Multiple scaling modes** for different use cases
- ✅ **Runtime configuration** without restart
- ✅ **Easy resolution changes** with automatic viewport update
- ✅ **Configurable background colors** when letterboxing is used
- ✅ **Clean separation** of viewport logic

### Improved Maintainability
- ✅ **Centralized configuration** in ViewportConfig struct
- ✅ **Simple API** for common operations
- ✅ **Easy testing** of different configurations
- ✅ **Future-proof design** for new scaling modes

## Configuration Examples

### Full Screen Gaming Setup
```cpp
ViewportConfig config;
config.windowWidth = 1920;
config.windowHeight = 1080;
config.scalingMode = ViewportConfig::FILL_WINDOW;
config.zoomFactor = 1.0f;
gameEngine.setViewportConfig(config);
```

### Retro Gaming Setup
```cpp
ViewportConfig config;
config.windowWidth = 800;
config.windowHeight = 600;
config.scalingMode = ViewportConfig::MAINTAIN_ASPECT;
config.zoomFactor = 0.8f;
config.backgroundColor = sf::Color(20, 20, 20);
gameEngine.setViewportConfig(config);
```

### Mobile-Style Setup
```cpp
ViewportConfig config;
config.windowWidth = 480;
config.windowHeight = 854;
config.scalingMode = ViewportConfig::STRETCH;
config.zoomFactor = 1.2f;
gameEngine.setViewportConfig(config);
```

## Future Enhancements

### Planned Features
- **Configuration file persistence** - Save/load viewport settings
- **Multi-monitor support** - Handle multiple displays
- **Dynamic resolution detection** - Auto-detect optimal settings
- **Custom scaling modes** - User-defined scaling algorithms
- **Performance optimization** - Viewport caching and smart updates

### Advanced Configuration
- **Per-scene viewport settings** - Different settings per game scene
- **Animated transitions** - Smooth viewport changes
- **Aspect ratio constraints** - Min/max aspect ratio limits
- **Resolution validation** - Ensure supported resolutions

## Migration from Old System

### Removed Components
- `m_letterboxTop`, `m_letterboxBottom`, `m_letterboxLeft`, `m_letterboxRight`
- Complex letterbox positioning calculations
- Letterbox drawing in render loop
- Hardcoded game dimensions

### New Components
- `ViewportConfig` structure
- Configurable scaling modes
- Runtime configuration methods
- Configuration menu scene

### Code Changes Required
```cpp
// Old way
m_gameWidth = 600;
m_gameHeight = 800;
calculateViewport(); // Complex letterbox calculations

// New way
ViewportConfig config;
config.gameWidth = 600;
config.gameHeight = 800;
config.scalingMode = ViewportConfig::FILL_WINDOW;
gameEngine.setViewportConfig(config);
```

The Configurable Viewport System provides a modern, flexible approach to viewport management that eliminates the complexity of letterboxing while providing powerful configuration options for different display scenarios.
