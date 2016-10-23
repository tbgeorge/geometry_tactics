//=================================================================================
// GameOverMenu.hpp
// Author: Tyler George
// Date  : November 15, 2015
//=================================================================================

#pragma once

#ifndef __included_GameOverMenu__
#define __included_GameOverMenu__

///---------------------------------------------------------------------------------
/// Includes
///---------------------------------------------------------------------------------
#include "GameCode/GameCommon.hpp"
#include "Engine/Renderer/OpenGLRenderer.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/FontRenderer.hpp"
#include "Engine/Utilities/StateMachine.hpp"
#include "Engine/UI/Button.hpp"
#include "Engine/UI/TextBox.hpp"
#include "Engine/UI/Image.hpp"

///---------------------------------------------------------------------------------
/// Typedefs
///---------------------------------------------------------------------------------

///---------------------------------------------------------------------------------
/// Enums
///---------------------------------------------------------------------------------

enum GameOverMenuState
{
    GOS_DO_NOTHING,
    GOS_IS_READY_TO_QUIT,
    GOS_SWITCH_TO_MAIN
};

////===========================================================================================
///===========================================================================================
// GameOverMenu Class
///===========================================================================================
////===========================================================================================
class GameOverMenu
{
public:
    ///---------------------------------------------------------------------------------
    /// Constructors/Destructors
    ///---------------------------------------------------------------------------------
    GameOverMenu( OpenGLRenderer* renderer );
    ~GameOverMenu();

    ///---------------------------------------------------------------------------------
    /// Initialization
    ///---------------------------------------------------------------------------------
    void Startup();

    ///---------------------------------------------------------------------------------
    /// Accessors/Queries
    ///---------------------------------------------------------------------------------
    GameOverMenuState GetCurrentState();


    ///---------------------------------------------------------------------------------
    /// Mutators
    ///---------------------------------------------------------------------------------
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

    Font* m_titleFont;
    Font* m_font;

    StateMachine* m_gameOverMenuStateMachine;

    Image* m_backgroundImage;

    UIElement* m_buttonParent;
    Button* m_quitButton;
    Button* m_menuButton;

    TextBox* m_menuTitle;
    TextBox* m_menuTitle2;
};

///---------------------------------------------------------------------------------
/// Inline function implementations
///---------------------------------------------------------------------------------

#endif