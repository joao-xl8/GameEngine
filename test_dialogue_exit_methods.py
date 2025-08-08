#!/usr/bin/env python3

import os

def check_dialogue_system():
    """Check if dialogue system is properly set up"""
    level_file = "metadata/levels/level1.txt"
    dialogue_file = "metadata/dialogues/npcs/actor_idle/enhanced_greeting.txt"
    
    level_exists = os.path.exists(level_file)
    dialogue_exists = os.path.exists(dialogue_file)
    
    print(f"Level file: {'✅' if level_exists else '❌'} {level_file}")
    print(f"Dialogue file: {'✅' if dialogue_exists else '❌'} {dialogue_file}")
    
    if level_exists:
        with open(level_file, 'r') as f:
            content = f.read()
        has_npc = "NPC Actor_idle" in content
        has_spawn = "PlayerSpawn" in content
        print(f"NPC in level: {'✅' if has_npc else '❌'} Actor_idle")
        print(f"PlayerSpawn in level: {'✅' if has_spawn else '❌'} PlayerSpawn tile")
    
    return level_exists and dialogue_exists

if __name__ == "__main__":
    print("=== Dialogue Exit Methods Test ===")
    
    print("1. Checking dialogue system setup...")
    system_ready = check_dialogue_system()
    
    if not system_ready:
        print("❌ System not ready for testing")
        exit(1)
    
    print("\n✅ System ready for testing!")
    
    print("\n=== Test Scenarios ===")
    print("We need to test BOTH ways of exiting dialogue:")
    print("1. 📖 NORMAL COMPLETION: Go through entire dialogue")
    print("2. 🚪 ESC EXIT: Press ESC to exit dialogue early")
    print("Both should preserve player position!")
    
    print("\n=== Test Procedure ===")
    print("🎮 Run the game: ./build/release/GameEngine")
    print("🎯 Start new game or load save")
    print("🚶 Move player to MEMORABLE position (e.g., far right corner)")
    print("📍 Remember exact position (e.g., 'near the right wall')")
    
    print("\n--- Test 1: Normal Completion ---")
    print("1. 🗣️ Find NPC (Actor_idle) and press E")
    print("2. 📖 Go through ENTIRE dialogue (press SPACE repeatedly)")
    print("3. ✅ Dialogue ends naturally")
    print("4. 📍 Check: Player should be at SAME position")
    print("5. 🎉 SUCCESS: Position preserved through normal completion")
    
    print("\n--- Test 2: ESC Exit ---")
    print("1. 🚶 Move player to DIFFERENT memorable position")
    print("2. 🗣️ Find NPC (Actor_idle) and press E")
    print("3. 🚪 Press ESC immediately (don't complete dialogue)")
    print("4. ✅ Dialogue exits early")
    print("5. 📍 Check: Player should be at SAME position")
    print("6. 🎉 SUCCESS: Position preserved through ESC exit")
    
    print("\n=== Expected Debug Output ===")
    print("🔍 For BOTH test scenarios, you should see:")
    print("Starting dialogue with NPC: Actor_idle")
    print("Preserving game state:")
    print("  Level: metadata/levels/level1.txt")
    print("  Player Position: (your position)")
    print("  Health: 100")
    print("DEBUG: Scene_Dialogue constructor with preserved state called")
    print("Preserved state - Level: metadata/levels/level1.txt, Position: (X, Y)")
    
    print("\n--- Normal Completion Output ---")
    print("Dialogue complete!")
    print("Returning to play scene with preserved state:")
    print("  Position: (same as before)")
    print("Using custom spawn position from save: X.0, Y.0")
    
    print("\n--- ESC Exit Output ---")
    print("User pressed ESC during dialogue - completing dialogue normally to preserve state")
    print("Returning to play scene with preserved state:")
    print("  Position: (same as before)")
    print("Using custom spawn position from save: X.0, Y.0")
    
    print("\n=== Success Criteria ===")
    print("✅ Normal completion preserves position")
    print("✅ ESC exit preserves position")
    print("✅ Both methods use preserved state system")
    print("✅ No hardcoded level1.txt scene creation")
    print("✅ No teleportation to spawn point")
    
    print("\n=== Failure Signs (Should be FIXED now) ===")
    print("❌ Player teleports after ESC exit")
    print("❌ Different behavior between normal/ESC exit")
    print("❌ Position resets to spawn point")
    print("❌ Debug shows hardcoded level creation")
    
    print("\n=== Technical Fix Explanation ===")
    print("🔧 BEFORE (Broken):")
    print("  ESC → Direct Scene_Play creation → Position reset")
    print("  Normal → Preserved state system → Position preserved")
    print("  Result: Inconsistent behavior ❌")
    
    print("\n🔧 AFTER (Fixed):")
    print("  ESC → Set m_dialogueComplete = true → Preserved state system")
    print("  Normal → Set m_dialogueComplete = true → Preserved state system")
    print("  Result: Consistent behavior ✅")
    
    print("\n=== Code Changes Made ===")
    print("Changed ESC handler from:")
    print("  m_game->changeScene(\"Play\", std::make_shared<Scene_Play>(...));")
    print("To:")
    print("  m_dialogueComplete = true;")
    print("Now both exit methods use the same completion flow!")
    
    print("\n🎯 This ensures CONSISTENT position preservation regardless of how the dialogue ends!")
    
    print("\n=== Quick Test ===")
    print("1. Move to corner of level")
    print("2. Talk to NPC")
    print("3. Press ESC immediately")
    print("4. Should be back at corner (not spawn point)")
    print("5. If yes: ✅ Fix successful!")
    print("6. If no: ❌ Still broken (check debug output)")
