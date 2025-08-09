# Asset Tile Pattern - 64x64 Pixels Per Tile

## Overview
All assets in the game engine follow a strict **64x64 pixel per tile** pattern. This ensures consistent scaling, proper alignment, and seamless integration with the map editor's grid system.

## Tile Size Standard
- **Base Tile Size**: 64×64 pixels
- **Grid Alignment**: All assets align perfectly with the 64-pixel grid
- **Scaling Consistency**: Maintains uniform appearance across all assets

## Asset Dimensions

### Single-Cell Assets (1×1 = 64×64 pixels)
- **Ground**: 64×64 pixels ✓
- **Wall**: 64×64 pixels ✓
- **Bush**: 64×64 pixels ✓
- **SavePoint**: 64×64 pixels ✓
- **PlayerSpawn**: 64×64 pixels ✓

### L-Shaped Multi-Cell Assets
- **Misc1** (2×3 tiles): **128×192 pixels** ✓
  - Width: 2 tiles × 64px = 128px
  - Height: 3 tiles × 64px = 192px
  - Pattern: L-shaped brown/wooden structure

- **Misc2** (3×2 tiles): **192×128 pixels** ✓
  - Width: 3 tiles × 64px = 192px
  - Height: 2 tiles × 64px = 128px
  - Pattern: L-shaped gray/stone structure

### Large Test Assets
- **LargeCastle** (5×5 tiles): **320×320 pixels** ✓
  - Width: 5 tiles × 64px = 320px
  - Height: 5 tiles × 64px = 320px
  - Pattern: Square castle with towers and central keep

### Extreme Aspect Ratio Assets
- **WideWall** (5×1 tiles): **320×64 pixels** ✓
  - Width: 5 tiles × 64px = 320px
  - Height: 1 tile × 64px = 64px
  - Pattern: Horizontal brick wall

- **TallTower** (1×5 tiles): **64×320 pixels** ✓
  - Width: 1 tile × 64px = 64px
  - Height: 5 tiles × 64px = 320px
  - Pattern: Vertical stone tower

## Calculation Formula
For any multi-cell asset:
```
Asset Width (pixels) = Tile Width × 64
Asset Height (pixels) = Tile Height × 64
```

### Examples
- **2×3 asset**: 2×64 = 128px wide, 3×64 = 192px tall
- **5×5 asset**: 5×64 = 320px wide, 5×64 = 320px tall
- **5×1 asset**: 5×64 = 320px wide, 1×64 = 64px tall
- **1×5 asset**: 1×64 = 64px wide, 5×64 = 320px tall

## Benefits of 64×64 Pattern

### 🎯 Perfect Grid Alignment
- Assets align exactly with the map editor's 64-pixel grid
- No sub-pixel positioning issues
- Clean, crisp rendering at all zoom levels

### 🔧 Consistent Scaling
- All assets scale uniformly in the game engine
- Preview system calculations are accurate
- Multi-cell assets maintain proper proportions

### 🎨 Visual Consistency
- Uniform detail level across all assets
- Consistent visual weight and presence
- Professional appearance in-game

### ⚡ Performance Optimization
- Efficient texture memory usage
- Optimal rendering performance
- Predictable memory footprint

## Asset Creation Guidelines

### For New Assets
1. **Determine tile dimensions** (e.g., 3×2, 4×4, etc.)
2. **Calculate pixel dimensions** (tiles × 64)
3. **Create asset at exact dimensions**
4. **Ensure content fits within tile boundaries**
5. **Test in map editor for proper alignment**

### Quality Checklist
- ✅ Dimensions are exact multiples of 64 pixels
- ✅ Content is clearly visible at 64×64 per tile
- ✅ Borders and details are crisp and well-defined
- ✅ Transparent areas are properly handled
- ✅ Asset aligns with grid in map editor

## Technical Integration

### Map Editor Support
- **Grid System**: Uses TILE_SIZE = 64 constant
- **Preview System**: Scales assets proportionally
- **Placement System**: Aligns to 64-pixel grid boundaries
- **Multi-cell Support**: Handles any tile dimensions correctly

### Game Engine Integration
- **Rendering**: Optimized for 64×64 base tiles
- **Collision**: Aligned with tile boundaries
- **Physics**: Grid-based movement system
- **Memory**: Efficient texture management

## Verification
All assets have been verified to follow the 64×64 pixel per tile pattern:

```
✓ misc1.png (2×3): 128×192 pixels
✓ misc2.png (3×2): 192×128 pixels  
✓ large_castle.png (5×5): 320×320 pixels
✓ wide_wall.png (5×1): 320×64 pixels
✓ tall_tower.png (1×5): 64×320 pixels
```

This ensures perfect integration with the game engine's tile-based system and optimal performance across all platforms.
