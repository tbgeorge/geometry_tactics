//=================================================================================
// InterruptMenu.cpp
// Author: Tyler George
// Date  : November 18, 2015
//=================================================================================


////===========================================================================================
///===========================================================================================
// Includes
///===========================================================================================
////===========================================================================================

#include "GameCode/UI/TurnMenus/InterruptMenu.hpp"


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
InterruptMenu::InterruptMenu( OpenGLRenderer* renderer )
    : m_renderer( renderer )
    , m_backgroundImage( nullptr )
    , m_buttonParent( nullptr )
    , m_disableAIButton( nullptr )
    , m_enableAIButton( nullptr )
    , m_doneButton( nullptr )
    , m_interruptMenuMachine( nullptr )
    , m_currentActorIsAIControlled( false )
{

    Font* font = Font::CreateOrGetFont( "Data/Fonts/Elephant" );
    Texture* background = Texture::CreateOrGetTexture( "Data/Images/menuBackground.png" );

    int fontSize = 36;

    m_backgroundImage = new Image( renderer, nullptr, background, Vector2( 200.0f, 100.0f ), CENTER_LEFT, Vector2( 70.0f, 0.0f ) );
    m_buttonParent = new UIElement( renderer, m_backgroundImage->GetSize(), m_backgroundImage );
    m_disableAIButton = new Button( renderer, m_buttonParent, font, fontSize, nullptr, Vector2( 200.0f, 50.0f ), TOP_LEFT, Vector2::ZERO, 0.99f, Rgba::BLACK, "Disable AI", nullptr, Rgba::WHITE );
    m_enableAIButton = new Button( renderer, m_buttonParent, font, fontSize, nullptr, Vector2( 200.0f, 50.0f ), TOP_LEFT, Vector2::ZERO, 0.99f, Rgba::BLACK, "Enable AI", nullptr, Rgba::WHITE );
    m_doneButton = new Button( renderer, m_buttonParent, font, fontSize, nullptr, Vector2( 200.0f, 50.0f ), TOP_LEFT, Vector2( 0.0f, 50.0f ), 0.99f, Rgba::BLACK, "Done", nullptr, Rgba::WHITE );


    m_interruptMenuMachine = new StateMachine( "InterruptMenuState" );
    m_interruptMenuMachine->PushState( State_e( IMS_DO_NOTHING ) );

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
InterruptMenu::~InterruptMenu()
{
    delete m_backgroundImage;
    delete m_buttonParent;
    delete m_enableAIButton;
    delete m_disableAIButton;
    delete m_doneButton;

    delete m_interruptMenuMachine;
}

////===========================================================================================
///===========================================================================================
// Initialization
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void InterruptMenu::Startup()
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
void InterruptMenu::SetCurrentActorAIControlled( bool actorIsAIControlled )
{
    m_currentActorIsAIControlled = actorIsAIControlled;
    if (actorIsAIControlled)
    {
        m_enableAIButton->Disable();
        m_disableAIButton->Enable();
    }
    else
    {
        m_disableAIButton->Disable();
        m_enableAIButton->Enable();
    }
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void InterruptMenu::Reset()
{
    m_interruptMenuMachine->PushState( State_e( IMS_DO_NOTHING ) );
    m_disableAIButton->Enable();
    m_enableAIButton->Enable();
}

////===========================================================================================
///===========================================================================================
// Update
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void InterruptMenu::ProcessInput( InputSystem* inputSystem )
{
    m_enableAIButton->ProcessInput( inputSystem );
    m_disableAIButton->ProcessInput( inputSystem );
    m_doneButton->ProcessInput( inputSystem );

    bool enableAIButtonPushed = m_enableAIButton->WasJustClicked();
    bool disableAIButtonPushed = m_disableAIButton->WasJustClicked();
    bool doneButtonPushed = m_doneButton->WasJustClicked();


    if (enableAIButtonPushed)
        m_interruptMenuMachine->PushState( State_e( IMS_ENABLE_AI_CONTROL ) );

    if (disableAIButtonPushed)
        m_interruptMenuMachine->PushState( State_e( IMS_DISABLE_AI_CONTROL ) );

    if (doneButtonPushed)
        m_interruptMenuMachine->PushState( State_e( IMS_DONE ) );

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void InterruptMenu::Update( bool debugModeEnabled )
{
    m_disableAIButton->Update( debugModeEnabled );
    m_enableAIButton->Update( debugModeEnabled );
    m_doneButton->Update( debugModeEnabled );
}


////===========================================================================================
///===========================================================================================
// Render
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void InterruptMenu::Render( bool debugModeEnabled )
{
    if (!m_renderer)
        return;

    FontRenderer* fontRenderer = m_renderer->GetFontRenderer();

    m_backgroundImage->Render( debugModeEnabled );

    if( m_currentActorIsAIControlled )
        m_disableAIButton->Render( debugModeEnabled );
    else
        m_enableAIButton->Render( debugModeEnabled );

    m_doneButton->Render( debugModeEnabled );

}

////===========================================================================================
///===========================================================================================
// Private Functions
///===========================================================================================
////===========================================================================================

