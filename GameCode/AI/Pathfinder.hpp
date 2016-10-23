//=================================================================================
// Pathfinder.hpp
// Author: Tyler George
// Date  : September 3, 2015
//=================================================================================

#pragma once

#ifndef __included_Pathfinder__
#define __included_Pathfinder__

///---------------------------------------------------------------------------------
/// Includes
///---------------------------------------------------------------------------------
#include <map>

#include "GameCode/GameCommon.hpp"
class Map;
class Actor;

///---------------------------------------------------------------------------------
/// Typedefs
///---------------------------------------------------------------------------------


///---------------------------------------------------------------------------------
/// PathNode Struct
///---------------------------------------------------------------------------------
struct PathNode
{
	MapPosition m_position;
	float m_avoidanceCost;
	float m_distanceCost;
	float m_localCost; // localCost = avoidanceCost + distanceCost
	float m_parentCost; // parentCost = parent->fixedCost
	float m_fixedCost; // fixedCost = localCost + parentCost
	float m_heuristicCost; 
	float m_totalCost; // totalCost = fixedCost + hueristicCost
	PathNode* m_parent;

	///---------------------------------------------------------------------------------
	/// Constructors/Destructors
	///---------------------------------------------------------------------------------
	PathNode( const MapPosition& position, const float& avoidanceCost, const float& distanceCost, const float& hueristicCost, PathNode* parent )
		: m_position( position )
		, m_avoidanceCost( avoidanceCost )
		, m_distanceCost( distanceCost )
		, m_localCost( avoidanceCost + distanceCost )
		, m_parentCost( parent ? parent->m_fixedCost : 0.0f )
		, m_fixedCost( m_localCost + m_parentCost )
		, m_heuristicCost( hueristicCost )
		, m_totalCost( m_fixedCost + m_heuristicCost )
		, m_parent( parent ) {}

	PathNode() {}

	///---------------------------------------------------------------------------------
	/// Mutators
	///---------------------------------------------------------------------------------
	void UpdateNode( const float& avoidanceCost, const float& distanceCost, PathNode* parent );
};

///---------------------------------------------------------------------------------
/// Path Class
///---------------------------------------------------------------------------------
class Path
{
public:
	///---------------------------------------------------------------------------------
	/// Constructors/Destructors
	///---------------------------------------------------------------------------------
	Path();
	~Path();

	///---------------------------------------------------------------------------------
	/// Accessors
	///---------------------------------------------------------------------------------
	PathNode* GetNextStep();
    int GetNumberOfSteps() const { return m_numberOfSteps; }

	///---------------------------------------------------------------------------------
	/// Mutators
	///---------------------------------------------------------------------------------
	void ProcessOneStep();

	///---------------------------------------------------------------------------------
	/// Public Member Variables
	///---------------------------------------------------------------------------------
	std::map< MapPosition, PathNode* > m_openList;
	std::map< MapPosition, PathNode* > m_closedList;
	PathNode* m_answer;
	PathNode* m_active;
	bool m_isFinished;
    bool m_reachedGoal;
	bool m_ignoreActors;
    bool m_ignoreMoveRange;
	MapPosition m_goal;
	Map* m_map;
	Actor* m_actor;
    PathNode* m_currentNode;
    int m_numberOfSteps;

};
typedef std::map< MapPosition, PathNode* > PathNodeMap;

////===========================================================================================
///===========================================================================================
// Pathfinder Class
///===========================================================================================
////===========================================================================================
class Pathfinder
{
public:
	///---------------------------------------------------------------------------------
	/// Constructors/Destructors
	///---------------------------------------------------------------------------------
	Pathfinder();
	~Pathfinder();

	///---------------------------------------------------------------------------------
	/// Accessors/Queries
	///---------------------------------------------------------------------------------
	static Path* CalculatePath( Map* map, Actor* actor, const MapPosition& start, const MapPosition& goal, const bool& computeFullPath, const bool& ignoreMoveRange, const bool& ignoreActors );


private:
	///---------------------------------------------------------------------------------
	/// Private Member Variables
	///---------------------------------------------------------------------------------
};

///---------------------------------------------------------------------------------
/// Inline function implementations
///---------------------------------------------------------------------------------

#endif