# Collision System Update

## Overview
Removed automatic collision assignment for decoration layers (1-3) in `Scene_PlayGrid`. Now collision components are only added when explicitly specified in the level file, providing precise control over which tiles have collision.

## Changes Made

### Before (Automatic Collision)
```cpp
// Add collision for decoration layers (1-3) or if explicitly specified
if ((layer >= CLayer::DECORATION_1 && layer <= CLayer::DECORATION_3) || collision == 1) {
    e->addComponent<CBoundingBox>(std::make_shared<CBoundingBox>(m_tileSize));
}
```

**Issues with old system:**
- ❌ All decoration layer tiles automatically had collision
- ❌ No way to place decorative tiles without collision
- ❌ Less control over collision boundaries
- ❌ Could cause unwanted collision with decorative elements

### After (Explicit Collision Only)
```cpp
// Add collision only if explicitly specified in the level file
if (collision == 1) {
    e->addComponent<CBoundingBox>(std::make_shared<CBoundingBox>(m_tileSize));
    std::printf("Added collision to %s at (%d, %d)\n", spriteName.c_str(), x, y);
}
```

**Benefits of new system:**
- ✅ Collision only when explicitly requested
- ✅ Full control over which tiles have collision
- ✅ Can place decorative tiles without collision
- ✅ Matches the grid map editor's collision system
- ✅ Clear logging when collision is added

## Impact on Level Files

### Grid Map Editor Behavior
The grid map editor already provides explicit collision control:
- Assets have default collision settings in `config/asset_properties.txt`
- Users can toggle collision per tile with 'T' key
- Level files save explicit collision flags: `1` = collision, `0` = no collision

### Level File Format
```
Layer SpriteName X Y [Collision] [Rotation] [Width] [Height] [OriginX] [OriginY]
```

**Examples:**
```
1 TallTower 5 5 1 90 1 5 5 5    # Has collision (collision=1)
1 Bush 10 10 0 0 1 1 10 10      # No collision (collision=0)
2 Decoration 15 15 0 0 1 1 15 15 # No collision (collision=0)
```

## Layer System Clarification

### Layer Types
- **Layer 0**: Ground/Background - typically no collision
- **Layer 1**: Decoration 1 - collision only if specified
- **Layer 2**: Decoration 2 - collision only if specified  
- **Layer 3**: Decoration 3 - collision only if specified
- **Layer 4**: Entities - special handling (NPCs, spawn points, etc.)

### Collision Control
- **Editor**: Use 'T' key to toggle collision on individual tiles
- **Config**: Set default collision in `config/asset_properties.txt`
- **Level File**: Explicit collision flag in extended format

## Migration Notes

### Existing Levels
- **Old format levels**: Will work but won't have collision (since no collision flag)
- **New format levels**: Will respect explicit collision flags
- **Mixed format**: Supported - each line parsed according to its format

### Asset Properties
The grid map editor uses `config/asset_properties.txt` for default collision:
```
TallTower 1 5 1 0    # width height collision rotation
Bush 1 1 0 0         # No default collision
Wall 1 1 1 0         # Has default collision
```

## Benefits

### Precise Control
- Place decorative elements without unwanted collision
- Create complex collision layouts
- Match visual and collision boundaries exactly

### Consistency
- Play scene now matches grid map editor collision behavior
- What you set in the editor is what you get in gameplay
- No surprises with automatic collision

### Flexibility
- Can have collision on any layer when needed
- Can have decorative tiles without collision
- Full control over gameplay boundaries

## Testing

### Verification Steps
1. **Create Level**: Use grid map editor to place tiles on layers 1-3
2. **Set Collision**: Use 'T' key to enable/disable collision on specific tiles
3. **Save Level**: Save with explicit collision flags
4. **Play Level**: Verify only tiles with collision=1 have collision components

### Expected Results
- Tiles without collision flag: No collision component, player can walk through
- Tiles with collision=1: Has collision component, player cannot walk through
- Clear logging shows when collision is added

## Files Modified
- `src/scenes/scene_play_grid.cpp`: Removed automatic layer-based collision

This change provides much more precise control over collision boundaries and ensures perfect consistency between the editor and play experience.
