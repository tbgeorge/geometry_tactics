//=================================================================================
// Map.hpp
// Author: Tyler George
// Date  : October 28, 2015
//=================================================================================

#pragma once

#ifndef __included_Map__
#define __included_Map__

///---------------------------------------------------------------------------------
/// Includes
///---------------------------------------------------------------------------------
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/OpenGLRenderer.hpp"
#include "Engine/Renderer/PuttyMesh.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/MeshRenderer.hpp"
#include "GameCode/GameCommon.hpp"
#include "GameCode/Cell.hpp"

enum Faction;

///---------------------------------------------------------------------------------
/// Structs
///---------------------------------------------------------------------------------
struct CameraLocationData
{
    CameraLocationData(){}

    CameraLocationData( const Vector3& pos, const EulerAngles& orientation )
        : cameraPosition( pos ), cameraOrientation( orientation ) {}

    const bool operator==(const CameraLocationData& equalTo)
    {
        if (cameraPosition == equalTo.cameraPosition && cameraOrientation == equalTo.cameraOrientation)
            return true;
        return false;
    }

    const bool operator!=(const CameraLocationData& equalTo)
    {
        if (cameraPosition == equalTo.cameraPosition && cameraOrientation == equalTo.cameraOrientation)
            return false;
        return true;
    }

    Vector3 cameraPosition;
    EulerAngles cameraOrientation;
};

struct PossibleMove
{
    PossibleMove( Cell* cell )
        : m_cell( cell ), m_hasBeenChecked( false ) {}

    Cell* m_cell;
    bool m_hasBeenChecked;
};

///---------------------------------------------------------------------------------
/// Typedefs
///---------------------------------------------------------------------------------
typedef std::vector< CameraLocationData > CameraLocations;
typedef std::vector< PossibleMove > PossibleMoves;

////===========================================================================================
///===========================================================================================
// Map Class
///===========================================================================================
////===========================================================================================
class Map
{
public:
	///---------------------------------------------------------------------------------
	/// Constructors/Destructors
	///---------------------------------------------------------------------------------
    Map( IntVector2 mapSizeInCells );
    Map( const std::string& filePath );
    ~Map();

	///---------------------------------------------------------------------------------
	/// Initialization
	///---------------------------------------------------------------------------------
    void Startup( OpenGLRenderer* renderer );

	///---------------------------------------------------------------------------------
	/// Accessors/Queries
	///---------------------------------------------------------------------------------
    IntVector2 GetMapSize() { return m_mapSizeCells; }
    CameraLocationData GetCurrentCameraLoc();
    CameraLocationData GetNextCameraLoc();
    CameraLocationData GetPreviousCameraLoc();

    Cell* GetCellAtMapPos( const MapPosition& mapPos );
    Cell* GetCellAtWorldPosition( const Vector3& worldPos );
    MapPosition GetRandomOpenPosition();

    CellPtrs GetNeighbors( const MapPosition& pos );
    MapPositions GetValidNeighbors( Actor* actor, const MapPosition& pos, const MapPosition& goalPos, bool ignoreActors, bool ignoreMoveRange );
    Actors GetAllActors();

    void CalculateLocalCost( Actor* actor, const MapPosition& startPosition, const MapPosition& endPosition, float& out_avoidanceCost, float& out_distanceCost );
    int CalculateDistToNearestActorOfFaction( const MapPosition& pos, Faction* faction );

    static int CalculateManhattanDistance( const MapPosition& start, const MapPosition& end );
    static bool IsAdjacent( const MapPosition& a, const MapPosition& b );
    

	///---------------------------------------------------------------------------------
	/// Mutators
	///---------------------------------------------------------------------------------
    void InitializeEmptyMap( IntVector2 mapSizeInCells );
    void SetActorAtMapPosition( Actor* actor, MapPosition mapPos );
    void SetFeatureAtMapPosition( Feature* feature, MapPosition mapPos );
    void RemoveActor( Actor* actor );

	///---------------------------------------------------------------------------------
	/// Update
	///---------------------------------------------------------------------------------
    void ProcessInput( InputSystem* inputSystem, double deltaSeconds );
    void Update( double deltaSeconds, bool debugModeEnabled );

	///---------------------------------------------------------------------------------
	/// Render
	///---------------------------------------------------------------------------------
    void Render( OpenGLRenderer* renderer, bool debugModeEnabled );

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
    OpenGLRenderer* m_renderer;

    IntVector2 m_mapSizeCells;
    Vector2 m_mapSize;

    CellMap m_cells;

    CameraLocations m_cameraLocs;
    int m_currentCameraLoc;


    PuttyMesh* m_mesh;
    Material* m_material;
    MeshRenderer* m_meshRenderer;
};

///---------------------------------------------------------------------------------
/// Inline function implementations
///---------------------------------------------------------------------------------

#endif