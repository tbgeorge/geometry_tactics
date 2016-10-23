//=================================================================================
// Pathfinder.cpp
// Author: Tyler George
// Date  : September 3, 2015
//=================================================================================


////===========================================================================================
///===========================================================================================
// Includes
///===========================================================================================
////===========================================================================================

#include "GameCode/AI/Pathfinder.hpp"
#include "GameCode/Entities/Actor.hpp"
#include "GameCode/Map.hpp"


////===========================================================================================
///===========================================================================================
// Static Variable Initialization
///===========================================================================================
////===========================================================================================

///===========================================================================================
////===========================================================================================
///===========================================================================================
// PathNode Class
///===========================================================================================
////===========================================================================================
///===========================================================================================

////===========================================================================================
///===========================================================================================
// Mutators
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void PathNode::UpdateNode( const float& avoidanceCost, const float& distanceCost, PathNode* parent )
{
    m_avoidanceCost = avoidanceCost;
    m_distanceCost = distanceCost;
    m_localCost = m_avoidanceCost + m_distanceCost;
    m_parentCost = parent->m_fixedCost;
    m_fixedCost = m_localCost + m_parentCost;
    m_totalCost = m_fixedCost + m_heuristicCost;
    m_parent = parent;
}

///===========================================================================================
////===========================================================================================
///===========================================================================================
// Path Class
///===========================================================================================
////===========================================================================================
///===========================================================================================

////===========================================================================================
///===========================================================================================
// Constructors/Destructors
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Path::Path()
    : m_answer( nullptr )
    , m_active( nullptr )
    , m_isFinished( false )
    , m_reachedGoal( false )
    , m_goal( MapPosition( -1, -1 ) )
    , m_map( nullptr )
    , m_actor( nullptr )
    , m_currentNode( nullptr )
    , m_ignoreActors( true )
    , m_numberOfSteps( 0 )
{

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Path::~Path()
{
    for (PathNodeMap::iterator pathNodeIter = m_openList.begin(); pathNodeIter != m_openList.end(); ++pathNodeIter)
    {
        delete pathNodeIter->second;
    }

    for (PathNodeMap::iterator pathNodeIter = m_closedList.begin(); pathNodeIter != m_closedList.end(); ++pathNodeIter)
    {
        delete pathNodeIter->second;
    }
}

////===========================================================================================
///===========================================================================================
// Accessors
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
PathNode* Path::GetNextStep()
{
    PathNode* node = m_answer;

    if (m_currentNode == m_answer)
        return nullptr;

    if (node->m_parent)
    {
        while (node)
        {
            if (node->m_parent && node->m_parent == m_currentNode)
            {
                m_currentNode = node;
                break;
            }
            node = node->m_parent;
        }
    }
    return node;
}

////===========================================================================================
///===========================================================================================
// Mutators
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Path::ProcessOneStep()
{
    if (m_openList.empty())
    {
        m_isFinished = true;
        m_answer = m_active;

        bool finished = false;
        PathNode* node = m_answer;
        while (!finished && node)
        {
            if (node->m_parent)
            {
                node = node->m_parent;
                m_numberOfSteps++;
            }
            else
            {
                finished = true;
                m_currentNode = node;
            }
        }

        return;
    }

    PathNode* lowestTotalCostNode = nullptr;
    for (PathNodeMap::iterator pathNodeIter = m_openList.begin(); pathNodeIter != m_openList.end(); ++pathNodeIter)
    {
        PathNode* node = pathNodeIter->second;

        if (!lowestTotalCostNode || node->m_totalCost < lowestTotalCostNode->m_totalCost)
            lowestTotalCostNode = node;
    }

    m_openList.erase( lowestTotalCostNode->m_position );
    m_closedList[lowestTotalCostNode->m_position] = lowestTotalCostNode;

    m_active = lowestTotalCostNode;

    if (m_active->m_position == m_goal)
    {
        m_isFinished = true;
        m_reachedGoal = true;
        m_answer = m_active;

        // create forward path
        bool finished = false;
        PathNode* node = m_answer;
        while (!finished && node)
        {
            if (node->m_parent)
            {
                node = node->m_parent;
                m_numberOfSteps++;
            }
            else
            {
                finished = true;
                m_currentNode = node;
            }
        }
        return;
    }

    MapPositions adjacentNeighbors = m_map->GetValidNeighbors( m_actor, m_active->m_position, m_goal, m_ignoreActors, m_ignoreMoveRange );

    for (MapPositions::iterator neighborIter = adjacentNeighbors.begin(); neighborIter != adjacentNeighbors.end(); ++neighborIter)
    {
        MapPosition neighborPos = *neighborIter;

        if (m_closedList.find( neighborPos ) != m_closedList.end())
            continue;
        
        PathNodeMap::iterator existingOpenListEntryIter = m_openList.find( neighborPos );

        if (existingOpenListEntryIter == m_openList.end())
        {

            float avoidanceCost = 0.0f;
            float distanceCost = 0.0f;
            m_map->CalculateLocalCost( m_actor, m_active->m_position, neighborPos, avoidanceCost, distanceCost );
            float heuristicCost = (float) Map::CalculateManhattanDistance( neighborPos, m_goal );
            PathNode* newNode = new PathNode( neighborPos, avoidanceCost, distanceCost, heuristicCost, m_active );
            m_openList.insert( std::pair< MapPosition, PathNode* >( neighborPos, newNode ) );
            continue;
        }

        else
        {
            PathNode* existingOpenListEntry = existingOpenListEntryIter->second;

            float avoidanceCost = 0.0f;
            float distanceCost = 0.0f;
            m_map->CalculateLocalCost( m_actor, m_active->m_position, neighborPos, avoidanceCost, distanceCost );
            float fixedCost = avoidanceCost + distanceCost + m_active->m_fixedCost;

            if (fixedCost < existingOpenListEntry->m_fixedCost)
                existingOpenListEntry->UpdateNode( avoidanceCost, distanceCost, m_active );
        }
    }
}

///===========================================================================================
////===========================================================================================
///===========================================================================================
// Pathfinder Class
///===========================================================================================
////===========================================================================================
///===========================================================================================

////===========================================================================================
///===========================================================================================
// Constructors/Destructors
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Pathfinder::Pathfinder()
{

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Pathfinder::~Pathfinder()
{

}

////===========================================================================================
///===========================================================================================
// Accessors/Queries
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Path* Pathfinder::CalculatePath( Map* map, Actor* actor, const MapPosition& start, const MapPosition& goal, const bool& computeFullPath, const bool& ignoreMoveRange, const bool& ignoreActors )
{
    Path* path = new Path();
    path->m_map = map;
    path->m_goal = goal;
    path->m_actor = actor;
    path->m_ignoreActors = ignoreActors;
    path->m_ignoreMoveRange = ignoreMoveRange;

    float startingPosAvoidanceCost = 0;
    float startingPosDistanceCost = 0;
    map->CalculateLocalCost( actor, start, start, startingPosAvoidanceCost, startingPosDistanceCost );
    float startingPosHeuristicCost = (float) Map::CalculateManhattanDistance( start, goal );

    PathNode* startingNode = new PathNode( start, startingPosAvoidanceCost, startingPosDistanceCost, startingPosHeuristicCost, nullptr );
    path->m_openList.insert( std::pair< MapPosition, PathNode* >( start, startingNode ) );

    if (computeFullPath)
    {
        while (!path->m_isFinished)
            path->ProcessOneStep();
    }
    
    return path;
}

////===========================================================================================
///===========================================================================================
// Mutators
///===========================================================================================
////===========================================================================================


