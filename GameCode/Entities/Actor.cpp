//=================================================================================
// Actor.cpp
// Author: Tyler George
// Date  : November 4, 2015
//=================================================================================


////===========================================================================================
///===========================================================================================
// Includes
///===========================================================================================
////===========================================================================================

#include "GameCode/Entities/Actor.hpp"
#include "GameCode/AI/Pathfinder.hpp"
#include "GameCode/Map.hpp"
#include "GameCode/CombatManager.hpp"
#include "Engine/Utilities/Profiler.hpp"
#include <math.h>
#include "Engine/Systems/Particles/Render_Strategies/PointsRenderStrategy.hpp"
#include "Engine/Systems/Particles/Update_Strategies/ExplosionUpdateStrategy.hpp"
#include "Engine/Systems/Particles/Update_Strategies/FountainUpdateStrategy.hpp"


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
Actor::Actor( OpenGLRenderer* renderer, Clock* parentClock, Faction faction, const std::string& jobName )
    : Entity( renderer, parentClock )
    , m_moveState( HAS_NOT_MOVED )
    , m_actState( HAS_NOT_ACTED )
    , m_currentMovePath( nullptr )
    , m_currentNextNode( nullptr )
    , m_faction( faction )
    , m_hasFinishedTurn( false )
    , m_isControlledByAI( false )
    , m_job( nullptr )
    , m_projectile( nullptr )
    , m_explosion( nullptr )
    , m_heal( nullptr )
{
    switch (m_faction)
    {
    case ENEMY:
        m_isControlledByAI = true;
        break;
    case ALLY:
        m_isControlledByAI = false;
        break;
    case NEUTRAL:
        m_isControlledByAI = true;
        break;
    default:
        break;
    }

    ChangeJob( jobName );

    // AI behaviors 
//     AIBehaviorRegistryMap* registry = AIBehaviorRegistration::GetAIBehaviorRegistry();
//     AIBehaviorRegistryMap::iterator chaseIter = registry->find( "chase" );
//     if (chaseIter != registry->end())
//     {
//         BaseAIBehavior* behavior = chaseIter->second->CreateAIBehavior( XMLNode::emptyNode() );
//         behavior->SetActor( this );
//         m_behaviors.push_back( behavior );
// 
//     }
// 
//     AIBehaviorRegistryMap::iterator meleeIter = registry->find( "melee" );
//     if (meleeIter != registry->end())
//     {
//         BaseAIBehavior* behavior = meleeIter->second->CreateAIBehavior( XMLNode::emptyNode() );
//         behavior->SetActor( this );
//         m_behaviors.push_back( behavior );
// 
//     }
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Actor::~Actor()
{
    if (m_currentMovePath)
        delete m_currentMovePath;
    delete m_clock;

    for (AIBehaviors::iterator behaviorIter = m_behaviors.begin(); behaviorIter != m_behaviors.end();)
    {
        BaseAIBehavior* behavior = *behaviorIter;
        behaviorIter = m_behaviors.erase( behaviorIter );

        delete behavior;
    }
}

////===========================================================================================
///===========================================================================================
// Initialization
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Actor::Startup()
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
CellPtrs Actor::GetPossibleMoves()
{
    if (m_possibleMoves.empty())
        UpdatePossibleMoves();

    return m_possibleMoves;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
CellPtrs Actor::GetPossibleAttacks()
{
    if (m_possibleAttacks.empty())
        UpdatePossibleAttacks();

    return m_possibleAttacks;
}


////===========================================================================================
///===========================================================================================
// Mutators
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
bool Actor::ApplyDamage( int damage )
{
    m_stats.m_health -= damage;

    bool actorDied = false;
    if (m_stats.m_health <= 0)
    {
        m_stats.m_health = 0;
        actorDied = true;
    }

    if (m_stats.m_health > m_stats.m_maxHealth)
        m_stats.m_health = m_stats.m_maxHealth;
    
    return actorDied;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Actor::UpdatePossibleMoves()
{
    int moveRange = GetMoveRange();
    MapPosition actorPos = GetMapPosition();

    CellPtrs moves;

    for (int x = actorPos.x - moveRange; x <= actorPos.x + moveRange; ++x)
    {
        for (int y = actorPos.y - moveRange; y <= actorPos.y + moveRange; ++y)
        {
            if (x == actorPos.x && y == actorPos.y)
                continue;

            MapPosition pos( x, y );

            Path* pathToCell = Pathfinder::CalculatePath( m_owningMap, this, actorPos, pos, true, false, false );

            if (pathToCell->m_reachedGoal && pathToCell->GetNumberOfSteps() <= moveRange )
            {
                Cell* cell = m_owningMap->GetCellAtMapPos( pos );
                if (cell && !cell->GetActor())
                    moves.push_back( cell );
            }

            delete pathToCell;
        }
    }
    m_possibleMoves = moves;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Actor::UpdatePossibleAttacks()
{
    MapPosition actorPos = GetMapPosition();
    Cell* actorCell = m_owningMap->GetCellAtMapPos( actorPos );

    CellPtrs attackPositions;

    // Melee
    if (m_job->GetName() == "Fighter")
    {
        attackPositions = m_owningMap->GetNeighbors( actorPos );
        for (CellPtrs::iterator cellIter = attackPositions.begin(); cellIter != attackPositions.end();)
        {
            Cell* cell = *cellIter;

            if ((cell && (abs( actorCell->GetHeight() - cell->GetHeight() )) >= 2.0f) || !cell)
                cellIter = attackPositions.erase( cellIter );
            else
                ++cellIter;
        }

        m_possibleAttacks = attackPositions;
    }

    else if (m_job->GetName() == "Archer")
    {
        m_possibleRangedAttacks.clear();

        static const float gravity = 9.81f;
        static const FltRange range = FltRange( 3.0f, 4.0f );
        static const int minRange = 2;

        // Ranged
        IntVector2 mapSize = m_owningMap->GetMapSize();
        for (int x = 0; x < mapSize.x; ++x)
        {
            for (int y = 0; y < mapSize.y; ++y)
            {
                if (x == m_mapPos.x && y == m_mapPos.y)
                    continue;

                Cell* cell = m_owningMap->GetCellAtMapPos( MapPosition( x, y ) );

                FlightPathData data( MapPosition( -1, -1 ), Vector3::ZERO );
                bool pathFound = Projectile::CalculateFlightPath( this, MapPosition( x, y ), data );

                if (pathFound)
                {
                    m_possibleRangedAttacks.insert( std::pair< MapPosition, FlightPathData > ( MapPosition( x, y ), data ) );
                    m_possibleAttacks.push_back( cell );
                }
            }
        }
    }
    
    else if (m_job->GetName() == "Wizard")
    {
        int spellRange = 3; // TODO: magic number
        MapPosition actorPos = GetMapPosition();

        CellPtrs attacks;

        for (int x = actorPos.x - spellRange; x <= actorPos.x + spellRange; ++x)
        {
            for (int y = actorPos.y - spellRange; y <= actorPos.y + spellRange; ++y)
            {
                if (m_owningMap->CalculateManhattanDistance( actorPos, MapPosition( x, y ) ) > spellRange)
                    continue;

                MapPosition pos( x, y );

                Cell* cell = m_owningMap->GetCellAtMapPos( pos );
                if (cell)
                {
                    Feature* feature = cell->GetFeature();
                    if (!feature || (feature && !feature->BlocksMovement()))
                        attacks.push_back( cell );

                }

             }
        }
        m_possibleAttacks = attacks;
    }
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Actor::MoveActor( const MapPosition& goal )
{
    SetMoveState( IS_MOVING );
    if (m_currentMovePath)
        delete m_currentMovePath;
    m_currentMovePath = Pathfinder::CalculatePath( m_owningMap, this, m_mapPos, goal, true, false, false );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Actor::AttackPosition( const MapPosition& targetPos )
{
    SetActState( IS_ACTING );
    m_currentTarget = targetPos;
    Cell* srcCell = m_owningMap->GetCellAtMapPos( m_mapPos );
    Cell* dstCell = m_owningMap->GetCellAtMapPos( targetPos );

    if (m_job->GetName() == "Archer")
    {

        FlightPathData flightPath = m_possibleRangedAttacks[targetPos];

        m_projectile = new Projectile( m_renderer, m_clock, flightPath.startingVelocity );
        m_projectile->SetSourceAndDest( Vector3( m_mapPos.x, srcCell->GetHeight(), m_mapPos.y ), Vector3( targetPos.x, dstCell->GetHeight(), targetPos.y ) );
        //s_theSoundSystem->PlayStreamingSound( g_arrow );
    }
    else if (m_job->GetName() == "Wizard")
    {
        static PointsRenderStrategy particleRenderStrat( 15.0f );
        static ExplosionUpdateStrategy explosionUpdateStrat( 1.0f, 1.0f, 2.0f, true );
        static FountainUpdateStrategy healUpdateStrat = FountainUpdateStrategy( 1.0f, 1.0f, Vector3( 0.0f, 1.0f, 0.0f ), 1.0f, 0.2f, 2.0f, false );

        Actor* target = m_owningMap->GetCellAtMapPos( m_currentTarget )->GetActor();

        if (target && target->GetFaction() == m_faction)
        {
            if (m_heal)
                __debugbreak();

            m_heal = new ParticleEmitter( Vector3( m_currentTarget.x + 0.5f, dstCell->GetHeight() + 0.5f, m_currentTarget.y + 0.5f ), 50, 0.3f, 1.0f, 2.0f, &healUpdateStrat, &particleRenderStrat );
            //s_theSoundSystem->PlayStreamingSound( g_heal );

        }
        else
        {
            if (m_explosion)
                __debugbreak();

            m_explosion = new ParticleEmitter( Vector3( m_currentTarget.x + 0.5f, dstCell->GetHeight() + 0.5f, m_currentTarget.y + 0.5f ), 50, 0.3f, 1.0f, 2.0f, &explosionUpdateStrat, &particleRenderStrat );
            //s_theSoundSystem->PlayStreamingSound( g_explosion );
        }
    }
    //else
        //s_theSoundSystem->PlayStreamingSound( g_hitA );


}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Actor::ChangeJob( const std::string& jobName )
{
    // remove old behaviors
    for each (BaseAIBehavior* behavior in m_behaviors)
        delete behavior;
    m_behaviors.clear();

    // add new behaviors
    UnitJob* newJob = UnitJob::FindUnitJobByName( jobName );
    m_job = newJob;
    AIBehaviors& behaviors = newJob->GetBehaviors();
    for each (BaseAIBehavior* behavior in behaviors)
        AddBehavior( behavior->Clone() );

    // change mesh
    Rgba meshColor;
    switch (m_faction)
    {
    case ENEMY:
        meshColor = Rgba( 0x663300, 1.0f );
        break;
    case ALLY:
        meshColor = Rgba( 0x336600, 1.0f );
        break;
    case NEUTRAL:
        meshColor = Rgba( 0x333300, 1.0f );
        break;
    default:
        break;
    }

    ChangeMesh( newJob->GetVerts(), newJob->GetIndicies(), meshColor );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Actor::AddBehavior( BaseAIBehavior* behavior )
{
    behavior->SetActor( this );
    m_behaviors.push_back( behavior );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Actor::Think()
{
    // TODO: calc utility for all ai behaviors and process one

    BaseAIBehavior* highestUtilityBehavior = nullptr;
    float highestUtility = 0.0f;
    for (AIBehaviors::iterator behaviorIter = m_behaviors.begin(); behaviorIter != m_behaviors.end(); ++behaviorIter)
    {
        BaseAIBehavior* behavior = *behaviorIter;

        float utility = behavior->CalcUtility();
        if (utility > highestUtility)
        {
            highestUtilityBehavior = behavior;
            highestUtility = utility;
        }

    }

    if (highestUtilityBehavior)
    {
        highestUtilityBehavior->Think();
    }

    // if no behavior has utility, and actor has not moved or acted then move randomly
    if (highestUtility == 0.0f && m_moveState == HAS_NOT_MOVED && m_actState == HAS_NOT_ACTED)
    {
        CellPtrs moves = GetPossibleMoves();
        if (!moves.empty())
        {
            int randomMove = GetRandomIntLessThan( moves.size() );

            MoveActor( moves[randomMove]->GetMapPosition() );
        }
        else
            SetFinishedTurn( true );
    }
    else if (highestUtility == 0.0f)
        SetFinishedTurn( true );
}

////===========================================================================================
///===========================================================================================
// Update
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Actor::ProcessInput( InputSystem* inputSystem )
{
    UNUSED( inputSystem );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Actor::Update( bool debugModeEnabled )
{
    UNUSED( debugModeEnabled );

    if (m_possibleMoves.empty())
        UpdatePossibleMoves();
    if (m_possibleAttacks.empty())
        UpdatePossibleAttacks();

    InterpolatePosition();
    InterpolateAttack();
    
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Actor::InterpolatePosition()
{
    if (m_moveState == IS_MOVING)
    {
        if (!m_currentNextNode)
            m_currentNextNode = m_currentMovePath->GetNextStep();

        Vector3 currentActorPos = m_renderPosition;

        MapPosition nextStopMapPos = m_currentNextNode->m_position;
        Cell* nextStopCell = m_owningMap->GetCellAtMapPos( nextStopMapPos );
        Vector3 nextStop = Vector3( (float)nextStopMapPos.x, (float)nextStopCell->GetHeight(), (float)nextStopMapPos.y );

        Vector3 dirToGo = ( Vector3( nextStop.x, 0.0f, nextStop.z ) - Vector3( currentActorPos.x, 0.0f, currentActorPos.z ) ).Normalized();
        
        if (AreFloatsEqual( nextStop.y, currentActorPos.y, 0.2f ))
            dirToGo = (nextStop - currentActorPos).Normalized(); 
        else if (nextStop.y > currentActorPos.y)
            dirToGo = Vector3( 0.0f, 1.0f, 0.0f );
        else if (nextStop.y < currentActorPos.y && ( AreFloatsEqual( nextStop.x, currentActorPos.x, 0.2f ) && AreFloatsEqual( nextStop.z, currentActorPos.z, 0.2f ) ) )
            dirToGo = Vector3( 0.0f, -1.0f, 0.0f );

        Vector3 newPos = currentActorPos + ( ( 7.0f * (float)m_clock->GetLastDeltaSeconds() ) * dirToGo);
        m_renderPosition = newPos;

        if (AreVectorsEqual( nextStop, newPos, 0.2f ))
        {
            m_currentNextNode = m_currentMovePath->GetNextStep();
            if (!m_currentNextNode)
            {
                m_owningMap->SetActorAtMapPosition( this, m_currentMovePath->m_goal );
                SetMoveState( HAS_MOVED );
                UpdatePossibleAttacks();
                //UpdatePossibleMoves();
            }
        }
    }
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Actor::InterpolateAttack()
{
    static bool hasHitTarget = false;

    if (m_actState == IS_ACTING)
    {
        if (!hasHitTarget)
        {
            if (m_job->GetName() == "Fighter")
            {
                Cell* targetCell = m_owningMap->GetCellAtMapPos( m_currentTarget );
                Vector3 targetPos = Vector3( (float)m_currentTarget.x, (float)targetCell->GetHeight(), (float)m_currentTarget.y );
                Vector3 currentActorPos = m_renderPosition;

                Vector3 dirToGo = (targetPos - currentActorPos).Normalized();

                Vector3 newPos = currentActorPos + ((7.0f * (float)m_clock->GetLastDeltaSeconds()) * dirToGo);
                m_renderPosition = newPos;

                if (AreVectorsEqual( newPos, targetPos, 0.3f ))
                {
                    hasHitTarget = true;
                    // Do damage here

                    AttackData data;
                    data.attacker = this;
                    data.chanceToHit = 1.0f;
                    data.chanceToCrit = 0.0f;
                    data.damageRange = IntRange( 10, 20 );
                    data.target = m_owningMap->GetCellAtMapPos( m_currentTarget )->GetActor();

                    AttackResult result = CombatManager::PerformMeleeAttack( data );

                    if (result.targetDied)
                    {
                        m_owningMap->RemoveActor( data.target );
                        switch (data.target->GetFaction())
                        {
                        case ALLY:
                        case NEUTRAL:
                            //s_theSoundSystem->PlayStreamingSound( g_deathMale );
                            break;
                        case ENEMY:
                            //s_theSoundSystem->PlayStreamingSound( g_deathMonster );
                            break;
                        }
                    }
                }

            }
            else if (m_job->GetName() == "Archer" && m_projectile )
            {
                m_projectile->Update();
                if (m_projectile->HasReachedDestination())
                {
                    hasHitTarget = true;

                    AttackData data;
                    data.attacker = this;
                    data.chanceToHit = 1.0f;
                    data.chanceToCrit = 0.0f;
                    data.damageRange = IntRange( 10, 20 );
                    data.target = m_owningMap->GetCellAtMapPos( m_currentTarget )->GetActor();

                    AttackResult result = CombatManager::PerformMeleeAttack( data );

                    //s_theSoundSystem->PlayStreamingSound( g_hitB );
                    if (result.targetDied)
                    {
                        m_owningMap->RemoveActor( data.target );

                        switch (data.target->GetFaction())
                        {
                        case ALLY:
                        case NEUTRAL:
                            //s_theSoundSystem->PlayStreamingSound( g_deathMale );
                            break;
                        case ENEMY:
                            //s_theSoundSystem->PlayStreamingSound( g_deathMonster );
                            break;
                        }
                    }
                }
            }
            else if (m_job->GetName() == "Wizard" )
            {
                if (m_explosion)
                    m_explosion->Update( m_clock->GetLastDeltaSeconds() );
                if (m_heal)
                    m_heal->Update( m_clock->GetLastDeltaSeconds() );

                if (m_explosion && m_explosion->IsFinished())
                {
                    delete m_explosion;
                    m_explosion = nullptr;

                    hasHitTarget = true;

                    AttackData data;
                    data.attacker = this;
                    data.chanceToHit = 1.0f;
                    data.chanceToCrit = 0.0f;
                    data.damageRange = IntRange( 10, 20 );
                    data.target = m_owningMap->GetCellAtMapPos( m_currentTarget )->GetActor();

                    AttackResult result = CombatManager::PerformMeleeAttack( data );

                    if (result.targetDied)
                    {
                        m_owningMap->RemoveActor( data.target );
                        switch (data.target->GetFaction())
                        {
                        case ALLY:
                        case NEUTRAL:
                            //s_theSoundSystem->PlayStreamingSound( g_deathMale );
                            break;
                        case ENEMY:
                            //s_theSoundSystem->PlayStreamingSound( g_deathMonster );
                            break;
                        }
                    }
                }

                if (m_heal && m_heal->IsFinished())
                {
                    delete m_heal;
                    m_heal = nullptr;

                    hasHitTarget = true;

                    Actor* target = m_owningMap->GetCellAtMapPos( m_currentTarget )->GetActor();
                    target->ApplyDamage( -(GetRandomIntInRange( 15, 20 )) );

                }
            }
        }
        else
        {
            if (m_job->GetName() == "Fighter")
            {
                Cell* myCell = m_owningMap->GetCellAtMapPos( m_mapPos );
                Vector3 targetPos = Vector3( (float)m_mapPos.x, (float)myCell->GetHeight(), (float)m_mapPos.y );
                Vector3 currentActorPos = m_renderPosition;

                Vector3 dirToGo = (targetPos - currentActorPos).Normalized();

                Vector3 newPos = currentActorPos + ((7.0f * (float)m_clock->GetLastDeltaSeconds()) * dirToGo);
                m_renderPosition = newPos;

                if (AreVectorsEqual( newPos, targetPos, 0.1f ))
                {
                    hasHitTarget = false;
                    m_actState = HAS_ACTED;
                }
            }

            else if (m_job->GetName() == "Archer")
            {
                delete m_projectile;
                m_projectile = nullptr;
                hasHitTarget = false;
                m_actState = HAS_ACTED;
            }
            else if (m_job->GetName() == "Wizard")
            {
                hasHitTarget = false;
                m_actState = HAS_ACTED;
            }
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
void Actor::Render( bool debugModeEnabled )
{
    if (m_projectile)
        m_projectile->Render( debugModeEnabled );

    if (m_heal)
        m_heal->Render( m_renderer, OpenGLRenderer::CreateOrGetShader( "basic" ), Rgba::GREEN, Rgba::BLUE );

    if (m_explosion )
        m_explosion->Render( m_renderer, OpenGLRenderer::CreateOrGetShader( "basic" ), Rgba::YELLOW, Rgba::RED );

    for (PUC_Vertexes::iterator vertIter = m_verts.begin(); vertIter != m_verts.end(); ++vertIter)
    {
        Vertex3D_PUC& vert = *vertIter;

        switch (m_faction)
        {
        case ENEMY:
            vert.color = Rgba( 0x663300, 1.0f );
            break;
        case ALLY:
            vert.color = Rgba( 0x336600, 1.0f );
            break;
        case NEUTRAL:
            vert.color = Rgba( 0x333300, 1.0f );
            break;
        default:
            break;
        }

    }
    m_mesh->SetVertexData( m_verts.data(), DrawInstructions( GL_LINE_LOOP, m_verts.size(), m_indicies.size(), true ), Vertex3D_PUC::GetVertexInfo() );

    Entity::Render( debugModeEnabled );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Actor::RenderPossibleMoves( OpenGLRenderer* renderer )
{
    renderer->Enable( GL_BLEND );
    renderer->BlendFunct( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    for (CellPtrs::const_iterator moveIter = m_possibleMoves.begin(); moveIter != m_possibleMoves.end(); ++moveIter)
    {
        Cell* move = *moveIter;
        MapPosition mapPos = move->GetMapPosition();

        float minX = (float)mapPos.x;
        float minY = 0.0f;
        float minZ = (float)mapPos.y;
        
        float maxX = minX + CELL_SIZE;
        float maxY = minY + move->GetHeight();
        float maxZ = minZ + CELL_SIZE;
        
        PUC_Vertexes verts;
        
        verts.push_back( Vertex3D_PUC( Vector3( minX, maxY + 0.01f, minZ ), Vector2::ZERO, Rgba( 0x00FFFF, 0.7f ) ) ); // 0
        verts.push_back( Vertex3D_PUC( Vector3( minX, maxY + 0.01f, maxZ ), Vector2::ZERO, Rgba( 0x00FFFF, 0.7f ) ) ); // 3
        verts.push_back( Vertex3D_PUC( Vector3( maxX, maxY + 0.01f, maxZ ), Vector2::ZERO, Rgba( 0x00FFFF, 0.7f ) ) ); // 2
        verts.push_back( Vertex3D_PUC( Vector3( maxX, maxY + 0.01f, minZ ), Vector2::ZERO, Rgba( 0x00FFFF, 0.7f ) ) ); // 1
        
        renderer->DrawVertexes( NULL, verts, GL_QUADS );
    }
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Actor::RenderPossibleAttacks( OpenGLRenderer* renderer )
{
    renderer->Enable( GL_BLEND );
    renderer->BlendFunct( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

    for (CellPtrs::const_iterator moveIter = m_possibleAttacks.begin(); moveIter != m_possibleAttacks.end(); ++moveIter)
    {
        Cell* move = *moveIter;
        MapPosition mapPos = move->GetMapPosition();

        float minX = (float)mapPos.x;
        float minY = 0.0f;
        float minZ = (float)mapPos.y;

        float maxX = minX + CELL_SIZE;
        float maxY = minY + move->GetHeight();
        float maxZ = minZ + CELL_SIZE;

        Actor* actor = move->GetActor();
        Rgba overlayColor = Rgba( 0xFF0000, 0.7f );
        if (actor)
        {
            switch (actor->GetFaction())
            {
            case ALLY:
                overlayColor = Rgba( 0x00FF00, 0.7f );
                break;
            default:
                break;
            }
        }
        PUC_Vertexes verts;

        verts.push_back( Vertex3D_PUC( Vector3( minX, maxY + 0.01f, minZ ), Vector2::ZERO, overlayColor ) ); // 0
        verts.push_back( Vertex3D_PUC( Vector3( minX, maxY + 0.01f, maxZ ), Vector2::ZERO, overlayColor ) ); // 3
        verts.push_back( Vertex3D_PUC( Vector3( maxX, maxY + 0.01f, maxZ ), Vector2::ZERO, overlayColor ) ); // 2
        verts.push_back( Vertex3D_PUC( Vector3( maxX, maxY + 0.01f, minZ ), Vector2::ZERO, overlayColor ) ); // 1



        renderer->DrawVertexes( NULL, verts, GL_QUADS );


        if (m_job->GetName() == "Archer")
        {
            if (move->IsHovered())
            {
                FlightPathMap::iterator flightPathIter = m_possibleRangedAttacks.find( move->GetMapPosition() );
                if (flightPathIter != m_possibleRangedAttacks.end())
                {
                    renderer->SetLineSize( 5.0f );
                    renderer->DrawVertexes( NULL, flightPathIter->second.arcVerts, GL_LINES );
                }
            }
        }
    }
}

////===========================================================================================
///===========================================================================================
// Private Functions
///===========================================================================================
////===========================================================================================

