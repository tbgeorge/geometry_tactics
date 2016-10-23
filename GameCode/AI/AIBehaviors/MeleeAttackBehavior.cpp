//=================================================================================
// MeleeAttackBehavior.cpp
// Author: Tyler George
// Date  : November 15, 2015
//=================================================================================

////===========================================================================================
///===========================================================================================
// Includes
///===========================================================================================
////===========================================================================================

#include "GameCode/AI/AIBehaviors/MeleeAttackBehavior.hpp"
#include "GameCode/Entities/Actor.hpp"
#include "GameCode/CombatManager.hpp"


////===========================================================================================
///===========================================================================================
// Static Variable Initialization
///===========================================================================================
////===========================================================================================

AIBehaviorRegistration MeleeAttackBehavior::s_MeleeAttackBehaviorRegistration( "Melee", &MeleeAttackBehavior::CreateAIBehavior );


////===========================================================================================
///===========================================================================================
// Constructors/Destructors
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
MeleeAttackBehavior::MeleeAttackBehavior( const std::string& name, const XMLNode& behaviorRoot )
    : BaseAIBehavior( name, behaviorRoot )
    , m_chanceToHit( 1.0f )
    , m_chanceToCrit( 0.2f )
    , m_target( nullptr )
{
    m_damageRange = GetIntIntervalProperty( behaviorRoot, "damage" );
    m_chanceToHit = GetFloatProperty( behaviorRoot, "chanceToHit", 1.0f );
    m_chanceToCrit = GetFloatProperty( behaviorRoot, "chanceToCrit", 0.0f );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
MeleeAttackBehavior::MeleeAttackBehavior( const MeleeAttackBehavior& copy )
    : BaseAIBehavior( copy.m_name )
{
    m_chanceToCalcUtility = copy.m_chanceToCalcUtility;
    m_damageRange = copy.m_damageRange;
    m_chanceToHit = copy.m_chanceToHit;
    m_chanceToCrit = copy.m_chanceToCrit;
    m_target = copy.m_target;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
MeleeAttackBehavior::~MeleeAttackBehavior()
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
BaseAIBehavior* MeleeAttackBehavior::CreateAIBehavior( const std::string& name, const XMLNode& behaviorRoot )
{
    return new MeleeAttackBehavior( name, behaviorRoot );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
float MeleeAttackBehavior::CalcUtility()
{
    if (m_actor->GetActState() == HAS_ACTED)
        return 0.0f;

    bool calcUtility = GetRandomFloatZeroToOne() <= m_chanceToCalcUtility ? true : false;

    if (!calcUtility)
        return 0.0f;

    // Getting possible attack locations
    MapPosition actorPos = m_actor->GetMapPosition();
    Cell* actorCell = m_actor->GetMap()->GetCellAtMapPos( actorPos );

    CellPtrs attackPositions;

    attackPositions = m_actor->GetMap()->GetNeighbors( actorPos );
    for (CellPtrs::iterator cellIter = attackPositions.begin(); cellIter != attackPositions.end();)
    {
        Cell* cell = *cellIter;

        if ((cell && (abs( actorCell->GetHeight() - cell->GetHeight() )) >= 2.0f) || !cell)
            cellIter = attackPositions.erase( cellIter );
        else
            ++cellIter;
    }


    // finding a valid target
    Actor* hostileTarget = nullptr;
    Actor* neutralTarget = nullptr;

    Actors allActors = m_actor->GetMap()->GetAllActors();

    for (Actors::iterator actorIter = allActors.begin(); actorIter != allActors.end(); ++actorIter)
    {
        Actor* actor = *actorIter;

        if (actor == m_actor)
            continue;

        Faction actorFaction = actor->GetFaction();

        MapPosition actorPos = actor->GetMapPosition();

        for (CellPtrs::iterator cellIter = attackPositions.begin(); cellIter != attackPositions.end(); ++cellIter )
        {
            Cell* cell = *cellIter;
            MapPosition cellPos = cell->GetMapPosition();

            if (actorPos == cellPos)
            {
                if (actorFaction == NEUTRAL)
                    neutralTarget = actor;
                else if (actorFaction != m_actor->GetFaction())
                {
                    hostileTarget = actor;
                    break;
                }
            }
        }
        if (hostileTarget)
            break;
    }

    if (hostileTarget)
    {
        m_target = hostileTarget;
        return 10.0f;
    }
    else if (neutralTarget)
    {
        m_target = neutralTarget;
        return 5.0f;
    }
    else
    {
        m_target = nullptr;
        return 0.0f;
    }
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void MeleeAttackBehavior::Think()
{
    if (m_target)
    {
//         AttackData data;
//         data.attacker = m_actor;
//         data.target = m_target;
//         data.chanceToCrit = m_chanceToCrit;
//         data.chanceToHit = m_chanceToHit;
//         data.damageRange = m_damageRange;
//         data.armorRange = IntRange::ZERO;

        m_actor->AttackPosition( m_target->GetMapPosition() );
        m_target = nullptr;

//         AttackResult result = CombatManager::PerformMeleeAttack( data );
    }
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
BaseAIBehavior* MeleeAttackBehavior::Clone()
{
    BaseAIBehavior* clone = new MeleeAttackBehavior( *this );

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

