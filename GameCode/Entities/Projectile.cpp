//=================================================================================
// Projectile.cpp
// Author: Tyler George
// Date  : December 6, 2015
//=================================================================================

////===========================================================================================
///===========================================================================================
// Includes
///===========================================================================================
////===========================================================================================

#include "GameCode/Entities/Projectile.hpp"
#include "GameCode/Entities/Actor.hpp"

////===========================================================================================
///===========================================================================================
// Projectile Class 
///===========================================================================================
////===========================================================================================

////===========================================================================================
///===========================================================================================
// Constructors/Destructors
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Projectile::Projectile( OpenGLRenderer* renderer, Clock* parentClock, const Vector3& velocity )
    : Entity( renderer, parentClock )
    , m_hasReachedTarget( false )
    , m_velocity( velocity )
{

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Projectile::~Projectile()
{

}


////===========================================================================================
///===========================================================================================
// Accessors/Queries
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
/// returns true if calculation was successful
///---------------------------------------------------------------------------------
bool Projectile::CalculateFlightPath( Actor* actor, const MapPosition& target, FlightPathData& out_data )
{
    Map* map = actor->GetMap();
    Cell* targetCell = map->GetCellAtMapPos( target );
    MapPosition startPos = actor->GetMapPosition();
    float startHeight = map->GetCellAtMapPos( startPos )->GetHeight();

    // if target is valid
    if (targetCell)
    {
        // if the target is closer than the actor's minimum range
        if (map->CalculateManhattanDistance( startPos, targetCell->GetMapPosition() ) <= 2 /* TODO: Put a min range on actor */)
            return false;

        // calculate height difference
        float targetHeight = targetCell->GetHeight();
        float heightDifference = 1.0f * (targetHeight - startHeight);

        // calculate lateral distance
        float distSquared2D = CalcDistanceSquared( targetCell->GetMapPosition(), startPos );
        float gravityAdjustedDistance = distSquared2D * GRAVITY;
        float heightAdjustment = 2.0f * heightDifference * PROJECTILE_SPEED_SQUARED;

        // if this is positive, an angle exists that could potentially reach the target
        float value = PROJECTILE_SPEED_SQUARED_SQUARED - (GRAVITY * (gravityAdjustedDistance + heightAdjustment));

        if (value > 0.0f)
        {

            float valueSqrt = sqrt( value );
            float tanArcAngleRadiansPos = (PROJECTILE_SPEED_SQUARED + valueSqrt) / (GRAVITY * sqrt( distSquared2D ));
            float tanArcAngleRadiansNeg = (PROJECTILE_SPEED_SQUARED - valueSqrt) / (GRAVITY * sqrt( distSquared2D ));

            float radiansPos = atan( tanArcAngleRadiansPos );
            float radiansNeg = atan( tanArcAngleRadiansNeg );

            //             float degreesPos = ConvertRadiansToDegrees( radiansPos );
            //             float degreesNeg = ConvertRadiansToDegrees( radiansNeg );

            // determine the yaw at which to fire
            Vector2 dir = (Vector2( targetCell->GetMapPosition().x, targetCell->GetMapPosition().y ) - Vector2( startPos.x, startPos.y )).Normalized();
            Vector2 initial( 0.0f, 1.0f );
            float cosTheta = DotProduct( dir, initial );
            float yawRadians = acos( cosTheta );
            if (dir.x < 0.0f)
                yawRadians *= -1.0f;

            //             float yawDegrees = ConvertRadiansToDegrees( yawRadians );


            // calculate the initial velocity for the high angle
            float pitchRadians = -radiansPos;
            float cosPitch = cos( pitchRadians );
            Vector3 initialVelDir = Vector3( cosPitch * sin( yawRadians ), -sin( pitchRadians ), cosPitch * cos( yawRadians ) ).Normalized();
            Vector3 start = Vector3( startPos.x, startHeight, startPos.y );
            Vector3 end = start + initialVelDir;
            Vector3 velocity = PROJECTILE_SPEED * initialVelDir;

            // if high angle is obstructed
            if (Projectile::CheckFlightPathForObstructions( map, velocity, startPos, target, out_data.arcVerts ))
            {
                pitchRadians = -radiansPos;
                cosPitch = cos( pitchRadians );
                initialVelDir = Vector3( cosPitch * sin( yawRadians ), -sin( pitchRadians ), cosPitch * cos( yawRadians ) ).Normalized();
                end = start + initialVelDir;
                velocity = PROJECTILE_SPEED * initialVelDir;

                // if low angle is obstructed
                if (Projectile::CheckFlightPathForObstructions( map, velocity, startPos, target, out_data.arcVerts ))
                    return false;
                else
                {
                    out_data.startingVelocity = velocity;
                    out_data.target = target;
                    return true;
                }
            }
            else
            {
                out_data.startingVelocity = velocity;
                out_data.target = target;
                return true;
            }
        }
        else
            return false;
    }
    else
        return false;
}

///---------------------------------------------------------------------------------
/// // returns true if obstructed
///---------------------------------------------------------------------------------
bool Projectile::CheckFlightPathForObstructions( Map* map, const Vector3& initialVelocity, const MapPosition& startPos, const MapPosition& target, PUC_Vertexes& out_verts )
{

    out_verts.clear();

    float startHeight = map->GetCellAtMapPos( startPos )->GetHeight();

    // initial position and velocity
    Vector3 pos = Vector3( startPos.x, startHeight, startPos.y );
    Vector3 velocity = initialVelocity;

    float delta = 0.001f;
    for (float t = 0.0f; t > -1.0f; t += delta)
    {
        out_verts.push_back( Vertex3D_PUC( pos + Vector3( 0.5f, 0.0f, 0.5f ), Vector2::ZERO, Rgba::BLACK + (Rgba::GREEN * (t / 2.0f)) ) );

        // adjust pos and velocity
        pos += velocity * delta;
        velocity += Vector3( 0.0f, -GRAVITY, 0.0f ) * delta;

        // check if current pos is obstructed
        Cell* cellAtPos = map->GetCellAtWorldPosition( pos + Vector3( 0.5f, 0.0f, 0.5f ) );
        Cell* cellAtMapPos = map->GetCellAtMapPos( MapPosition( (int)floorf( pos.x + 0.5f ), (int)floorf( pos.z + 0.5f ) ) );

        if (cellAtPos || (cellAtMapPos && cellAtMapPos->GetFeature() && cellAtMapPos->GetFeature()->BlocksLOS() && cellAtMapPos->GetRealHeight() > pos.y))
        {
            out_verts.clear();
            return true;
        }

        // if the projectile has reached it's mapPosition
        if (AreVectorsEqual( Vector2( pos.x, pos.z ), Vector2( target.x, target.y ), 0.2f ) || pos.y <= 0.0f )
        {
            if (out_verts.size() % 2 != 0)
                out_verts.push_back( Vertex3D_PUC( pos + Vector3( 0.5f, 0.0f, 0.5f ), Vector2::ZERO, Rgba::BLACK + (Rgba::GREEN * (t / 2.0f)) ) );

            return false;
        }
    }

    return false;
}


////===========================================================================================
///===========================================================================================
// Mutators
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Projectile::SetSourceAndDest( const Vector3& source, const Vector3& destination )
{
    m_source = source;
    m_destination = destination;

    m_renderPosition = m_source;
}


////===========================================================================================
///===========================================================================================
// Update
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Projectile::Update()
{
    
    m_renderPosition += (m_velocity * (2.0f * m_clock->GetLastDeltaSeconds()));
    m_velocity += Vector3( 0.0f, -GRAVITY, 0.0f ) * ( 2.0f * m_clock->GetLastDeltaSeconds() );

    if (AreVectorsEqual( m_renderPosition, m_destination, 0.3f ) || m_renderPosition.y <= 0.0f )
        m_hasReachedTarget = true;

}


////===========================================================================================
///===========================================================================================
// Render
///===========================================================================================
////==================w=========================================================================