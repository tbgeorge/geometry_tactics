//=================================================================================
// InterruptMenu.hpp
// Author: Tyler George
// Date  : November 18, 2015
//=================================================================================

#pragma once

#ifndef __included_InterruptMenu__
#define __included_InterruptMenu__

///---------------------------------------------------------------------------------
/// Includes
///---------------------------------------------------------------------------------
#include "GameCode/GameCommon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/OpenGLRenderer.hpp"
#include "Engine/UI/Image.hpp"
#include "Engine/UI/Button.hpp"
#include "Engine/Utilities/StateMachine.hpp"


///---------------------------------------------------------------------------------
/// Typedefs
///---------------------------------------------------------------------------------

///---------------------------------------------------------------------------------
/// Enums
///---------------------------------------------------------------------------------
enum InterruptMenuState
{
    IMS_DO_NOTHING,
    IMS_DISABLE_AI_CONTROL,
    IMS_ENABLE_AI_CONTROL,
    IMS_DONE
};

////===========================================================================================
///===========================================================================================
// InterruptMenu Class
///===========================================================================================
////===========================================================================================
class InterruptMenu
{
public:
    ///---------------------------------------------------------------------------------
    /// Constructors/Destructors
    ///---------------------------------------------------------------------------------
    InterruptMenu( OpenGLRenderer* renderer );
    ~InterruptMenu();

    ///---------------------------------------------------------------------------------
    /// Initialization
    ///---------------------------------------------------------------------------------
    void Startup();

    ///---------------------------------------------------------------------------------
    /// Accessors/Queries
    ///---------------------------------------------------------------------------------
    InterruptMenuState GetCurrentState() { return (InterruptMenuState)m_interruptMenuMachine->GetCurrentStateID(); }

    ///---------------------------------------------------------------------------------
    /// Mutators
    ///---------------------------------------------------------------------------------
    void EnableDisableAIButton() { m_disableAIButton->Enable(); }
    void DisableDisableAIButton() { m_disableAIButton->Disable(); }
    void EnableEnableAIButton() { m_enableAIButton->Enable(); }
    void DisableEnableAIButton() { m_enableAIButton->Disable(); }
    void SetCurrentActorAIControlled( bool actorIsAIControlled );
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
    Image* m_backgroundImage;
    UIElement* m_buttonParent;
    Button* m_disableAIButton;
    Button* m_enableAIButton;
    Button* m_doneButton;

    bool m_currentActorIsAIControlled;

    StateMachine* m_interruptMenuMachine;
};

///---------------------------------------------------------------------------------
/// Inline function implementations
///---------------------------------------------------------------------------------

#endif