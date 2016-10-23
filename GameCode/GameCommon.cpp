//=================================================================================
// GameCommon.cpp
// Author: Tyler George
// Date  : November 7, 2015
//=================================================================================

////===========================================================================================
///===========================================================================================
// Includes
///===========================================================================================
////===========================================================================================

#include "GameCode/GameCommon.hpp"
#include "GameCode/Game.hpp"

////===========================================================================================
///===========================================================================================
// Global Initialization
///===========================================================================================
////===========================================================================================
//SoundID g_cancel = -1;
//SoundID g_confirm = -1;
//SoundID g_cursorA = -1;
//SoundID g_cursorB = -1;
//SoundID g_deathMale = -1;
//SoundID g_deathMonster = -1;
//SoundID g_helpPopup = -1;
//SoundID g_hitA = -1;
//SoundID g_hitB = -1;
//SoundID g_interrupt = -1;
//SoundID g_invalid = -1;
//SoundID g_heal = -1;
//SoundID g_arrow = -1;
//SoundID g_explosion = -1;

////===========================================================================================
///===========================================================================================
// Helper Functions
///===========================================================================================
////===========================================================================================


///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
RaycastResult DoRaycast( const Vector3& start, const Vector3& end, const int& numSteps )
{
    Vector3 currentPos = start;

    Vector3 direction = (end - start).Normalized();
    float stepSize = CalcDistance( start, end ) / numSteps;

    RaycastResult result;
    result.didHit = false;
    result.cellHit = nullptr;
    result.hitLocation = start;

    Map* map = Game::GetGameInstance()->GetCurrentMap();
    for (int step = 0; step < numSteps; ++step)
    {
        Cell* cellAtCurrentPos = map->GetCellAtWorldPosition( currentPos );
        if (cellAtCurrentPos)
        {
            result.didHit = true;
            result.cellHit = cellAtCurrentPos;
            result.hitLocation = currentPos;

            return result;
        }

        currentPos += (direction * stepSize);
    }

    return result;

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
RaycastResult DoMouseToWorldRaycast( InputSystem* inputSystem, OpenGLRenderer* renderer )
{
    // Mouse world position
    Vector3 nearWorldPos = inputSystem->GetWNormalizedMouseNearPosition( renderer );
    Vector3 farWorldPos = inputSystem->GetWNormalizedMouseFarPosition( renderer );

    return DoRaycast( nearWorldPos, farWorldPos, 8000 );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void InitializeCommonSounds()
{
    //g_cancel = s_theSoundSystem->LoadStreamingSound( "Data/Sounds/cancel.ogg", 3 );
    //g_confirm = s_theSoundSystem->LoadStreamingSound( "Data/Sounds/confirm.ogg", 3 );
    //g_cursorA = s_theSoundSystem->LoadStreamingSound( "Data/Sounds/cursor_a.ogg", 3 );
    //g_cursorB = s_theSoundSystem->LoadStreamingSound( "Data/Sounds/cursor_b.ogg", 3 );
    //g_deathMale = s_theSoundSystem->LoadStreamingSound( "Data/Sounds/death_male.ogg", 3 );
    //g_deathMonster = s_theSoundSystem->LoadStreamingSound( "Data/Sounds/death_monster.ogg", 3 );
    //g_helpPopup = s_theSoundSystem->LoadStreamingSound( "Data/Sounds/help_popup.ogg", 3 );
    //g_hitA = s_theSoundSystem->LoadStreamingSound( "Data/Sounds/hit_a.ogg", 3 );
    //g_hitB = s_theSoundSystem->LoadStreamingSound( "Data/Sounds/hit_b.ogg", 3 );
    //g_interrupt = s_theSoundSystem->LoadStreamingSound( "Data/Sounds/interrupt.ogg", 3 );
    //g_invalid = s_theSoundSystem->LoadStreamingSound( "Data/Sounds/invalid.ogg", 3 );
    //g_heal = s_theSoundSystem->LoadStreamingSound( "Data/Sounds/heal.ogg", 3 );
    //g_explosion = s_theSoundSystem->LoadStreamingSound( "Data/Sounds/explosion.ogg", 3 );
    //g_arrow = s_theSoundSystem->LoadStreamingSound( "Data/Sounds/arrow.ogg", 3 );
}
