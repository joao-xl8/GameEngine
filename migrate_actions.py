#!/usr/bin/env python3
"""
Script to help migrate hardcoded action strings to use ActionTypes constants.
This script will show you what needs to be changed in each file.
"""

import os
import re
from pathlib import Path

# Define the action mappings
ACTION_MAPPINGS = {
    '"UP"': 'ActionTypes::UP',
    '"DOWN"': 'ActionTypes::DOWN',
    '"LEFT"': 'ActionTypes::LEFT',
    '"RIGHT"': 'ActionTypes::RIGHT',
    '"CONFIRM"': 'ActionTypes::CONFIRM',
    '"SELECT"': 'ActionTypes::SELECT',
    '"ACCEPT"': 'ActionTypes::ACCEPT',
    '"YES"': 'ActionTypes::YES',
    '"BACK"': 'ActionTypes::BACK',
    '"CANCEL"': 'ActionTypes::CANCEL',
    '"NO"': 'ActionTypes::NO',
    '"ESCAPE"': 'ActionTypes::ESCAPE',
    '"QUIT"': 'ActionTypes::QUIT',
    '"PAUSE"': 'ActionTypes::PAUSE',
    '"MENU"': 'ActionTypes::MENU',
    '"INTERACT"': 'ActionTypes::INTERACT',
    '"ATTACK"': 'ActionTypes::ATTACK',
    '"DEFEND"': 'ActionTypes::DEFEND',
    '"USE_ITEM"': 'ActionTypes::USE_ITEM',
    '"CAST_SPELL"': 'ActionTypes::CAST_SPELL',
    '"SHOW_LOG"': 'ActionTypes::SHOW_LOG',
    '"TOGGLE_INVENTORY"': 'ActionTypes::TOGGLE_INVENTORY',
    '"SAVE"': 'ActionTypes::SAVE',
    '"LOAD"': 'ActionTypes::LOAD',
}

def find_scene_files():
    """Find all scene .cpp and .hpp files"""
    scene_dir = Path("src/scenes")
    if not scene_dir.exists():
        print("Scene directory not found!")
        return []
    
    files = []
    for ext in ['*.cpp', '*.hpp']:
        files.extend(scene_dir.glob(ext))
    return files

def analyze_file(file_path):
    """Analyze a file for action string usage"""
    try:
        with open(file_path, 'r') as f:
            content = f.read()
        
        changes_needed = []
        
        # Check for registerAction calls
        register_pattern = r'registerAction\([^,]+,\s*("[^"]+")(?:\s*,\s*"[^"]*")?\)'
        for match in re.finditer(register_pattern, content):
            action_str = match.group(1)
            if action_str in ACTION_MAPPINGS:
                changes_needed.append({
                    'type': 'registerAction',
                    'old': action_str,
                    'new': ACTION_MAPPINGS[action_str],
                    'line': content[:match.start()].count('\n') + 1
                })
        
        # Check for action.getName() comparisons
        getname_pattern = r'action\.getName\(\)\s*==\s*("[^"]+")(?!\s*\))'
        for match in re.finditer(getname_pattern, content):
            action_str = match.group(1)
            if action_str in ACTION_MAPPINGS:
                changes_needed.append({
                    'type': 'getName comparison',
                    'old': action_str,
                    'new': ACTION_MAPPINGS[action_str],
                    'line': content[:match.start()].count('\n') + 1
                })
        
        # Check if action_types.hpp is included
        has_include = '#include "../action_types.hpp"' in content or '#include "action_types.hpp"' in content
        
        return {
            'file': file_path,
            'changes_needed': changes_needed,
            'has_include': has_include,
            'needs_include': len(changes_needed) > 0 and not has_include
        }
        
    except Exception as e:
        print(f"Error analyzing {file_path}: {e}")
        return None

def main():
    print("=== Action Migration Analysis ===\n")
    
    scene_files = find_scene_files()
    if not scene_files:
        print("No scene files found!")
        return
    
    total_changes = 0
    files_needing_changes = 0
    
    for file_path in sorted(scene_files):
        analysis = analyze_file(file_path)
        if not analysis:
            continue
            
        if analysis['changes_needed'] or analysis['needs_include']:
            files_needing_changes += 1
            print(f"📁 {analysis['file']}")
            
            if analysis['needs_include']:
                print("   ⚠️  Needs #include \"../action_types.hpp\"")
            
            for change in analysis['changes_needed']:
                print(f"   📝 Line {change['line']}: {change['type']}")
                print(f"      Old: {change['old']}")
                print(f"      New: {change['new']}")
                total_changes += 1
            
            print()
    
    print(f"Summary:")
    print(f"  Files needing changes: {files_needing_changes}")
    print(f"  Total changes needed: {total_changes}")
    
    if files_needing_changes > 0:
        print(f"\nTo complete the migration:")
        print(f"1. Add #include \"../action_types.hpp\" to files that need it")
        print(f"2. Replace the old string literals with ActionTypes constants")
        print(f"3. Test compilation and functionality")

if __name__ == "__main__":
    main()
