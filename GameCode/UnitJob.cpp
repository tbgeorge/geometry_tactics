//=================================================================================
// UnitJob.cpp
// Author: Tyler George
// Date  : November 23, 2015
//=================================================================================


////===========================================================================================
///===========================================================================================
// Includes
///===========================================================================================
////===========================================================================================

#include "GameCode/UnitJob.hpp"
#include "Engine/Utilities/DeveloperConsole.hpp"
#include "Engine/Utilities/Error.hpp"


////===========================================================================================
///===========================================================================================
// Static Variable Initialization
///===========================================================================================
////===========================================================================================
UnitJobMap UnitJob::s_allUnitJobs;


////===========================================================================================
///===========================================================================================
// Constructors/Destructors
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
UnitJob::UnitJob( const XMLNode& unitJobNode )
{
    m_name = GetStringProperty( unitJobNode, "name", "", false );
    RECOVERABLE_ASSERT( m_name != "" );


    // Generate AI Behaviors list
    AIBehaviorRegistryMap* behaviorMap = AIBehaviorRegistration::GetAIBehaviorRegistry();

    int behaviorCounter = 0;
    bool hasAnotherBehavior;

    do
    {
        XMLNode behaviorNode = unitJobNode.getChildNode( "AIBehavior", behaviorCounter++ );
        if (!behaviorNode.isEmpty())
        {
            hasAnotherBehavior = true;
            std::string name = GetStringProperty( behaviorNode, "name", "unknown behavior", true );
            if (name == "unknown behavior")
            {
                // Soft fail
                DeveloperConsole::WriteLine( "Attempted to load AI Behavior " + std::to_string( behaviorCounter ) + " on JOB: " + m_name + ". XML doesn't contain \"name\".", WARNING_TEXT_COLOR );
                continue;
            }

            AIBehaviorRegistryMap::const_iterator behaviorIter = behaviorMap->find( name );
            if (behaviorIter == behaviorMap->end())
            {
                // Soft fail
                DeveloperConsole::WriteLine( "Attempted to load AI Behavior " + std::to_string( behaviorCounter ) + " on JOB: " + m_name + ". Behavior: " + name + " does not exist.", WARNING_TEXT_COLOR );
                continue;
            }

            AIBehaviorRegistration* behaviorRegistration = behaviorIter->second;
            BaseAIBehavior* behavior = behaviorRegistration->CreateAIBehavior( behaviorNode );

            m_behaviors.push_back( behavior );
        }
        else
            hasAnotherBehavior = false;

    } while (hasAnotherBehavior);

    // Get Vertex Info
    XMLNode vertInfoNode = unitJobNode.getChildNode( "VertexInfo" );

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

                if (AreVectorsEqual( vertPos, Vector3( -1000.0f, -1000.0f, -1000.0f ), 0.5f ))
                {
                    // Soft fail
                    DeveloperConsole::WriteLine( "Attempted to load Entity vertex " + std::to_string( vertexCounter ) + ". XML doesn't contain \"pos\".", WARNING_TEXT_COLOR );
                    continue;
                }
                else
                    m_verts.push_back( Vertex3D_PUC( vertPos, vertUV, Rgba() ) );
            }
            else
                hasAnotherVertex = false;

        } while (hasAnotherVertex);

        vertInfoNode.deleteNodeContent();

        // Get Index info
        XMLNode indexInfoNode = unitJobNode.getChildNode( "IndexInfo" );

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
UnitJob::~UnitJob()
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
void UnitJob::LoadAllUnitJobs()
{
    std::string directoryToSearch = "Data/UnitJobs/";
    Strings FeatureFiles;
    bool success = FindAllFilesOfType( directoryToSearch, "*.UnitJob.xml", FeatureFiles );

    if (success)
    {
        for (Strings::const_iterator UnitJobFileIter = FeatureFiles.begin(); UnitJobFileIter != FeatureFiles.end(); ++UnitJobFileIter)
        {
            std::string filePath = *UnitJobFileIter;
            // need to handle error checking here
            XMLNode unitJobsRoot = XMLNode::parseFile( filePath.c_str(), "UnitJobs" );


            int unitJobCounter = 0;
            bool hasAnotherUnitJob = false;

            do
            {
                XMLNode unitJobNode = unitJobsRoot.getChildNode( "UnitJob", unitJobCounter++ );
                if (!unitJobNode.isEmpty())
                {
                    hasAnotherUnitJob = true;
                    std::string name = GetStringProperty( unitJobNode, "name", "unknown UnitJob", false );
                    if (name == "unknown UnitJob")
                    {
                        // Soft fail
                        DeveloperConsole::WriteLine( "Attempted to load UnitJob " + std::to_string( unitJobCounter ) + ". XML doesn't contain \"name\".", WARNING_TEXT_COLOR );
                        continue;
                    }

                    UnitJob* unitJob = new UnitJob( unitJobNode );
                    s_allUnitJobs.insert( std::pair< std::string, UnitJob*>( name, unitJob ) );

                }
                else
                    hasAnotherUnitJob = false;

            } while (hasAnotherUnitJob);

            unitJobsRoot.deleteNodeContent();

        }
    }
}

////===========================================================================================
///===========================================================================================
// Accessors/Queries
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
UnitJob* UnitJob::FindUnitJobByName( const std::string& name )
{
    UnitJobMap::iterator jobIter = s_allUnitJobs.find( name );

    if (jobIter != s_allUnitJobs.end())
        return jobIter->second;

    return nullptr;
}

////===========================================================================================
///===========================================================================================
// Mutators
///===========================================================================================
////===========================================================================================

////===========================================================================================
///===========================================================================================
// Private Functions
///===========================================================================================
////===========================================================================================

