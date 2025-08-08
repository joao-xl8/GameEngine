#!/usr/bin/env python3

import os
import time

def check_save_slot(slot_number):
    """Check if a save slot exists and show its content"""
    save_file = f"saves/save_slot_{slot_number}.sav"
    if not os.path.exists(save_file):
        print(f"Slot {slot_number}: EMPTY")
        return False
    
    try:
        with open(save_file, 'r') as f:
            lines = f.readlines()
        
        save_name = "Unknown"
        player_x = "Unknown"
        player_y = "Unknown"
        save_time = "Unknown"
        
        for line in lines:
            line = line.strip()
            if line.startswith("SAVE_NAME "):
                save_name = line.split("SAVE_NAME ")[1]
            elif line.startswith("PLAYER_X "):
                player_x = line.split("PLAYER_X ")[1]
            elif line.startswith("PLAYER_Y "):
                player_y = line.split("PLAYER_Y ")[1]
            elif line.startswith("SAVE_TIME "):
                save_time = line.split("SAVE_TIME ")[1]
        
        print(f"Slot {slot_number}: {save_name}")
        print(f"  Position: ({player_x}, {player_y})")
        print(f"  Time: {save_time}")
        return True
        
    except Exception as e:
        print(f"Slot {slot_number}: ERROR - {e}")
        return False

def create_test_slot():
    """Create a test slot for overwrite testing"""
    saves_dir = "saves"
    if not os.path.exists(saves_dir):
        os.makedirs(saves_dir)
    
    slot_number = 15
    save_file = os.path.join(saves_dir, f"save_slot_{slot_number}.sav")
    
    with open(save_file, 'w') as f:
        f.write("SAVE_VERSION 1.0\n")
        f.write("SAVE_NAME SLOT NUMBER TEST - Original\n")
        f.write("LEVEL_NAME Test Level\n")
        f.write("CURRENT_LEVEL metadata/levels/level1.txt\n")
        f.write("PLAYER_X 999\n")
        f.write("PLAYER_Y 888\n")
        f.write("PLAYER_HEALTH 50\n")
        f.write(f"SAVE_TIME {int(time.time()) - 1800}\n")  # 30 minutes ago
        f.write("PLAY_TIME 600\n")
        f.write("FLAGS_START\n")
        f.write("FLAGS_END\n")
        f.write("VARIABLES_START\n")
        f.write("VARIABLES_END\n")
    
    print(f"Created test slot {slot_number} for overwrite testing")
    return slot_number

if __name__ == "__main__":
    print("=== Slot Number Fix Test ===")
    
    # Create test slot
    test_slot = create_test_slot()
    
    print("\nBefore test:")
    check_save_slot(test_slot)
    
    print(f"\n=== Test Instructions ===")
    print("1. Run the game: ./build/release/GameEngine")
    print("2. Start new game or load existing save")
    print("3. Move player to a different position")
    print("4. Find save point (red square) and press E")
    print(f"5. Navigate to slot {test_slot} (should show test data)")
    print("6. Press SPACE to trigger overwrite dialog")
    print("7. Select YES to overwrite")
    print("8. Check console output - should NOT show 'slot -1'")
    
    print(f"\n=== Expected Debug Output (GOOD) ===")
    print("Slot is occupied, showing overwrite dialog...")
    print("Overwrite dialog input: SELECT")
    print("SELECT pressed, confirm = YES")
    print(f"Overwriting slot {test_slot}...")
    print(f"Performing save to slot {test_slot}")
    print(f"Overwriting slot {test_slot} with new data:")
    print(f"  Save Name: Level 1 - Slot {test_slot}")
    print("  Player Position: (your current position)")
    print(f"✅ Slot {test_slot} completely overwritten with new save data!")
    print(f"Slot {test_slot} has been overwritten successfully!")
    
    print(f"\n=== Bad Output (FIXED) ===")
    print("❌ Overwriting slot -1...")
    print("❌ Invalid save slot number: -1 (valid range: 0-99)")
    print("❌ Failed to save game to slot -1")
    print("❌ Slot -1 has been overwritten successfully!")
    
    print(f"\n=== After Testing ===")
    print("Run this script again to see if the slot was updated:")
    print("python3 test_slot_number_fix.py")
    print(f"Slot {test_slot} should show new data with current timestamp")
