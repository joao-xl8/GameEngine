# Viewport System - Aspect Ratio Preservation

The viewport system maintains the game's native aspect ratio by adding black bars (letterboxing/pillarboxing) when the window size doesn't match the game's resolution.

## How It Works

### **Native Game Resolution**
```cpp
int m_gameWidth = 1280;   // Native game resolution width
int m_gameHeight = 720;   // Native game resolution height (16:9 aspect ratio)
```

### **Automatic Scaling**
The system automatically:
1. **Calculates aspect ratios** of both game and window
2. **Determines scaling method** (letterbox vs pillarbox)
3. **Adds black bars** to maintain aspect ratio
4. **Centers the game view** in the window

## Visual Examples

### **Letterboxing** (Window wider than game)
```
┌─────────────────────────────────┐
│ ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■ │ ← Black bar (top)
│                                 │
│         GAME CONTENT            │
│         (1280x720)              │
│                                 │
│ ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■ │ ← Black bar (bottom)
└─────────────────────────────────┘
```

### **Pillarboxing** (Window taller than game)
```
┌─────────────────────┐
│ ■■■  GAME CONTENT  ■■■ │
│ ■■■  (1280x720)   ■■■ │
│ ■■■                ■■■ │
│ ■■■                ■■■ │
│ ■■■                ■■■ │
└─────────────────────┘
  ↑                    ↑
Black bars          Black bars
(left)              (right)
```

## Configuration

### **Change Game Resolution**
```cpp
// For different aspect ratios:
int m_gameWidth = 1920;   // 16:9 HD
int m_gameHeight = 1080;

int m_gameWidth = 1280;   // 16:10
int m_gameHeight = 800;

int m_gameWidth = 1024;   // 4:3 Classic
int m_gameHeight = 768;

int m_gameWidth = 800;    // Square-ish
int m_gameHeight = 600;
```

### **Common Resolutions**
```cpp
// Classic 4:3
m_gameWidth = 640, m_gameHeight = 480    // VGA
m_gameWidth = 800, m_gameHeight = 600    // SVGA
m_gameWidth = 1024, m_gameHeight = 768   // XGA

// Widescreen 16:9
m_gameWidth = 1280, m_gameHeight = 720   // HD
m_gameWidth = 1920, m_gameHeight = 1080  // Full HD
m_gameWidth = 854, m_gameHeight = 480    // WVGA

// Widescreen 16:10
m_gameWidth = 1280, m_gameHeight = 800   // WXGA
m_gameWidth = 1440, m_gameHeight = 900   // WXGA+
```

## Features

### **Automatic Viewport Calculation**
```cpp
void GameEngine::calculateViewport()
{
    // Calculates optimal viewport size and position
    // Maintains aspect ratio regardless of window size
    // Centers game content in window
    // Adds appropriate black bars
}
```

### **Dynamic Resizing**
- **Window Resize**: Viewport recalculates automatically
- **Fullscreen Toggle**: Maintains aspect ratio in fullscreen
- **Real-time Updates**: No restart required

### **Perfect Pixel Scaling**
- Game renders at native resolution
- Clean scaling without distortion
- Crisp pixel art preservation
- No stretching or squashing

## Benefits

### **1. Consistent Experience**
- Game looks identical on all screen sizes
- UI elements maintain proper proportions
- Text remains readable at all resolutions
- Gameplay area stays consistent

### **2. Professional Appearance**
- Clean black bars like console games
- No stretched or distorted graphics
- Maintains artistic vision
- Looks polished and intentional

### **3. Multi-Platform Ready**
- Works on any screen resolution
- Handles ultrawide monitors gracefully
- Perfect for different aspect ratios
- Future-proof design

### **4. Performance Optimized**
- Game renders at fixed resolution
- No dynamic resolution changes
- Consistent performance across devices
- Predictable memory usage

## Implementation Details

### **Viewport Setup**
```cpp
// Game view setup
m_gameView.setSize(m_gameWidth, m_gameHeight);
m_gameView.setCenter(m_gameWidth/2, m_gameHeight/2);

// Viewport calculation
sf::FloatRect viewport(
    viewportX / windowWidth,
    viewportY / windowHeight,
    viewportWidth / windowWidth,
    viewportHeight / windowHeight
);
m_gameView.setViewport(viewport);
```

### **Rendering Pipeline**
```cpp
// 1. Clear entire window (black)
m_window.clear(sf::Color::Black);

// 2. Set game view
m_window.setView(m_gameView);

// 3. Render game content
scene->update();

// 4. Reset to default view
m_window.setView(m_window.getDefaultView());

// 5. Draw black bars
m_window.draw(letterboxBars);
```

### **Event Handling**
```cpp
// Window resize events
if (event.type == sf::Event::Resized) {
    calculateViewport(); // Recalculate on resize
}

// Fullscreen toggle
void toggleFullscreen() {
    // ... change window mode ...
    calculateViewport(); // Recalculate for new mode
}
```

## Usage Examples

### **Classic Arcade Style**
```cpp
m_gameWidth = 320;    // 4:3 aspect ratio
m_gameHeight = 240;   // Pixel-perfect retro feel
```

### **Modern Indie Game**
```cpp
m_gameWidth = 1280;   // 16:9 aspect ratio
m_gameHeight = 720;   // HD resolution
```

### **Mobile-Style Portrait**
```cpp
m_gameWidth = 480;    // 3:4 aspect ratio
m_gameHeight = 640;   // Portrait orientation
```

## Troubleshooting

### **Game Appears Too Small**
- Increase `m_gameWidth` and `m_gameHeight`
- Maintain aspect ratio when scaling

### **Black Bars Too Large**
- Window aspect ratio differs significantly from game
- Consider changing game resolution to match common displays

### **Performance Issues**
- Game resolution affects rendering performance
- Lower resolution = better performance
- Higher resolution = better visual quality

## Customization

### **Different Bar Colors**
```cpp
// Change from black to other colors
m_letterboxTop.setFillColor(sf::Color::Blue);    // Blue bars
m_letterboxTop.setFillColor(sf::Color(32,32,32)); // Dark gray bars
```

### **Custom Aspect Ratios**
```cpp
// Ultra-wide support
m_gameWidth = 2560;   // 21:9 aspect ratio
m_gameHeight = 1080;

// Square games
m_gameWidth = 800;    // 1:1 aspect ratio
m_gameHeight = 800;
```

This viewport system ensures your game maintains its intended visual appearance across all devices and screen sizes, just like professional console games! 🎮📺✨
