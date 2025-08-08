#!/usr/bin/env python3

import os

def check_level_has_npc():
    """Check if level1.txt has an NPC for dialogue testing"""
    level_file = "metadata/levels/level1.txt"
    if not os.path.exists(level_file):
        print("❌ Level file not found: " + level_file)
        return False
    
    with open(level_file, 'r') as f:
        content = f.read()
    
    if "NPC Actor_idle" in content:
        print("✅ Found NPC in level1.txt for dialogue testing")
        return True
    else:
        print("⚠️ No NPC found in level1.txt - dialogue testing may not work")
        return False

def check_dialogue_files():
    """Check if dialogue files exist"""
    dialogue_files = [
        "metadata/dialogue/actor_idle_dialogue.txt",
        "metadata/dialogue/test_dialogue.txt"
    ]
    
    found_files = []
    for file_path in dialogue_files:
        if os.path.exists(file_path):
            print(f"✅ Found dialogue file: {file_path}")
            found_files.append(file_path)
        else:
            print(f"⚠️ Dialogue file not found: {file_path}")
    
    return found_files

if __name__ == "__main__":
    print("=== Dialogue Position Preservation Test ===")
    
    print("1. Checking level setup...")
    has_npc = check_level_has_npc()
    
    print("\n2. Checking dialogue files...")
    dialogue_files = check_dialogue_files()
    
    print("\n=== Test Instructions ===")
    if has_npc and dialogue_files:
        print("✅ Setup looks good! Follow these steps:")
        print("1. Run the game: ./build/release/GameEngine")
        print("2. Start a new game or load existing save")
        print("3. Move player to a SPECIFIC position (e.g., far right or left)")
        print("4. Note the exact position where you are")
        print("5. Find the NPC (Actor_idle) and press E to interact")
        print("6. Go through the dialogue (press SPACE to advance)")
        print("7. When dialogue ends, check if player is at SAME position")
        
        print("\n=== Expected Debug Output ===")
        print("Starting dialogue with NPC: Actor_idle")
        print("Preserving game state:")
        print("  Level: metadata/levels/level1.txt")
        print("  Player Position: (your current position)")
        print("  Health: 100")
        print("  Play Time: X seconds")
        print("DEBUG: Scene_Dialogue constructor with preserved state called")
        print("Preserved state - Level: metadata/levels/level1.txt, Position: (X, Y)")
        print("...")
        print("Returning to play scene with preserved state:")
        print("  Level: metadata/levels/level1.txt")
        print("  Position: (same as before dialogue)")
        print("  Health: 100")
        print("Using custom spawn position from save: X.0, Y.0")
        
        print("\n=== Success Criteria ===")
        print("✅ Player returns to EXACT same position after dialogue")
        print("✅ No teleportation to spawn point or level center")
        print("✅ All game state preserved (health, play time, etc.)")
        
        print("\n=== Failure Signs (Fixed) ===")
        print("❌ Player teleports to spawn point after dialogue")
        print("❌ Player appears at level center (640, 448)")
        print("❌ Position resets to default spawn location")
        
    else:
        print("⚠️ Setup incomplete:")
        if not has_npc:
            print("  - Need NPC in level for dialogue testing")
        if not dialogue_files:
            print("  - Need dialogue files for NPC interaction")
        print("  - You can still test by creating dialogue files or using existing NPCs")
    
    print("\n=== Manual Test Steps ===")
    print("1. Move player to position (800, 200) - remember this position")
    print("2. Find NPC and press E")
    print("3. Complete dialogue")
    print("4. Check if player is still at (800, 200)")
    print("5. If yes: ✅ Position preservation works!")
    print("6. If no: ❌ Position was reset (bug still exists)")
    
    print("\n=== Technical Details ===")
    print("The fix works by:")
    print("1. Scene_Play preserves current game state before starting dialogue")
    print("2. Scene_Dialogue stores the preserved state in member variables")
    print("3. When dialogue ends, Scene_Dialogue creates new Scene_Play")
    print("4. New Scene_Play uses setCustomSpawnPosition() with preserved position")
    print("5. Player spawns at preserved position instead of default spawn")
    
    print("\nThis should completely fix the position reset issue after dialogue!")
