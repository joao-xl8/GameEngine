// Example of how to use the new external animation definition system

// In your Scene_Play::spawnPlayer() method, you can now do:

void Scene_Play::spawnPlayer() {
    // ... existing player setup code ...
    
    // Add animation component with flexible animation definitions
    auto animationComponent = std::make_shared<CAnimation>(Vec2{32, 32});

    // Define animations - you can use the same texture with different rows
    // or different textures for each animation
    animationComponent->addAnimation("idle", "Player", 6, 0.2f, false, 0);        // Row 0, 6 frames, slow
    animationComponent->addAnimation("walk_down", "Player", 6, 0.15f, false, 3);  // Row 3, 6 frames
    animationComponent->addAnimation("walk_up", "Player", 6, 0.15f, false, 2);    // Row 2, 6 frames
    animationComponent->addAnimation("walk_right", "Player", 6, 0.15f, false, 1); // Row 1, 6 frames
    animationComponent->addAnimation("walk_left", "Player", 6, 0.15f, true, 1);   // Row 1, flipped

    // Example of how you could add animations from separate files:
    // animationComponent->addAnimation("attack", "PlayerAttack", 4, 0.1f, false, 0);  // Different texture
    // animationComponent->addAnimation("jump", "PlayerJump", 8, 0.08f, false, 0);     // Different texture
    // animationComponent->addAnimation("death", "PlayerDeath", 10, 0.12f, false, 0, false);  // Non-repeating

    animationComponent->play("idle"); // Start with idle animation
    m_player->addComponent<CAnimation>(animationComponent);
    
    // ... rest of player setup ...
}

// Alternative approach: Create animation definition functions

std::shared_ptr<CAnimation> createPlayerAnimations() {
    auto anim = std::make_shared<CAnimation>(Vec2{32, 32});
    
    // Basic movement animations
    anim->addAnimation("idle", "Player", 6, 0.2f, false, 0);
    anim->addAnimation("walk_down", "Player", 6, 0.15f, false, 3);
    anim->addAnimation("walk_up", "Player", 6, 0.15f, false, 2);
    anim->addAnimation("walk_right", "Player", 6, 0.15f, false, 1);
    anim->addAnimation("walk_left", "Player", 6, 0.15f, true, 1);
    
    // Combat animations
    anim->addAnimation("attack_down", "Player", 4, 0.1f, false, 4);
    anim->addAnimation("attack_up", "Player", 4, 0.1f, false, 5);
    anim->addAnimation("attack_right", "Player", 4, 0.1f, false, 6);
    anim->addAnimation("attack_left", "Player", 4, 0.1f, true, 6);
    
    // Special animations (non-repeating)
    anim->addAnimation("death", "Player", 8, 0.15f, false, 7, false);
    anim->addAnimation("hurt", "Player", 3, 0.1f, false, 8, false);
    
    return anim;
}

std::shared_ptr<CAnimation> createEnemyAnimations() {
    auto anim = std::make_shared<CAnimation>(Vec2{24, 24});
    
    anim->addAnimation("idle", "Enemy", 4, 0.3f, false, 0);
    anim->addAnimation("walk", "Enemy", 4, 0.2f, false, 1);
    anim->addAnimation("attack", "Enemy", 6, 0.12f, false, 2, false);
    anim->addAnimation("death", "Enemy", 5, 0.15f, false, 3, false);
    
    return anim;
}

// Then in your spawn functions:
void Scene_Play::spawnPlayer() {
    // ... setup code ...
    auto animationComponent = createPlayerAnimations();
    animationComponent->play("idle");
    m_player->addComponent<CAnimation>(animationComponent);
    // ... rest of setup ...
}

void Scene_Play::spawnEnemy(Vec2 pos) {
    // ... setup code ...
    auto animationComponent = createEnemyAnimations();
    animationComponent->play("idle");
    enemy->addComponent<CAnimation>(animationComponent);
    // ... rest of setup ...
}
