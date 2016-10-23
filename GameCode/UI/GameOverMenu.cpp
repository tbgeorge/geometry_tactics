//=================================================================================
// GameOverMenu.cpp
// Author: Tyler George
// Date  : November 15, 2015
//=================================================================================

////===========================================================================================
///===========================================================================================
// Includes
///===========================================================================================
////===========================================================================================

#include "Engine/Utilities/DeveloperConsole.hpp"
#include "GameCode/UI/GameOverMenu.hpp"
#include "Engine/Utilities/FileUtilities.hpp"

////===========================================================================================
///===========================================================================================
// Static Variable Initialization
///===========================================================================================
////===========================================================================================


////===========================================================================================
///===========================================================================================
// Constructors/Destructors
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
GameOverMenu::GameOverMenu( OpenGLRenderer* renderer )
    : m_renderer( renderer )
    , m_gameOverMenuStateMachine( new StateMachine( "GameOverMenuState" ) )
    , m_buttonParent( nullptr )
    , m_menuButton( nullptr )
    , m_quitButton( nullptr )
    , m_menuTitle( nullptr )
    , m_menuTitle2( nullptr )
    , m_backgroundImage( nullptr )
{
    m_gameOverMenuStateMachine->PushState( State_e( GOS_DO_NOTHING ) );

    m_titleFont = Font::CreateOrGetFont( "Data/Fonts/Elephant" );
    m_font = Font::CreateOrGetFont( "Data/Fonts/CourierNew" );

    Texture* buttonBackground = Texture::CreateOrGetTexture( "Data/Images/buttonBackground.png" );
    Texture* GameOverMenuBackground = Texture::CreateOrGetTexture( "Data/Images/MainMenuBackground.png" );

    m_backgroundImage = new Image( renderer, nullptr, GameOverMenuBackground, Vector2( renderer->GetDisplayWidth(), renderer->GetDisplayHeight() ) );
    m_menuTitle = new TextBox( renderer, nullptr, m_titleFont, 128, TOP_CENTER, Vector2( 0.0f, 60.0f ), Rgba::ORANGE, "Game Over", 0.98f );
    m_menuTitle2 = new TextBox( renderer, nullptr, m_titleFont, 128, TOP_CENTER, Vector2( 0.5f, 65.0f ), Rgba::BLACK, "Game Over", 0.99f );

    m_buttonParent = new UIElement( renderer, Vector2( 400.0f, 290.0f ), nullptr, CENTER_CENTER );
    m_menuButton = new Button( renderer, m_buttonParent, m_font, 72, buttonBackground, Vector2( 400.0f, 90.0f ), TOP_CENTER, Vector2( 0.0f, 0.0f ), 0.99f, Rgba::BLACK, "Main Menu", nullptr, Rgba::WHITE );
    m_quitButton = new Button( renderer, m_buttonParent, m_font, 72, buttonBackground, Vector2( 400.0f, 90.0f ), TOP_CENTER, Vector2( 0.0f, 100.0f ), 0.99f, Rgba::BLACK, "Quit", nullptr, Rgba::WHITE );

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
GameOverMenu::~GameOverMenu()
{
    delete m_gameOverMenuStateMachine;
    delete m_buttonParent;
    delete m_menuButton;
    delete m_quitButton;
    delete m_menuTitle;
    delete m_menuTitle2;
    delete m_backgroundImage;
}

////===========================================================================================
///===========================================================================================
// Initialization
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void GameOverMenu::Startup()
{
}

////===========================================================================================
///===========================================================================================
// Accessors/Queries
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
GameOverMenuState GameOverMenu::GetCurrentState()
{
    return (GameOverMenuState)m_gameOverMenuStateMachine->GetCurrentStateID();
}

////===========================================================================================
///===========================================================================================
// Mutators
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void GameOverMenu::Reset()
{
    m_gameOverMenuStateMachine->PushState( State_e( GOS_DO_NOTHING ) );
}

////===========================================================================================
///===========================================================================================
// Update
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void GameOverMenu::ProcessInput( InputSystem* inputSystem )
{
    m_menuButton->ProcessInput( inputSystem );
    m_quitButton->ProcessInput( inputSystem );

    if (inputSystem->WasKeyJustReleased( VK_ESCAPE ) || inputSystem->WasKeyJustReleased( 'Q' ) || m_quitButton->WasJustClicked())
        m_gameOverMenuStateMachine->PushState( State_e( GOS_IS_READY_TO_QUIT ) );


    if (m_menuButton->WasJustClicked())
    {
        m_gameOverMenuStateMachine->PushState( State_e( GOS_SWITCH_TO_MAIN ) );
    }

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void GameOverMenu::Update( bool debugModeEnabled )
{
    UNUSED( debugModeEnabled );

    m_menuButton->Update( debugModeEnabled );
    m_quitButton->Update( debugModeEnabled );
}

////===========================================================================================
///===========================================================================================
// Render
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void GameOverMenu::Render( bool debugModeEnabled )
{
    if (!m_renderer)
        return;

    m_renderer->Disable( GL_CULL_FACE );
    m_renderer->Disable( GL_DEPTH_TEST );
    m_renderer->Enable( GL_BLEND );
    m_renderer->Enable( GL_TEXTURE_2D );
    m_renderer->BlendFunct( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    m_backgroundImage->Render( debugModeEnabled );
    m_menuTitle2->Render( debugModeEnabled );
    m_menuTitle->Render( debugModeEnabled );
    m_menuButton->Render( debugModeEnabled );
    m_quitButton->Render( debugModeEnabled );

}

////===========================================================================================
///===========================================================================================
// Private Functions
///===========================================================================================
////===========================================================================================