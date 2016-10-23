//=================================================================================
// ActorInfoPanel.cpp
// Author: Tyler George
// Date  : November 16, 2015
//=================================================================================

////===========================================================================================
///===========================================================================================
// Includes
///===========================================================================================
////===========================================================================================

#include "GameCode/UI/ActorInfoPanel.hpp"
#include "GameCode/Entities/Actor.hpp"

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
ActorInfoPanel::ActorInfoPanel( OpenGLRenderer* renderer, Anchor alignment )
    : m_renderer( renderer )
    , m_backgroundImage( nullptr )
    , m_panelParent( nullptr )
    , m_alignment( alignment )
    , m_healthBar( nullptr )
    , m_healthLabel( nullptr )
    , m_unitFaction( nullptr )
    , m_unitMoveRange( nullptr )
    , m_unitSpeed( nullptr )
    , m_actor( nullptr )
    , m_lastPercent( 0.0f )
{

    Font* font = Font::CreateOrGetFont( "Data/Fonts/Elephant" );
    Texture* background = Texture::CreateOrGetTexture( "Data/Images/menuBackground.png" );

    m_panelParent = new UIElement( renderer, Vector2( 500.0f, 100.0f ), nullptr, m_alignment );

    m_backgroundImage = new Image( renderer, m_panelParent, background, m_panelParent->GetSize() );
    m_healthBar = new ProgressBar( renderer, m_panelParent, IntRange( 0, 0 ), font, Vector2( 300.0f, 30.0f ), Rgba::BLACK, Rgba::RED, TOP_RIGHT, Vector2( 20.0f, 20.0f ) );
    m_healthBar->SetFontSize( 36 );

    m_healthLabel = new TextBox( renderer, m_panelParent, font, 32, TOP_LEFT, Vector2( 20.0f, 20.0f ), Rgba::BLACK, "Health:", 0.9f);
    m_unitFaction = new TextBox( renderer, m_panelParent, font, 32, BOTTOM_LEFT, Vector2( 20.0f, 20.0f ), Rgba::BLACK, "", 0.9f );
    m_unitMoveRange = new TextBox( renderer, m_panelParent, font, 32, BOTTOM_CENTER, Vector2( 0.0f, 20.0f ), Rgba::BLACK, "", 0.9f );
    m_unitSpeed = new TextBox( renderer, m_panelParent, font, 32, BOTTOM_RIGHT, Vector2( 20.0f, 20.0f ), Rgba::BLACK, "", 0.9f);

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
ActorInfoPanel::~ActorInfoPanel()
{
    delete m_backgroundImage;
    delete m_panelParent;
    delete m_healthLabel;
    delete m_healthBar;
    delete m_unitSpeed;
    delete m_unitFaction;
    delete m_unitMoveRange;

}

////===========================================================================================
///===========================================================================================
// Initialization
///===========================================================================================
////===========================================================================================


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
void ActorInfoPanel::SetActor( Actor* actor )
{
    m_actor = actor;

    Faction actorFaction = m_actor->GetFaction();
    int actorSpeed = m_actor->GetSpeed();
    int actorRange = m_actor->GetMoveRange();

    switch (actorFaction)
    {
    case ENEMY:
        m_unitFaction->SetText( "Enemy" );
        break;
    case ALLY:
        if (m_actor->IsControlledByAI())
            m_unitFaction->SetText( "Ally (AI)" );
        else
            m_unitFaction->SetText( "Ally" );
        break;
    case NEUTRAL:
        m_unitFaction->SetText( "Neutral" );
        break;
    default:
        break;
    }

    m_unitSpeed->SetText( "Speed: " + std::to_string( actorSpeed ) );
    m_unitMoveRange->SetText( "Move Range: " + std::to_string( actorRange ) );

    m_healthBar->SetValues( IntRange( 0, m_actor->GetMaxHealth() ) );

    
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void ActorInfoPanel::SetAlignment( Anchor alignment )
{
    m_panelParent->SetAnchor( alignment );
}

////===========================================================================================
///===========================================================================================
// Update
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void ActorInfoPanel::ProcessInput( InputSystem* inputSystem )
{
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void ActorInfoPanel::Update( bool debugModeEnabled )
{
    if (m_actor)
    {
        float healthPercent = ((float)m_actor->GetHealth() / (float)m_actor->GetMaxHealth());
        if (healthPercent != m_lastPercent)
        {
            m_lastPercent = healthPercent;
            m_healthBar->SetPercentage( healthPercent );
            m_healthBar->Update( debugModeEnabled );
        }
    }
}


////===========================================================================================
///===========================================================================================
// Render
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void ActorInfoPanel::Render( bool debugModeEnabled )
{
    if (!m_renderer)
        return;

    m_backgroundImage->Render( debugModeEnabled );
    m_healthBar->Render( debugModeEnabled );
    m_healthLabel->Render( debugModeEnabled );
    m_unitFaction->Render( debugModeEnabled );
    m_unitMoveRange->Render( debugModeEnabled );
    m_unitSpeed->Render( debugModeEnabled );

}

////===========================================================================================
///===========================================================================================
// Private Functions
///===========================================================================================
////===========================================================================================

