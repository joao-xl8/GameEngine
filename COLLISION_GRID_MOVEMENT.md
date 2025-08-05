# Collision-Aware Grid Movement System

The enhanced grid movement system now includes collision detection to prevent the player from moving through walls, tiles, or outside the game boundaries.

## How It Works

### 1. **Pre-Movement Collision Checking**
Before allowing any grid movement, the system checks if the target position would cause a collision:

```cpp
// Check if movement is valid before starting
if (gridMovement->startMoveWithCollisionCheck(direction, currentPos, entitySize, collisionCheck)) {
    // Movement is valid - start moving
    animation->play("walk_direction");
} else {
    // Movement blocked - stay in place
    // No animation change, player remains idle
}
```

### 2. **Collision Detection Types**

#### **Tile Collisions**
- Checks against all entities with "Tile" tag
- Uses bounding box collision detection
- Prevents movement into solid tiles/walls

#### **Boundary Collisions**
- Prevents movement outside window bounds
- Checks all four edges (left, right, top, bottom)
- Keeps player within the playable area

### 3. **Movement Flow**

1. **Input Received**: Player presses movement key
2. **Collision Check**: System calculates target grid position
3. **Validation**: Checks if target position is valid (no collisions)
4. **Movement Decision**: 
   - ✅ **Valid**: Start smooth movement to target
   - ❌ **Invalid**: Stay in current position, no movement

## Key Features

### **Predictable Movement**
- Player can only move to valid grid positions
- No partial movements or getting stuck in walls
- Clear feedback when movement is blocked

### **Smooth Integration**
- Works seamlessly with existing animation system
- Maintains smooth interpolation between valid positions
- Collision checking doesn't affect visual smoothness

### **Performance Optimized**
- Collision checking only happens on movement attempts
- No continuous collision detection during interpolation
- Efficient tile entity lookup

## Configuration Examples

### **Basic Setup with Collision**
```cpp
// Player spawning with collision-aware grid movement
auto gridMovement = std::make_shared<CGridMovement>(32, 4.0f, true);
gridMovement->snapToGrid(startPos);
m_player->addComponent<CGridMovement>(gridMovement);

// Bounding box is required for collision detection
m_player->addComponent<CBoundingBox>(std::make_shared<CBoundingBox>(Vec2{32, 32}));
```

### **Different Collision Behaviors**

#### **Strict Grid Movement** (Recommended)
```cpp
auto gridMovement = std::make_shared<CGridMovement>(32, 3.0f, true);
// Player can only move to completely free grid positions
```

#### **Fast Response Grid**
```cpp
auto gridMovement = std::make_shared<CGridMovement>(32, 6.0f, true);
// Quick movement with collision checking
```

#### **Instant Snap with Collision**
```cpp
auto gridMovement = std::make_shared<CGridMovement>(32, 10.0f, false);
// Instant movement to valid positions only
```

## Collision System Integration

### **Tile-Based Level Design**
```cpp
// Level file format supports collision tiles
// Tile Ground 0 0    <- Walkable
// Tile Wall 1 0      <- Blocks movement
// Tile Water 2 0     <- Blocks movement
```

### **Custom Collision Rules**
You can extend the collision system for different tile types:

```cpp
bool Scene_Play::wouldCollideAtPosition(const Vec2& position, const Vec2& size)
{
    // Check boundaries
    if (isOutOfBounds(position, size)) return true;
    
    // Check different tile types
    for (auto &entity : m_entityManager.getEntities("Tile"))
    {
        auto sprite = entity->getComponent<CSprite>();
        
        // Different collision rules for different tiles
        if (sprite->name == "Wall" || sprite->name == "Rock") {
            // Solid tiles block movement
            if (isColliding(position, size, entity->pos, entity->size)) {
                return true;
            }
        }
        // Water, Lava, etc. could have different rules
    }
    
    return false;
}
```

## Benefits

### **1. Authentic Retro Feel**
- Mimics classic tile-based games perfectly
- No sliding through walls or getting stuck
- Predictable, grid-locked movement

### **2. Level Design Friendly**
- Easy to create tile-based levels
- Clear collision boundaries
- Supports complex level layouts

### **3. Player Experience**
- Responsive controls (no lag from collision resolution)
- Clear visual feedback when movement is blocked
- Smooth animations even with collision constraints

### **4. Developer Benefits**
- Simple collision system integration
- Easy to debug movement issues
- Extensible for different game mechanics

## Troubleshooting

### **Player Gets Stuck**
- Check if `snapToGrid()` places player in valid position
- Verify tile collision boxes don't overlap walkable areas
- Ensure grid size matches tile size

### **Movement Feels Sluggish**
- Increase movement speed: `CGridMovement(32, 6.0f, true)`
- Consider instant movement: `CGridMovement(32, 10.0f, false)`

### **Collision Detection Issues**
- Verify bounding box sizes match sprite sizes
- Check tile positioning in level files
- Ensure collision method accounts for coordinate system

This system provides the foundation for solid, collision-aware tile-based gameplay that feels authentic to classic games while maintaining modern smoothness and responsiveness!
