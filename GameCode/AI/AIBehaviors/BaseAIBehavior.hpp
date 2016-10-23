//=================================================================================
// BaseAIBehavior.hpp
// Author: Tyler George
// Date  : November 11, 2015
//=================================================================================

#pragma once

#ifndef __included_BaseAIBehavior__
#define __included_BaseAIBehavior__

///---------------------------------------------------------------------------------
/// Includes
///---------------------------------------------------------------------------------
#include <string>
#include <map>

#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/OpenGLRenderer.hpp"
#include "Engine/Renderer/FontRenderer.hpp"
#include "Engine/Utilities/Utilities.hpp"
#include "Engine/Utilities/XMLHelper.hpp"
#include "GameCode/GameCommon.hpp"
#include "GameCode/Map.hpp"

class Actor;
struct InteractionEvent;

class AIBehaviorRegistration;
class BaseAIBehavior;

///---------------------------------------------------------------------------------
/// Typedefs
///---------------------------------------------------------------------------------
typedef std::map< std::string, AIBehaviorRegistration* > AIBehaviorRegistryMap;
typedef BaseAIBehavior* (AIBehaviorCreationFunc)(const std::string& name, const XMLNode& behaviorRoot );

////===========================================================================================
///===========================================================================================
// MapGeneratorRegistration Class
///===========================================================================================
////===========================================================================================
class AIBehaviorRegistration
{
public:
    ///---------------------------------------------------------------------------------
    /// Constructors/Destructors
    ///---------------------------------------------------------------------------------
    AIBehaviorRegistration( const std::string& name, AIBehaviorCreationFunc* creationFunction )
        : m_name( name )
        , m_creationFunc( creationFunction )
    {
        if (!s_registry)
            s_registry = new AIBehaviorRegistryMap();

        std::string lowercaseName = ConvertToLowerCase( name );
        s_registry->insert( std::pair< std::string, AIBehaviorRegistration* >( lowercaseName, this ) );
    }

    ///---------------------------------------------------------------------------------
    /// Accessors/Queries
    ///---------------------------------------------------------------------------------
    const std::string& GetName() const { return m_name; }
    static AIBehaviorRegistryMap* GetAIBehaviorRegistry() { return s_registry; }
    BaseAIBehavior* CreateAIBehavior( const XMLNode& behaviorRoot ) { return (*m_creationFunc)(m_name, behaviorRoot ); }

private:
    ///---------------------------------------------------------------------------------
    /// Private Member Variables
    ///---------------------------------------------------------------------------------
    AIBehaviorCreationFunc*   m_creationFunc;
    std::string                 m_name;

    ///---------------------------------------------------------------------------------
    /// Static Private Member Variables
    ///---------------------------------------------------------------------------------
    static AIBehaviorRegistryMap* s_registry;
};

////===========================================================================================
///===========================================================================================
// BaseAIBehavior Class
///===========================================================================================
////===========================================================================================
class BaseAIBehavior
{
public:
    ///---------------------------------------------------------------------------------
    /// Constructors/Destructors
    ///---------------------------------------------------------------------------------
    BaseAIBehavior( const std::string& name, const XMLNode& behaviorNode );
    BaseAIBehavior( const std::string& name );
    ~BaseAIBehavior();

    ///---------------------------------------------------------------------------------
    /// Initialization
    ///---------------------------------------------------------------------------------

    ///---------------------------------------------------------------------------------
    /// Accessors/Queries
    ///---------------------------------------------------------------------------------
    const std::string& GetName() { return m_name; }
    virtual float CalcUtility() = 0;
    virtual void Think() = 0;
    virtual BaseAIBehavior* Clone() = 0;


    ///---------------------------------------------------------------------------------
    /// Mutators
    ///---------------------------------------------------------------------------------
    void SetActor( Actor* actor ) { m_actor = actor; }

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


protected:
    ///---------------------------------------------------------------------------------
    /// Protected Functions
    ///---------------------------------------------------------------------------------

    ///---------------------------------------------------------------------------------
    /// Protected Member Variables
    ///---------------------------------------------------------------------------------
    std::string m_name;
    float m_chanceToCalcUtility;
    Actor* m_actor;
};

typedef std::vector< BaseAIBehavior* > AIBehaviors;
///---------------------------------------------------------------------------------
/// Inline function implementations
///---------------------------------------------------------------------------------

#endif