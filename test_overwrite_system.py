#!/usr/bin/env python3

import os
import time

def check_save_file_content(slot_number):
    """Check the content of a specific save file"""
    save_file = f"saves/save_slot_{slot_number}.sav"
    if not os.path.exists(save_file):
        print(f"  Slot {slot_number}: EMPTY (file doesn't exist)")
        return None
    
    try:
        with open(save_file, 'r') as f:
            lines = f.readlines()
        
        save_data = {}
        for line in lines:
            line = line.strip()
            if line.startswith("SAVE_NAME "):
                save_data['name'] = line.split("SAVE_NAME ")[1]
            elif line.startswith("PLAYER_X "):
                save_data['x'] = line.split("PLAYER_X ")[1]
            elif line.startswith("PLAYER_Y "):
                save_data['y'] = line.split("PLAYER_Y ")[1]
            elif line.startswith("PLAYER_HEALTH "):
                save_data['health'] = line.split("PLAYER_HEALTH ")[1]
            elif line.startswith("SAVE_TIME "):
                save_data['time'] = line.split("SAVE_TIME ")[1]
            elif line.startswith("PLAY_TIME "):
                save_data['playtime'] = line.split("PLAY_TIME ")[1]
            elif line.startswith("CURRENT_LEVEL "):
                save_data['level'] = line.split("CURRENT_LEVEL ")[1]
        
        print(f"  Slot {slot_number}: {save_data.get('name', 'Unknown')}")
        print(f"    Position: ({save_data.get('x', '?')}, {save_data.get('y', '?')})")
        print(f"    Health: {save_data.get('health', '?')}")
        print(f"    Level: {save_data.get('level', '?')}")
        print(f"    Play Time: {save_data.get('playtime', '?')} seconds")
        print(f"    Save Time: {save_data.get('time', '?')}")
        
        return save_data
        
    except Exception as e:
        print(f"  Slot {slot_number}: ERROR reading file - {e}")
        return None

def create_overwrite_test_saves():
    """Create test saves with different data for overwrite testing"""
    saves_dir = "saves"
    if not os.path.exists(saves_dir):
        os.makedirs(saves_dir)
    
    # Create test saves with distinct, easily identifiable data
    test_saves = [
        {
            "slot": 13,
            "name": "OLD SAVE - Before Overwrite",
            "x": 100,
            "y": 200,
            "health": 50,
            "playtime": 300,
            "level": "metadata/levels/level1.txt",
            "description": "Original save - should be completely replaced"
        },
        {
            "slot": 14,
            "name": "ORIGINAL DATA - Test Slot",
            "x": 500,
            "y": 600,
            "health": 75,
            "playtime": 1800,
            "level": "metadata/levels/spawn_test.txt",
            "description": "Another original save for overwrite testing"
        }
    ]
    
    for save_data in test_saves:
        save_file = os.path.join(saves_dir, f"save_slot_{save_data['slot']}.sav")
        with open(save_file, 'w') as f:
            f.write("SAVE_VERSION 1.0\n")
            f.write(f"SAVE_NAME {save_data['name']}\n")
            f.write("LEVEL_NAME Original Level\n")
            f.write(f"CURRENT_LEVEL {save_data['level']}\n")
            f.write(f"PLAYER_X {save_data['x']}\n")
            f.write(f"PLAYER_Y {save_data['y']}\n")
            f.write(f"PLAYER_HEALTH {save_data['health']}\n")
            f.write(f"SAVE_TIME {int(time.time()) - 3600}\n")  # 1 hour ago
            f.write(f"PLAY_TIME {save_data['playtime']}\n")
            f.write("FLAGS_START\n")
            f.write("FLAGS_END\n")
            f.write("VARIABLES_START\n")
            f.write("VARIABLES_END\n")
        
        print(f"Created test save slot {save_data['slot']}: {save_data['description']}")

def check_all_test_saves():
    """Check the current state of all test save slots"""
    print("=== Current Save Slot Status ===")
    for slot in [13, 14]:
        check_save_file_content(slot)
        print()

if __name__ == "__main__":
    print("=== Overwrite System Test Setup ===")
    print("1. Creating test saves with original data...")
    create_overwrite_test_saves()
    
    print("\n2. Current save file contents:")
    check_all_test_saves()
    
    print("=== Overwrite Test Instructions ===")
    print("1. Run the game: ./build/release/GameEngine")
    print("2. Start a new game or load an existing save")
    print("3. Move player to a DIFFERENT position (e.g., 800, 400)")
    print("4. Find a save point (red square) and press E")
    print("5. Navigate to slot 13 or 14 (should show old data)")
    print("6. Press SPACE to trigger overwrite dialog")
    print("7. Select YES to overwrite")
    print("8. Check that the slot IMMEDIATELY shows NEW data:")
    print("   - New save name (e.g., 'Level 1 - Slot 13')")
    print("   - New player position (e.g., 800, 400)")
    print("   - New timestamp (current time)")
    print("   - New play time")
    print("   - ALL old data should be completely replaced")
    
    print("\n=== Expected Behavior ===")
    print("BEFORE overwrite (slot 13):")
    print("  Name: 'OLD SAVE - Before Overwrite'")
    print("  Position: (100, 200)")
    print("  Health: 50")
    print("  Play Time: 300 seconds")
    print("  Old timestamp")
    print("\nAFTER overwrite (slot 13):")
    print("  Name: 'Level 1 - Slot 13' (or similar)")
    print("  Position: (800, 400) (your current position)")
    print("  Health: 100 (current health)")
    print("  Play Time: (current play time)")
    print("  New timestamp (current time)")
    print("\n✅ SUCCESS: All old data completely replaced")
    print("✅ SUCCESS: UI immediately shows new data")
    print("❌ FAILURE: Old data still visible or mixed with new data")
    
    print("\n=== Debug Output to Watch For ===")
    print("Overwriting slot 13 with new data:")
    print("  Save Name: Level 1 - Slot 13")
    print("  Player Position: (800.0, 400.0)")
    print("  Level: metadata/levels/level1.txt")
    print("  Health: 100")
    print("  Play Time: 1234 seconds")
    print("  Timestamp: 1754675000")
    print("✅ Slot 13 completely overwritten with new save data!")
    print("Refreshing save slots from disk...")
    print("✅ Verified slot 13 updated:")
    print("  Display Name: Level 1 - Slot 13")
    print("  Time String: [current time]")
    print("  Is Empty: NO")
    print("  Player Pos: (800.0, 400.0)")
    print("✅ UI updated - slot 13 now shows new save data on screen")
    
    print("\n=== After Testing ===")
    print("Run this script again to see the updated save file contents:")
    print("python3 test_overwrite_system.py")
