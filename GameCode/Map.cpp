//=================================================================================
// Map.cpp
// Author: Tyler George
// Date  : October 28, 2015
//=================================================================================


////===========================================================================================
///===========================================================================================
// Includes
///===========================================================================================
////===========================================================================================

#include "GameCode/Map.hpp"
#include "Engine/Math/Noise.hpp"
#include "Engine/Utilities/XMLParser.h"
#include "Engine/Utilities/Utilities.hpp"

#include "GameCode/Entities/Actor.hpp"
#include "AI/Pathfinder.hpp"
#include "Engine/Utilities/DeveloperConsole.hpp"
#include "FeatureFactory.hpp"

////===========================================================================================
///===========================================================================================
// Static Variable Initialization
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
Map::Map( IntVector2 mapSizeInCells )
    : m_currentCameraLoc( 0 )
    , m_mesh( nullptr )
    , m_material( nullptr )
    , m_meshRenderer( nullptr )
{
    InitializeEmptyMap( mapSizeInCells );

    m_mapSizeCells = mapSizeInCells;
    m_mapSize = Vector2( m_mapSizeCells.x * CELL_SIZE, m_mapSizeCells.y * CELL_SIZE );

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Map::Map( const std::string& filePath )
    : m_currentCameraLoc( 0 )
    , m_mesh( nullptr )
    , m_material( nullptr )
    , m_meshRenderer( nullptr )
{
    XMLNode mapDataNode = XMLNode::parseFile( filePath.c_str(), "MapData" );

    XMLNode heightDataNode = mapDataNode.getChildNode( "HeightData" );

    std::string mapText( heightDataNode.getText() );

    Strings mapRows;
    Tokenize( mapText, mapRows, "\r\n" );


    int x = 0;
    int y = 0;

    int maxX = 0;
    float maxHeight = 0;

    for (Strings::reverse_iterator mapRowIter = mapRows.rbegin(); mapRowIter != mapRows.rend(); ++mapRowIter)
    {
        std::string mapRow = *mapRowIter;

        Strings rowHeights;
        Tokenize( mapRow, rowHeights, " " );


        for (Strings::const_iterator cellHeightIter = rowHeights.begin(); cellHeightIter != rowHeights.end(); ++cellHeightIter)
        {
            std::string cellHeightStr = *cellHeightIter;
            float height = stof( cellHeightStr );
            if (height > maxHeight)
                maxHeight = height;

            MapPosition pos( x, y );
            m_cells.insert( std::pair< MapPosition, Cell >( pos, Cell( pos, height ) ) );
            x++;

            if (x > maxX)
                maxX = x;

        }
        
        x = 0;
        y++;

    }

    XMLNode featuresRoot = mapDataNode.getChildNode( "Features" );

    if (!featuresRoot.isEmpty())
    {
        int featureCounter = 0;
        bool hasAnotherFeature = false;

        do
        {
            XMLNode featureNode = featuresRoot.getChildNode( "Feature", featureCounter++ );
            if (!featureNode.isEmpty())
            {
                hasAnotherFeature = true;
                std::string name = GetStringProperty( featureNode, "name", "unknown Feature", false );
                if (name == "unknown Feature")
                {
                    // Soft fail
                    DeveloperConsole::WriteLine( "Attempted to load Feature " + std::to_string( featureCounter ) + ". XML doesn't contain \"name\".", WARNING_TEXT_COLOR );
                    continue;
                }

                MapPosition pos = GetIntVector2Property( featureNode, "mapPos", MapPosition( -1, -1 ) );
                if (pos == MapPosition( -1, -1 ))
                {
                    // Soft fail
                    DeveloperConsole::WriteLine( "Attempted to load Feature " + std::to_string( featureCounter ) + ". XML doesn't contain \"mapPos\".", WARNING_TEXT_COLOR );
                    continue;
                }
                FeatureFactory* featureFactory = FeatureFactory::FindFactoryByName( name );
                SetFeatureAtMapPosition( featureFactory->SpawnFeature( XMLNode::emptyNode() ), pos );

            }
            else
                hasAnotherFeature = false;

        } while (hasAnotherFeature);

        featuresRoot.deleteNodeContent();
    }

    mapDataNode.deleteNodeContent();


    m_mapSizeCells = IntVector2( maxX, y );
    m_mapSize = Vector2( m_mapSizeCells.x * CELL_SIZE, m_mapSizeCells.y * CELL_SIZE );

    m_cameraLocs.push_back( CameraLocationData( Vector3( -15.0f, maxHeight + 10.0f, -15.0f ), EulerAngles( 45.0f, 20.0f, 0.0f ) ) );
    m_cameraLocs.push_back( CameraLocationData( Vector3( -15.0f, maxHeight + 10.0f, y + 15.0f ), EulerAngles( 135.0f, 20.0f, 0.0f ) ) );
    m_cameraLocs.push_back( CameraLocationData( Vector3( maxX + 15.0f, maxHeight + 10.0f, y + 15.0f ), EulerAngles( -135.0f, 20.0f, 0.0f ) ) );
    m_cameraLocs.push_back( CameraLocationData( Vector3( maxX + 15.0f, maxHeight + 10.0f, -15.0f ), EulerAngles( -45.0f, 20.0f, 0.0f ) ) );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Map::~Map()
{
    delete m_meshRenderer;
    delete m_material;
    delete m_mesh;
}

////===========================================================================================
///===========================================================================================
// Initialization
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Map::Startup( OpenGLRenderer* renderer )
{
    m_mesh = new PuttyMesh( renderer );

    RenderState rs( true, true, false, false );
    m_material = new Material( renderer, OpenGLRenderer::CreateOrGetShader( "Data/Shaders/textured") , rs );
    m_material->SetTextureUniform( "gTexture", Texture::CreateOrGetTexture( "Data/Images/ground.png" ) );

    m_meshRenderer = new MeshRenderer( renderer );

    m_meshRenderer->SetMaterial( m_material, false );


    PUC_Vertexes vertexes;
    std::vector<unsigned int> indexes;

    for (CellMap::iterator cellIter = m_cells.begin(); cellIter != m_cells.end(); ++cellIter)
    {
        Cell& currentCell = cellIter->second;
        currentCell.CalculateVertexes( vertexes, indexes, vertexes.size() );
    }

    m_mesh->SetVertexData( vertexes.data(), DrawInstructions( GL_TRIANGLES, vertexes.size(), indexes.size(), true ), Vertex3D_PUC::GetVertexInfo() );
    m_mesh->SetIndexData( indexes.data(), indexes.size() );

    m_meshRenderer->SetMesh( m_mesh );
}

////===========================================================================================
///===========================================================================================
// Accessors/Queries
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
CameraLocationData Map::GetCurrentCameraLoc()
{
    return m_cameraLocs[m_currentCameraLoc];
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
CameraLocationData Map::GetNextCameraLoc()
{
    int nextCameraLoc = m_currentCameraLoc + 1;

    if (nextCameraLoc == m_cameraLocs.size())
        nextCameraLoc = 0;

    m_currentCameraLoc = nextCameraLoc;

    return m_cameraLocs[m_currentCameraLoc];
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
CameraLocationData Map::GetPreviousCameraLoc()
{
    int prevCameraLoc = m_currentCameraLoc - 1;

    if (prevCameraLoc == -1 )
        prevCameraLoc = m_cameraLocs.size() - 1;

    m_currentCameraLoc = prevCameraLoc;

    return m_cameraLocs[m_currentCameraLoc];
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Cell* Map::GetCellAtMapPos( const MapPosition& mapPos )
{
    Cell* cell = nullptr;
    CellMap::iterator cellIter = m_cells.find( mapPos );
    if (cellIter != m_cells.end())
        cell = &m_cells[mapPos];
    return cell;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Cell* Map::GetCellAtWorldPosition( const Vector3& worldPos )
{
    if (worldPos.x < 0.0f || worldPos.x > m_mapSize.x || worldPos.z < 0.0f || worldPos.z > m_mapSize.y || worldPos.y < 0.0f)
        return nullptr;

    int x = floorf( worldPos.x / CELL_SIZE );
    int y = floorf( worldPos.z / CELL_SIZE );

    Cell* cell = GetCellAtMapPos( MapPosition( x, y ) );
    if (cell->GetHeight() > worldPos.y)
        return cell;

    return nullptr;

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
MapPosition Map::GetRandomOpenPosition()
{
    for (;;)
    {
        int randomX = GetRandomIntLessThan( m_mapSize.x );
        int randomY = GetRandomIntLessThan( m_mapSize.y );

        MapPosition pos( randomX, randomY );

        Cell* randomCell = GetCellAtMapPos( pos );

        // if no actor an no movement blocking feature
        if (!randomCell->GetActor() && ( !randomCell->GetFeature() || ( randomCell->GetFeature() && !randomCell->GetFeature()->BlocksMovement() ) ) )
            return pos;
    }
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
CellPtrs Map::GetNeighbors( const MapPosition& pos )
{
    CellPtrs neighbors;

    for (int x = -1; x <= 1; x++ )
    {
        for (int y = -1; y <= 1; y++ )
        {
            if (x != 0 && y != 0)
                continue;

            if (x == 0 && y == 0)
                continue;

            MapPosition mapPos( pos.x + x, pos.y + y );
            
            neighbors.push_back( GetCellAtMapPos( mapPos ) );
        }
    }

    return neighbors;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
MapPositions Map::GetValidNeighbors( Actor* actor, const MapPosition& pos, const MapPosition& goalPos, bool ignoreActors, bool ignoreMoveRange )
{
    MapPositions neighbors;

    float currentHeight = GetCellAtMapPos( pos )->GetHeight();
    float actorJumpRange = actor->GetJumpRange();

    MapPosition actorPos = actor->GetMapPosition();
    int actorMoveRange = actor->GetMoveRange();

    for (int x = -1; x <= 1; x++)
    {
        for (int y = -1; y <= 1; y++)
        {
            if (x != 0 && y != 0)
                continue;

            MapPosition mapPos( pos.x + x, pos.y + y );

            Cell* cell = GetCellAtMapPos( mapPos );
            if (cell)
            {
                float neighborHeight = cell->GetHeight();
                Feature* cellFeature = cell->GetFeature();
                if (cellFeature && cellFeature->BlocksMovement())
                    continue;

                if ( abs( neighborHeight - currentHeight ) <= actorJumpRange)
                {
                    if (ignoreMoveRange)
                    {
                        if (ignoreActors)
                            neighbors.push_back( mapPos );
                        else
                        {
                            if ( (cell->GetActor() && !cell->GetActor()->IsDead()) && cell->GetMapPosition() != goalPos)
                                continue;
                            neighbors.push_back( mapPos );
                        }
                    }
                    else if ( CalculateManhattanDistance( mapPos, actorPos ) <= actorMoveRange)
                    {
                        if (ignoreActors)
                            neighbors.push_back( mapPos );
                        else
                        {
                            if ((cell->GetActor() && !cell->GetActor()->IsDead()) && cell->GetMapPosition() != goalPos)
                                continue;
                            neighbors.push_back( mapPos );
                        }
                    }

                }
            }
        }
    }

    return neighbors;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Actors Map::GetAllActors()
{
    Actors allActors;

    for (CellMap::iterator cellIter = m_cells.begin(); cellIter != m_cells.end(); ++cellIter)
    {
        Cell cell = cellIter->second;

        Actor* cellActor = cell.GetActor();
        if (cellActor)
            allActors.push_back( cellActor );
    }

    return allActors;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Map::CalculateLocalCost( Actor* actor, const MapPosition& startPosition, const MapPosition& endPosition, float& out_avoidanceCost, float& out_distanceCost )
{
    out_avoidanceCost = 1.0f;

    out_distanceCost = CalcDistance( startPosition, endPosition );

    Cell* start = GetCellAtMapPos( startPosition );
    Cell* end = GetCellAtMapPos( endPosition );
    out_distanceCost += abs( end->GetHeight() - start->GetHeight() );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
int Map::CalculateDistToNearestActorOfFaction( const MapPosition& pos, Faction* faction )
{
    Actors actors = GetAllActors();

    int nearestDist = -1;

    for (Actors::iterator actorIter = actors.begin(); actorIter != actors.end(); ++actorIter)
    {
        Actor* actor = *actorIter;

        if (actor->GetFaction() == *faction)
        {
            int dist = CalculateManhattanDistance( pos, actor->GetMapPosition() );

            if (dist < nearestDist || nearestDist == -1)
                nearestDist = dist;
        }
    }

    return nearestDist;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
int Map::CalculateManhattanDistance( const MapPosition& start, const MapPosition& end )
{
    return abs( end.x - start.x ) + abs( end.y - start.y );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
bool Map::IsAdjacent( const MapPosition& a, const MapPosition& b )
{
    int xDiff = abs( a.x - b.x );
    int yDiff = abs( a.y - b.y );

    if (xDiff <= 1)
    {
        if (yDiff <= 1)
            return true;
        return false;
    }

    else if (yDiff <= 1)
    {
        if (xDiff <= 1)
            return true;
        return false;
    }

    else
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
void Map::InitializeEmptyMap( IntVector2 mapSizeInCells )
{
    int seed = TripleRandomInt();

    float maxHeight = 0.0f;
    for (int x = 0; x < mapSizeInCells.x; ++x)
    {
        for (int y = 0; y < mapSizeInCells.y; ++y)
        {
            MapPosition pos( x, y );
            float height = 8.0f + ComputePerlinNoiseValueAtPosition2D( Vector2( (float)x, (float)y ), 40.0f, 3, 10.0f, 0.8f, seed );

            if (height > maxHeight)
                maxHeight = height;

            m_cells.insert( std::pair< MapPosition, Cell >( pos, Cell( pos, height ) ) );
        }
    }
    // -15.0f, maxHeight + 10.0f, -15.0f )
    //  45.0f, 20.0f, 0.0f
    m_cameraLocs.push_back( CameraLocationData( Vector3( -15.0f, maxHeight + 10.0f, -15.0f ), EulerAngles( 45.0f, 20.0f, 0.0f ) ) );
    m_cameraLocs.push_back( CameraLocationData( Vector3( -15.0f, maxHeight + 10.0f, mapSizeInCells.y + 15.0f ), EulerAngles( 135.0f, 20.0f, 0.0f ) ) );
    m_cameraLocs.push_back( CameraLocationData( Vector3( mapSizeInCells.x + 15.0f, maxHeight + 10.0f, mapSizeInCells.y + 15.0f ), EulerAngles( 225.0f, 20.0f, 0.0f ) ) );
    m_cameraLocs.push_back( CameraLocationData( Vector3( mapSizeInCells.x + 15.0f, maxHeight + 10.0f, -15.0f ), EulerAngles( 315.0f, 20.0f, 0.0f ) ) );

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Map::SetActorAtMapPosition( Actor* actor, MapPosition mapPos )
{
    Cell* prevCell = GetCellAtMapPos( actor->GetMapPosition() );
    if ( prevCell )
        GetCellAtMapPos( actor->GetMapPosition() )->SetActor( nullptr );

    actor->SetMapPosition( mapPos );
    GetCellAtMapPos( mapPos )->SetActor( actor );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Map::SetFeatureAtMapPosition( Feature* feature, MapPosition mapPos )
{
    feature->SetMapPosition( mapPos );
    feature->SetMap( this );

    Cell* cell = GetCellAtMapPos( mapPos );
    if (cell)
    {
        Vector3 renderPos = Vector3( (float)mapPos.x, cell->GetHeight(), (float)mapPos.y );
        feature->SetRenderPosition( renderPos );

        cell->SetFeature( feature );
    }
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Map::RemoveActor( Actor* actor )
{
    Cell* actorCell = GetCellAtMapPos( actor->GetMapPosition() );
   
    if (actorCell)
        actorCell->SetActor( nullptr );
}

////===========================================================================================
///===========================================================================================
// Update
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Map::ProcessInput( InputSystem* inputSystem, double deltaSeconds )
{

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Map::Update( double deltaSeconds, bool debugModeEnabled )
{
    for (CellMap::iterator cellIter = m_cells.begin(); cellIter != m_cells.end(); ++cellIter)
    {
        Cell* cell = &cellIter->second;
        cell->Update( deltaSeconds, debugModeEnabled );
    }

//     for each (std::pair< MapPosition, Cell > cellIter in m_cells)
//     {
//         cellIter.second.Update( deltaSeconds, debugModeEnabled );
//     }
}


////===========================================================================================
///===========================================================================================
// Render
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Map::Render( OpenGLRenderer* renderer, bool debugModeEnabled )
{
    if (!renderer)
        return;

    m_meshRenderer->Render( Matrix4f::CreateIdentity(), renderer->GetViewMatrix(), renderer->GetPerspectiveMatrix() );


    for (CellMap::iterator cellIter = m_cells.begin(); cellIter != m_cells.end(); ++cellIter)
    {
        Cell& currentCell = cellIter->second;
        currentCell.Render( renderer, debugModeEnabled );
    }


}

////===========================================================================================
///===========================================================================================
// Private Functions
///===========================================================================================
////===========================================================================================

