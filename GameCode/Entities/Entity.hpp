//=================================================================================
// Entity.hpp
// Author: Tyler George
// Date  : November 2, 2015
//=================================================================================

#pragma once

#ifndef __included_Entity__
#define __included_Entity__

///---------------------------------------------------------------------------------
/// Includes
///---------------------------------------------------------------------------------
#include "GameCode/GameCommon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/OpenGLRenderer.hpp"
#include "Engine/Renderer/PuttyMesh.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/MeshRenderer.hpp"
#include "Engine/Utilities/XMLHelper.hpp"
#include "Engine/Utilities/Time.hpp"

class Map;


///---------------------------------------------------------------------------------
/// Typedefs
///---------------------------------------------------------------------------------


////===========================================================================================
///===========================================================================================
// Entity Class
///===========================================================================================
////===========================================================================================
class Entity
{
public:
	///---------------------------------------------------------------------------------
	/// Constructors/Destructors
	///---------------------------------------------------------------------------------
    Entity( OpenGLRenderer* renderer, Clock* parentClock );
    Entity( OpenGLRenderer* renderer, const XMLNode& entityNode, Clock* parentClock );
    Entity( const Entity& copy, OpenGLRenderer* renderer, const XMLNode& entityNode, Clock* parentClock );
    ~Entity();

	///---------------------------------------------------------------------------------
	/// Initialization
	///---------------------------------------------------------------------------------
    void Startup();

	///---------------------------------------------------------------------------------
	/// Accessors/Queries
	///---------------------------------------------------------------------------------
    Map* GetMap() const { return m_owningMap; }
    MapPosition GetMapPosition() const { return m_mapPos; }

	///---------------------------------------------------------------------------------
	/// Mutators
	///---------------------------------------------------------------------------------
    void SetMap( Map* map ) { m_owningMap = map; }
    void SetMapPosition( const MapPosition& mapPos ); 
    void SetRenderPosition( const Vector3& renderPos ) { m_renderPosition = renderPos; }
    void LoadMesh( const XMLNode& meshNode, const Rgba& defaultColor = Rgba::BLACK );
    void ChangeMesh( const PUC_Vertexes& verts, const std::vector<unsigned int> indexes, const Rgba& newColor );
    void FinalizeMesh();

	///---------------------------------------------------------------------------------
	/// Update
	///---------------------------------------------------------------------------------
    void ProcessInput( InputSystem* inputSystem, double deltaSeconds );
    void Update( double deltaSeconds, bool debugModeEnabled );

	///---------------------------------------------------------------------------------
	/// Render
	///---------------------------------------------------------------------------------
    virtual void Render( bool debugModeEnabled );

	///---------------------------------------------------------------------------------
	/// Public Member Variables
	///---------------------------------------------------------------------------------

	///---------------------------------------------------------------------------------
	/// Static Variables
	///---------------------------------------------------------------------------------


protected:
	///---------------------------------------------------------------------------------
	/// Private Functions
	///---------------------------------------------------------------------------------

	///---------------------------------------------------------------------------------
	/// Private Member Variables
	///---------------------------------------------------------------------------------
    int m_entityID;
    Clock* m_clock;
    std::string m_entityName;
    unsigned int m_entityNameID;
    Map* m_owningMap;
    MapPosition m_mapPos;
    Vector3 m_renderPosition;

    PuttyMesh* m_mesh;
    Material* m_material;
    MeshRenderer* m_meshRenderer;

    PUC_Vertexes m_verts;
    std::vector< unsigned int > m_indicies;

    OpenGLRenderer* m_renderer;

    ///---------------------------------------------------------------------------------
    /// Static private member variables
    ///---------------------------------------------------------------------------------
    static int s_entityID;
};

///---------------------------------------------------------------------------------
/// Inline function implementations
///---------------------------------------------------------------------------------

#endif