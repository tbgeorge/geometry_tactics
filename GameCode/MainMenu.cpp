//=================================================================================
// MainMenu.cpp
// Author: Tyler George
// Date  : August 20, 2015
//=================================================================================


////===========================================================================================
///===========================================================================================
// Includes
///===========================================================================================
////===========================================================================================

#include "Engine/Utilities/DeveloperConsole.hpp"
#include "GameCode/MainMenu.hpp"
#include "Game.hpp"
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
MainMenu::MainMenu( Vector2 displaySize, unsigned int basicShaderID, unsigned int fontShaderID )
    : m_displaySize( displaySize )
    , m_basicShaderID( basicShaderID )
    , m_fontShaderID( fontShaderID )
    , m_saveExists( false )
    , m_state( MMS_DO_NOTHING )
{

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
MainMenu::~MainMenu()
{

}

////===========================================================================================
///===========================================================================================
// Initialization
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void MainMenu::Startup( Font* titleFont, Font* font)
{
    m_font = font;
    m_titleFont = titleFont;

    m_saveExists = DoesFileExist( "Data/Saves/SaveGame.sav.xml" );
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
void MainMenu::Reset()
{
    SetState( MMS_DO_NOTHING );

    m_saveExists = DoesFileExist( "Data/Saves/SaveGame.sav.xml" );
}

////===========================================================================================
///===========================================================================================
// Update
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void MainMenu::ProcessInput( InputSystem* inputSystem, double deltaSeconds )
{
    UNUSED( deltaSeconds );

    m_mouseCoords = inputSystem->GetMousePosition();

    if (inputSystem->WasKeyJustReleased( VK_ESCAPE ) || inputSystem->WasKeyJustReleased( 'Q' ))
        SetState( MMS_IS_READY_TO_QUIT );

    if (inputSystem->WasKeyJustReleased( 'N' ))
        SetState( MMS_SWITCH_TO_PLAY );

    if (inputSystem->WasKeyJustReleased( 'C' ) && m_saveExists )
        SetState( MMS_CONTINUE );

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
MainMenuState MainMenu::Update( double deltaseconds, bool debugModeEnabled )
{
    UNUSED( deltaseconds );
    UNUSED( debugModeEnabled );

    return m_state;
}

////===========================================================================================
///===========================================================================================
// Render
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void MainMenu::Render( OpenGLRenderer* renderer, FontRenderer* fontRenderer, bool debugModeEnabled )
{
    renderer->Disable( GL_CULL_FACE );
    renderer->Disable( GL_DEPTH_TEST );
    renderer->Enable( GL_BLEND );
    renderer->Enable( GL_TEXTURE_2D );
    renderer->BlendFunct( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    // Title
    int titleSize = 120;
    std::string title = "Midnight Dream";
    float titleWidth = fontRenderer->CalcTextWidth( titleSize, *m_titleFont, title );
    float xOffsetTitle = (m_displaySize.x - titleWidth) / 2.0f;

    Vector3 titlePos( xOffsetTitle, 600.0f, 1.0f );
    fontRenderer->DrawFontText( m_fontShaderID, titleSize, *m_titleFont, title, titlePos, Rgba( 0x660066 ) );

    float buttonXOffset = 700.0f;
    float buttonYSpacing = 20.0f;
    int buttonSize = 48;
    Rgba buttonColor( 0x660033 );

    float nextButtonYPos = 450.0f;


    // Play Button
    std::string playText = "N) New Game";
    Vector3 playButtonPos( buttonXOffset, nextButtonYPos, 1.0f );
    fontRenderer->DrawFontText( m_fontShaderID, buttonSize, *m_titleFont, playText, playButtonPos, buttonColor );
    nextButtonYPos = nextButtonYPos - buttonSize - buttonYSpacing;

    if (m_saveExists)
    {
        // Continue Button
        std::string continueText = "C) Continue";
        Vector3 continueButtonPos( buttonXOffset, nextButtonYPos, 1.0f );
        fontRenderer->DrawFontText( m_fontShaderID, buttonSize, *m_titleFont, continueText, continueButtonPos, buttonColor );
        nextButtonYPos = nextButtonYPos - buttonSize - buttonYSpacing;

    }

    // Quit Button
    std::string quitText = "Q) Quit";
    Vector3 quitButtonPos( buttonXOffset, nextButtonYPos, 1.0f );
    fontRenderer->DrawFontText( m_fontShaderID, buttonSize, *m_titleFont, quitText, quitButtonPos, buttonColor );
    nextButtonYPos = nextButtonYPos - buttonSize - buttonYSpacing;


    if (debugModeEnabled)
    {
        std::string mouseCoords = std::to_string( m_mouseCoords.x ) + ", " + std::to_string( m_mouseCoords.y );
        Vector3 mousePos( 10.0f, m_displaySize.y - 58.0f, 1.0f );
        fontRenderer->DrawFontText( m_fontShaderID, 48, *m_font, mouseCoords, mousePos, Rgba::WHITE );

    }


}

////===========================================================================================
///===========================================================================================
// Private Functions
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void MainMenu::SetState( MainMenuState newState )
{
    if (m_state != newState)
    {
        m_state = newState;
    }
    else
    {
        DeveloperConsole::WriteLine( "MainMenu: Resetting State", WARNING_TEXT_COLOR );
    }
}