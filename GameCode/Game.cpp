//=================================================================================
// Game.cpp
// Author: Tyler George
// Date  : August 20, 2015
//=================================================================================


////===========================================================================================
///===========================================================================================
// Includes
///===========================================================================================
////===========================================================================================

#include "Engine/Utilities/DeveloperConsole.hpp"

#include "GameCode/Game.hpp"
#include "Engine/UI/Button.hpp"
#include "GameCode/GameCommon.hpp"
#include "Engine/Utilities/Error.hpp"
#include "FeatureFactory.hpp"
#include "UnitJob.hpp"
#include "Engine/Systems/Particles/Update_Strategies/ExplosionUpdateStrategy.hpp"
#include "Engine/Systems/Particles/Render_Strategies/PointsRenderStrategy.hpp"


////===========================================================================================
///===========================================================================================
// Static Variable Initialization
///===========================================================================================
////===========================================================================================
Game* Game::s_theGame = nullptr;

////===========================================================================================
///===========================================================================================
// Constants
///===========================================================================================
////===========================================================================================

const float CAMERA_SPEED_UNITS_PER_SECOND = 40.0f;
const float YAW_DEGREES_PER_SECOND = 70.0f;
const float PITCH_DEGREES_PER_SECOND = 30.0f;

////===========================================================================================
///===========================================================================================
// Constructors/Destructors
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Game::Game( const Vector2& displaySize )
    : m_displaySize( displaySize )
    , m_camera( nullptr )
    , m_isReadyToQuit( false )
    , m_gameClock( nullptr )
    , m_basicShaderID( 0 )
    , m_fontShaderID( 0 )
    , m_renderer( nullptr )
    , m_font( nullptr )
    , m_mainMenu( nullptr )
    , m_gameOverMenu( nullptr )
    , m_hasReachedDesiredLocation( true )
    , m_map( nullptr )
    , m_turnController( nullptr )
    , m_currentActor( nullptr )
    , m_playerWon( nullptr )
    , m_currentSpeedValue( 0.0f )
    //, m_backgroundMusic( nullptr )
    , m_explosion( nullptr )
    // debugging variables
    , m_graphicsDebugger( nullptr )
    , m_showAxes( false )

{
    srand( (unsigned int) time(NULL) );

    if (!s_theGame)
        s_theGame = this;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Game::~Game()
{
    CleanUp();

    delete m_camera;
    delete m_gameStateMachine;
    delete m_mainMenu;
    delete m_gameOverMenu;
    delete m_turnController;

    delete m_gameClock;

//     delete m_test
}

////===========================================================================================
///===========================================================================================
// Initialization
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Game::Startup( OpenGLRenderer* renderer )
{
    // Component Initialization
    m_renderer = renderer;
    m_camera = new Camera3D( Vector3( 0.0f, 0.0f, 0.0f ), EulerAngles( 0.0f, 45.0f, 0.0f ) );

    m_font = Font::CreateOrGetFont( "Data/Fonts/Elephant" );

    m_graphicsDebugger = GraphicsDebugger::GetInstance();

    m_gameClock = new Clock( nullptr, 0.5 );

    InitializeCommonSounds();

    // Shader Initialization
    renderer->GLCheckError();

    m_gameStateMachine = new StateMachine( "GameState" );
    m_gameStateMachine->PushState( State_e( MAIN_MENU ) );

    m_mainMenu = new MainMenu( m_renderer );
    m_mainMenu->Startup();

    m_gameOverMenu = new GameOverMenu( m_renderer );
    m_gameOverMenu->Startup();

    m_turnController = new TurnController( m_renderer );

    FeatureFactory::LoadAllFeatureFactories( renderer, m_gameClock );
    UnitJob::LoadAllUnitJobs();

   /* m_menuBackgroundMusic = s_theSoundSystem->LoadStreamingSound( "Data/Sounds/Music/menu.ogg", 1 );
    m_battleBackgroundMusic = s_theSoundSystem->LoadStreamingSound( "Data/Sounds/Music/battle.ogg", 1 );
    m_winMusic = s_theSoundSystem->LoadStreamingSound( "Data/Sounds/Music/win.ogg", 1 );
    m_loseMusic = s_theSoundSystem->LoadStreamingSound( "Data/Sounds/Music/lose.ogg", 1 );*/
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Game::Shutdown()
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
void Game::RemoveDeadActors()
{
    // remove dead actors
    for (ActorMapBySpeed::iterator speedKeyIter = m_actorsBySpeed.begin(); speedKeyIter != m_actorsBySpeed.end(); )
    {
        float speedKey = speedKeyIter->first;

        std::pair< ActorMapBySpeed::iterator, ActorMapBySpeed::iterator > actorsOfSpeed;
        actorsOfSpeed = m_actorsBySpeed.equal_range( speedKey );

        for (ActorMapBySpeed::iterator actorIter = actorsOfSpeed.first; actorIter != actorsOfSpeed.second;)
        {
            Actor* actor = actorIter->second;
            if (actor->IsDead())
            {
                actorIter = m_actorsBySpeed.erase( actorIter );
                m_map->RemoveActor( actor );
                delete actor;
            }
            else
                ++actorIter;
        }

        speedKeyIter = actorsOfSpeed.second;
    }

    CheckForGameOver();
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Game::CheckForGameOver()
{
    int numEnemies = 0;
    int numAllies = 0;

    for (ActorMapBySpeed::iterator speedKeyIter = m_actorsBySpeed.begin(); speedKeyIter != m_actorsBySpeed.end(); ++speedKeyIter)
    {
        float speedKey = speedKeyIter->first;

        std::pair< ActorMapBySpeed::iterator, ActorMapBySpeed::iterator > actorsOfSpeed;
        actorsOfSpeed = m_actorsBySpeed.equal_range( speedKey );

        for (ActorMapBySpeed::iterator actorIter = actorsOfSpeed.first; actorIter != actorsOfSpeed.second; ++actorIter)
        {
            Actor* actor = actorIter->second;
            Faction actorFaction = actor->GetFaction();

            switch (actorFaction)
            {
            case ENEMY:
                ++numEnemies;
                break;
            case ALLY:
                ++numAllies;
                break;
            default:
                break;
            }
        }
    }

    if (m_currentActor)
    {
        switch (m_currentActor->GetFaction())
        {
        case ENEMY:
            ++numEnemies;
            break;
        case ALLY:
            ++numAllies;
            break;
        default:
            break;
        }
    }

    if (numEnemies == 0 || numAllies == 0)
    {
        m_gameStateMachine->PushState( State_e( GAME_OVER ) );
        CleanUp();
        if (numEnemies == 0 && numAllies != 0)
        {
            m_playerWon = true;
            //m_backgroundMusic->Stop();
            //m_backgroundMusic = s_theSoundSystem->PlayStreamingSound( m_winMusic, -1, 0.6f );
        }
        else if (numAllies == 0 && numEnemies != 0)
        {
            m_playerWon = false;
            /*m_backgroundMusic->Stop();
            m_backgroundMusic = s_theSoundSystem->PlayStreamingSound( m_loseMusic, -1, 0.6f );*/
        }
    }
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Game::CleanUp()
{
    // remove all actors
    for (ActorMapBySpeed::iterator speedKeyIter = m_actorsBySpeed.begin(); speedKeyIter != m_actorsBySpeed.end();)
    {
        float speedKey = speedKeyIter->first;

        std::pair< ActorMapBySpeed::iterator, ActorMapBySpeed::iterator > actorsOfSpeed;
        actorsOfSpeed = m_actorsBySpeed.equal_range( speedKey );

        for (ActorMapBySpeed::iterator actorIter = actorsOfSpeed.first; actorIter != actorsOfSpeed.second;)
        {
            Actor* actor = actorIter->second;

            actorIter = m_actorsBySpeed.erase( actorIter );
            delete actor;
        }

        speedKeyIter = actorsOfSpeed.second;
    }

    RECOVERABLE_ASSERT( m_actorsBySpeed.size() == 0 );

    delete m_map;
    m_map = nullptr;

    if (m_currentActor)
        delete m_currentActor;
    m_currentActor = nullptr;

    m_currentSpeedValue = 0.0f;

}

////===========================================================================================
///===========================================================================================
// Update
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Game::ProcessInput( InputSystem* inputSystem )
{
    switch (m_gameStateMachine->GetCurrentStateID())
    {
    case MAIN_MENU:
        m_mainMenu->ProcessInput( inputSystem );
        break;
    case IN_GAME:
        m_map->Update( m_gameClock->GetLastDeltaSeconds(), false );
        ProcessGameInput( inputSystem );
        break;
    case GAME_OVER:
        m_gameOverMenu->ProcessInput( inputSystem );
        break;
    default:
        break;
    }
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Game::ProcessGameInput( InputSystem* inputSystem )
{
    static bool inManualMode = false;

    if (inputSystem->WasKeyJustReleased( 'O' ))
        m_showAxes = !m_showAxes;

    ///---------------------------------------------------------------------------------
    /// Mouse/Keyboard controls
    ///---------------------------------------------------------------------------------

    /// DEVELOPER CONTROLS FOR CAMERA
    Vector3 cameraForward = m_camera->GetForwardVector();
    Vector3 cameraRight = CrossProduct( cameraForward, Vector3( 0.0f, 1.0f, 0.0f ) );
    Vector3 cameraUp = CrossProduct( cameraRight, cameraForward );

    Vector3 cameraMovementVector( 0.0f, 0.0f, 0.0f );

    if (inputSystem->IsMouseButtonDown( MOUSE_RIGHT ))
    {
        if (!inManualMode )
        {
            inputSystem->SetMousePosition( Vector2( 800.0f, 450.0f ) );
            inputSystem->ShowMousePointer( false );
            inManualMode = true;
        }
        else
        {

            if (inputSystem->IsKeyDown( 'W' ))
                cameraMovementVector += cameraForward;
            if (inputSystem->IsKeyDown( 'A' ))
                cameraMovementVector -= cameraRight;
            if (inputSystem->IsKeyDown( 'S' ))
                cameraMovementVector -= cameraForward;
            if (inputSystem->IsKeyDown( 'D' ))
                cameraMovementVector += cameraRight;
            if (inputSystem->IsKeyDown( 'Q' ))
                cameraMovementVector -= cameraUp;
            if (inputSystem->IsKeyDown( 'E' ))
                cameraMovementVector += cameraUp;

            cameraMovementVector.Normalize();

            cameraMovementVector *= (10.0f * (float)m_gameClock->GetLastDeltaSeconds());
            if (inputSystem->IsKeyDown( VK_SHIFT ))
                cameraMovementVector *= 2.0f;

            m_camera->m_position += cameraMovementVector;

            Vector2 mouseResetPosition( 800.0f, 450.0f );
            Vector2 mousePosition = inputSystem->GetMousePosition();
            Vector2 distanceMouseHasMovedSinceLastFrame = mousePosition - mouseResetPosition;
            inputSystem->SetMousePosition( mouseResetPosition );

            m_camera->m_orientation.pitchDegrees -= 0.15f * distanceMouseHasMovedSinceLastFrame.y;
            m_camera->m_orientation.yawDegrees -= 0.15f * distanceMouseHasMovedSinceLastFrame.x;

            if (m_camera->m_orientation.pitchDegrees < -89.0f)
                m_camera->m_orientation.pitchDegrees = -89.0f;
            if (m_camera->m_orientation.pitchDegrees > 89.0f)
                m_camera->m_orientation.pitchDegrees = 89.0f;
        }
    }
    else
    {
        if ( inManualMode )
        {
            inManualMode = false;
            inputSystem->ShowMousePointer( true );
        }

        switch (m_gameStateMachine->GetCurrentStateID())
        {
        case IN_GAME:
            switch (m_turnController->GetCurrentState())
            {
            case TS_EXIT:
                m_gameStateMachine->PopState();
                m_mainMenu->Reset();
                m_turnController->Reset();
                break;
            default:
                m_turnController->ProcessInput( inputSystem );
                break;
            }
            break;
        default:
            break;
        }
    }
    

    if (inputSystem->WasKeyJustReleased( VK_LEFT ) && m_hasReachedDesiredLocation )
    {
        CameraLocationData prev = m_map->GetPreviousCameraLoc();
        m_desiredCameraLocation.cameraPosition.x = prev.cameraPosition.x;
        m_desiredCameraLocation.cameraPosition.z = prev.cameraPosition.z;
        m_desiredCameraLocation.cameraOrientation.yawDegrees = prev.cameraOrientation.yawDegrees;
        m_hasReachedDesiredLocation = false;
    }


    if (inputSystem->WasKeyJustReleased( VK_RIGHT ) && m_hasReachedDesiredLocation )
    {
        CameraLocationData next = m_map->GetNextCameraLoc();
        m_desiredCameraLocation.cameraPosition.x = next.cameraPosition.x;
        m_desiredCameraLocation.cameraPosition.z = next.cameraPosition.z;
        m_desiredCameraLocation.cameraOrientation.yawDegrees = next.cameraOrientation.yawDegrees;
        m_hasReachedDesiredLocation = false;

    }

    static bool cameraIsInUpPosition = false;
    if (inputSystem->WasKeyJustReleased( VK_UP ) && !cameraIsInUpPosition && m_hasReachedDesiredLocation )
    {
        m_desiredCameraLocation.cameraPosition.y += 20.0f;
        m_desiredCameraLocation.cameraOrientation.pitchDegrees += 15.0f;
        cameraIsInUpPosition = true;
        m_hasReachedDesiredLocation = false;

    }


    if (inputSystem->WasKeyJustReleased( VK_DOWN ) && cameraIsInUpPosition && m_hasReachedDesiredLocation )
    {
        m_desiredCameraLocation.cameraPosition.y -= 20.0f;
        m_desiredCameraLocation.cameraOrientation.pitchDegrees -= 15.0f;
        cameraIsInUpPosition = false;
        m_hasReachedDesiredLocation = false;

    }



}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Game::Update( const bool& debugModeEnabled )
{
    UNUSED( debugModeEnabled );

    switch (m_gameStateMachine->GetCurrentStateID())
    {
    case MAIN_MENU:
       /* if (!m_backgroundMusic)
        {
            m_backgroundMusic = s_theSoundSystem->PlayStreamingSound( m_menuBackgroundMusic, -1 );
        }*/

        m_mainMenu->Update( debugModeEnabled );

        if (m_mainMenu->GetCurrentState() == MMS_IS_READY_TO_QUIT)
            m_isReadyToQuit = true;

        if (m_mainMenu->GetCurrentState() == MMS_SWITCH_TO_PLAY)
        {
            /*m_backgroundMusic->Stop();
            m_backgroundMusic = s_theSoundSystem->PlayStreamingSound( m_battleBackgroundMusic, -1, 0.6f );*/

            m_mainMenu->Reset();
            m_gameStateMachine->PushState( State_e( IN_GAME ) );

            m_map = new Map( IntVector2( 20, 20 ) );
//             m_map = new Map( "Data/Maps/TestMap1.map.xml" );
            m_map->Startup( m_renderer );

            FeatureFactory* rockFactory = FeatureFactory::FindFactoryByName( "Small Grey Rock" );
            FeatureFactory* treeFactory = FeatureFactory::FindFactoryByName( "Basic Tree" );


            CameraLocationData startingCameraPos = m_map->GetCurrentCameraLoc();
            m_desiredCameraLocation = startingCameraPos;

            m_camera->m_position = startingCameraPos.cameraPosition;
            m_camera->m_orientation = startingCameraPos.cameraOrientation;


            for (int rockNum = 0; rockNum < 20; ++rockNum)
            {
                MapPosition pos = m_map->GetRandomOpenPosition();
                m_map->SetFeatureAtMapPosition( rockFactory->SpawnFeature( XMLNode::emptyNode() ), pos );
            }

            for (int treeNum = 0; treeNum < 15; ++treeNum)
            {
                MapPosition pos = m_map->GetRandomOpenPosition();
                Cell* cell = m_map->GetCellAtMapPos( pos );
                while (cell->GetHeight() < 0.0f)
                {
                    pos = m_map->GetRandomOpenPosition();
                    cell = m_map->GetCellAtMapPos( pos );
                }
                m_map->SetFeatureAtMapPosition( treeFactory->SpawnFeature( XMLNode::emptyNode() ), pos );
            }
            

            for (int enemyNum = 0; enemyNum < 10; ++enemyNum)
            {
                std::string startingJob = "Wizard";
                int val = GetRandomIntLessThan( 3 );
                switch (val)
                {
                case 0:
                    startingJob = "Fighter";
                    break;
                case 1:
                    startingJob = "Archer";
                    break;
                case 2:
                    startingJob = "Wizard";
                    break;
                default:
                    break;
                }

                Actor* enemy = new Actor( m_renderer, m_gameClock, ENEMY, startingJob );
                int speed = GetRandomIntInRange( 3, 5 );
                enemy->SetSpeed( speed );
                enemy->SetMap( m_map );


                MapPosition pos = m_map->GetRandomOpenPosition();
                while (pos.x < m_map->GetMapSize().x / 2 || pos.y < m_map->GetMapSize().y / 2)
                    pos = m_map->GetRandomOpenPosition();

                m_map->SetActorAtMapPosition( enemy, pos );
                m_actorsBySpeed.insert( std::pair< float, Actor*>( (float)enemy->GetSpeed(), enemy ) );

            }

            for (int allyNum = 0; allyNum < 10; ++allyNum)
            {
                std::string startingJob = "Wizard";
                int val = GetRandomIntLessThan( 3 );
                switch (val)
                {
                case 0:
                    startingJob = "Fighter";
                    break;
                case 1:
                    startingJob = "Archer";
                    break;
                case 2:
                    startingJob = "Wizard";
                    break;
                default:
                    break;
                }

                Actor* ally = new Actor( m_renderer, m_gameClock, ALLY, startingJob );
                int speed = GetRandomIntInRange( 3, 5 );
                ally->SetSpeed( speed );
                ally->SetMap( m_map );

                MapPosition pos = m_map->GetRandomOpenPosition();
                while (pos.x > m_map->GetMapSize().x / 2 || pos.y > m_map->GetMapSize().y / 2)
                    pos = m_map->GetRandomOpenPosition();

                m_map->SetActorAtMapPosition( ally, pos );
                m_actorsBySpeed.insert( std::pair< float, Actor*>( (float)ally->GetSpeed(), ally ) );

            }
        }
        break;
    case IN_GAME:
        UpdateGame( debugModeEnabled );
        break;
    case GAME_OVER:
        m_gameOverMenu->Update( debugModeEnabled );
        
        switch (m_gameOverMenu->GetCurrentState())
        {
        case GOS_DO_NOTHING:
            break;
        case GOS_IS_READY_TO_QUIT:
            m_isReadyToQuit = true;
            break;
        case GOS_SWITCH_TO_MAIN:
            m_gameOverMenu->Reset();
            m_gameStateMachine->PushState( State_e( MAIN_MENU ) );
           /* m_backgroundMusic->Stop();
            m_backgroundMusic = s_theSoundSystem->PlayStreamingSound( m_menuBackgroundMusic, -1 );*/
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Game::UpdateGame( const bool& debugModeEnabled )
{
    UNUSED( debugModeEnabled );

    if ( !m_hasReachedDesiredLocation )
        InterpolateCameraLocation();

    if (!m_currentActor)
    {
        m_currentSpeedValue = m_actorsBySpeed.begin()->first;
        m_currentActor = m_actorsBySpeed.begin()->second;
        m_actorsBySpeed.erase( m_actorsBySpeed.begin() );

        m_currentActor->SetMoveState( HAS_NOT_MOVED );
        m_currentActor->SetActState( HAS_NOT_ACTED );
        m_currentActor->SetFinishedTurn( false );
        m_turnController->StartNewTurn( m_currentActor );
    }

    m_turnController->Update( debugModeEnabled );

    if (m_currentActor->HasFinishedTurn())
    {
        if (m_currentActor->GetMoveState() == HAS_MOVED)
            m_currentSpeedValue += (float)m_currentActor->GetSpeed() / 2.0f;
        if (m_currentActor->GetActState() == HAS_ACTED)
            m_currentSpeedValue += (float)m_currentActor->GetSpeed() / 2.0f;

        // small penalty for doing nothing
        if (m_currentActor->GetMoveState() == HAS_NOT_MOVED && m_currentActor->GetActState() == HAS_NOT_ACTED)
            m_currentSpeedValue += (float)m_currentActor->GetSpeed() / 4.0f;

        m_actorsBySpeed.insert( std::pair< float, Actor* >( m_currentSpeedValue, m_currentActor ) );
        m_currentActor = nullptr;

        RemoveDeadActors();
    }
    else
    {
        m_currentActor->Update( debugModeEnabled );
    }

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Game::InterpolateCameraLocation()
{
    double deltaSeconds = m_gameClock->GetLastDeltaSeconds();

    CameraLocationData currentCameraLoc( m_camera->m_position, m_camera->m_orientation );

    bool hasReachedDestination = true;
    if (currentCameraLoc != m_desiredCameraLocation)
    {
        // check for equality of positions
        if (!AreFloatsEqual( currentCameraLoc.cameraPosition.x, m_desiredCameraLocation.cameraPosition.x, 0.1f )
            || !AreFloatsEqual( currentCameraLoc.cameraPosition.y, m_desiredCameraLocation.cameraPosition.y, 0.1f )
            || !AreFloatsEqual( currentCameraLoc.cameraPosition.z, m_desiredCameraLocation.cameraPosition.z, 0.1f ))
        {
            Vector3 dirToGo = m_desiredCameraLocation.cameraPosition - currentCameraLoc.cameraPosition;
            dirToGo.Normalize();

            m_camera->m_position += dirToGo * (float) (CAMERA_SPEED_UNITS_PER_SECOND * deltaSeconds);

            hasReachedDestination = false;
        }



        float targetYawDegrees = DegreeZeroTo360( m_desiredCameraLocation.cameraOrientation.yawDegrees );
        float currentYawDegrees = DegreeZeroTo360( m_camera->m_orientation.yawDegrees );

        static bool hasCalculatedYawModifer = false;
        static int yawModifier = 0;
        if (!AreFloatsEqual( targetYawDegrees, currentYawDegrees, 0.2f ))
        {
            if (!hasCalculatedYawModifer)
            {
                float difference = targetYawDegrees - currentYawDegrees;
                yawModifier = difference < 0.0f ? -1 : 1;
                yawModifier = abs( difference ) > 180.0f ? -yawModifier : yawModifier;
                hasCalculatedYawModifer = true;
            }

            m_camera->m_orientation.yawDegrees += (float) ( yawModifier * (YAW_DEGREES_PER_SECOND * deltaSeconds ) );
            hasReachedDestination = false;
        }
        else
        {
            hasCalculatedYawModifer = false;
            yawModifier = 0;
        }


        float targetPitchDegrees = DegreeZeroTo360( m_desiredCameraLocation.cameraOrientation.pitchDegrees );
        float currentPitchDegrees = DegreeZeroTo360( m_camera->m_orientation.pitchDegrees );

        static bool hasCalculatedPitchModifer = false;
        static int pitchModifier = 0;
        if (!AreFloatsEqual( targetPitchDegrees, currentPitchDegrees, 0.1f ))
        {
            if (!hasCalculatedPitchModifer)
            {
                float difference = targetPitchDegrees - currentPitchDegrees;
                pitchModifier = difference < 0.0f ? -1 : 1;
                pitchModifier = abs( difference ) > 180.0f ? -pitchModifier : pitchModifier;
                hasCalculatedPitchModifer = true;
            }

            m_camera->m_orientation.pitchDegrees += (float) ( pitchModifier * (PITCH_DEGREES_PER_SECOND * deltaSeconds) );
            hasReachedDestination = false;
        }
        else
        {
            hasCalculatedPitchModifer = false;
            pitchModifier = 0;
        }


    }
    m_hasReachedDesiredLocation = hasReachedDestination;
}

////===========================================================================================
///===========================================================================================
// Render
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Game::Render( bool debugModeEnabled )
{
    switch (m_gameStateMachine->GetCurrentStateID())
    {
    case MAIN_MENU:
        m_mainMenu->Render( debugModeEnabled );
        break;
    case IN_GAME:
        RenderGame( debugModeEnabled );
        break;
    case GAME_OVER:
        m_gameOverMenu->Render( debugModeEnabled );
        break;
    default:
        break;
    }
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Game::RenderGame( bool debugModeEnabled )
{
    UNUSED( debugModeEnabled );

    if (!m_renderer)
        return;

    m_renderer->CalculateViewMatrix( *m_camera, true );

    m_renderer->Enable( GL_DEPTH_TEST );

    if (m_showAxes)
    {
        float axisLength = 1000.0f;
        m_graphicsDebugger->AddLineSegment( Vector3::ZERO, Vector3( axisLength, 0.0f, 0.0f ), 0, DT_DUAL, Rgba::RED, Rgba::RED );
        m_graphicsDebugger->AddLineSegment( Vector3::ZERO, Vector3( 0.0f, axisLength, 0.0f ), 0, DT_DUAL, Rgba::GREEN, Rgba::GREEN );
        m_graphicsDebugger->AddLineSegment( Vector3::ZERO, Vector3( 0.0f, 0.0f, axisLength ), 0, DT_DUAL, Rgba::BLUE, Rgba::BLUE );
    }


    // render map
    m_map->Render( m_renderer, debugModeEnabled );

    // Render actors
    for (ActorMapBySpeed::iterator speedKeyIter = m_actorsBySpeed.begin(); speedKeyIter != m_actorsBySpeed.end(); ++speedKeyIter)
    {
        float speedKey = speedKeyIter->first;

        std::pair< ActorMapBySpeed::iterator, ActorMapBySpeed::iterator > actorsOfSpeed;
        actorsOfSpeed = m_actorsBySpeed.equal_range( speedKey );

        for (ActorMapBySpeed::iterator actorIter = actorsOfSpeed.first; actorIter != actorsOfSpeed.second; ++actorIter)
        {
            Actor* actor = actorIter->second;
            if (!actor->IsDead())
                actor->Render( debugModeEnabled );
        }
    }

    if (m_currentActor)
        m_currentActor->Render( debugModeEnabled );

    // render turn controller
    m_turnController->Render( debugModeEnabled );

    static PUC_Vertexes backgroundVerts;
    if (backgroundVerts.empty())
    {
        backgroundVerts.push_back( Vertex3D_PUC( Vector3( 0.0f, 0.0f, 9999.0f ), Vector2::ZERO, Rgba( 0x1E2059, 1.0f ) ) );
        backgroundVerts.push_back( Vertex3D_PUC( Vector3( m_renderer->GetDisplayWidth(), 0.0f, 9999.0f ), Vector2::ZERO, Rgba( 0x1E2059, 1.0f ) ) );

        backgroundVerts.push_back( Vertex3D_PUC( Vector3( m_renderer->GetDisplayWidth(), m_renderer->GetDisplayHeight(), 9999.0f ), Vector2::ZERO, Rgba( 0xC8D5FA, 1.0f ) ) );
        backgroundVerts.push_back( Vertex3D_PUC( Vector3( 0.0f, m_renderer->GetDisplayHeight(), 9999.0f ), Vector2::ZERO, Rgba( 0xC8D5FA, 1.0f ) ) );
    }

    m_renderer->DrawVertexesOrtho( NULL, backgroundVerts, GL_QUADS );
}

////===========================================================================================
///===========================================================================================
// Private Functions
///===========================================================================================
////===========================================================================================
