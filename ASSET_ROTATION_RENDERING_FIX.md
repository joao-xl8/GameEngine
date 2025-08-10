# Asset Rotation Rendering Fix

## Issue Identified
Based on the logs provided, the rotation placement calculations were working correctly:
- Rotation values were being stored properly (0°, 90°, 180°, 270°)
- Placement positions were calculated correctly (-4,0), (0,-4), etc.
- Grid preview was showing correct rotated dimensions

However, **the placed assets were not visually rotating** - they appeared in their original orientation despite having the correct rotation values stored.

## Root Cause
The problem was in the multi-cell asset rendering logic in the `drawGrid()` function:

### 1. **Incorrect Scaling Calculation**
```cpp
// WRONG: Used rotated dimensions for scaling
int renderWidth = props.width;
int renderHeight = props.height;
if (cell.rotation == 90.0f || cell.rotation == 270.0f) {
    std::swap(renderWidth, renderHeight);  // This was wrong!
}
float scaleX = static_cast<float>(renderWidth * TILE_SIZE) / textureSize.x;
float scaleY = static_cast<float>(renderHeight * TILE_SIZE) / textureSize.y;
```

### 2. **Incorrect Positioning Calculation**
```cpp
// WRONG: Used rotated dimensions for center positioning
float centerX = x * TILE_SIZE + (renderWidth * TILE_SIZE) / 2.0f;
float centerY = y * TILE_SIZE + (renderHeight * TILE_SIZE) / 2.0f;
```

## Solution Applied

### 1. **Fixed Scaling**
```cpp
// CORRECT: Always use original dimensions for scaling
float scaleX = static_cast<float>(props.width * TILE_SIZE) / textureSize.x;
float scaleY = static_cast<float>(props.height * TILE_SIZE) / textureSize.y;
sprite.setScale(scaleX, scaleY);
```

### 2. **Fixed Positioning**
```cpp
// CORRECT: Use original dimensions for center calculation
float centerX = x * TILE_SIZE + (props.width * TILE_SIZE) / 2.0f;
float centerY = y * TILE_SIZE + (props.height * TILE_SIZE) / 2.0f;
sprite.setPosition(centerX, centerY);
```

## Why This Works

1. **Scaling**: The sprite should always be scaled based on the original asset dimensions (1x5 for TallTower). The rotation is handled separately by `setRotation()`.

2. **Positioning**: The center position should be calculated using the original dimensions because:
   - The placement position `(x, y)` is already correctly calculated by `calculateRotatedPlacement()`
   - We just need to find the center of the original asset area at that position
   - The rotation around that center point is handled by SFML's rotation system

3. **Rotation**: The `sprite.setRotation(cell.rotation)` call handles the actual visual rotation around the center origin.

## Expected Behavior After Fix

When placing a TallTower (1x5) at position (0,0) with different rotations:

- **0°**: Vertical tower from (0,0) to (0,4)
- **90°**: Horizontal tower from (-4,0) to (0,0) - rotated clockwise
- **180°**: Vertical tower from (0,-4) to (0,0) - upside down
- **270°**: Horizontal tower from (0,0) to (4,0) - rotated counter-clockwise

The asset should now **visually rotate** to match the grid preview and placement calculations.

## Files Modified
- `src/scenes/scene_map_editor.cpp`: Fixed multi-cell asset rendering logic

## Testing
1. Place a TallTower at (0,0) with 0° rotation - should appear vertical
2. Rotate to 90° and place - should appear horizontal
3. Continue testing all rotations to verify visual rotation matches placement
