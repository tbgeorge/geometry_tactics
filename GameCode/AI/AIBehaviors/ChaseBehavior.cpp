//=================================================================================
// ChaseBehavior.cpp
// Author: Tyler George
// Date  : September 18, 2015
//=================================================================================


////===========================================================================================
///===========================================================================================
// Includes
///===========================================================================================
////===========================================================================================

#include "GameCode/AI/AIBehaviors/ChaseBehavior.hpp"
#include "GameCode/Entities/Actor.hpp"
#include "../Pathfinder.hpp"


////===========================================================================================
///===========================================================================================
// Static Variable Initialization
///===========================================================================================
////===========================================================================================

AIBehaviorRegistration ChaseBehavior::s_ChaseBehaviorRegistration( "Chase", &ChaseBehavior::CreateAIBehavior );


////===========================================================================================
///===========================================================================================
// Constructors/Destructors
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
ChaseBehavior::ChaseBehavior( const std::string& name, const XMLNode& behaviorRoot )
    : BaseAIBehavior( name, behaviorRoot )
    , m_range( 0 )
    , m_maxDistance( 0 )
    , m_chanceToChase( 1.0f )
    , m_target( nullptr )
{
    m_maxDistance = GetIntProperty( behaviorRoot, "maxDistance", 0 );
    m_range = GetIntProperty( behaviorRoot, "range", 5 );
    m_chanceToChase = GetFloatProperty( behaviorRoot, "chanceToChase", 1.0f );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
ChaseBehavior::ChaseBehavior( const ChaseBehavior& copy )
    : BaseAIBehavior( copy.m_name )
{
    m_chanceToCalcUtility = copy.m_chanceToCalcUtility;
    m_maxDistance = copy.m_maxDistance;
    m_range = copy.m_range;
    m_chanceToChase = copy.m_chanceToChase;
    m_target = copy.m_target;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
ChaseBehavior::~ChaseBehavior()
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
BaseAIBehavior* ChaseBehavior::CreateAIBehavior( const std::string& name, const XMLNode& behaviorRoot )
{
    return new ChaseBehavior( name, behaviorRoot );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
float ChaseBehavior::CalcUtility()
{
    if (m_actor->GetMoveState() == HAS_MOVED || m_actor->GetActState() == HAS_ACTED )
        return 0.0f;

    bool calcUtility = GetRandomFloatZeroToOne() <= m_chanceToCalcUtility ? true : false;

    if (!calcUtility)
        return 0.0f;

    Actor* hostileTarget = nullptr;
    int numStepsToHostile = -1;
    Actor* neutralTarget = nullptr;
    int numStepsToNeutral = -1;


    Actors allActors = m_actor->GetMap()->GetAllActors();

    // find closest hostile and neutral targets
    for (Actors::iterator actorIter = allActors.begin(); actorIter != allActors.end(); ++actorIter)
    {
        Actor* actor = *actorIter;
        if (actor == m_actor)
            continue;

        Faction actorFaction = actor->GetFaction();

        Path* pathToActor = Pathfinder::CalculatePath( m_actor->GetMap(), m_actor, m_actor->GetMapPosition(), actor->GetMapPosition(), true, true, true );
        int numSteps = pathToActor->GetNumberOfSteps();

        if (pathToActor->m_isFinished)
        {
            if (actorFaction == NEUTRAL)
            {
                if ((numStepsToNeutral != -1 && numSteps < numStepsToNeutral) || numStepsToNeutral == -1)
                {
                    numStepsToNeutral = numSteps;
                    neutralTarget = actor;
                }
            }
            else if (actorFaction != m_actor->GetFaction())
            {
                if ((numStepsToHostile != -1 && numSteps < numStepsToHostile) || numStepsToHostile == -1)
                {
                    numStepsToHostile = numSteps;
                    hostileTarget = actor;
                }
            }
        }

        delete pathToActor;
    }

    // found a hostile target
    if (hostileTarget)
    {
        int distToHostile = Map::CalculateManhattanDistance( m_actor->GetMapPosition(), hostileTarget->GetMapPosition() );
        if ( ( m_maxDistance != 0 &&  distToHostile > m_maxDistance ) || ( m_maxDistance == 0 && distToHostile > 1 ) )
        {
            m_target = hostileTarget;
            return (2.0f);// +(float)numStepsToHostile);
        }
    }
    // no hostile target but found neutral target
    else if (neutralTarget)
    {
        int distToNeutral = Map::CalculateManhattanDistance( m_actor->GetMapPosition(), neutralTarget->GetMapPosition() );
        if ( ( m_maxDistance != 0 && distToNeutral > m_maxDistance ) || ( m_maxDistance == 0 && distToNeutral > 1 ) )
        {
            m_target = neutralTarget;
            return (1.0f);// +(float)numStepsToNeutral);
        }
    }
    // found no targets
    else
    {
        m_target = nullptr;
        return 0.0f;
    }

    return 0.0f;
   
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void ChaseBehavior::Think()
{

    if (m_target)
    {
        Path* pathToTarget = Pathfinder::CalculatePath( m_actor->GetMap(), m_actor, m_actor->GetMapPosition(), m_target->GetMapPosition(), true, true, false );

        PathNode* nodeIter = pathToTarget->GetNextStep();
        
        CellPtrs moves = m_actor->GetPossibleMoves();

        MapPosition currentMovePos( -1, -1 );

        while (nodeIter)
        {
            MapPosition nodePos = nodeIter->m_position;

            bool canBeMovedTo = false;
            for (CellPtrs::iterator moveIter = moves.begin(); moveIter != moves.end(); ++moveIter)
            {
                Cell* cell = *moveIter;
                MapPosition cellPos = cell->GetMapPosition();

                if (nodePos == cellPos && !cell->GetActor() && m_actor->GetMap()->CalculateManhattanDistance( nodePos, m_target->GetMapPosition() ) >= m_maxDistance )
                {
                    canBeMovedTo = true;
                    break;
                }
            }


            if (canBeMovedTo)
                currentMovePos = nodePos;

            nodeIter = pathToTarget->GetNextStep();
        }

        if (currentMovePos != MapPosition( -1, -1 ))
        {
            m_actor->MoveActor( currentMovePos );
            m_target = nullptr;
        }

        delete pathToTarget;
    }
    return;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
BaseAIBehavior* ChaseBehavior::Clone()
{
    BaseAIBehavior* clone = new ChaseBehavior( *this );

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

