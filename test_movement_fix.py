#!/usr/bin/env python3

import os
import time

def create_movement_test_saves():
    """Create test saves at specific positions to verify movement fix"""
    saves_dir = "saves"
    if not os.path.exists(saves_dir):
        os.makedirs(saves_dir)
    
    test_saves = [
        {
            "slot": 8,
            "name": "Movement Test Far Right",
            "x": 960,  # 15 tiles right (15 * 64 = 960)
            "y": 320,  # 5 tiles down (5 * 64 = 320)
            "description": "Far right position - should move from here, not teleport to spawn"
        },
        {
            "slot": 9,
            "name": "Movement Test Bottom",
            "x": 320,  # 5 tiles right (5 * 64 = 320)
            "y": 640,  # 10 tiles down (10 * 64 = 640)
            "description": "Bottom position - should move from here, not teleport to spawn"
        }
    ]
    
    for save_data in test_saves:
        save_file = os.path.join(saves_dir, f"save_slot_{save_data['slot']}.sav")
        with open(save_file, 'w') as f:
            f.write("SAVE_VERSION 1.0\n")
            f.write(f"SAVE_NAME {save_data['name']}\n")
            f.write("LEVEL_NAME Movement Test Level\n")
            f.write("CURRENT_LEVEL metadata/levels/level1.txt\n")
            f.write(f"PLAYER_X {save_data['x']}\n")
            f.write(f"PLAYER_Y {save_data['y']}\n")
            f.write("PLAYER_HEALTH 100\n")
            f.write(f"SAVE_TIME {int(time.time())}\n")
            f.write("PLAY_TIME 60\n")
            f.write("FLAGS_START\n")
            f.write("FLAGS_END\n")
            f.write("VARIABLES_START\n")
            f.write("VARIABLES_END\n")
        
        print(f"Created movement test slot {save_data['slot']}: {save_data['description']}")
        print(f"  Position: ({save_data['x']}, {save_data['y']}) = Grid ({save_data['x']//64}, {save_data['y']//64})")

if __name__ == "__main__":
    print("=== Movement Fix Test Setup ===")
    print("Creating test saves at specific positions...")
    create_movement_test_saves()
    
    print("\n=== Test Instructions ===")
    print("1. Run the game: ./build/release/GameEngine")
    print("2. Test movement fix:")
    print("   a. Load slot 8 (Far Right Position)")
    print("      - Should spawn at (960, 320) = Grid (15, 5)")
    print("      - Press W/A/S/D - player should move FROM that position")
    print("      - Should NOT teleport to spawn point (640, 448)")
    print("   b. Load slot 9 (Bottom Position)")
    print("      - Should spawn at (320, 640) = Grid (5, 10)")
    print("      - Press W/A/S/D - player should move FROM that position")
    print("      - Should NOT teleport to spawn point")
    print("3. Check console output:")
    print("   - Look for 'Using custom spawn position: X, Y'")
    print("   - Look for 'Initialized grid position to: X, Y'")
    print("   - Look for 'UP pressed - Current grid pos: X, Y'")
    print("   - Grid position should match spawn position divided by 64")
    print("4. Expected behavior:")
    print("   - Player spawns at saved position")
    print("   - First movement moves FROM that position (no teleport)")
    print("   - Subsequent movements work normally")
    print("\n=== Debug Output to Watch For ===")
    print("GOOD:")
    print("  Using custom spawn position: 960.0, 320.0")
    print("  Initialized grid position to: 15, 5")
    print("  UP pressed - Current grid pos: 15, 5")
    print("  UP movement successful - New grid pos: 15, 4")
    print("\nBAD (if bug still exists):")
    print("  Using custom spawn position: 960.0, 320.0")
    print("  Initialized grid position to: 15, 5")
    print("  UP pressed - Current grid pos: 10, 7  <- Wrong! Should be 15, 5")
    print("  (Player teleports to spawn point)")
