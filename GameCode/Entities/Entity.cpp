//=================================================================================
// Entity.cpp
// Author: Tyler George
// Date  : November 2, 2015
//=================================================================================


////===========================================================================================
///===========================================================================================
// Includes
///===========================================================================================
////===========================================================================================

#include "GameCode/Entities/Entity.hpp"
#include "GameCode/Map.hpp"
#include "Engine/Utilities/StringTable.hpp"
#include "Engine/Utilities/DeveloperConsole.hpp"

////===========================================================================================
///===========================================================================================
// Static Variable Initialization
///===========================================================================================
////===========================================================================================

int Entity::s_entityID = 1;


////===========================================================================================
///===========================================================================================
// Constructors/Destructors
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Entity::Entity( OpenGLRenderer* renderer, Clock* parentClock )
    : m_entityID( s_entityID++ )
    , m_clock( new Clock( parentClock, 0.5 ) )
    , m_renderer( renderer )
    , m_owningMap( nullptr )
    , m_mapPos( MapPosition( -1, -1 ) )
    , m_renderPosition( Vector3( -1.0f, -1.0f, -1.0f ) )
    , m_mesh( nullptr )
    , m_material( nullptr )
    , m_meshRenderer( nullptr )
    , m_entityName( "entity_" + std::to_string( m_entityID ) )
{
    m_verts.push_back( Vertex3D_PUC( Vector3( 0.45f, 0.4f, 0.45f ), Vector2::ZERO, Rgba::BLACK ) ); // 0
    m_verts.push_back( Vertex3D_PUC( Vector3( 0.45f, 0.4f, 0.55f ), Vector2::ZERO, Rgba::BLACK ) ); // 1
    m_verts.push_back( Vertex3D_PUC( Vector3( 0.55f, 0.4f, 0.55f ), Vector2::ZERO, Rgba::BLACK ) ); // 2
    m_verts.push_back( Vertex3D_PUC( Vector3( 0.55f, 0.4f, 0.45f ), Vector2::ZERO, Rgba::BLACK ) ); // 3
    m_verts.push_back( Vertex3D_PUC( Vector3( 0.5f, 0.5f, 0.5f ), Vector2::ZERO, Rgba::BLACK ) ); // 4

    m_indicies.push_back( 0 );
    m_indicies.push_back( 3 );
    m_indicies.push_back( 2 );

    m_indicies.push_back( 2 );
    m_indicies.push_back( 1 );
    m_indicies.push_back( 0 );

    m_indicies.push_back( 1 );
    m_indicies.push_back( 2 );
    m_indicies.push_back( 4 );

    m_indicies.push_back( 2 );
    m_indicies.push_back( 3 );
    m_indicies.push_back( 4 );

    m_indicies.push_back( 3 );
    m_indicies.push_back( 0 );
    m_indicies.push_back( 4 );

    m_indicies.push_back( 0 );
    m_indicies.push_back( 1 );
    m_indicies.push_back( 4 );


    FinalizeMesh();
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Entity::Entity( OpenGLRenderer* renderer, const XMLNode& entityNode, Clock* parentClock )
    : m_entityID( s_entityID++ )
    , m_clock( new Clock( parentClock, 0.5 ) )
    , m_renderer( renderer )
    , m_owningMap( nullptr )
    , m_mapPos( MapPosition( -1, -1 ) )
    , m_renderPosition( Vector3( -1.0f, -1.0f, -1.0f ) )
    , m_mesh( nullptr )
    , m_material( nullptr )
    , m_meshRenderer( nullptr )
{
    m_entityName = GetStringProperty( entityNode, "name", "entity_" + std::to_string( m_entityID ), false );
    m_entityNameID = StringTable::GetStringID( m_entityName );

    m_mapPos = GetIntVector2Property( entityNode, "mapPosition", MapPosition( -1, -1 ) );    

    LoadMesh( entityNode );

    FinalizeMesh();
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Entity::Entity( const Entity& copy, OpenGLRenderer* renderer, const XMLNode& entityNode, Clock* parentClock )
    : m_entityID( s_entityID++ )
    , m_clock( new Clock( parentClock, 0.5 ) )
    , m_renderer( renderer )
    , m_owningMap( copy.m_owningMap )
    , m_mapPos( copy.m_mapPos )
    , m_renderPosition( copy.m_renderPosition )
    , m_entityName( copy.m_entityName )
    , m_entityNameID( copy.m_entityNameID )
    , m_mesh( nullptr )
    , m_material( nullptr )
    , m_meshRenderer( nullptr )
{
    m_entityName = GetStringProperty( entityNode, "name", m_entityName, false );
    m_entityNameID = StringTable::GetStringID( m_entityName );

    m_mapPos = GetIntVector2Property( entityNode, "mapPosition", MapPosition( -1, -1 ) );

    // copy verts and indexes
    for (PUC_Vertexes::const_iterator copyVertIter = copy.m_verts.begin(); copyVertIter != copy.m_verts.end(); ++copyVertIter)
    {
        Vertex3D_PUC vert = *copyVertIter;
        m_verts.push_back( vert );
    }

    for (std::vector< unsigned int >::const_iterator copyIndexIter = copy.m_indicies.begin(); copyIndexIter != copy.m_indicies.end(); ++copyIndexIter)
    {
        unsigned int index = *copyIndexIter;
        m_indicies.push_back( index );
    }

    LoadMesh( entityNode );

    FinalizeMesh();
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Entity::~Entity()
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
void Entity::Startup()
{

}

////===========================================================================================
///===========================================================================================
// Accessors/Queries
///===========================================================================================
////===========================================================================================


////===========================================================================================
///===========================================================================================
// Mutators
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Entity::SetMapPosition( const MapPosition& mapPos )
{
    if (!m_owningMap)
        return;

    m_mapPos = mapPos;
    m_renderPosition = Vector3( (float)m_mapPos.x, (float) (m_owningMap->GetCellAtMapPos( mapPos )->GetHeight()), (float)m_mapPos.y );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Entity::LoadMesh( const XMLNode& meshNode, const Rgba& defaultColor )
{
    // Get Vertex Info
    XMLNode vertInfoNode = meshNode.getChildNode( "VertexInfo" );

    if (!vertInfoNode.isEmpty())
    {
        m_verts.clear();
        m_indicies.clear();

        int vertexCounter = 0;
        bool hasAnotherVertex = false;

        do
        {
            XMLNode vertexNode = vertInfoNode.getChildNode( "Vertex", vertexCounter++ );
            if (!vertexNode.isEmpty())
            {
                hasAnotherVertex = true;
                Vector3 vertPos = GetVector3Property( vertexNode, "pos", Vector3( -1000.0f, -1000.0f, -1000.0f ) );
                Vector2 vertUV = GetVector2Property( vertexNode, "uv", Vector2::ZERO );
                Rgba vertColor = GetRgbaProperty( vertexNode, "color", defaultColor );

                if (AreVectorsEqual( vertPos, Vector3( -1000.0f, -1000.0f, -1000.0f ), 0.5f ))
                {
                    // Soft fail
                    DeveloperConsole::WriteLine( "Attempted to load Entity vertex " + std::to_string( vertexCounter ) + ". XML doesn't contain \"pos\".", WARNING_TEXT_COLOR );
                    continue;
                }
                else
                    m_verts.push_back( Vertex3D_PUC( vertPos, vertUV, vertColor ) );
            }
            else
                hasAnotherVertex = false;

        } while (hasAnotherVertex);

        vertInfoNode.deleteNodeContent();

        // Get Index info
        XMLNode indexInfoNode = meshNode.getChildNode( "IndexInfo" );

        int indexCounter = 0;
        bool hasAnotherIndex = false;

        do
        {
            XMLNode indexNode = indexInfoNode.getChildNode( "Index", indexCounter++ );
            if (!indexNode.isEmpty())
            {
                hasAnotherIndex = true;
                int indexVal = GetIntProperty( indexNode, "val", -1 );

                if (indexVal == -1)
                {
                    // Soft fail
                    DeveloperConsole::WriteLine( "Attempted to load Entity index " + std::to_string( indexCounter ) + ". XML doesn't contain \"val\".", WARNING_TEXT_COLOR );
                    continue;
                }
                else
                    m_indicies.push_back( indexVal );
            }
            else
                hasAnotherIndex = false;

        } while (hasAnotherIndex);

        indexInfoNode.deleteNodeContent();
    }
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Entity::ChangeMesh( const PUC_Vertexes& verts, const std::vector<unsigned int> indexes, const Rgba& newColor )
{
    m_verts.clear();
    m_indicies.clear();

    for each (Vertex3D_PUC vert in verts)
        m_verts.push_back( Vertex3D_PUC( vert.position, vert.uv, newColor ) );

    for each (unsigned int index in indexes)
        m_indicies.push_back( index );

    m_mesh->SetVertexData( m_verts.data(), DrawInstructions( GL_TRIANGLES, m_verts.size(), m_indicies.size(), true ), Vertex3D_PUC::GetVertexInfo() );
    m_mesh->SetIndexData( m_indicies.data(), m_indicies.size() );

    m_meshRenderer->SetMesh( m_mesh );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Entity::FinalizeMesh()
{
    // create mesh and renderer
    m_mesh = new PuttyMesh( m_renderer );

    m_mesh->SetVertexData( m_verts.data(), DrawInstructions( GL_TRIANGLES, m_verts.size(), m_indicies.size(), true ), Vertex3D_PUC::GetVertexInfo() );
    m_mesh->SetIndexData( m_indicies.data(), m_indicies.size() );

    RenderState rs( false, true, true, false );
    rs.SetBlendMode( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    m_material = new Material( m_renderer, OpenGLRenderer::CreateOrGetShader( "Data/Shaders/basic" ), rs );

    m_meshRenderer = new MeshRenderer( m_renderer );

    m_meshRenderer->SetMesh( m_mesh );
    m_meshRenderer->SetMaterial( m_material, false );
}

////===========================================================================================
///===========================================================================================
// Update
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Entity::ProcessInput( InputSystem* inputSystem, double deltaSeconds )
{

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Entity::Update( double deltaSeconds, bool debugModeEnabled )
{

}


////===========================================================================================
///===========================================================================================
// Render
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Entity::Render( bool debugModeEnabled )
{
    if (!m_renderer)
        return;

    Matrix4f modelTransform = Matrix4f::CreateTranslation( Vector3( m_renderPosition.x, m_renderPosition.y, m_renderPosition.z ) );

    m_mesh->SetDrawInstructions( DrawInstructions( GL_TRIANGLES, m_verts.size(), m_indicies.size(), true ) );
    m_meshRenderer->Render( modelTransform, m_renderer->GetViewMatrix(), m_renderer->GetPerspectiveMatrix() );

    m_renderer->SetLineSize( 3.0f );

    modelTransform.Scale( 1.1f );
    modelTransform.Translate( Vector3( -0.05f, -0.05f, -0.05f ) );
    for (PUC_Vertexes::iterator vertIter = m_verts.begin(); vertIter != m_verts.end(); ++vertIter)
    {
        Vertex3D_PUC& vert = *vertIter;

        vert.color = Rgba::BLACK;

    }
    m_mesh->SetVertexData( m_verts.data(), DrawInstructions( GL_LINE_LOOP, m_verts.size(), m_indicies.size(), true ), Vertex3D_PUC::GetVertexInfo() );

//     m_mesh->SetDrawInstructions( DrawInstructions( GL_LINE_LOOP, m_verts.size(), m_indicies.size(), true ) );
    m_meshRenderer->Render( modelTransform, m_renderer->GetViewMatrix(), m_renderer->GetPerspectiveMatrix() );




}

////===========================================================================================
///===========================================================================================
// Private Functions
///===========================================================================================
////===========================================================================================

