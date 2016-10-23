//=================================================================================
// RangedAttackBehavior.cpp
// Author: Tyler George
// Date  : November 15, 2015
//=================================================================================

////===========================================================================================
///===========================================================================================
// Includes
///===========================================================================================
////===========================================================================================

#include "GameCode/AI/AIBehaviors/RangedAttackBehavior.hpp"
#include "GameCode/Entities/Actor.hpp"
#include "GameCode/CombatManager.hpp"


////===========================================================================================
///===========================================================================================
// Static Variable Initialization
///===========================================================================================
////===========================================================================================

AIBehaviorRegistration RangedAttackBehavior::s_rangedAttackBehaviorRegistration( "Ranged", &RangedAttackBehavior::CreateAIBehavior );


////===========================================================================================
///===========================================================================================
// Constructors/Destructors
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
RangedAttackBehavior::RangedAttackBehavior( const std::string& name, const XMLNode& behaviorRoot )
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
RangedAttackBehavior::RangedAttackBehavior( const RangedAttackBehavior& copy )
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
RangedAttackBehavior::~RangedAttackBehavior()
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
BaseAIBehavior* RangedAttackBehavior::CreateAIBehavior( const std::string& name, const XMLNode& behaviorRoot )
{
    return new RangedAttackBehavior( name, behaviorRoot );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
float RangedAttackBehavior::CalcUtility()
{
    if (m_actor->GetActState() == HAS_ACTED)
        return 0.0f;

    bool calcUtility = GetRandomFloatZeroToOne() <= m_chanceToCalcUtility ? true : false;

    if (!calcUtility)
        return 0.0f;

    // Getting possible attack locations
    CellPtrs attackPositions = m_actor->GetPossibleAttacks();


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
                else if (actorFaction != m_actor->GetFaction() && ( ( hostileTarget && actor->GetHealth() < hostileTarget->GetHealth()) || !hostileTarget ) )
                {
                    hostileTarget = actor;
                    break;
                }
            }
        }
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
void RangedAttackBehavior::Think()
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

//         AttackResult result = CombatManager::PerformRangedAttack( data );
    }
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
BaseAIBehavior* RangedAttackBehavior::Clone()
{
    BaseAIBehavior* clone = new RangedAttackBehavior( *this );

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

