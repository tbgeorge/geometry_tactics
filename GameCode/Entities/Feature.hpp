//=================================================================================
// Feature.hpp
// Author: Tyler George
// Date  : November 23, 2015
//=================================================================================

#pragma once


#ifndef __included_Feature__
#define __included_Feature__

///---------------------------------------------------------------------------------
/// Includes
///---------------------------------------------------------------------------------
#include "GameCode/Entities/Entity.hpp"
#include "Engine/Math/IntRange.hpp"

///---------------------------------------------------------------------------------
/// Typedefs
///---------------------------------------------------------------------------------

///---------------------------------------------------------------------------------
/// Enums
///---------------------------------------------------------------------------------
enum FeatureType
{
    FT_TREE,
    FT_SMALL_ROCK,
    FT_LARGE_ROCK,
    FT_INVALID
    
};

////===========================================================================================
///===========================================================================================
// Feature Class
///===========================================================================================
////===========================================================================================
class Feature : public Entity
{
public:
    ///---------------------------------------------------------------------------------
    /// Constructors/Destructors
    ///---------------------------------------------------------------------------------
    Feature( OpenGLRenderer* renderer, Clock* parentClock, const XMLNode& featureNode );
    Feature( const Feature& copy, OpenGLRenderer* renderer, Clock* parentClock, const XMLNode& featureNode );
    ~Feature();

    ///---------------------------------------------------------------------------------
    /// Initialization
    ///---------------------------------------------------------------------------------

    ///---------------------------------------------------------------------------------
    /// Accessors/Queries
    ///---------------------------------------------------------------------------------
    FeatureType GetType() { return m_type; }
    bool BlocksLOS() { return m_blocksLOS; }
    bool BlocksMovement() { return m_blocksMovement; }
    static std::string GetTypeAsString( FeatureType type );
    float GetHeight() { return m_height; }

    ///---------------------------------------------------------------------------------
    /// Mutators
    ///---------------------------------------------------------------------------------

    ///---------------------------------------------------------------------------------
    /// Update
    ///---------------------------------------------------------------------------------

    ///---------------------------------------------------------------------------------
    /// Render
    ///---------------------------------------------------------------------------------
    virtual void Render( const bool& debugModeEnabled );

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
    FeatureType m_type;

    bool m_blocksLOS;
    bool m_blocksMovement;
    float m_height;

};

typedef std::vector< Feature* > Features;

///---------------------------------------------------------------------------------
/// Inline function implementations
///---------------------------------------------------------------------------------

#endif