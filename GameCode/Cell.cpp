//=================================================================================
// Cell.cpp
// Author: Tyler George
// Date  : October 28, 2015
//=================================================================================


////===========================================================================================
///===========================================================================================
// Includes
///===========================================================================================
////===========================================================================================

#include "GameCode/Cell.hpp"


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
Cell::Cell()
    : m_mapPos( MapPosition::ZERO )
    , m_height( 0.0f )
    , m_isHovered( false )
    , m_actor( nullptr )
    , m_feature( nullptr )
{

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Cell::Cell( const MapPosition& mapPos, float height )
    : m_mapPos( mapPos )
    , m_height( height )
    , m_isHovered( false )
    , m_actor( nullptr )
    , m_feature( nullptr )
{

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Cell::~Cell()
{

}

////===========================================================================================
///===========================================================================================
// Initialization
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Cell::Startup()
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
float Cell::GetRealHeight()
{
    if (m_feature)
        return m_height + m_feature->GetHeight();
    else
        return m_height;
}

////===========================================================================================
///===========================================================================================
// Mutators
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Cell::SetHovered( bool isHovered )
{
    m_isHovered = isHovered;
    //ConsolePrintf( "SetHovered %s: %i, %i\n", isHovered ? "true" : "false", m_mapPos.x, m_mapPos.y );

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Cell::CalculateVertexes( PUC_Vertexes& verts, std::vector< unsigned int >& indexes, int indexOffset )
{

    float minX = (float)m_mapPos.x;
    float minY = 0.0f;
    float minZ = (float)m_mapPos.y;

    float maxX = minX + CELL_SIZE;
    float maxY = minY + m_height;
    float maxZ = minZ + CELL_SIZE;

    // Vertexes

    // -y face
    verts.push_back( Vertex3D_PUC( Vector3( minX, minY, minZ ), Vector2( 1.0f, 0.0f ), Rgba::BLACK + Rgba() * 0.2f  ) ); // 0
    verts.push_back( Vertex3D_PUC( Vector3( maxX, minY, minZ ), Vector2( 0.0f, 0.0f ), Rgba::BLACK + Rgba() * 0.2f  ) ); // 1
    verts.push_back( Vertex3D_PUC( Vector3( maxX, minY, maxZ ), Vector2( 0.0f, 1.0f ), Rgba::BLACK + Rgba() * 0.2f  ) ); // 2
    verts.push_back( Vertex3D_PUC( Vector3( minX, minY, maxZ ), Vector2( 1.0f, 1.0f ), Rgba::BLACK + Rgba() * 0.2f  ) ); // 3
                                                                                             
    // +y face                                                                               
    verts.push_back( Vertex3D_PUC( Vector3( minX, maxY, minZ ), Vector2( 1.0f, 1.0f ), m_height >= 0.0f ? Rgba::BLACK + Rgba() * 0.2f  + (Rgba::WHITE * RangeMap( m_height, 0.0f, 30.0f, 0.0f, 1.0f )) : Rgba::BLUE ) ); // 4
    verts.push_back( Vertex3D_PUC( Vector3( maxX, maxY, minZ ), Vector2( 0.0f, 1.0f ), m_height >= 0.0f ? Rgba::BLACK + Rgba() * 0.2f  + (Rgba::WHITE * RangeMap( m_height, 0.0f, 30.0f, 0.0f, 1.0f )) : Rgba::BLUE ) ); // 5
    verts.push_back( Vertex3D_PUC( Vector3( maxX, maxY, maxZ ), Vector2( 0.0f, 0.0f ), m_height >= 0.0f ? Rgba::BLACK + Rgba() * 0.2f  + (Rgba::WHITE * RangeMap( m_height, 0.0f, 30.0f, 0.0f, 1.0f )) : Rgba::BLUE ) ); // 6
    verts.push_back( Vertex3D_PUC( Vector3( minX, maxY, maxZ ), Vector2( 1.0f, 0.0f ), m_height >= 0.0f ? Rgba::BLACK + Rgba() * 0.2f  + (Rgba::WHITE * RangeMap( m_height, 0.0f, 30.0f, 0.0f, 1.0f )) : Rgba::BLUE ) ); // 7
                                                                                             
    // +x face                                                                               
    verts.push_back( Vertex3D_PUC( Vector3( maxX, minY, maxZ ), Vector2( 0.0f, m_height ), Rgba::BLACK + Rgba() * 0.2f  ) ); // 8
    verts.push_back( Vertex3D_PUC( Vector3( maxX, minY, minZ ), Vector2( 1.0f, m_height ), Rgba::BLACK + Rgba() * 0.2f  ) ); // 9
    verts.push_back( Vertex3D_PUC( Vector3( maxX, maxY, minZ ), Vector2( 1.0f, 0.0f ), m_height >= 0.0f ? Rgba::BLACK + Rgba() * 0.2f  + (Rgba::WHITE * RangeMap( m_height, 0.0f, 30.0f, 0.0f, 1.0f )) : Rgba::BLUE ) ); // 10
    verts.push_back( Vertex3D_PUC( Vector3( maxX, maxY, maxZ ), Vector2( 0.0f, 0.0f ), m_height >= 0.0f ? Rgba::BLACK + Rgba() * 0.2f  + (Rgba::WHITE * RangeMap( m_height, 0.0f, 30.0f, 0.0f, 1.0f )) : Rgba::BLUE ) ); // 11
                                                                                             
    // -x face                                                                               
    verts.push_back( Vertex3D_PUC( Vector3( minX, minY, minZ ), Vector2( 0.0f, m_height ), Rgba::BLACK + Rgba() * 0.2f  ) ); // 12
    verts.push_back( Vertex3D_PUC( Vector3( minX, minY, maxZ ), Vector2( 1.0f, m_height ), Rgba::BLACK + Rgba() * 0.2f  ) ); // 13
    verts.push_back( Vertex3D_PUC( Vector3( minX, maxY, maxZ ), Vector2( 1.0f, 0.0f ), m_height >= 0.0f ? Rgba::BLACK + Rgba() * 0.2f  + (Rgba::WHITE * RangeMap( m_height, 0.0f, 30.0f, 0.0f, 1.0f )) : Rgba::BLUE ) ); // 14
    verts.push_back( Vertex3D_PUC( Vector3( minX, maxY, minZ ), Vector2( 0.0f, 0.0f ), m_height >= 0.0f ? Rgba::BLACK + Rgba() * 0.2f  + (Rgba::WHITE * RangeMap( m_height, 0.0f, 30.0f, 0.0f, 1.0f )) : Rgba::BLUE ) ); // 15
                                                                                             
    // +z face                                                                               
    verts.push_back( Vertex3D_PUC( Vector3( minX, minY, maxZ ), Vector2( 0.0f, m_height ), Rgba::BLACK + Rgba() * 0.2f  ) ); // 16
    verts.push_back( Vertex3D_PUC( Vector3( maxX, minY, maxZ ), Vector2( 1.0f, m_height ), Rgba::BLACK + Rgba() * 0.2f  ) ); // 17
    verts.push_back( Vertex3D_PUC( Vector3( maxX, maxY, maxZ ), Vector2( 1.0f, 0.0f ), m_height >= 0.0f ? Rgba::BLACK + Rgba() * 0.2f  + (Rgba::WHITE * RangeMap( m_height, 0.0f, 30.0f, 0.0f, 1.0f )) : Rgba::BLUE ) ); // 18
    verts.push_back( Vertex3D_PUC( Vector3( minX, maxY, maxZ ), Vector2( 0.0f, 0.0f ), m_height >= 0.0f ? Rgba::BLACK + Rgba() * 0.2f  + (Rgba::WHITE * RangeMap( m_height, 0.0f, 30.0f, 0.0f, 1.0f )) : Rgba::BLUE ) ); // 19
                                                                                             
    // -z face                                                                               
    verts.push_back( Vertex3D_PUC( Vector3( maxX, minY, minZ ), Vector2( 0.0f, m_height ), Rgba::BLACK + Rgba() * 0.2f  ) ); // 20
    verts.push_back( Vertex3D_PUC( Vector3( minX, minY, minZ ), Vector2( 1.0f, m_height ), Rgba::BLACK + Rgba() * 0.2f  ) ); // 21
    verts.push_back( Vertex3D_PUC( Vector3( minX, maxY, minZ ), Vector2( 1.0f, 0.0f ), m_height >= 0.0f ? Rgba::BLACK + Rgba() * 0.2f  + (Rgba::WHITE * RangeMap( m_height, 0.0f, 30.0f, 0.0f, 1.0f )) : Rgba::BLUE ) ); // 22
    verts.push_back( Vertex3D_PUC( Vector3( maxX, maxY, minZ ), Vector2( 0.0f, 0.0f ), m_height >= 0.0f ? Rgba::BLACK + Rgba() * 0.2f  + (Rgba::WHITE * RangeMap( m_height, 0.0f, 30.0f, 0.0f, 1.0f )) : Rgba::BLUE ) ); // 23

    // Indicies

    // -y face
    indexes.push_back( indexOffset + 0 );
    indexes.push_back( indexOffset + 1 );
    indexes.push_back( indexOffset + 2 );
    indexes.push_back( indexOffset + 2 );
    indexes.push_back( indexOffset + 3 );
    indexes.push_back( indexOffset + 0 );

    // +y face
    indexes.push_back( indexOffset + 7 );
    indexes.push_back( indexOffset + 6 );
    indexes.push_back( indexOffset + 5 );
    indexes.push_back( indexOffset + 5 );
    indexes.push_back( indexOffset + 4 );
    indexes.push_back( indexOffset + 7 );

    // +x face
    indexes.push_back( indexOffset + 8 );
    indexes.push_back( indexOffset + 9 );
    indexes.push_back( indexOffset + 10 );
    indexes.push_back( indexOffset + 10);
    indexes.push_back( indexOffset + 11 );
    indexes.push_back( indexOffset + 8 );

    // -x face
    indexes.push_back( indexOffset + 12 );
    indexes.push_back( indexOffset + 13 );
    indexes.push_back( indexOffset + 14 );
    indexes.push_back( indexOffset + 14 );
    indexes.push_back( indexOffset + 15 );
    indexes.push_back( indexOffset + 12 );

    // +z face
    indexes.push_back( indexOffset + 16 );
    indexes.push_back( indexOffset + 17 );
    indexes.push_back( indexOffset + 18 );
    indexes.push_back( indexOffset + 18 );
    indexes.push_back( indexOffset + 19 );
    indexes.push_back( indexOffset + 16 );

    // -z face
    indexes.push_back( indexOffset + 20 );
    indexes.push_back( indexOffset + 21 );
    indexes.push_back( indexOffset + 22 );
    indexes.push_back( indexOffset + 22 );
    indexes.push_back( indexOffset + 23 );
    indexes.push_back( indexOffset + 20 );

}

////===========================================================================================
///===========================================================================================
// Update
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Cell::ProcessInput( InputSystem* inputSystem, double deltaSeconds )
{

}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Cell::Update( double deltaSeconds, bool debugModeEnabled )
{
    if (m_isHovered)
    {
        SetHovered( false );
    }

}


////===========================================================================================
///===========================================================================================
// Render
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
void Cell::Render( OpenGLRenderer* renderer, bool debugModeEnabled )
{
    if (!renderer)
        return;

    if (m_isHovered)
    {
        float minX = (float)m_mapPos.x;
        float minY = 0.0f;
        float minZ = (float)m_mapPos.y;

        float maxX = minX + CELL_SIZE;
        float maxY = minY + m_height;
        float maxZ = minZ + CELL_SIZE;

        PUC_Vertexes verts;

        verts.push_back( Vertex3D_PUC( Vector3( minX, maxY + 0.01f, minZ ), Vector2::ZERO, Rgba::MAGENTA ) ); // 0
        verts.push_back( Vertex3D_PUC( Vector3( minX, maxY + 0.01f, maxZ ), Vector2::ZERO, Rgba::MAGENTA ) ); // 1
        verts.push_back( Vertex3D_PUC( Vector3( maxX, maxY + 0.01f, maxZ ), Vector2::ZERO, Rgba::MAGENTA ) ); // 2
        verts.push_back( Vertex3D_PUC( Vector3( maxX, maxY + 0.01f, minZ ), Vector2::ZERO, Rgba::MAGENTA ) ); // 3

        renderer->SetLineSize( 5.0f );
        renderer->DrawVertexes( NULL, verts, GL_QUADS );

        verts[0].color = Rgba::BLACK;
        verts[1].color = Rgba::BLACK;
        verts[2].color = Rgba::BLACK;
        verts[3].color = Rgba::BLACK;

        renderer->DrawVertexes( NULL, verts, GL_LINE_LOOP );
    }
    
    if (m_feature)
        m_feature->Render( debugModeEnabled );

}

////===========================================================================================
///===========================================================================================
// Private Functions
///===========================================================================================
////===========================================================================================

