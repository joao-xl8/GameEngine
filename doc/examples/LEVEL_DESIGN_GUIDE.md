# Level Design Guide

This guide explains how to create levels with walkable paths, decorative elements, and collision boundaries.

## Level File Format

Level files are located in `metadata/level_1.txt` and use this format:
```
Type SpriteName X Y
```

### Entity Types

#### **Tile** - Solid Objects (With Collision)
```
Tile Ground 5 3    # Walkable ground tile
Tile Wall 0 0      # Solid wall that blocks movement
```
- **Purpose**: Creates solid objects that block player movement
- **Collision**: YES - Player cannot move through these
- **Use for**: Walls, barriers, solid ground tiles

#### **Dec** - Decorations (No Collision)  
```
Dec Bush 2 5       # Decorative bush player can walk through
Dec Tree 10 8      # Decorative tree (if you have the asset)
```
- **Purpose**: Visual decoration that doesn't block movement
- **Collision**: NO - Player can walk through these
- **Use for**: Bushes, flowers, background decorations

## Available Assets

Current assets in `assets/imgs/`:
- **Ground**: `ground.png` - Walkable floor tiles
- **Wall**: `wall.png` - Solid barriers  
- **Bush**: `bush.png` - Decorative foliage

## Level Design Principles

### 1. **Create Boundaries**
Always surround your level with walls to prevent the player from walking off-screen:
```
# Top border
Tile Wall 0 0
Tile Wall 1 0
Tile Wall 2 0
...

# Left and right borders  
Tile Wall 0 1
Tile Wall 19 1
...

# Bottom border
Tile Wall 0 14
Tile Wall 1 14
...
```

### 2. **Design Walkable Paths**
Use Ground tiles to create paths the player can walk on:
```
# Horizontal path
Tile Ground 5 5
Tile Ground 6 5
Tile Ground 7 5

# Vertical path
Tile Ground 5 6
Tile Ground 5 7
Tile Ground 5 8
```

### 3. **Add Visual Interest with Decorations**
Place bushes and other decorations to make the environment look natural:
```
# Bushes along the path (no collision)
Dec Bush 4 5
Dec Bush 8 5
Dec Bush 6 7
```

### 4. **Create Interesting Layouts**
Use walls to create rooms, corridors, and obstacles:
```
# Create a room
Tile Wall 10 10
Tile Wall 11 10
Tile Wall 12 10
Tile Wall 10 11
Tile Wall 12 11
Tile Wall 10 12
Tile Wall 11 12
Tile Wall 12 12

# Fill with walkable ground
Tile Ground 11 11
```

## Current Level Layout

The current level (`level_1.txt`) features:

### **Environment Design**
- **20x15 grid** with wall borders
- **Winding paths** created with Ground tiles
- **Internal walls** creating interesting navigation
- **Decorative bushes** scattered throughout (no collision)

### **Walkable Areas**
- Main pathways connecting different areas
- Open spaces for exploration
- Multiple routes between locations

### **Collision Layout**
- ✅ **Walls**: Block movement, create boundaries
- ✅ **Ground**: Walkable surface  
- ❌ **Bushes**: Decorative only, no collision

## Customization Examples

### **Garden Maze**
```
# Create maze walls
Tile Wall 5 5
Tile Wall 6 5
Tile Wall 7 5

# Add decorative elements
Dec Bush 4 4
Dec Bush 8 4

# Create walkable paths
Tile Ground 5 4
Tile Ground 5 6
```

### **Forest Path**
```
# Tree line (walls)
Tile Wall 3 3
Tile Wall 4 3
Tile Wall 5 3

# Forest floor (walkable)
Tile Ground 3 4
Tile Ground 4 4
Tile Ground 5 4

# Scattered bushes (decoration)
Dec Bush 2 4
Dec Bush 6 4
Dec Bush 4 5
```

### **Castle Courtyard**
```
# Castle walls
Tile Wall 0 0
Tile Wall 1 0
Tile Wall 2 0

# Courtyard ground
Tile Ground 1 1
Tile Ground 2 1

# Garden decorations
Dec Bush 1 2
Dec Bush 2 2
```

## Tips for Level Design

### **Scale Considerations**
- Current scale: 64x64 pixels per tile
- Grid coordinates: (0,0) is top-left
- Recommended level size: 20x15 for good gameplay

### **Player Experience**
- Ensure there are clear paths to navigate
- Don't make areas too cramped (player is 64x64)
- Use decorations to guide the player's eye
- Create interesting shapes and layouts

### **Performance**
- More entities = more rendering work
- Decorations don't affect collision performance
- Keep levels reasonably sized for smooth gameplay

## Adding New Assets

To add new visual elements:

1. **Add image file** to `assets/imgs/`
2. **Update assets.txt**:
   ```
   Texture TreeName assets/imgs/tree.png
   ```
3. **Use in level file**:
   ```
   Dec TreeName 5 5    # For decoration
   Tile TreeName 5 5   # For collision
   ```

This system gives you complete control over creating engaging, walkable environments with proper collision detection! 🎮🌳
