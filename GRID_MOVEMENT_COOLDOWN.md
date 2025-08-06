# Grid Movement Cooldown System

The `changeGridSleep` system prevents buggy multi-tile movement by adding a configurable cooldown after each grid movement.

## The Problem

Without cooldown, the grid movement system could be buggy:
- Player presses key once but moves 2+ tiles
- Input system registers multiple key presses too quickly
- Movement feels unresponsive or unpredictable
- Hard to control precise movement

## The Solution

### **Cooldown Timer System**
```cpp
float m_changeGridSleep = 0.2f; // Configurable delay in seconds
float m_gridMoveTimer = 0.0f;   // Current cooldown timer
```

### **How It Works**
1. **Key Press**: Player presses movement key
2. **Movement Check**: System checks if cooldown timer has expired (`m_gridMoveTimer <= 0.0f`)
3. **Movement Execution**: If timer expired, execute movement and start cooldown
4. **Cooldown Active**: Set `m_gridMoveTimer = m_changeGridSleep`
5. **Timer Countdown**: Timer decreases each frame until it reaches 0
6. **Ready for Next Move**: Player can move again when timer expires

## Configuration Options

### **Fast Response** (Arcade Style)
```cpp
float m_changeGridSleep = 0.1f; // 100ms delay
```
- **Feel**: Very responsive, fast-paced
- **Good for**: Action games, quick navigation
- **Risk**: Might still allow accidental double moves

### **Balanced** (Default)
```cpp
float m_changeGridSleep = 0.2f; // 200ms delay
```
- **Feel**: Responsive but controlled
- **Good for**: Most games, balanced gameplay
- **Benefit**: Prevents accidental moves while staying responsive

### **Deliberate** (Strategy Style)
```cpp
float m_changeGridSleep = 0.3f; // 300ms delay
```
- **Feel**: Slower, more deliberate movement
- **Good for**: Puzzle games, turn-based feel
- **Benefit**: Very precise control, no accidental moves

### **Turn-Based** (Classic RPG)
```cpp
float m_changeGridSleep = 0.5f; // 500ms delay
```
- **Feel**: Slow, methodical movement
- **Good for**: Classic RPG feel, careful navigation
- **Benefit**: Maximum precision, retro game feel

## Implementation Details

### **Timer Update**
```cpp
// Update grid movement timer each frame
if (m_gridMoveTimer > 0.0f) {
    m_gridMoveTimer -= m_deltaTime;
}
```

### **Movement Gating**
```cpp
// Only allow movement if cooldown has expired
if (m_gridMoveTimer <= 0.0f && !gridMovement->isMoving) {
    // Process movement input
    if (input->upPressed) {
        // Execute movement
        m_gridMoveTimer = m_changeGridSleep; // Start cooldown
    }
}
```

### **Benefits**

#### **1. Predictable Movement**
- Exactly one tile per key press
- No accidental multi-tile jumps
- Consistent timing between moves

#### **2. Configurable Feel**
- Adjust `m_changeGridSleep` for different game styles
- Easy to tune for your specific gameplay needs
- Single variable controls entire movement feel

#### **3. Input Reliability**
- Prevents input system quirks from affecting gameplay
- Works regardless of frame rate or input polling
- Consistent across different hardware

#### **4. Player Control**
- Players can predict exactly when next move is available
- No frustrating "stuck" or "too fast" movement
- Clear feedback through animation timing

## Tuning Guidelines

### **For Different Game Types**

#### **Puzzle Games**
```cpp
float m_changeGridSleep = 0.25f; // Deliberate, precise
```

#### **Action RPGs**
```cpp
float m_changeGridSleep = 0.15f; // Quick but controlled
```

#### **Classic RPGs**
```cpp
float m_changeGridSleep = 0.3f; // Traditional feel
```

#### **Strategy Games**
```cpp
float m_changeGridSleep = 0.4f; // Thoughtful movement
```

### **Testing Different Values**
1. Start with default `0.2f`
2. Play test the movement feel
3. Adjust up for more deliberate feel
4. Adjust down for more responsive feel
5. Find the sweet spot for your game

## Integration with Animation

The cooldown system works perfectly with the animation system:
- Movement starts → Animation plays → Cooldown begins
- Smooth interpolation continues during cooldown
- Next movement available when animation feels complete
- Visual feedback matches input timing

## Troubleshooting

### **Movement Feels Too Slow**
- Decrease `m_changeGridSleep` value
- Try `0.15f` or `0.1f`

### **Still Getting Double Moves**
- Increase `m_changeGridSleep` value
- Check input system for multiple key press events
- Ensure `resetPressFlags()` is being called

### **Movement Feels Laggy**
- Balance between cooldown and animation speed
- Ensure smooth interpolation is enabled
- Consider adjusting grid movement speed as well

This system ensures your grid movement is rock-solid and feels exactly how you want it! 🎮⏱️
