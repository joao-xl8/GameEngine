# Centered Viewport System with Minimum Size Enforcement

The viewport system now ensures the game view is always perfectly centered in the window and enforces a minimum window size for optimal gameplay experience.

## Key Features

### **1. Perfect Centering**
The game view is always centered in the middle of the screen, regardless of window size or aspect ratio.

### **2. Minimum Size Enforcement**
- **Minimum Width**: 480 pixels
- **Minimum Height**: 640 pixels
- **Prevents**: Window from becoming too small to be playable

### **3. Automatic Letterboxing/Pillarboxing**
- **Letterboxing**: Black bars on top/bottom when window is too wide
- **Pillarboxing**: Black bars on left/right when window is too tall
- **Always Centered**: Game content stays in the exact center

## Visual Examples

### **Wide Window (Pillarboxing)**
```
┌─────────────────────────────────────┐
│ ■■■■    GAME CONTENT     ■■■■ │
│ ■■■■    (1280x720)       ■■■■ │
│ ■■■■    CENTERED         ■■■■ │
│ ■■■■                     ■■■■ │
└─────────────────────────────────────┘
  ↑                           ↑
Black bars                Black bars
(left)                    (right)
```

### **Tall Window (Letterboxing)**
```
┌─────────────────────────────┐
│ ■■■■■■■■■■■■■■■■■■■■■■■■■■■ │ ← Black bar (top)
│                             │
│      GAME CONTENT           │
│      (1280x720)             │
│      CENTERED               │
│                             │
│ ■■■■■■■■■■■■■■■■■■■■■■■■■■■ │ ← Black bar (bottom)
└─────────────────────────────┘
```

## Implementation Details

### **Minimum Size Enforcement**

#### **Window Creation**
```cpp
// Enforce minimum size at creation
int windowWidth = std::max(800, 480);   // At least 480px wide
int windowHeight = std::max(600, 640);  // At least 640px tall
m_window.create(sf::VideoMode(windowWidth, windowHeight), "Game Engine", sf::Style::Default);
```

#### **Resize Event Handling**
```cpp
if (event.type == sf::Event::Resized) {
    // Check if window is too small
    sf::Vector2u currentSize = m_window.getSize();
    sf::Vector2u newSize = currentSize;
    bool needsResize = false;
    
    if (currentSize.x < 480) {
        newSize.x = 480;
        needsResize = true;
    }
    if (currentSize.y < 640) {
        newSize.y = 640;
        needsResize = true;
    }
    
    // Resize to minimum if needed
    if (needsResize) {
        m_window.setSize(newSize);
    }
    
    // Recalculate centered viewport
    calculateViewport();
}
```

### **Perfect Centering Algorithm**

#### **Horizontal Centering (Pillarboxing)**
```cpp
if (windowAspect > gameAspect) {
    // Window is wider than game
    viewportHeight = windowHeight;                    // Fill height
    viewportWidth = windowHeight * gameAspect;        // Calculate width
    viewportX = (windowWidth - viewportWidth) / 2.0f; // Center horizontally
    viewportY = 0.0f;                                 // Top aligned
}
```

#### **Vertical Centering (Letterboxing)**
```cpp
else {
    // Window is taller than game
    viewportWidth = windowWidth;                       // Fill width
    viewportHeight = windowWidth / gameAspect;         // Calculate height
    viewportX = 0.0f;                                  // Left aligned
    viewportY = (windowHeight - viewportHeight) / 2.0f; // Center vertically
}
```

#### **Normalized Viewport Coordinates**
```cpp
sf::FloatRect viewport(
    viewportX / windowWidth,        // Left edge (0.0 to 1.0)
    viewportY / windowHeight,       // Top edge (0.0 to 1.0)
    viewportWidth / windowWidth,    // Width (0.0 to 1.0)
    viewportHeight / windowHeight   // Height (0.0 to 1.0)
);
m_gameView.setViewport(viewport);
```

## Configuration Options

### **Minimum Size Customization**
```cpp
// Current settings
const int MIN_WIDTH = 480;   // Minimum window width
const int MIN_HEIGHT = 640;  // Minimum window height

// For different game types:
// Mobile-style: 320x480
// Classic: 640x480  
// Modern: 800x600
// HD: 1280x720
```

### **Game Resolution**
```cpp
// Current game resolution (maintains aspect ratio)
int m_gameWidth = 1280;   // Native game width
int m_gameHeight = 720;   // Native game height (16:9 aspect)

// Other common resolutions:
// 4:3 Classic: 1024x768
// 16:10 Wide: 1280x800
// Square: 800x800
```

## Benefits

### **1. Consistent User Experience**
- Game always appears in the center of the screen
- No awkward positioning or off-center content
- Professional appearance like commercial games

### **2. Playability Assurance**
- Minimum size prevents unusably small windows
- UI elements remain readable and clickable
- Game controls stay accessible

### **3. Visual Quality**
- Perfect aspect ratio preservation
- No stretching or distortion of game content
- Clean black bars that look intentional

### **4. Cross-Platform Compatibility**
- Works on any screen size or resolution
- Handles different monitor aspect ratios
- Consistent experience across devices

## Usage Examples

### **Different Window Scenarios**

#### **Square Window (1000x1000)**
- Game centered with pillarboxes on left/right
- Perfect vertical centering
- Game content maintains 16:9 aspect ratio

#### **Ultra-wide Window (2560x1080)**
- Game centered with large pillarboxes
- Content stays readable and properly sized
- Professional letterboxing appearance

#### **Portrait Window (800x1200)**
- Game centered with letterboxes top/bottom
- Horizontal centering maintained
- Optimal use of available space

#### **Minimum Size Window (480x640)**
- Game fills available space optimally
- All UI elements remain accessible
- Maintains playability at smallest size

## Technical Specifications

### **Viewport Calculation**
- **Precision**: Float-based calculations for smooth scaling
- **Centering**: Mathematical center calculation for perfect alignment
- **Aspect Ratio**: Preserved using precise ratio calculations
- **Normalization**: SFML viewport uses 0.0-1.0 coordinate system

### **Performance**
- **Calculation**: Only runs on window resize events
- **Rendering**: No performance impact during gameplay
- **Memory**: Minimal overhead for letterbox rectangles
- **Efficiency**: Optimized viewport updates

### **Compatibility**
- **SFML**: Uses standard SFML viewport system
- **Cross-Platform**: Works on Windows, macOS, Linux
- **Graphics Cards**: Compatible with all modern GPUs
- **Monitors**: Supports any resolution or aspect ratio

## Troubleshooting

### **Game Appears Too Small**
- Increase minimum window size values
- Check if game resolution matches intended size
- Verify viewport calculations are correct

### **Black Bars Too Large**
- Window aspect ratio differs significantly from game
- Consider adjusting game resolution to match common displays
- Normal behavior for extreme aspect ratios

### **Centering Issues**
- Check viewport calculation math
- Verify window size detection is accurate
- Ensure SFML view is set correctly

This centered viewport system provides a professional, polished experience that automatically adapts to any screen while maintaining perfect visual quality! 🎮📺✨
