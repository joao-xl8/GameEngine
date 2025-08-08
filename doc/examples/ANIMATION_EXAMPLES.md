# Animation System Examples

The refactored animation system makes it very easy to add new animations and behaviors.

## How the New System Works

### 1. Clean sAnimation Function
```cpp
void Scene_Play::sAnimation()
{
    for (auto &entity : m_entityManager.getEntities())
    {
        if (entity->hasComponent<CAnimation>() && entity->hasComponent<CSprite>())
        {
            auto animation = entity->getComponent<CAnimation>();
            auto sprite = entity->getComponent<CSprite>();
            
            // Let the animation component handle its own logic
            animation->update(m_deltaTime, sprite->sprite);
        }
    }
}
```

### 2. Self-Contained Animation Logic
All animation logic is now in the CAnimation component:
- Frame timing
- Sprite sheet row selection
- Horizontal flipping
- Frame advancement
- Texture rectangle updates

## Adding New Animations

### Example 1: Adding a Jump Animation
```cpp
// In the CAnimation constructor, add:
animations["jump"] = {4, false, 4, 0.1f}; // row 4, no flip, 4 frames, fast

// In your movement code:
if (input->space) 
{
    animation->play("jump");
}
```

### Example 2: Adding Attack Animations
```cpp
// Different attack directions
animations["attack_up"] = {5, false, 6, 0.08f};
animations["attack_down"] = {6, false, 6, 0.08f};
animations["attack_left"] = {7, true, 6, 0.08f};  // flipped
animations["attack_right"] = {7, false, 6, 0.08f};

// In your input handling:
if (input->attack) 
{
    if (input->up) animation->play("attack_up");
    else if (input->down) animation->play("attack_down");
    else if (input->left) animation->play("attack_left");
    else if (input->right) animation->play("attack_right");
}
```

### Example 3: Non-Repeating Animations
```cpp
// For one-time animations like death or special moves
auto animation = entity->getComponent<CAnimation>();
animation->repeat = false; // Don't loop
animation->play("death");

// Check if finished
if (animation->hasFinished()) {
    // Do something when animation completes
    entity->destroy();
}
```

## Benefits of This Design

1. **Separation of Concerns**: Animation logic is in the component, not the system
2. **Reusability**: Any entity can use the same animation component
3. **Easy Extension**: Adding new animations is just adding entries to the map
4. **Clean Code**: sAnimation function is simple and focused
5. **Flexibility**: Each animation can have different timing, flipping, frame counts

## Current Animation Definitions

```cpp
animations["idle"] = {0, false, 6, 0.2f};        // Row 0, slow idle
animations["walk_down"] = {3, false, 6, 0.15f};  // Row 3, walking down
animations["walk_left"] = {1, true, 6, 0.15f};   // Row 1, flipped for left
animations["walk_right"] = {1, false, 6, 0.15f}; // Row 1, normal right
animations["walk_up"] = {2, false, 6, 0.15f};    // Row 2, walking up
```

## Usage in Game Logic

```cpp
// Simple animation playing
animation->play("walk_left");

// Check current animation
if (animation->getCurrentAnimation() == "idle") {
    // Player is idle
}

// Non-repeating animations
animation->repeat = false;
animation->play("special_attack");
if (animation->hasFinished()) {
    // Attack animation completed
}
```

This design makes the animation system much more maintainable and extensible!
