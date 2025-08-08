#!/usr/bin/env python3

import os
import time

def check_save_files():
    """Check existing save files and their player positions"""
    saves_dir = "saves"
    if not os.path.exists(saves_dir):
        print("No saves directory found")
        return
    
    save_files = [f for f in os.listdir(saves_dir) if f.endswith('.sav')]
    print(f"Found {len(save_files)} save files:")
    
    for save_file in save_files:
        file_path = os.path.join(saves_dir, save_file)
        try:
            with open(file_path, 'r') as f:
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
            
            print(f"  {save_file}:")
            print(f"    Name: {save_name}")
            print(f"    Position: ({player_x}, {player_y})")
            print(f"    Time: {save_time}")
            print()
            
        except Exception as e:
            print(f"  {save_file}: Error reading file - {e}")

def create_test_saves():
    """Create test saves with different positions"""
    saves_dir = "saves"
    if not os.path.exists(saves_dir):
        os.makedirs(saves_dir)
    
    test_saves = [
        {
            "slot": 5,
            "name": "Position Test 1",
            "x": 320,
            "y": 240,
            "description": "Center position"
        },
        {
            "slot": 6,
            "name": "Position Test 2", 
            "x": 640,
            "y": 480,
            "description": "Bottom right"
        },
        {
            "slot": 7,
            "name": "Position Test 3",
            "x": 128,
            "y": 128,
            "description": "Top left area"
        }
    ]
    
    for save_data in test_saves:
        save_file = os.path.join(saves_dir, f"save_slot_{save_data['slot']}.sav")
        with open(save_file, 'w') as f:
            f.write("SAVE_VERSION 1.0\n")
            f.write(f"SAVE_NAME {save_data['name']}\n")
            f.write("LEVEL_NAME Test Level\n")
            f.write("CURRENT_LEVEL metadata/levels/level1.txt\n")
            f.write(f"PLAYER_X {save_data['x']}\n")
            f.write(f"PLAYER_Y {save_data['y']}\n")
            f.write("PLAYER_HEALTH 100\n")
            f.write(f"SAVE_TIME {int(time.time())}\n")
            f.write("PLAY_TIME 300\n")
            f.write("FLAGS_START\n")
            f.write("FLAGS_END\n")
            f.write("VARIABLES_START\n")
            f.write("VARIABLES_END\n")
        
        print(f"Created test save slot {save_data['slot']}: {save_data['description']} at ({save_data['x']}, {save_data['y']})")

if __name__ == "__main__":
    print("=== Save Position Test Setup ===")
    print("1. Checking existing saves...")
    check_save_files()
    
    print("2. Creating test saves with different positions...")
    create_test_saves()
    
    print("\n3. Updated save list:")
    check_save_files()
    
    print("=== Test Instructions ===")
    print("1. Run the game: ./build/release/GameEngine")
    print("2. Test main menu controls:")
    print("   - Use SPACE to select options")
    print("   - Use ESC to exit game (not C)")
    print("3. Test save position preservation:")
    print("   - Load slot 5 (should spawn at center: 320, 240)")
    print("   - Load slot 6 (should spawn at bottom right: 640, 480)")
    print("   - Load slot 7 (should spawn at top left: 128, 128)")
    print("4. Test save menu return:")
    print("   - Move player to a different position")
    print("   - Press E at save point to open save menu")
    print("   - Press C to return - player should be at same position")
    print("5. Check console output for spawn position debug messages")
