# Grid Movement System

The grid movement system allows you to create classic tile-based movement like old-school RPGs and puzzle games.

## How It Works

The `CGridMovement` component handles:
- **Grid Positioning**: Keeps track of logical grid coordinates
- **Smooth Interpolation**: Optional smooth movement between grid positions
- **Movement Timing**: Controls how fast the player moves between tiles
- **Input Handling**: Processes discrete key presses instead of continuous input

## Configuration Options

### Basic Setup
```cpp
// 32px grid, 4 moves per second, smooth interpolation
auto gridMovement = std::make_shared<CGridMovement>(32, 4.0f, true);
```

### Different Movement Styles

#### 1. Classic RPG Style (Smooth)
```cpp
auto gridMovement = std::make_shared<CGridMovement>(32, 3.0f, true);
// Smooth movement, moderate speed, feels like classic JRPGs
```

#### 2. Puzzle Game Style (Instant)
```cpp
auto gridMovement = std::make_shared<CGridMovement>(32, 8.0f, false);
// Instant snapping, fast response, good for puzzle games
```

#### 3. Retro Platformer Style
```cpp
auto gridMovement = std::make_shared<CGridMovement>(16, 6.0f, true);
// Smaller grid, faster movement, smooth interpolation
```

#### 4. Turn-Based Style
```cpp
auto gridMovement = std::make_shared<CGridMovement>(32, 2.0f, true);
// Slower, deliberate movement for turn-based games
```

## Key Features

### Input Handling
- Uses **key press events** instead of held keys
- Player moves one tile per key press
- No diagonal movement (can be added if needed)
- Prevents movement while already moving

### Grid Snapping
```cpp
// Snap player to nearest grid position on spawn
gridMovement->snapToGrid(startPos);
```

### Boundary Checking
You can add boundary checking in the `startMove` method:
```cpp
// In CGridMovement::startMove()
if (newGridPos.x < 0 || newGridPos.x >= mapWidth || 
    newGridPos.y < 0 || newGridPos.y >= mapHeight) return false;
```

### Animation Integration
- Animations play when movement starts
- Idle animation plays when movement stops
- Works seamlessly with the existing animation system

## Usage Examples

### Player Setup
```cpp
// Add grid movement to player
auto gridMovement = std::make_shared<CGridMovement>(32, 4.0f, true);
gridMovement->snapToGrid(startPos);
m_player->addComponent<CGridMovement>(gridMovement);
```

### Different Grid Sizes
```cpp
// Large tiles (64x64) - good for strategy games
auto gridMovement = std::make_shared<CGridMovement>(64, 2.0f, true);

// Small tiles (16x16) - good for detailed pixel art games
auto gridMovement = std::make_shared<CGridMovement>(16, 8.0f, true);
```

### Movement Speed Variations
```cpp
// Slow and deliberate
auto slowMovement = std::make_shared<CGridMovement>(32, 1.5f, true);

// Fast and responsive
auto fastMovement = std::make_shared<CGridMovement>(32, 8.0f, true);

// Instant (no interpolation)
auto instantMovement = std::make_shared<CGridMovement>(32, 10.0f, false);
```

## Benefits

1. **Precise Movement**: Player always aligns perfectly with the grid
2. **Classic Feel**: Recreates the movement style of retro games
3. **Collision-Friendly**: Easy to implement tile-based collision detection
4. **Predictable**: Player position is always known and controlled
5. **Flexible**: Can be configured for different game styles

## Integration with Other Systems

### Collision Detection
```cpp
// Check if target grid position is valid before moving
bool isValidPosition(Vec2 gridPos) {
    // Check against tile map, walls, other entities, etc.
    return !isWall(gridPos) && !isOccupied(gridPos);
}
```

### Map Integration
```cpp
// Convert between grid coordinates and tile map indices
int tileIndex = gridPos.y * mapWidth + gridPos.x;
```

### Entity Interaction
```cpp
// Check for entities at target grid position
auto entitiesAtPosition = getEntitiesAtGridPosition(targetGridPos);
```

This system provides the foundation for classic tile-based gameplay while maintaining modern smooth visuals!
