#!/usr/bin/env python3

import os
import time

def create_spawn_test_levels():
    """Create test levels with different PlayerSpawn positions"""
    levels_dir = "metadata/levels"
    if not os.path.exists(levels_dir):
        os.makedirs(levels_dir)
    
    # Test Level 1: PlayerSpawn at bottom-right
    test_level_1 = """# PlayerSpawn Test Level 1 - Bottom Right Spawn
# Format: Type SpriteName X Y

# Simple 6x6 level
Tile Wall 0 0
Tile Wall 1 0
Tile Wall 2 0
Tile Wall 3 0
Tile Wall 4 0
Tile Wall 5 0

Tile Wall 0 1
Tile Wall 5 1
Tile Wall 0 2
Tile Wall 5 2
Tile Wall 0 3
Tile Wall 5 3
Tile Wall 0 4
Tile Wall 5 4

Tile Wall 0 5
Tile Wall 1 5
Tile Wall 2 5
Tile Wall 3 5
Tile Wall 4 5
Tile Wall 5 5

# Ground tiles
Dec Ground 1 1
Dec Ground 2 1
Dec Ground 3 1
Dec Ground 4 1
Dec Ground 1 2
Dec Ground 2 2
Dec Ground 3 2
Dec Ground 4 2
Dec Ground 1 3
Dec Ground 2 3
Dec Ground 3 3
Dec Ground 4 3
Dec Ground 1 4
Dec Ground 2 4
Dec Ground 3 4
Dec Ground 4 4

# PlayerSpawn at bottom-right
Dec PlayerSpawn 4 4

# Save point
Dec SavePoint 1 1
"""

    # Test Level 2: PlayerSpawn at top-left
    test_level_2 = """# PlayerSpawn Test Level 2 - Top Left Spawn
# Format: Type SpriteName X Y

# Simple 6x6 level
Tile Wall 0 0
Tile Wall 1 0
Tile Wall 2 0
Tile Wall 3 0
Tile Wall 4 0
Tile Wall 5 0

Tile Wall 0 1
Tile Wall 5 1
Tile Wall 0 2
Tile Wall 5 2
Tile Wall 0 3
Tile Wall 5 3
Tile Wall 0 4
Tile Wall 5 4

Tile Wall 0 5
Tile Wall 1 5
Tile Wall 2 5
Tile Wall 3 5
Tile Wall 4 5
Tile Wall 5 5

# Ground tiles
Dec Ground 1 1
Dec Ground 2 1
Dec Ground 3 1
Dec Ground 4 1
Dec Ground 1 2
Dec Ground 2 2
Dec Ground 3 2
Dec Ground 4 2
Dec Ground 1 3
Dec Ground 2 3
Dec Ground 3 3
Dec Ground 4 3
Dec Ground 1 4
Dec Ground 2 4
Dec Ground 3 4
Dec Ground 4 4

# PlayerSpawn at top-left
Dec PlayerSpawn 1 1

# Save point
Dec SavePoint 4 4
"""

    # Test Level 3: No PlayerSpawn (should use fallback)
    test_level_3 = """# PlayerSpawn Test Level 3 - No PlayerSpawn (Fallback Test)
# Format: Type SpriteName X Y

# Simple 6x6 level
Tile Wall 0 0
Tile Wall 1 0
Tile Wall 2 0
Tile Wall 3 0
Tile Wall 4 0
Tile Wall 5 0

Tile Wall 0 1
Tile Wall 5 1
Tile Wall 0 2
Tile Wall 5 2
Tile Wall 0 3
Tile Wall 5 3
Tile Wall 0 4
Tile Wall 5 4

Tile Wall 0 5
Tile Wall 1 5
Tile Wall 2 5
Tile Wall 3 5
Tile Wall 4 5
Tile Wall 5 5

# Ground tiles
Dec Ground 1 1
Dec Ground 2 1
Dec Ground 3 1
Dec Ground 4 1
Dec Ground 1 2
Dec Ground 2 2
Dec Ground 3 2
Dec Ground 4 2
Dec Ground 1 3
Dec Ground 2 3
Dec Ground 3 3
Dec Ground 4 3
Dec Ground 1 4
Dec Ground 2 4
Dec Ground 3 4
Dec Ground 4 4

# NO PlayerSpawn tile - should use fallback position

# Save point
Dec SavePoint 2 2
"""

    test_levels = [
        ("spawn_test_bottom_right.txt", test_level_1, "Bottom-right spawn (4, 4) = World (256, 256)"),
        ("spawn_test_top_left.txt", test_level_2, "Top-left spawn (1, 1) = World (64, 64)"),
        ("spawn_test_no_spawn.txt", test_level_3, "No PlayerSpawn - fallback to (10, 7) = World (640, 448)")
    ]
    
    for filename, content, description in test_levels:
        filepath = os.path.join(levels_dir, filename)
        with open(filepath, 'w') as f:
            f.write(content)
        print(f"Created {filename}: {description}")

def create_spawn_test_saves():
    """Create saves that load the test levels"""
    saves_dir = "saves"
    if not os.path.exists(saves_dir):
        os.makedirs(saves_dir)
    
    test_saves = [
        {
            "slot": 10,
            "name": "Test Bottom Right Spawn",
            "level": "metadata/levels/spawn_test_bottom_right.txt",
            "description": "New game should spawn at bottom-right (256, 256)"
        },
        {
            "slot": 11,
            "name": "Test Top Left Spawn", 
            "level": "metadata/levels/spawn_test_top_left.txt",
            "description": "New game should spawn at top-left (64, 64)"
        },
        {
            "slot": 12,
            "name": "Test No Spawn Fallback",
            "level": "metadata/levels/spawn_test_no_spawn.txt", 
            "description": "New game should spawn at fallback (640, 448)"
        }
    ]
    
    for save_data in test_saves:
        save_file = os.path.join(saves_dir, f"save_slot_{save_data['slot']}.sav")
        with open(save_file, 'w') as f:
            f.write("SAVE_VERSION 1.0\n")
            f.write(f"SAVE_NAME {save_data['name']}\n")
            f.write("LEVEL_NAME Spawn Test\n")
            f.write(f"CURRENT_LEVEL {save_data['level']}\n")
            f.write("PLAYER_X 999\n")  # Dummy position - should be overridden by PlayerSpawn
            f.write("PLAYER_Y 999\n")  # Dummy position - should be overridden by PlayerSpawn
            f.write("PLAYER_HEALTH 100\n")
            f.write(f"SAVE_TIME {int(time.time())}\n")
            f.write("PLAY_TIME 0\n")  # New game
            f.write("FLAGS_START\n")
            f.write("FLAGS_END\n")
            f.write("VARIABLES_START\n")
            f.write("VARIABLES_END\n")
        
        print(f"Created save slot {save_data['slot']}: {save_data['description']}")

if __name__ == "__main__":
    print("=== PlayerSpawn System Test Setup ===")
    print("1. Creating test levels with different PlayerSpawn positions...")
    create_spawn_test_levels()
    
    print("\n2. Creating test saves for new games...")
    create_spawn_test_saves()
    
    print("\n=== Test Instructions ===")
    print("1. Run the game: ./build/release/GameEngine")
    print("2. Test Map Editor:")
    print("   - Go to Map Editor")
    print("   - Press Q/E to cycle through assets")
    print("   - Find 'PlayerSpawn' asset")
    print("   - Place PlayerSpawn tiles in levels")
    print("   - Save levels and test them")
    print("3. Test PlayerSpawn system:")
    print("   a. Start new game (should use PlayerSpawn from level1.txt at (3,7) = (192, 448))")
    print("   b. Load slot 10 (should spawn at bottom-right: 256, 256)")
    print("   c. Load slot 11 (should spawn at top-left: 64, 64)")
    print("   d. Load slot 12 (should spawn at fallback: 640, 448 with warning)")
    print("4. Check console output:")
    print("   - Look for 'Found PlayerSpawn at position...'")
    print("   - Look for 'Using level spawn position from PlayerSpawn tile...'")
    print("   - Look for 'Warning: No PlayerSpawn tile found...' (slot 12 only)")
    print("5. Test save system:")
    print("   - Move player to different position")
    print("   - Save game")
    print("   - Load that save - should spawn at saved position (overrides PlayerSpawn)")
    
    print("\n=== Expected Debug Output ===")
    print("New Game (level1.txt):")
    print("  Found PlayerSpawn at position (3, 7) -> world pos (192.0, 448.0)")
    print("  Using level spawn position from PlayerSpawn tile: 192.0, 448.0")
    print("  Initialized grid position to: 3, 7")
    print("\nSlot 10 (Bottom Right):")
    print("  Found PlayerSpawn at position (4, 4) -> world pos (256.0, 256.0)")
    print("  Using level spawn position from PlayerSpawn tile: 256.0, 256.0")
    print("  Initialized grid position to: 4, 4")
    print("\nSlot 12 (No Spawn):")
    print("  Using fallback default spawn position: 640.0, 448.0")
    print("  Warning: No PlayerSpawn tile found in level, using hardcoded position")
    print("  Initialized grid position to: 10, 7")
    
    print("\n=== Map Editor Test ===")
    print("1. Open Map Editor")
    print("2. Press Q/E to find PlayerSpawn asset")
    print("3. Press Z/C to select 'Dec' type")
    print("4. Place PlayerSpawn tile where you want players to start")
    print("5. Save level and test it")
    print("6. PlayerSpawn tiles are visible in the editor and level files")
