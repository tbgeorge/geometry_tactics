//=================================================================================
// HealBehavior.hpp
// Author: Tyler George
// Date  : November 15, 2015
//=================================================================================

#pragma once

#ifndef __included_HealBehavior__
#define __included_HealBehavior__

///---------------------------------------------------------------------------------
/// Includes
///---------------------------------------------------------------------------------
#include "GameCode/AI/AIBehaviors/BaseAIBehavior.hpp"


///---------------------------------------------------------------------------------
/// Typedefs
///---------------------------------------------------------------------------------


////===========================================================================================
///===========================================================================================
// HealBehavior Class
///===========================================================================================
////===========================================================================================
class HealBehavior : public BaseAIBehavior
{
public:
    ///---------------------------------------------------------------------------------
    /// Constructors/Destructors
    ///---------------------------------------------------------------------------------
    HealBehavior( const std::string& name, const XMLNode& behaviorRoot );
    HealBehavior( const HealBehavior& copy );
    ~HealBehavior();

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
    static AIBehaviorRegistration s_healBehaviorRegistration;

private:
    ///---------------------------------------------------------------------------------
    /// Private Functions
    ///---------------------------------------------------------------------------------

    ///---------------------------------------------------------------------------------
    /// Private Member Variables
    ///---------------------------------------------------------------------------------
    IntRange m_healRange;
    float m_chanceToHit;
    float m_chanceToCrit;

    Actor* m_target;
};

///---------------------------------------------------------------------------------
/// Inline function implementations
///---------------------------------------------------------------------------------

#endif