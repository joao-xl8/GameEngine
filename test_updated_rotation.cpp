#include <iostream>

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
        int rotatedWidth = height;
        placement.x = cursorX - (rotatedWidth - 1);
        placement.y = cursorY;
        
        if (placement.x < 0) {
            placement.x = 0;
            std::cout << "Warning: Adjusted placement to prevent negative X coordinate" << std::endl;
        }
    }
    else if (rotation == 180.0f) {
        placement.x = cursorX - (width - 1);
        placement.y = cursorY - (height - 1);
        
        if (placement.x < 0) {
            placement.x = 0;
            std::cout << "Warning: Adjusted placement to prevent negative X coordinate" << std::endl;
        }
        if (placement.y < 0) {
            placement.y = 0;
            std::cout << "Warning: Adjusted placement to prevent negative Y coordinate" << std::endl;
        }
    }
    else if (rotation == 270.0f) {
        int rotatedHeight = width;
        placement.x = cursorX;
        placement.y = cursorY - (rotatedHeight - 1);
        
        if (placement.y < 0) {
            placement.y = 0;
            std::cout << "Warning: Adjusted placement to prevent negative Y coordinate" << std::endl;
        }
    }
    
    return placement;
}

void testCase(const std::string& name, int width, int height, int cursorX, int cursorY, float rotation) {
    std::cout << "\n=== " << name << " ===" << std::endl;
    std::cout << "Asset: " << width << "x" << height << ", Cursor: (" << cursorX << ", " << cursorY << "), Rotation: " << rotation << "°" << std::endl;
    
    Vec2 placement = calculateRotatedPlacement(cursorX, cursorY, width, height, rotation);
    
    int finalWidth = width;
    int finalHeight = height;
    if (rotation == 90.0f || rotation == 270.0f) {
        std::swap(finalWidth, finalHeight);
    }
    
    std::cout << "Placement: (" << placement.x << ", " << placement.y << ")" << std::endl;
    std::cout << "Final dimensions: " << finalWidth << "x" << finalHeight << std::endl;
    std::cout << "Occupies: (" << placement.x << ", " << placement.y << ") to (" 
              << (placement.x + finalWidth - 1) << ", " << (placement.y + finalHeight - 1) << ")" << std::endl;
}

int main() {
    std::cout << "=== Testing Updated Rotation Logic ===" << std::endl;
    
    // Test the problematic cases
    testCase("TallTower near origin - 90°", 1, 5, 2, 2, 90.0f);
    testCase("TallTower at origin - 90°", 1, 5, 0, 0, 90.0f);
    testCase("TallTower normal - 90°", 1, 5, 10, 10, 90.0f);
    testCase("TallTower normal - 270°", 1, 5, 10, 10, 270.0f);
    
    return 0;
}
