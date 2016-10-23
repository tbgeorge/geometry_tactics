//=================================================================================
// TurnController.hpp
// Author: Tyler George
// Date  : November 7, 2015
//=================================================================================

#pragma once

#ifndef __included_TurnController__
#define __included_TurnController__

///---------------------------------------------------------------------------------
/// Includes
///---------------------------------------------------------------------------------
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/OpenGLRenderer.hpp"
#include "GameCode/Entities/Actor.hpp"
#include "GameCode/UI/TurnMenus/MainTurnMenu.hpp"
#include "Engine/Utilities/StateMachine.hpp"
#include "GameCode/GameCommon.hpp"
#include "UI/ActorInfoPanel.hpp"
#include "UI/TurnMenus/InterruptMenu.hpp"



///---------------------------------------------------------------------------------
/// Typedefs
///---------------------------------------------------------------------------------

///---------------------------------------------------------------------------------
/// Enums
///---------------------------------------------------------------------------------
enum TurnState
{
    TS_DO_NOTHING,
    TS_MOVE_ACT_EXIT_MENU,
    TS_MOVE_ACTOR,
    TS_ACT_ACTOR,
    TS_OBSERVE_MAP,
    TS_AI,
    TS_INTERRUPT,
    TS_EXIT
};

////===========================================================================================
///===========================================================================================
// TurnController Class
///===========================================================================================
////===========================================================================================
class TurnController
{
public:
	///---------------------------------------------------------------------------------
	/// Constructors/Destructors
	///---------------------------------------------------------------------------------
    TurnController( OpenGLRenderer* renderer );
    ~TurnController();

	///---------------------------------------------------------------------------------
	/// Initialization
	///---------------------------------------------------------------------------------
    void Startup();

	///---------------------------------------------------------------------------------
	/// Accessors/Queries
	///---------------------------------------------------------------------------------
    TurnState GetCurrentState() { return (TurnState) m_turnStateMachine->GetCurrentStateID(); }
    Actor* GetCurrentActor() { return m_selectedActor; }

	///---------------------------------------------------------------------------------
	/// Mutators
	///---------------------------------------------------------------------------------
    void SetActor( Actor* actor ) { m_selectedActor = actor; }
    void ClearActor() { m_selectedActor = nullptr; }
    void StartNewTurn( Actor* actor );

    void Reset();

	///---------------------------------------------------------------------------------
	/// Update
	///---------------------------------------------------------------------------------
    void ProcessInput( InputSystem* inputSystem );
    void Update( bool debugModeEnabled );

    ///---------------------------------------------------------------------------------
    /// Render
    ///---------------------------------------------------------------------------------
    void Render( bool debugModeEnabled );

	///---------------------------------------------------------------------------------
	/// Public Member Variables
	///---------------------------------------------------------------------------------

	///---------------------------------------------------------------------------------
	/// Static Variables
	///---------------------------------------------------------------------------------


private:
	///---------------------------------------------------------------------------------
	/// Private Functions
	///---------------------------------------------------------------------------------

	///---------------------------------------------------------------------------------
	/// Private Member Variables
	///---------------------------------------------------------------------------------
    OpenGLRenderer* m_renderer;
    MainTurnMenu* m_turnMenu;
    InterruptMenu* m_interruptMenu;

    Actor* m_selectedActor;
    Actor* m_targetActor;
    Actor* m_interruptActor;

    Cell* m_currentHoveredCell;

    StateMachine* m_turnStateMachine;

    TextBox* m_currentCellHeightTB;

    ActorInfoPanel* m_currentActorInfo;
    ActorInfoPanel* m_targetActorInfo;

};

///---------------------------------------------------------------------------------
/// Inline function implementations
///---------------------------------------------------------------------------------

#endif