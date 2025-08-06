#pragma once

// Main Components Header - Now organized into categories
// This maintains backward compatibility while providing better organization

#include "components/common_components.hpp"
#include "components/engine_components.hpp"
#include "components/game_components.hpp"

// All components are now available through the organized structure:
// - Engine components: CTransform, CSprite, CAnimation, CBoundingBox, CInput, CSound, CCamera
// - Common components: CGridMovement, CLifespan, CScore, CState, CText, CHealth, CTimer  
// - Game components: CCharacter, CBattleSystem, CInventory, CDialogue, CEncounterZone, CShop, CQuest

// This file maintains backward compatibility with existing code
// while providing access to the new organized component structure
