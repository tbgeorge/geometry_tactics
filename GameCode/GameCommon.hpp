//=================================================================================
// GameCommon.hpp
// Author: Tyler George
// Date  : November 7, 2015
//=================================================================================

#pragma once
#ifndef __included_GameCommon__
#define __included_GameCommon__

///---------------------------------------------------------------------------------
/// Includes
///---------------------------------------------------------------------------------
#include "Engine/Common/EngineCommon.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/IntVector2.hpp"
#include "Engine/Sound/SoundSystem.hpp"
class Cell;
class Actor;
class InputSystem;
class OpenGLRenderer;

///---------------------------------------------------------------------------------
/// Typedefs
///---------------------------------------------------------------------------------
typedef IntVector2 MapPosition;
typedef std::vector< MapPosition > MapPositions;
typedef std::vector< Cell* > CellPtrs;
typedef std::vector<Actor*> Actors;

///---------------------------------------------------------------------------------
/// Structs
///---------------------------------------------------------------------------------
struct RaycastResult
{
    bool didHit;
    Vector3 hitLocation;
    Cell* cellHit;
};


///---------------------------------------------------------------------------------
/// Helper Functions
///---------------------------------------------------------------------------------
RaycastResult DoRaycast( const Vector3& start, const Vector3& end, const int& numSteps );
RaycastResult DoMouseToWorldRaycast( InputSystem* inputSystem, OpenGLRenderer* renderer );

void InitializeCommonSounds();


///---------------------------------------------------------------------------------
/// Globals
///---------------------------------------------------------------------------------

// sound IDs - audio system not currently working
//extern SoundID g_cancel;
//extern SoundID g_confirm;
//extern SoundID g_cursorA;
//extern SoundID g_cursorB;
//extern SoundID g_deathMale;
//extern SoundID g_deathMonster;
//extern SoundID g_helpPopup;
//extern SoundID g_hitA;
//extern SoundID g_hitB;
//extern SoundID g_interrupt;
//extern SoundID g_invalid;
//extern SoundID g_heal;
//extern SoundID g_explosion;
//extern SoundID g_arrow;



#endif




