#!/usr/bin/env python3
"""
Level file validation script for GameEngine Map Editor
Validates that level files follow the correct format and contain valid data
Now supports infinite grid coordinates and the new levels/ directory structure!
"""

import sys
import os
import re

def validate_level_file(filename):
    """Validate a level file format"""
    if not os.path.exists(filename):
        print(f"Error: File {filename} does not exist")
        return False
    
    valid_types = {'Tile', 'Dec'}
    valid_assets = {'Ground', 'Wall', 'Bush', 'Player'}
    
    errors = []
    warnings = []
    line_count = 0
    object_count = 0
    
    # Track coordinate bounds for statistics
    min_x, max_x = float('inf'), float('-inf')
    min_y, max_y = float('inf'), float('-inf')
    
    print(f"Validating level file: {filename}")
    print("=" * 50)
    
    with open(filename, 'r') as f:
        for line_num, line in enumerate(f, 1):
            line_count += 1
            line = line.strip()
            
            # Skip empty lines and comments
            if not line or line.startswith('#'):
                continue
            
            # Parse line format: Type Asset X Y
            parts = line.split()
            if len(parts) != 4:
                errors.append(f"Line {line_num}: Invalid format '{line}' - expected 'Type Asset X Y'")
                continue
            
            obj_type, asset, x_str, y_str = parts
            
            # Validate type
            if obj_type not in valid_types:
                errors.append(f"Line {line_num}: Invalid type '{obj_type}' - must be one of {valid_types}")
            
            # Validate asset
            if asset not in valid_assets:
                warnings.append(f"Line {line_num}: Unknown asset '{asset}' - may not have texture")
            
            # Validate coordinates (now supports infinite range)
            try:
                x, y = int(x_str), int(y_str)
                
                # Update bounds for statistics
                min_x = min(min_x, x)
                max_x = max(max_x, x)
                min_y = min(min_y, y)
                max_y = max(max_y, y)
                
                # Optional: warn about extremely large coordinates
                if abs(x) > 10000 or abs(y) > 10000:
                    warnings.append(f"Line {line_num}: Very large coordinates ({x}, {y}) - may impact performance")
                    
            except ValueError:
                errors.append(f"Line {line_num}: Invalid coordinates '{x_str}, {y_str}' - must be integers")
            
            object_count += 1
    
    # Print results
    print(f"Lines processed: {line_count}")
    print(f"Objects found: {object_count}")
    
    if object_count > 0:
        print(f"Coordinate bounds: X({min_x} to {max_x}), Y({min_y} to {max_y})")
        print(f"Level dimensions: {max_x - min_x + 1} x {max_y - min_y + 1}")
    
    print()
    
    if warnings:
        print("WARNINGS:")
        for warning in warnings:
            print(f"  {warning}")
        print()
    
    if errors:
        print("ERRORS:")
        for error in errors:
            print(f"  {error}")
        print()
        print("❌ Validation FAILED")
        return False
    else:
        print("✅ Validation PASSED")
        if warnings:
            print("⚠️  File has warnings but is valid")
        else:
            print("🎉 Perfect level file!")
        return True

def main():
    if len(sys.argv) != 2:
        print("Usage: python3 validate_level.py <level_file.txt>")
        print("\nThis validator now supports infinite grid coordinates!")
        print("No more 20x15 size limitations - place objects anywhere!")
        print("\nNew directory structure:")
        print("  metadata/levels/     - Level files (.txt)")
        print("  metadata/assets.txt  - Asset configuration")
        print("\nExamples:")
        print("  python3 validate_level.py metadata/levels/level1.txt")
        print("  python3 validate_level.py metadata/levels/infinite_test.txt")
        sys.exit(1)
    
    filename = sys.argv[1]
    
    # Auto-prepend levels directory if just filename given
    if not filename.startswith("metadata/") and not os.path.exists(filename):
        levels_path = f"metadata/levels/{filename}"
        if os.path.exists(levels_path):
            filename = levels_path
            print(f"Auto-detected level file: {filename}")
    
    success = validate_level_file(filename)
    sys.exit(0 if success else 1)

if __name__ == "__main__":
    main()
