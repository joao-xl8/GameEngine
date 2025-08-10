#include <iostream>

// Simulate the calculateRotatedPlacement function
struct Vec2 {
    float x, y;
};

Vec2 calculateRotatedPlacement(int cursorX, int cursorY, int width, int height, float rotation)
{
    Vec2 placement;
    
    if (rotation == 0.0f) {
        placement.x = cursorX;
        placement.y = cursorY;
    }
    else if (rotation == 90.0f) {
        int rotatedWidth = height;  // After 90° rotation
        placement.x = cursorX - (rotatedWidth - 1);
        placement.y = cursorY;
    }
    else if (rotation == 180.0f) {
        placement.x = cursorX - (width - 1);
        placement.y = cursorY - (height - 1);
    }
    else if (rotation == 270.0f) {
        int rotatedHeight = width;  // After 270° rotation
        placement.x = cursorX;
        placement.y = cursorY - (rotatedHeight - 1);
    }
    
    return placement;
}

void testRotation(const std::string& assetName, int width, int height, int cursorX, int cursorY, float rotation) {
    std::cout << "\n=== Testing " << assetName << " (" << width << "x" << height << ") ===" << std::endl;
    std::cout << "Cursor at: (" << cursorX << ", " << cursorY << ")" << std::endl;
    std::cout << "Rotation: " << rotation << "°" << std::endl;
    
    Vec2 placement = calculateRotatedPlacement(cursorX, cursorY, width, height, rotation);
    std::cout << "Calculated placement: (" << placement.x << ", " << placement.y << ")" << std::endl;
    
    // Apply rotation to dimensions
    int finalWidth = width;
    int finalHeight = height;
    if (rotation == 90.0f || rotation == 270.0f) {
        std::swap(finalWidth, finalHeight);
    }
    
    std::cout << "Final dimensions: " << finalWidth << "x" << finalHeight << std::endl;
    std::cout << "Asset will occupy: (" << placement.x << ", " << placement.y << ") to (" 
              << (placement.x + finalWidth - 1) << ", " << (placement.y + finalHeight - 1) << ")" << std::endl;
    
    // Check if cursor is within the final asset area
    bool cursorInAsset = (cursorX >= placement.x && cursorX < placement.x + finalWidth &&
                         cursorY >= placement.y && cursorY < placement.y + finalHeight);
    std::cout << "Cursor within asset area: " << (cursorInAsset ? "YES" : "NO") << std::endl;
}

int main() {
    std::cout << "=== Rotation Placement Debug Test ===" << std::endl;
    
    // Test WideWall (5x1) at different rotations
    testRotation("WideWall", 5, 1, 10, 5, 0.0f);
    testRotation("WideWall", 5, 1, 10, 5, 90.0f);
    testRotation("WideWall", 5, 1, 10, 5, 180.0f);
    testRotation("WideWall", 5, 1, 10, 5, 270.0f);
    
    // Test TallTower (1x5) at different rotations
    testRotation("TallTower", 1, 5, 10, 12, 0.0f);
    testRotation("TallTower", 1, 5, 10, 12, 90.0f);
    testRotation("TallTower", 1, 5, 10, 12, 180.0f);
    testRotation("TallTower", 1, 5, 10, 12, 270.0f);
    
    return 0;
}
