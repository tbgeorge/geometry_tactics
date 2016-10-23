//=================================================================================
// TurnController.cpp
// Author: Tyler George
// Date  : November 7, 2015
//=================================================================================


////===========================================================================================
///===========================================================================================
// Includes
///===========================================================================================
////===========================================================================================

#include "GameCode/TurnController.hpp"
#include "GameCode/GameCommon.hpp"
#include "Game.hpp"


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
TurnController::TurnController( OpenGLRenderer* renderer )
    : m_selectedActor( nullptr )
    , m_targetActor( nullptr )
    , m_interruptActor( nullptr )
    , m_currentHoveredCell( nullptr )
    , m_turnMenu( new MainTurnMenu( renderer ) )
    , m_interruptMenu(new InterruptMenu( renderer ) )
    , m_renderer( renderer )
    , m_currentCellHeightTB( nullptr )
    , m_currentActorInfo( nullptr )
    , m_targetActorInfo( nullptr )
{
    m_turnStateMachine = new StateMachine( "TurnState" );
    m_turnStateMachine->PushState( State_e( TS_DO_NOTHING ) );

    m_currentCellHeightTB = new TextBox( renderer, nullptr, Font::CreateOrGetFont( "Data/Fonts/Elephant" ), 48, TOP_LEFT, Vector2( 20.0f, 20.0f ), Rgba::BLACK, "" );

    m_currentActorInfo = new ActorInfoPanel( renderer, BOTTOM_LEFT );
    m_targetActorInfo = new ActorInfoPanel( renderer, BOTTOM_RIGHT );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
TurnController::~TurnController()
{
    delete m_turnMenu;
    delete m_interruptMenu;
    delete m_turnStateMachine;
    delete m_currentCellHeightTB;
}

////===========================================================================================
///===========================================================================================
// Initialization
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void TurnController::Startup()
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
void TurnController::StartNewTurn( Actor* actor )
{
    if ( actor->IsControlledByAI() )
        m_turnStateMachine->PushState( State_e( TS_AI ) );
    else
        m_turnStateMachine->PushState( State_e( TS_MOVE_ACT_EXIT_MENU ) );

    m_turnMenu->Reset();
    m_selectedActor = actor;
    m_selectedActor->UpdatePossibleMoves();

    m_currentActorInfo->SetActor( m_selectedActor );
    m_targetActor = nullptr;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void TurnController::Reset()
{
    m_turnStateMachine->PopState();
}

////===========================================================================================
///===========================================================================================
// Update
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void TurnController::ProcessInput( InputSystem* inputSystem )
{

    RaycastResult result = DoMouseToWorldRaycast( inputSystem, m_renderer );
//     if ( m_currentHoveredCell && m_currentHoveredCell != result.cellHit)
//         m_currentHoveredCell->SetHovered( false );
    m_currentHoveredCell = result.cellHit;

//     if (!m_interruptActor)
//     {
        if (result.didHit)
            result.cellHit->SetHovered( true );
//     }

    switch (m_turnStateMachine->GetCurrentStateID())
    {
    case TS_MOVE_ACT_EXIT_MENU:
        if (result.didHit)
        {
            if (result.cellHit->GetActor() != m_targetActor)
            {
                m_targetActor = result.cellHit->GetActor();
                if (m_targetActor)
                    m_targetActorInfo->SetActor( m_targetActor );
            }
        }
        m_turnMenu->ProcessInput( inputSystem );

        break;
    case TS_MOVE_ACTOR:
        switch (m_selectedActor->GetMoveState() )
        {
        case HAS_NOT_MOVED:

            if (result.didHit)
            {
                if (result.cellHit->GetActor() != m_targetActor)
                {
                    m_targetActor = result.cellHit->GetActor();
                    if (m_targetActor)
                        m_targetActorInfo->SetActor( m_targetActor );
                }
            }

            if (inputSystem->WasMouseButtonJustReleased( MOUSE_LEFT ))
            {
                Cell* currentCell = result.cellHit;

                if (currentCell)
                {
                    MapPosition cellPos = currentCell->GetMapPosition();
                    if (m_selectedActor)
                    {
                        Map* map = Game::GetGameInstance()->GetCurrentMap();
                        CellPtrs moves = m_selectedActor->GetPossibleMoves();
                        bool validSelection = false;
                        for (CellPtrs::const_iterator moveIter = moves.begin(); moveIter != moves.end(); ++moveIter)
                        {
                            Cell* move = *moveIter;
                            if (move->GetMapPosition() == cellPos)
                            {
                                m_selectedActor->MoveActor( cellPos );
                                //s_theSoundSystem->PlayStreamingSound( g_confirm );
                                validSelection = true;
                                break;
                            }
                        }
                        //if (!validSelection)
                            //s_theSoundSystem->PlayStreamingSound( g_invalid );
                    }
                }
            }
            if (inputSystem->WasKeyJustReleased( VK_ESCAPE ))
            {
                m_turnMenu->Reset();
                m_turnStateMachine->PopState();
                //s_theSoundSystem->PlayStreamingSound( g_cancel );
            }
            break;
        case IS_MOVING:
            break;
        case HAS_MOVED:
            m_turnStateMachine->PopState();
            break;
        default:
            break;
        }
        break;
    case TS_ACT_ACTOR:
        //m_selectedActor->UpdatePossibleAttacks();

        switch (m_selectedActor->GetActState())
        {
        case HAS_NOT_ACTED:

            if (result.didHit)
            {
                if ( result.cellHit->GetActor() != m_targetActor)
                {
                    m_targetActor = result.cellHit->GetActor();
                    if ( m_targetActor )
                        m_targetActorInfo->SetActor( m_targetActor );
                }
            }

            if (inputSystem->WasMouseButtonJustReleased( MOUSE_LEFT ))
            {
                Cell* currentCell = result.cellHit;

                if (currentCell)
                {
                    MapPosition cellPos = currentCell->GetMapPosition();
                    if (m_selectedActor)
                    {
                        Map* map = Game::GetGameInstance()->GetCurrentMap();
                        CellPtrs attacks = m_selectedActor->GetPossibleAttacks();
                        bool validSelection = false;

                        for (CellPtrs::const_iterator attackIter = attacks.begin(); attackIter != attacks.end(); ++attackIter)
                        {
                            Cell* attack = *attackIter;
                            if (attack->GetMapPosition() == cellPos)
                            {
                                m_selectedActor->AttackPosition( cellPos );
                                //s_theSoundSystem->PlayStreamingSound( g_confirm );
                                validSelection = true;
                                break;
                            }
                        }
                        //if (!validSelection)
                            //s_theSoundSystem->PlayStreamingSound( g_invalid );
                    }
                }
            }
            if (inputSystem->WasKeyJustReleased( VK_ESCAPE ))
            {
                m_turnMenu->Reset();
                m_turnStateMachine->PopState();
                //s_theSoundSystem->PlayStreamingSound( g_cancel );
            }
            break;
        case IS_ACTING:
            break;
        case HAS_ACTED:
            m_turnStateMachine->PopState();
            m_targetActor = nullptr;
            break;
        default:
            break;
        }
        break;
    case TS_AI:
        if (inputSystem->WasKeyJustReleased( VK_ESCAPE ))
        {
            m_turnStateMachine->PushState( State_e( TS_OBSERVE_MAP ) );
            //s_theSoundSystem->PlayStreamingSound( g_interrupt );
        }
        break;
    case TS_OBSERVE_MAP:
        // TODO: put stuff here

        if (result.didHit)
        {
            if (result.cellHit->GetActor() != m_targetActor)
            {
                m_targetActor = result.cellHit->GetActor();
                if (m_targetActor)
                    m_targetActorInfo->SetActor( m_targetActor );
            }

            if (inputSystem->WasMouseButtonJustReleased( MOUSE_LEFT ))
            {
                Actor* actor = result.cellHit->GetActor();

                if (actor)
                {
                    m_interruptMenu->SetCurrentActorAIControlled( actor->IsControlledByAI() );
                    if (actor->GetFaction() != ALLY)
                    {
                        m_interruptMenu->DisableDisableAIButton();
                        m_interruptMenu->DisableEnableAIButton();
                    }

                    m_turnStateMachine->PushState( State_e( TS_INTERRUPT ) );
                    m_interruptActor = actor;
                }
            }
        }

        if (inputSystem->WasKeyJustReleased( VK_ESCAPE ))
        {
            m_turnMenu->Reset();
            m_turnStateMachine->PopState();
        }
        break;
    case TS_INTERRUPT:
        m_interruptMenu->ProcessInput( inputSystem );

        if (inputSystem->WasMouseButtonJustReleased( MOUSE_LEFT ))
        {
            Actor* actor = nullptr;

            if ( result.cellHit )
                actor = result.cellHit->GetActor();

            if (actor)
            {
                m_interruptMenu->Reset();
                m_interruptMenu->SetCurrentActorAIControlled( actor->IsControlledByAI() );
                if (actor->GetFaction() != ALLY)
                {
                    m_interruptMenu->DisableDisableAIButton();
                    m_interruptMenu->DisableEnableAIButton();
                }

                m_interruptActor = actor;
            }
        }

        if (inputSystem->WasKeyJustReleased( VK_ESCAPE ))
        {
            m_interruptMenu->Reset();
            m_turnStateMachine->PopState();
            m_interruptActor = nullptr;
        }
        break;
    default:
        break;
    }
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void TurnController::Update( bool debugModeEnabled )
{
    UNUSED( debugModeEnabled );

    static Cell* previousSelectedCell = m_currentHoveredCell;
    if (previousSelectedCell != m_currentHoveredCell)
    {
        //s_theSoundSystem->PlayStreamingSound( g_cursorA );
        previousSelectedCell = m_currentHoveredCell;
    }

    if (m_interruptActor)
    {
        m_interruptActor->GetMap()->GetCellAtMapPos( m_interruptActor->GetMapPosition() )->SetHovered( true );
        if (m_targetActor != m_interruptActor)
        {
            m_targetActor = m_interruptActor;
            m_targetActorInfo->SetActor( m_targetActor );
        }
    }

    if (m_selectedActor && !m_interruptActor )
        m_currentActorInfo->Update( debugModeEnabled );

    if ( m_targetActor )
        m_targetActorInfo->Update( debugModeEnabled );


    if (m_selectedActor)
    {
        if ( !m_interruptActor )
            m_selectedActor->GetMap()->GetCellAtMapPos( m_selectedActor->GetMapPosition() )->SetHovered( true );
        
        MoveState actorMoveState = m_selectedActor->GetMoveState();
        ActState actorActState = m_selectedActor->GetActState();


        if (actorMoveState == HAS_MOVED && actorActState == HAS_ACTED)
        {
            m_turnStateMachine->PopState();
            m_selectedActor->SetFinishedTurn( true );
            ClearActor();
        }

        switch (actorMoveState)
        {
        case HAS_NOT_MOVED:
            m_turnMenu->EnableMove();
            break;
        case HAS_MOVED:
            m_turnMenu->DisableMove();
            break;
        default:
            break;
        }

        switch (actorActState)
        {
        case HAS_NOT_ACTED:
            m_turnMenu->EnableAct();
            break;
        case HAS_ACTED:
            m_turnMenu->DisableAct();
            break;
        default:
            break;
        }

        switch (m_turnStateMachine->GetCurrentStateID() )
        {
        case TS_AI:
        {
            if ( ( actorMoveState == HAS_NOT_MOVED || actorActState == HAS_NOT_ACTED ) && actorMoveState != IS_MOVING && actorActState != IS_ACTING )
                m_selectedActor->Think();

            break;
        }
        default:
            break;
        
        }

        switch (m_turnStateMachine->GetCurrentStateID())
        {
        case TS_MOVE_ACT_EXIT_MENU:
            switch (m_turnMenu->GetCurrentState())
            {
            case TMS_DO_NOTHING:
                m_turnMenu->Update( debugModeEnabled );
                break;
            case TMS_SWITCH_TO_MOVE:
                m_turnStateMachine->PushState( State_e( TS_MOVE_ACTOR ) );
                m_turnMenu->Reset();
                //s_theSoundSystem->PlayStreamingSound( g_confirm );
                break;
            case TMS_SWITCH_TO_ACT:
                m_turnStateMachine->PushState( State_e( TS_ACT_ACTOR ) );
                m_turnMenu->Reset();
                //s_theSoundSystem->PlayStreamingSound( g_confirm );
                break;
            case TMS_ENABLE_AI_CONTROL:
                m_selectedActor->SetControlledByAI( true );
                m_turnStateMachine->PopState();
                m_turnStateMachine->PushState( State_e( TS_AI ) );
                //s_theSoundSystem->PlayStreamingSound( g_confirm );
                m_turnMenu->Reset();
                break;
            case TMS_END_TURN:
                m_selectedActor->SetFinishedTurn( true );
                ClearActor();
                m_turnStateMachine->PopState();
                //s_theSoundSystem->PlayStreamingSound( g_confirm );
                break;
            case TMS_EXIT:
                m_turnStateMachine->PushState( State_e( TS_OBSERVE_MAP ) );
                //s_theSoundSystem->PlayStreamingSound( g_confirm );
                break;
            default:
                break;
            }
            break;
        case TS_INTERRUPT:
            switch (m_interruptMenu->GetCurrentState())
            {
            case IMS_DO_NOTHING:
                m_interruptMenu->Update( debugModeEnabled );
                break;
            case IMS_ENABLE_AI_CONTROL:
                m_interruptActor->SetControlledByAI( true );
                m_interruptMenu->Reset();
                m_interruptMenu->SetCurrentActorAIControlled( true );
                m_targetActorInfo->SetActor( m_interruptActor );
                break;
            case IMS_DISABLE_AI_CONTROL:
                m_interruptActor->SetControlledByAI( false );
                m_interruptMenu->Reset();
                m_interruptMenu->SetCurrentActorAIControlled( false );
                m_targetActorInfo->SetActor( m_interruptActor );
                break;
            case IMS_DONE:
                m_turnStateMachine->PopState(); // pop interrupt
                m_turnStateMachine->PopState(); // pop observe
                m_interruptMenu->Reset();
                m_turnMenu->Reset();
                m_interruptActor = nullptr;
                break;
            }
            break;
        default:
            break;
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
void TurnController::Render( bool debugModeEnabled )
{
    if (!m_renderer)
        return;


    if (m_selectedActor)
    {
        switch (m_turnStateMachine->GetCurrentStateID())
        {
        case TS_MOVE_ACT_EXIT_MENU:
            m_turnMenu->Render( debugModeEnabled );
            break;
        case TS_MOVE_ACTOR:
            m_selectedActor->RenderPossibleMoves( m_renderer );
            break;
        case TS_ACT_ACTOR:
            m_selectedActor->RenderPossibleAttacks( m_renderer );
            break;
        case TS_INTERRUPT:
            m_interruptMenu->Render( debugModeEnabled );
            break;
        default:
            break;
        }
    }

    // render cell height
//     static Cell* previousCell = nullptr;
//     if (m_currentHovedCell)
//     {
//         if (m_currentHovedCell != previousCell)
//         {
//             m_currentCellHeightTB->SetText( std::to_string( m_currentHovedCell->GetHeight() ) );
//             previousCell = m_currentHovedCell;
//         }
//         m_currentCellHeightTB->Render( debugModeEnabled );
//     }

    if (m_selectedActor && !m_interruptActor )
        m_currentActorInfo->Render( debugModeEnabled );
    if (m_targetActor)
        m_targetActorInfo->Render( debugModeEnabled );
}

////===========================================================================================
///===========================================================================================
// Private Functions
///===========================================================================================
////===========================================================================================

