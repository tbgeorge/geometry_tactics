//=================================================================================
// Projectile.hpp
// Author: Tyler George
// Date  : December 6, 2015
//=================================================================================

#pragma once

#ifndef __included_Projectile__
#define __included_Projectile__

///---------------------------------------------------------------------------------
/// Includes
///---------------------------------------------------------------------------------
#include "GameCode/Entities/Entity.hpp"

class Actor;

///---------------------------------------------------------------------------------
/// constants
///---------------------------------------------------------------------------------
const float PROJECTILE_SPEED = 7.0f;
const float PROJECTILE_SPEED_SQUARED = PROJECTILE_SPEED * PROJECTILE_SPEED;
const float PROJECTILE_SPEED_SQUARED_SQUARED = PROJECTILE_SPEED_SQUARED * PROJECTILE_SPEED_SQUARED;
const float GRAVITY = 9.81f;

///---------------------------------------------------------------------------------
/// Structs
///---------------------------------------------------------------------------------
struct FlightPathData
{
    FlightPathData()
        : target(MapPosition(-1, -1)), startingVelocity(Vector3::ZERO){}

    FlightPathData( const MapPosition& pos, const Vector3& vel )
        : target( pos ), startingVelocity( vel ) {}

    MapPosition target;
    PUC_Vertexes arcVerts;
    Vector3 startingVelocity;
};
typedef std::vector<FlightPathData> FlightPaths;
typedef std::map< MapPosition, FlightPathData > FlightPathMap;

///---------------------------------------------------------------------------------
/// Projectile Class
///---------------------------------------------------------------------------------
class Projectile : public Entity
{
public:
    ///---------------------------------------------------------------------------------
    /// Constructors/Destructors
    ///---------------------------------------------------------------------------------
    Projectile( OpenGLRenderer* renderer, Clock* parentClock, const Vector3& velocity );
    ~Projectile();

    ///---------------------------------------------------------------------------------
    /// Accessors/Queries
    ///---------------------------------------------------------------------------------
    bool HasReachedDestination() { return m_hasReachedTarget; }
    static bool CalculateFlightPath( Actor* actor, const MapPosition& target, FlightPathData& out_data );
    static bool CheckFlightPathForObstructions( Map* map, const Vector3& initialVelocity, const MapPosition& startPos, const MapPosition& target, PUC_Vertexes& out_verts );

    ///---------------------------------------------------------------------------------
    /// Mutators
    ///---------------------------------------------------------------------------------
    void SetSourceAndDest( const Vector3& source, const Vector3& destination );

    ///---------------------------------------------------------------------------------
    /// Update
    ///---------------------------------------------------------------------------------
    void Update();

    ///---------------------------------------------------------------------------------
    /// Render
    ///---------------------------------------------------------------------------------
//     virtual void Render( bool debugModeEnabled );

private:
    ///---------------------------------------------------------------------------------
    /// Private Member Variables
    ///---------------------------------------------------------------------------------
    Vector3 m_destination;
    Vector3 m_source;

    bool m_hasReachedTarget;
    Vector3 m_velocity;
};

#endif