//=================================================================================
// MainMenu.cpp
// Author: Tyler George
// Date  : October 26, 2015
//=================================================================================


////===========================================================================================
///===========================================================================================
// Includes
///===========================================================================================
////===========================================================================================

#include "Engine/Utilities/DeveloperConsole.hpp"
#include "GameCode/UI/MainMenu.hpp"
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
MainMenu::MainMenu( OpenGLRenderer* renderer )
    : m_renderer( renderer )
    , m_mainMenuStateMachine( new StateMachine( "MainMenuState" ) )
    , m_buttonParent( nullptr )
    , m_playButton( nullptr )
    , m_quitButton( nullptr )
    , m_menuTitle( nullptr )
    , m_menuTitle2( nullptr )
    , m_backgroundImage( nullptr )
{
    m_mainMenuStateMachine->PushState( State_e( MMS_DO_NOTHING ) );

    m_titleFont = Font::CreateOrGetFont( "Data/Fonts/Elephant" );
    m_font = Font::CreateOrGetFont( "Data/Fonts/CourierNew" );

    Texture* buttonBackground = Texture::CreateOrGetTexture( "Data/Images/buttonBackground.png" );
    Texture* mainMenuBackground = Texture::CreateOrGetTexture( "Data/Images/MainMenuBackground.png" );

    m_backgroundImage = new Image( renderer, nullptr, mainMenuBackground, Vector2( renderer->GetDisplayWidth(), renderer->GetDisplayHeight() ) );
    m_menuTitle = new TextBox( renderer, nullptr, m_titleFont, 128, TOP_CENTER, Vector2( 0.0f, 60.0f ), Rgba::ORANGE, "Geometry Tactics", 0.98f );
    m_menuTitle2 = new TextBox( renderer, nullptr, m_titleFont, 128, TOP_CENTER, Vector2( 0.5f, 65.0f ), Rgba::BLACK, "Geometry Tactics", 0.99f );

    m_buttonParent = new UIElement( renderer, Vector2( 400.0f, 290.0f ), nullptr, CENTER_CENTER );
    m_playButton = new Button( renderer, m_buttonParent, m_font, 72, buttonBackground, Vector2( 400.0f, 90.0f ), TOP_CENTER, Vector2( 0.0f, 0.0f ), 0.99f, Rgba::BLACK, "New Game", nullptr, Rgba::WHITE );
    m_quitButton = new Button( renderer, m_buttonParent, m_font, 72, buttonBackground, Vector2( 400.0f, 90.0f ), TOP_CENTER, Vector2( 0.0f, 100.0f ), 0.99f, Rgba::BLACK, "Quit", nullptr, Rgba::WHITE );

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
MainMenu::~MainMenu()
{
    delete m_mainMenuStateMachine;
    delete m_buttonParent;
    delete m_playButton;
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
void MainMenu::Startup()
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
MainMenuState MainMenu::GetCurrentState()
{
    return (MainMenuState) m_mainMenuStateMachine->GetCurrentStateID();
}

////===========================================================================================
///===========================================================================================
// Mutators
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void MainMenu::Reset()
{
    m_mainMenuStateMachine->PushState( State_e( MMS_DO_NOTHING ) );
}

////===========================================================================================
///===========================================================================================
// Update
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void MainMenu::ProcessInput( InputSystem* inputSystem )
{
    m_playButton->ProcessInput( inputSystem );
    m_quitButton->ProcessInput( inputSystem );

    if (inputSystem->WasKeyJustReleased( VK_ESCAPE ) || inputSystem->WasKeyJustReleased( 'Q' ) || m_quitButton->WasJustClicked())
    {
        m_mainMenuStateMachine->PushState( State_e( MMS_IS_READY_TO_QUIT ) );
    }


    if (m_playButton->WasJustClicked())
    {
        //s_theSoundSystem->PlayStreamingSound( g_confirm );
        m_mainMenuStateMachine->PushState( State_e( MMS_SWITCH_TO_PLAY ) );
    }

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void MainMenu::Update( bool debugModeEnabled )
{
    UNUSED( debugModeEnabled );

    m_playButton->Update( debugModeEnabled );
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
void MainMenu::Render( bool debugModeEnabled )
{
    if (!m_renderer )
        return;

    m_renderer->Disable( GL_CULL_FACE );
    m_renderer->Disable( GL_DEPTH_TEST );
    m_renderer->Enable( GL_BLEND );
    m_renderer->Enable( GL_TEXTURE_2D );
    m_renderer->BlendFunct( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    m_backgroundImage->Render( debugModeEnabled );
    m_menuTitle2->Render( debugModeEnabled );
    m_menuTitle->Render( debugModeEnabled );
    m_playButton->Render( debugModeEnabled );
    m_quitButton->Render( debugModeEnabled );


}

////===========================================================================================
///===========================================================================================
// Private Functions
///===========================================================================================
////===========================================================================================