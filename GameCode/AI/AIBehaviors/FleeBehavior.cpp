//=================================================================================
// FleeBehavior.cpp
// Author: Tyler George
// Date  : September 18, 2015
//=================================================================================


////===========================================================================================
///===========================================================================================
// Includes
///===========================================================================================
////===========================================================================================

#include "GameCode/AI/AIBehaviors/FleeBehavior.hpp"
#include "GameCode/Entities/Actor.hpp"
#include "../Pathfinder.hpp"


////===========================================================================================
///===========================================================================================
// Static Variable Initialization
///===========================================================================================
////===========================================================================================

AIBehaviorRegistration FleeBehavior::s_fleeBehaviorRegistration( "Flee", &FleeBehavior::CreateAIBehavior );


////===========================================================================================
///===========================================================================================
// Constructors/Destructors
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
FleeBehavior::FleeBehavior( const std::string& name, const XMLNode& behaviorRoot )
    : BaseAIBehavior( name, behaviorRoot )
    , m_range( 0 )
    , m_minDistance( 0 )
    , m_chanceToFlee( 1.0f )
    , m_targetPos(MapPosition( -1, -1 ))
{
    m_minDistance = GetIntProperty( behaviorRoot, "minDistance", 0 );
    m_range = GetIntProperty( behaviorRoot, "range", 5 );
    m_chanceToFlee = GetFloatProperty( behaviorRoot, "chanceToFlee", 1.0f );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
FleeBehavior::FleeBehavior( const FleeBehavior& copy )
    : BaseAIBehavior( copy.m_name )
{
    m_chanceToCalcUtility = copy.m_chanceToCalcUtility;
    m_minDistance = copy.m_minDistance;
    m_range = copy.m_range;
    m_chanceToFlee = copy.m_chanceToFlee;
    m_targetPos = MapPosition( -1, -1 );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
FleeBehavior::~FleeBehavior()
{

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

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
BaseAIBehavior* FleeBehavior::CreateAIBehavior( const std::string& name, const XMLNode& behaviorRoot )
{
    return new FleeBehavior( name, behaviorRoot );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
float FleeBehavior::CalcUtility()
{
    if (m_actor->GetMoveState() == HAS_MOVED)
        return 0.0f;

    bool calcUtility = GetRandomFloatZeroToOne() <= m_chanceToCalcUtility ? true : false;

    if (!calcUtility)
        return 0.0f;


    Actors allActors = m_actor->GetMap()->GetAllActors();

    CellPtrs moves = m_actor->GetPossibleMoves();

    // find closest hostile and neutral targets
    for (Actors::iterator actorIter = allActors.begin(); actorIter != allActors.end(); ++actorIter)
    {
        Actor* actor = *actorIter;
        if (actor == m_actor)
            continue;

        Faction actorFaction = actor->GetFaction();

        Faction enemyFaction = NEUTRAL;
        switch (actorFaction )
        {
        case ENEMY:
            enemyFaction = ALLY;
            break;
        case ALLY:
            enemyFaction = ENEMY;
            break;
        default:
            break;
        }


        MapPosition actorPos = actor->GetMapPosition();
        int actorDist = m_actor->GetMap()->CalculateManhattanDistance( actorPos, m_actor->GetMapPosition() );

        if (actorFaction != m_actor->GetFaction() && actorFaction != NEUTRAL)
        {
            if ( actorDist < m_minDistance)
            { 
                int furthestMoveDist = -1;
                for (CellPtrs::iterator moveIter = moves.begin(); moveIter != moves.end(); ++moveIter)
                {
                    Cell* move = *moveIter;

                    int distToNearestHostile = m_actor->GetMap()->CalculateDistToNearestActorOfFaction( move->GetMapPosition(), &enemyFaction );
                    if (furthestMoveDist == -1 || distToNearestHostile > furthestMoveDist)
                    {
                        furthestMoveDist = distToNearestHostile;
                        m_targetPos = move->GetMapPosition();
                    }                        
                }

                if (furthestMoveDist != -1 || furthestMoveDist > actorDist)
                    return 3.0f;
                else
                {
                    m_targetPos = MapPosition( -1, -1 );
                    return 0.0f;
                }
            }
        }
    }

    return 0.0f;
   
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void FleeBehavior::Think()
{
    if ( m_targetPos != MapPosition( -1, -1 ) )
        m_actor->MoveActor( m_targetPos );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
BaseAIBehavior* FleeBehavior::Clone()
{
    BaseAIBehavior* clone = new FleeBehavior( *this );

    return clone;
}

////===========================================================================================
///===========================================================================================
// Mutators
///===========================================================================================
////===========================================================================================


////===========================================================================================
///===========================================================================================
// Update
///===========================================================================================
////===========================================================================================


////===========================================================================================
///===========================================================================================
// Render
///===========================================================================================
////===========================================================================================


////===========================================================================================
///===========================================================================================
// Private Functions
///===========================================================================================
////===========================================================================================

