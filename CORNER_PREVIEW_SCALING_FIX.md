# Corner Preview Scaling Fix

## Issue
When rotating a vertical asset (like TallTower 1x5) to horizontal (5x1) in the corner preview:
- ✅ Preview box correctly changed dimensions (tall → wide)
- ❌ Asset sprite didn't scale properly to fill the entire rotated box
- Result: Asset appeared small/incorrectly sized within the preview box

## Root Cause
The scaling calculation wasn't accounting for how texture dimensions map to preview dimensions after rotation:

### Before (Incorrect)
```cpp
// Always used the same mapping regardless of rotation
float spriteScaleX = previewWidth / static_cast<float>(textureSize.x);
float spriteScaleY = previewHeight / static_cast<float>(textureSize.y);
```

This meant:
- **0° rotation**: TallTower texture (tall) → preview box (tall) ✅ Correct
- **90° rotation**: TallTower texture (tall) → preview box (wide) ❌ Wrong mapping!

## Solution Applied

### After (Correct)
```cpp
if (m_currentRotation == 90.0f || m_currentRotation == 270.0f) {
    // For 90°/270° rotations, texture width maps to preview height and vice versa
    spriteScaleX = previewWidth / static_cast<float>(textureSize.y);
    spriteScaleY = previewHeight / static_cast<float>(textureSize.x);
} else {
    // For 0°/180° rotations, normal mapping
    spriteScaleX = previewWidth / static_cast<float>(textureSize.x);
    spriteScaleY = previewHeight / static_cast<float>(textureSize.y);
}
```

## How It Works

### TallTower Example (texture is tall: width=50, height=200)

#### 0° Rotation (Vertical)
- Preview box: tall (80×200)
- Texture mapping: width→width, height→height
- Scale: X = 80/50, Y = 200/200
- Result: ✅ Fills entire tall preview box

#### 90° Rotation (Horizontal)  
- Preview box: wide (200×80)
- Texture mapping: **width→height, height→width** (swapped!)
- Scale: X = 200/200, Y = 80/50
- Result: ✅ Fills entire wide preview box

## Visual Result

### Before Fix
- 0°: TallTower fills preview box ✅
- 90°: TallTower appears small in corner of wide preview box ❌

### After Fix  
- 0°: TallTower fills preview box ✅
- 90°: TallTower fills entire wide preview box ✅
- 180°: TallTower fills preview box (upside down) ✅
- 270°: TallTower fills entire wide preview box (opposite direction) ✅

## Key Insight
When rotating a sprite 90° or 270°:
- The sprite's **width** becomes the screen's **height**
- The sprite's **height** becomes the screen's **width**
- Scaling calculations must account for this dimension swap

## Files Modified
- `src/scenes/scene_map_editor.cpp`: Fixed scaling calculation in `drawAssetPreview()`

The corner preview now correctly shows rotated assets filling the entire preview box, providing accurate visual feedback for asset placement.
