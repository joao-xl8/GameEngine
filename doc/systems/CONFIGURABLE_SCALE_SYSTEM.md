# Configurable Scale System

The game now uses a single configurable scale variable (`m_gameScale`) that ensures all game elements are consistently sized.

## How It Works

### Single Scale Variable
```cpp
const int m_gameScale = 64; // Change this value to scale everything consistently
```

This variable controls:
- **Tile Size**: Ground tiles, walls, decorations
- **Player Size**: Sprite frames, bounding box, movement grid
- **Grid Movement**: Movement increments
- **Collision Detection**: Bounding box sizes

## Configuration Options

### Option 1: Large Scale (64x64) - Current Default
```cpp
const int m_gameScale = 64;
```
- **Best for**: Detailed sprites, modern displays
- **Tile size**: 64x64 pixels
- **Player size**: 64x64 pixels
- **Movement**: 64 pixel increments
- **Visual**: Larger, more detailed sprites

### Option 2: Classic Scale (32x32)
```cpp
const int m_gameScale = 32;
```
- **Best for**: Retro feel, pixel art games
- **Tile size**: 32x32 pixels
- **Player size**: 32x32 pixels
- **Movement**: 32 pixel increments
- **Visual**: Classic 8-bit/16-bit game feel

### Option 3: Small Scale (16x16)
```cpp
const int m_gameScale = 16;
```
- **Best for**: Detailed maps, strategy games
- **Tile size**: 16x16 pixels
- **Player size**: 16x16 pixels
- **Movement**: 16 pixel increments
- **Visual**: Tiny sprites, more content on screen

### Option 4: Extra Large Scale (128x128)
```cpp
const int m_gameScale = 128;
```
- **Best for**: Mobile games, accessibility
- **Tile size**: 128x128 pixels
- **Player size**: 128x128 pixels
- **Movement**: 128 pixel increments
- **Visual**: Very large, easy to see sprites

## What Gets Scaled

### Automatically Scaled Elements
1. **Tile System**
   ```cpp
   const Vec2 m_tileSize = {static_cast<float>(m_gameScale), static_cast<float>(m_gameScale)};
   ```

2. **Player Animation Frames**
   ```cpp
   auto animationComponent = std::make_shared<CAnimation>(Vec2{static_cast<float>(m_gameScale), static_cast<float>(m_gameScale)});
   ```

3. **Grid Movement**
   ```cpp
   auto gridMovement = std::make_shared<CGridMovement>(m_gameScale, 3.0f, true);
   ```

4. **Player Bounding Box**
   ```cpp
   m_player->addComponent<CBoundingBox>(std::make_shared<CBoundingBox>(Vec2{static_cast<float>(m_gameScale), static_cast<float>(m_gameScale)}));
   ```

## Benefits

### 1. **Consistent Sizing**
- Player and tiles are always the same size
- No more 1/4 size character in full-size tile
- Perfect grid alignment

### 2. **Easy Configuration**
- Change one variable to scale entire game
- No need to update multiple files
- Consistent across all game elements

### 3. **Flexible Game Styles**
```cpp
// Retro 8-bit style
const int m_gameScale = 16;

// Classic 16-bit style  
const int m_gameScale = 32;

// Modern pixel art style
const int m_gameScale = 64;

// Mobile-friendly style
const int m_gameScale = 128;
```

### 4. **Asset Compatibility**
- Works with any sprite sheet frame size
- Scales sprites to match chosen game scale
- Maintains aspect ratios

## Usage Examples

### For Different Game Types

#### **Classic RPG Style**
```cpp
const int m_gameScale = 32;
// Movement speed: 4.0f (moderate)
// Perfect for: Zelda-like games, JRPGs
```

#### **Modern Indie Game Style**
```cpp
const int m_gameScale = 64;
// Movement speed: 3.0f (smooth)
// Perfect for: Modern pixel art games
```

#### **Strategy Game Style**
```cpp
const int m_gameScale = 24;
// Movement speed: 6.0f (fast)
// Perfect for: Top-down strategy, more units on screen
```

#### **Mobile Game Style**
```cpp
const int m_gameScale = 96;
// Movement speed: 2.5f (deliberate)
// Perfect for: Touch controls, easy visibility
```

## Implementation Details

### Automatic Scaling
The system automatically scales:
- Tile positioning in level files
- Player spawn positions
- Collision detection boundaries
- Animation frame sizes
- Grid movement increments

### Manual Adjustments
You may need to adjust:
- Movement speed (`3.0f` in CGridMovement constructor)
- Animation frame counts (if using different sprite sheets)
- Level file coordinates (if changing scale significantly)

## Migration Guide

### Changing Scale
1. **Update the scale variable**:
   ```cpp
   const int m_gameScale = 32; // Your desired scale
   ```

2. **Adjust movement speed if needed**:
   ```cpp
   auto gridMovement = std::make_shared<CGridMovement>(m_gameScale, 4.0f, true);
   //                                                              ^^^^ Adjust this
   ```

3. **Test collision detection** - should work automatically

4. **Check sprite visibility** - larger scales show more detail, smaller scales fit more on screen

### Common Scale Values
- **16**: Very small, lots of content
- **24**: Small, good for strategy games  
- **32**: Classic retro game size
- **48**: Medium, good balance
- **64**: Large, modern pixel art
- **96**: Very large, mobile-friendly
- **128**: Extra large, accessibility

This system ensures your character and environment are always perfectly matched in size! 🎮✨
