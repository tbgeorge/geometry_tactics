//=================================================================================
// Actor.hpp
// Author: Tyler George
// Date  : November 4, 2015
//=================================================================================

#pragma once

#ifndef __included_Actor__
#define __included_Actor__

///---------------------------------------------------------------------------------
/// Includes
///---------------------------------------------------------------------------------
#include "GameCode/GameCommon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/OpenGLRenderer.hpp"

#include "GameCode/Entities/Entity.hpp"
#include "Engine/Math/IntRange.hpp"
#include "Engine/Utilities/Time.hpp"
#include "GameCode/AI/AIBehaviors/BaseAIBehavior.hpp"
#include "../UnitJob.hpp"
#include "Projectile.hpp"
#include "Engine/Systems/Particles/ParticleEmitter.hpp"

class Path;
struct PathNode;

///---------------------------------------------------------------------------------
/// Structs
///---------------------------------------------------------------------------------

struct ActorStats
{
    ActorStats()
        : m_health( 100 ), m_maxHealth( 100 ), m_speed( 10 ), m_moveRange( 4 ), m_jumpRange( 3 ), m_baseAttackRange( IntRange( 0, 0 ) ) {}

    int m_health;
    int m_maxHealth;
    int m_speed;
    int m_moveRange;
    int m_jumpRange;
    IntRange m_baseAttackRange;

};

///---------------------------------------------------------------------------------
/// Enums
///---------------------------------------------------------------------------------
enum MoveState
{
    HAS_NOT_MOVED,
    IS_MOVING,
    HAS_MOVED
};

enum ActState
{
    HAS_NOT_ACTED,
    IS_ACTING,
    HAS_ACTED
};

enum Faction
{
    ENEMY,
    ALLY,
    NEUTRAL
};

////===========================================================================================
///===========================================================================================
// Actor Class
///===========================================================================================
////===========================================================================================
class Actor : public Entity
{
public:
    ///---------------------------------------------------------------------------------
    /// Constructors/Destructors
    ///---------------------------------------------------------------------------------
    Actor( OpenGLRenderer* renderer, Clock* parentClock, Faction faction = NEUTRAL, const std::string& jobName = "Fighter" );
    ~Actor();

    ///---------------------------------------------------------------------------------
    /// Initialization
    ///---------------------------------------------------------------------------------
    void Startup();

    ///---------------------------------------------------------------------------------
    /// Accessors/Queries
    ///---------------------------------------------------------------------------------
    int GetMoveRange() const { return m_stats.m_moveRange; }
    int GetJumpRange() const { return m_stats.m_jumpRange; }
    int GetMaxHealth() const { return m_stats.m_maxHealth; }
    int GetHealth() const { return m_stats.m_health; }

    MoveState GetMoveState() const { return m_moveState; }
    ActState GetActState() const { return m_actState; }

    Faction GetFaction() const { return m_faction; }
    CellPtrs GetPossibleMoves(); //const { return m_possibleMoves; }
    CellPtrs GetPossibleAttacks(); // const { return m_possibleAttacks; }

    int GetSpeed() const { return m_stats.m_speed; }

    bool HasFinishedTurn() const { return m_hasFinishedTurn; }
    bool IsDead() const { return m_stats.m_health <= 0; }
    bool IsControlledByAI() const { return m_isControlledByAI; }

    ///---------------------------------------------------------------------------------
    /// Mutators
    ///---------------------------------------------------------------------------------
    //     void SetHealth( int health );
    //     void SetMaxHealth( int maxHealth ) { m_stats.m_maxHealth = maxHealth; }
    void SetSpeed( int speed ) { m_stats.m_speed = speed; }
    //     void SetMoveRange( int moveRange ) { m_stats.m_moveRange = moveRange; }
    //     void SetJumpRange( int jumpRange ) { m_stats.m_jumpRange = jumpRange; }
    void SetMoveState( MoveState newMoveState ) { m_moveState = newMoveState; }
    void SetActState( ActState newActState ) { m_actState = newActState; }

    bool ApplyDamage( int damage );

    void UpdatePossibleMoves();
    void UpdatePossibleAttacks();

    void MoveActor( const MapPosition& goal );
    void AttackPosition( const MapPosition& targetPos );
    
    void SetFaction( Faction faction ) { m_faction = faction; }

    void SetFinishedTurn( bool finished ) { m_hasFinishedTurn = finished; }
    void SetControlledByAI( bool aiControlled ) { m_isControlledByAI = aiControlled; }

    void ChangeJob( const std::string& jobName );
    void AddBehavior( BaseAIBehavior* behavior );

    // AI
    void Think();

    ///---------------------------------------------------------------------------------
    /// Update
    ///---------------------------------------------------------------------------------
    void ProcessInput( InputSystem* inputSystem );
    void Update( bool debugModeEnabled );

    void InterpolatePosition();
    void InterpolateAttack();

    ///---------------------------------------------------------------------------------
    /// Render
    ///---------------------------------------------------------------------------------
    void Render( bool debugModeEnabled );

    void RenderPossibleMoves( OpenGLRenderer* renderer );
    void RenderPossibleAttacks( OpenGLRenderer* renderer );

    ///---------------------------------------------------------------------------------
    /// Public Member Variables
    ///---------------------------------------------------------------------------------

    ///---------------------------------------------------------------------------------
    /// Static Variables
    ///---------------------------------------------------------------------------------


private:
    ///---------------------------------------------------------------------------------
    /// Private Functions
    ///---------------------------------------------------------------------------------

    ///---------------------------------------------------------------------------------
    /// Private Member Variables
    ///---------------------------------------------------------------------------------
    ActorStats m_stats;

    MoveState m_moveState;
    ActState m_actState;

    CellPtrs m_possibleMoves;
    CellPtrs m_possibleAttacks;
    FlightPathMap m_possibleRangedAttacks;

    MapPosition m_currentTarget;

    Path* m_currentMovePath;
    PathNode* m_currentNextNode;


    Faction m_faction;
    bool m_hasFinishedTurn;

    UnitJob* m_job;

    AIBehaviors m_behaviors;

    bool m_isControlledByAI;
    
    Projectile* m_projectile;
    ParticleEmitter* m_explosion;
    ParticleEmitter* m_heal;


//     PUC_Vertexes m_flightPathTest;
//     PUC_Vertexes m_angleTest;
};


///---------------------------------------------------------------------------------
/// Inline function implementations
///---------------------------------------------------------------------------------

#endif