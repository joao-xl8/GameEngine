#!/usr/bin/env python3

import os
import json
import time

def check_save_files():
    """Check what save files exist"""
    saves_dir = "saves"
    if not os.path.exists(saves_dir):
        print("No saves directory found")
        return
    
    save_files = [f for f in os.listdir(saves_dir) if f.endswith('.sav')]
    print(f"Found {len(save_files)} .sav files:")
    
    for save_file in save_files:
        file_path = os.path.join(saves_dir, save_file)
        try:
            with open(file_path, 'r') as f:
                lines = f.readlines()
            save_name = "Unknown"
            save_time = "Unknown"
            for line in lines:
                if line.startswith("SAVE_NAME "):
                    save_name = line.split("SAVE_NAME ")[1].strip()
                elif line.startswith("SAVE_TIME "):
                    save_time = line.split("SAVE_TIME ")[1].strip()
            print(f"  {save_file}: {save_name} (time: {save_time})")
        except Exception as e:
            print(f"  {save_file}: Error reading file - {e}")

def create_test_save():
    """Create a test save file in .sav format"""
    saves_dir = "saves"
    if not os.path.exists(saves_dir):
        os.makedirs(saves_dir)
    
    save_file = os.path.join(saves_dir, "save_slot_4.sav")
    with open(save_file, 'w') as f:
        f.write("SAVE_VERSION 1.0\n")
        f.write("SAVE_NAME Test Overwrite Save\n")
        f.write("LEVEL_NAME Test Level\n")
        f.write("CURRENT_LEVEL metadata/levels/level1.txt\n")
        f.write("PLAYER_X 640\n")
        f.write("PLAYER_Y 320\n")
        f.write("PLAYER_HEALTH 100\n")
        f.write(f"SAVE_TIME {int(time.time())}\n")
        f.write("PLAY_TIME 1800\n")
        f.write("FLAGS_START\n")
        f.write("FLAGS_END\n")
        f.write("VARIABLES_START\n")
        f.write("VARIABLES_END\n")
    
    print(f"Created test save file: {save_file}")

if __name__ == "__main__":
    print("=== Save System Test ===")
    print("1. Checking existing save files...")
    check_save_files()
    
    print("\n2. Creating test save file...")
    create_test_save()
    
    print("\n3. Checking save files after creation...")
    check_save_files()
    
    print("\n=== Test Instructions ===")
    print("1. Run the game: ./build/release/GameEngine")
    print("2. Test pause menu:")
    print("   - Press ESC to open pause menu")
    print("   - Use arrow keys or WASD to navigate")
    print("   - Press E or SPACE to select")
    print("   - Test Resume and Main Menu options")
    print("3. Test save system:")
    print("   - Find red save point and press E")
    print("   - Navigate to slot 4 (should show 'Test Overwrite Save')")
    print("   - Press SPACE to trigger overwrite dialog")
    print("   - Test overwrite functionality")
    print("4. Check console output for debug messages")
