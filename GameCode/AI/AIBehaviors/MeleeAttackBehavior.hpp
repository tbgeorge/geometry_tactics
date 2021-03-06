//=================================================================================
// MeleeAttackBehavior.hpp
// Author: Tyler George
// Date  : November 15, 2015
//=================================================================================

#pragma once

#ifndef __included_MeleeAttackBehavior__
#define __included_MeleeAttackBehavior__

///---------------------------------------------------------------------------------
/// Includes
///---------------------------------------------------------------------------------
#include "GameCode/AI/AIBehaviors/BaseAIBehavior.hpp"


///---------------------------------------------------------------------------------
/// Typedefs
///---------------------------------------------------------------------------------


////===========================================================================================
///===========================================================================================
// MeleeAttackBehavior Class
///===========================================================================================
////===========================================================================================
class MeleeAttackBehavior : public BaseAIBehavior
{
public:
    ///---------------------------------------------------------------------------------
    /// Constructors/Destructors
    ///---------------------------------------------------------------------------------
    MeleeAttackBehavior( const std::string& name, const XMLNode& behaviorRoot );
    MeleeAttackBehavior( const MeleeAttackBehavior& copy );
    ~MeleeAttackBehavior();

    ///---------------------------------------------------------------------------------
    /// Initialization
    ///---------------------------------------------------------------------------------

    ///---------------------------------------------------------------------------------
    /// Accessors/Queries
    ///---------------------------------------------------------------------------------
    static BaseAIBehavior* CreateAIBehavior( const std::string& name, const XMLNode& behaviorRoot );
    float CalcUtility();
    void Think();
    BaseAIBehavior* Clone();

    ///---------------------------------------------------------------------------------
    /// Mutators
    ///---------------------------------------------------------------------------------

    ///---------------------------------------------------------------------------------
    /// Update
    ///---------------------------------------------------------------------------------

    ///---------------------------------------------------------------------------------
    /// Render
    ///---------------------------------------------------------------------------------

    ///---------------------------------------------------------------------------------
    /// Public Member Variables
    ///---------------------------------------------------------------------------------

    ///---------------------------------------------------------------------------------
    /// Static Variables
    ///---------------------------------------------------------------------------------
    static AIBehaviorRegistration s_MeleeAttackBehaviorRegistration;

private:
    ///---------------------------------------------------------------------------------
    /// Private Functions
    ///---------------------------------------------------------------------------------

    ///---------------------------------------------------------------------------------
    /// Private Member Variables
    ///---------------------------------------------------------------------------------
    IntRange m_damageRange;
    float m_chanceToHit;
    float m_chanceToCrit;

    Actor* m_target;
};

///---------------------------------------------------------------------------------
/// Inline function implementations
///---------------------------------------------------------------------------------

#endif