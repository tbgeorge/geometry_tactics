//=================================================================================
// Game.hpp
// Author: Tyler George
// Date  : January 29, 2015
//=================================================================================

#pragma once
#ifndef __included_Game__
#define __included_Game__

#include <map>
#include "Engine/Math/Vector2.hpp"
#include "Engine/Renderer/OpenGLRenderer.hpp"
#include "Engine/Renderer/Camera3D.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/Math2D.hpp"
#include "Engine/Utilities/Time.hpp"
#include "Engine/Renderer/FontRenderer.hpp"
#include "Engine/Renderer/GraphicsDebugger.hpp"
#include "Engine/Utilities/StateMachine.hpp"
#include "GameCode/GameCommon.hpp"
#include "GameCode/UI/MainMenu.hpp"
#include "GameCode/UI/GameOverMenu.hpp"
#include "GameCode/Map.hpp"
#include "GameCode/Entities/Actor.hpp"
#include "GameCode/TurnController.hpp"
#include "Engine/Sound/SoundSystem.hpp"
#include "Engine/Systems/Particles/ParticleEmitter.hpp"



///---------------------------------------------------------------------------------
/// Structs/Enums
///---------------------------------------------------------------------------------

enum GameState
{
    MAIN_MENU,
    IN_GAME,
    GAME_OVER
};


///---------------------------------------------------------------------------------
/// Constants
///---------------------------------------------------------------------------------

///---------------------------------------------------------------------------------
/// Typedefs
///---------------------------------------------------------------------------------
typedef std::multimap< float, Actor* > ActorMapBySpeed;

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
class Game
{
public:
	///---------------------------------------------------------------------------------
	/// Constructor/Destructor
	///---------------------------------------------------------------------------------
	Game( const Vector2& displaySize );
	~Game();

	///---------------------------------------------------------------------------------
	/// Initialization Functions
	///---------------------------------------------------------------------------------
	void Startup( OpenGLRenderer* renderer );
	void Shutdown();

	///---------------------------------------------------------------------------------
	/// Accessors/Queries
	///---------------------------------------------------------------------------------
    bool IsReadyToQuit() const { return m_isReadyToQuit; }
    Map* GetCurrentMap() const { return m_map; }
    static Game* GetGameInstance() { return s_theGame; }

    ///---------------------------------------------------------------------------------
    /// Mutators
    ///---------------------------------------------------------------------------------
    void RemoveDeadActors();
    void CheckForGameOver();
    void CleanUp();

	///---------------------------------------------------------------------------------
	/// Update Functions
	///---------------------------------------------------------------------------------
	void ProcessInput( InputSystem* inputSystem );
    void ProcessGameInput( InputSystem* inputSystem );
	void Update( const bool& debugModeEnabled );
    void UpdateGame( const bool& debugModeEnabled );

    void InterpolateCameraLocation();

	///---------------------------------------------------------------------------------
	/// Render Functions
	///---------------------------------------------------------------------------------
	void Render( bool debugModeEnabled );
    void RenderGame( bool debugModeEnabled );


private:
    ///---------------------------------------------------------------------------------
    /// Private Functions
    ///---------------------------------------------------------------------------------

	///---------------------------------------------------------------------------------
	/// Private member variables
	///---------------------------------------------------------------------------------
	Vector2		m_displaySize;
	Camera3D*	m_camera;
    
    bool m_isReadyToQuit;

    OpenGLRenderer* m_renderer;
    Font* m_font;

    Clock* m_gameClock;

    unsigned int m_basicShaderID;
    unsigned int m_fontShaderID;

    StateMachine* m_gameStateMachine;

    MainMenu* m_mainMenu;
    GameOverMenu* m_gameOverMenu;

    CameraLocationData m_desiredCameraLocation;
    bool m_hasReachedDesiredLocation;

    Map* m_map;
    TurnController* m_turnController;

    Actor* m_currentActor;
    float m_currentSpeedValue;
    ActorMapBySpeed m_actorsBySpeed;

    bool m_playerWon;

    //SoundID m_menuBackgroundMusic;
    //SoundID m_battleBackgroundMusic;
    //SoundID m_winMusic;
    //SoundID m_loseMusic;
    //Sound* m_backgroundMusic;

    ParticleEmitter* m_explosion;

    // Debugging variables
    GraphicsDebugger* m_graphicsDebugger;
    bool m_showAxes;



    ///---------------------------------------------------------------------------------
    /// Private Static Variables
    ///---------------------------------------------------------------------------------
    static Game* s_theGame;
};



#endif