#!/usr/bin/env python3

import os
import json
import time

def create_test_save():
    """Create a test save file to test overwrite functionality"""
    saves_dir = "saves"
    if not os.path.exists(saves_dir):
        os.makedirs(saves_dir)
    
    test_save = {
        "saveName": "Test Save for Overwrite",
        "saveTime": int(time.time()),
        "currentLevel": "metadata/levels/level1.txt",
        "levelName": "Test Level",
        "playerPosX": 640.0,
        "playerPosY": 448.0,
        "playerHealth": 100,
        "playTimeSeconds": 3600
    }
    
    save_file = os.path.join(saves_dir, "save_slot_1.json")
    with open(save_file, 'w') as f:
        json.dump(test_save, f, indent=2)
    
    print(f"Created test save file: {save_file}")
    print(f"Save content: {test_save}")

def check_save_files():
    """Check what save files exist"""
    saves_dir = "saves"
    if not os.path.exists(saves_dir):
        print("No saves directory found")
        return
    
    save_files = [f for f in os.listdir(saves_dir) if f.endswith('.json')]
    print(f"Found {len(save_files)} save files:")
    
    for save_file in save_files:
        file_path = os.path.join(saves_dir, save_file)
        try:
            with open(file_path, 'r') as f:
                data = json.load(f)
            print(f"  {save_file}: {data.get('saveName', 'Unknown')} - {data.get('levelName', 'Unknown Level')}")
        except Exception as e:
            print(f"  {save_file}: Error reading file - {e}")

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
    print("2. Go to main menu and select a save point (red square)")
    print("3. Press E near a save point to open save menu")
    print("4. Navigate to slot 1 (should show 'Test Save for Overwrite')")
    print("5. Press SPACE to trigger overwrite dialog")
    print("6. Use arrow keys to select YES")
    print("7. Press SPACE to confirm overwrite")
    print("8. Check console output for debug messages")
