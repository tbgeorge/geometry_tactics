//=================================================================================
// MainTurnMenu.cpp
// Author: Tyler George
// Date  : November 7, 2015
//=================================================================================


////===========================================================================================
///===========================================================================================
// Includes
///===========================================================================================
////===========================================================================================

#include "GameCode/UI/TurnMenus//MainTurnMenu.hpp"


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
MainTurnMenu::MainTurnMenu( OpenGLRenderer* renderer )
    : m_renderer( renderer )
    , m_backgroundImage( nullptr )
    , m_buttonParent( nullptr )
    , m_moveButton( nullptr )
    , m_actButton( nullptr )
    , m_enableAIButton( nullptr )
    , m_endTurnButton( nullptr )
    , m_exitButton( nullptr )
    , m_turnMenuMachine( nullptr )
{
    
    Font* font = Font::CreateOrGetFont( "Data/Fonts/Elephant" );
    Texture* background = Texture::CreateOrGetTexture( "Data/Images/menuBackground.png" );

    int fontSize = 36;

    m_backgroundImage = new Image( renderer, nullptr, background, Vector2( 220.0f, 260.0f ), CENTER_LEFT, Vector2( 70.0f, 0.0f ) );
    m_buttonParent = new UIElement( renderer, m_backgroundImage->GetSize(), m_backgroundImage );
    m_moveButton = new Button( renderer, m_buttonParent, font, fontSize, nullptr, Vector2( 220.0f, 50.0f ), TOP_LEFT, Vector2::ZERO, 0.99f, Rgba::BLACK, "Move", nullptr, Rgba::WHITE );
    m_actButton = new Button( renderer, m_buttonParent, font, fontSize, nullptr, Vector2( 220.0f, 50.0f ), TOP_LEFT, Vector2( 0.0f, 50.0f ), 0.99f, Rgba::BLACK, "Act", nullptr, Rgba::WHITE );
    m_enableAIButton = new Button( renderer, m_buttonParent, font, fontSize, nullptr, Vector2( 220.0f, 50.0f ), TOP_LEFT, Vector2( 0.0f, 100.0f ), 0.99f, Rgba::BLACK, "Enable AI", nullptr, Rgba::WHITE );
    m_endTurnButton = new Button( renderer, m_buttonParent, font, fontSize, nullptr, Vector2( 220.0f, 50.0f ), TOP_LEFT, Vector2( 0.0f, 150.0f ), 0.99f, Rgba::BLACK, "End Turn", nullptr, Rgba::WHITE );
    m_exitButton = new Button( renderer, m_buttonParent, font, fontSize, nullptr, Vector2( 220.0f, 50.0f ), TOP_LEFT, Vector2( 0.0f, 200.0f ), 0.99f, Rgba::BLACK, "Inspect Map", nullptr, Rgba::WHITE );
   

    m_turnMenuMachine = new StateMachine( "TurnMenuState" );
    m_turnMenuMachine->PushState( State_e( TMS_DO_NOTHING ) );

    m_actButton->Disable();
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
MainTurnMenu::~MainTurnMenu()
{
    delete m_backgroundImage;
    delete m_buttonParent;
    delete m_moveButton;
    delete m_actButton;
    delete m_enableAIButton;
    delete m_endTurnButton;
    delete m_exitButton;

    delete m_turnMenuMachine;
}

////===========================================================================================
///===========================================================================================
// Initialization
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void MainTurnMenu::Startup()
{

}

////===========================================================================================
///===========================================================================================
// Accessors/Queries
///===========================================================================================
////===========================================================================================


////===========================================================================================
///===========================================================================================
// Mutators
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void MainTurnMenu::Reset()
{
    m_turnMenuMachine->PushState( State_e( TMS_DO_NOTHING ) );
}

////===========================================================================================
///===========================================================================================
// Update
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void MainTurnMenu::ProcessInput( InputSystem* inputSystem )
{
    m_moveButton->ProcessInput( inputSystem );
    m_actButton->ProcessInput( inputSystem );
    m_enableAIButton->ProcessInput( inputSystem );
    m_endTurnButton->ProcessInput( inputSystem );
    m_exitButton->ProcessInput( inputSystem );

    bool moveButtonPushed = m_moveButton->WasJustClicked();
    bool actButtonPushed = m_actButton->WasJustClicked();
    bool enableAIButtonPushed = m_enableAIButton->WasJustClicked();
    bool endTurnButtonPushed = m_endTurnButton->WasJustClicked();
    bool exitButtonPushed = m_exitButton->WasJustClicked();

    if (moveButtonPushed)
        m_turnMenuMachine->PushState( State_e( TMS_SWITCH_TO_MOVE ) );

    if (actButtonPushed)
        m_turnMenuMachine->PushState( State_e( TMS_SWITCH_TO_ACT ) );

    if (enableAIButtonPushed)
        m_turnMenuMachine->PushState( State_e( TMS_ENABLE_AI_CONTROL ) );

    if (endTurnButtonPushed)
        m_turnMenuMachine->PushState( State_e( TMS_END_TURN ) );

    if (exitButtonPushed)
        m_turnMenuMachine->PushState( State_e( TMS_EXIT ) );


}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void MainTurnMenu::Update( bool debugModeEnabled )
{
    m_moveButton->Update( debugModeEnabled );
    m_actButton->Update( debugModeEnabled );
    m_enableAIButton->Update( debugModeEnabled );
    m_endTurnButton->Update( debugModeEnabled );
    m_exitButton->Update( debugModeEnabled );
}


////===========================================================================================
///===========================================================================================
// Render
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void MainTurnMenu::Render( bool debugModeEnabled )
{
    if (!m_renderer)
        return;

    FontRenderer* fontRenderer = m_renderer->GetFontRenderer();

    m_backgroundImage->Render( debugModeEnabled );

    m_moveButton->Render( debugModeEnabled );
    m_actButton->Render( debugModeEnabled );
    m_enableAIButton->Render( debugModeEnabled );
    m_endTurnButton->Render( debugModeEnabled );
    m_exitButton->Render( debugModeEnabled );

}

////===========================================================================================
///===========================================================================================
// Private Functions
///===========================================================================================
////===========================================================================================

