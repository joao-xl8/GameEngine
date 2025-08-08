#pragma once
#include "base_component.hpp"

// Layer-based rendering component for 5-layer world system
// Rendering order: 0 -> 1 -> 2 -> 3 -> 4
class CLayer : public Component {
public:
    enum LayerType {
        GROUND = 0,      // Layer 0: Ground tiles - NO collision, base walkable surface
        DECORATION_1 = 1, // Layer 1: First decoration layer - HAS collision
        DECORATION_2 = 2, // Layer 2: Second decoration layer - HAS collision  
        DECORATION_3 = 3, // Layer 3: Third decoration layer - HAS collision
        ENTITY = 4       // Layer 4: Entity layer - NPCs, Script Tiles, player "habitat"
    };
    
    LayerType layer;
    int subLayer;  // For fine-grained ordering within the same layer (0 = bottom, higher = top)
    
    CLayer(LayerType l, int sub = 0) : layer(l), subLayer(sub) {}
    ~CLayer() {}
    
    // Helper methods
    static const char* getLayerName(LayerType layer) {
        switch(layer) {
            case GROUND: return "Ground";
            case DECORATION_1: return "Decoration_1";
            case DECORATION_2: return "Decoration_2";
            case DECORATION_3: return "Decoration_3";
            case ENTITY: return "Entity";
            default: return "Unknown";
        }
    }
    
    // Get total rendering order for sorting (layer * 100 + subLayer)
    int getRenderOrder() const {
        return static_cast<int>(layer) * 100 + subLayer;
    }
    
    // Check if this layer should have collision
    bool hasCollision() const {
        return layer >= DECORATION_1 && layer <= DECORATION_3;
    }
    
    // Check if this is the entity layer (for NPCs, Script Tiles)
    bool isEntityLayer() const {
        return layer == ENTITY;
    }
};
