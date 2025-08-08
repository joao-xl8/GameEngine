#!/usr/bin/env python3
"""
Simple test to verify the viewport configuration menu is working.
This script will launch the game and provide instructions for manual testing.
"""

import subprocess
import time
import sys

def main():
    print("🎮 GameEngine Viewport Configuration Test")
    print("=" * 50)
    print()
    print("This test will launch the GameEngine and guide you through testing")
    print("the viewport configuration menu.")
    print()
    print("📋 Test Steps:")
    print("1. Game will launch and show the main menu")
    print("2. Navigate to 'Viewport Config' using arrow keys or WASD")
    print("3. Press Space or Enter to enter the viewport configuration")
    print("4. Verify you can see:")
    print("   - Title: 'Viewport Configuration'")
    print("   - Instructions text")
    print("   - Menu options with current values")
    print("   - Current configuration display at bottom")
    print("5. Test navigation with arrow keys/WASD")
    print("6. Test changing values with left/right arrows")
    print("7. Test 'Apply Changes' to see resolution changes")
    print("8. Press Escape to return to main menu")
    print()
    
    input("Press Enter to start the test...")
    print()
    print("🚀 Launching GameEngine...")
    print("(The game will run for 30 seconds, then automatically close)")
    print()
    
    try:
        # Launch the game
        process = subprocess.Popen(
            ["./build/release/GameEngine"],
            cwd="/Users/joaopauloarnoldbarros/Developer/GameEngine",
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        )
        
        # Let it run for 30 seconds
        time.sleep(30)
        
        # Terminate the process
        process.terminate()
        process.wait(timeout=5)
        
        print("✅ Test completed!")
        print()
        print("🔍 Expected Results:")
        print("- Main menu should be visible and centered")
        print("- 'Viewport Config' option should be in the menu")
        print("- Viewport config scene should show properly centered content")
        print("- All text should be readable and properly positioned")
        print("- Background should cover the entire view")
        print()
        print("❓ If you saw a black screen in viewport config:")
        print("- This indicates a positioning issue that needs fixing")
        print("- Check that game view coordinates are being used correctly")
        print()
        
    except subprocess.TimeoutExpired:
        print("⚠️  Game process didn't terminate cleanly")
        process.kill()
    except FileNotFoundError:
        print("❌ Error: GameEngine executable not found!")
        print("Make sure to build the project first with 'make'")
        return 1
    except Exception as e:
        print(f"❌ Error running test: {e}")
        return 1
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
