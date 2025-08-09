#pragma once

// Main Components Header - Organized into 2 categories
// This maintains backward compatibility while providing better organization

#include "components/base_component.hpp"
#include "components/engine_components.hpp"
#include "components/game_components.hpp"

// All components are now available through the organized structure:
// 
// Engine Components (Core engine systems):
// - CTransform: Position, velocity, scale, rotation
// - CSprite: Texture rendering
// - CAnimation: Animation system  
// - CBoundingBox: Collision detection
// - CInput: Input handling
// - CSound: Audio system
// - CCamera: Camera following system
// - CLayer: Layer-based rendering (0-4)
// - CScriptTile: Interactive tiles that trigger actions
// - CSave: Save point system
//
// Game Components (Game-specific logic that affects gameplay):
// - CNPCDialogue: NPC dialogue state management
// - CNPCInteraction: NPC interaction rules and requirements
// - CPlayerMovement: Player-specific movement with grid movement
// - CPlayerInput: Player input handling with action mapping
// - CPlayerStats: Player stats and progression system
// - CPlayerInventory: Player inventory management
// - CPlayerState: Player state management (idle, walking, etc.)
// - All RPG-related components: CCharacter, CBattleSystem, CInventory, etc.

// This file maintains backward compatibility with existing code
// while providing access to the new organized component structure
