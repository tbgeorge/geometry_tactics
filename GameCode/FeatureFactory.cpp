//=================================================================================
// FeatureFactory.cpp
// Author: Tyler George
// Date  : October 4, 2015
//=================================================================================


////===========================================================================================
///===========================================================================================
// Includes
///===========================================================================================
////===========================================================================================
#include "Engine/Utilities/FileUtilities.hpp"
#include "Engine/Utilities/Utilities.hpp"
#include "Engine/Utilities/XMLHelper.hpp"

#include "GameCode/FeatureFactory.hpp"
#include "Engine/Utilities/DeveloperConsole.hpp"


////===========================================================================================
///===========================================================================================
// Static Variable Initialization
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
/// Protected Variables
///---------------------------------------------------------------------------------
FeatureFactories FeatureFactory::s_featureFactories;

////===========================================================================================
///===========================================================================================
// Constructors/Destructors
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
FeatureFactory::FeatureFactory( OpenGLRenderer* renderer, Clock* parentClock, XMLNode& featureNode )
    : m_renderer( renderer )
    , m_parentClock( parentClock )
{
    m_templateFeature = new Feature( renderer, parentClock, featureNode );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
FeatureFactory::~FeatureFactory()
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
bool FeatureFactory::LoadAllFeatureFactories( OpenGLRenderer* renderer, Clock* parentClock )
{
    std::string directoryToSearch = "Data/Features/";
    Strings FeatureFiles;
    bool success = FindAllFilesOfType( directoryToSearch, "*.feature.xml", FeatureFiles );
 
    if (success)
    {
        for (Strings::const_iterator featureFileIter = FeatureFiles.begin(); featureFileIter != FeatureFiles.end(); ++featureFileIter)
        {
            std::string filePath = *featureFileIter;
            // need to handle error checking here
            XMLNode featuresRoot = XMLNode::parseFile( filePath.c_str(), "Features" );


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

                    FeatureFactory* featureFactory = new FeatureFactory( renderer, parentClock, featureNode );
                    s_featureFactories.insert( std::pair< std::string, FeatureFactory* >( name, featureFactory ) );

                }
                else
                    hasAnotherFeature = false;

            } while (hasAnotherFeature);

            featuresRoot.deleteNodeContent();

        }
        return true;
    }
    else
        return false;
}

////===========================================================================================
///===========================================================================================
// Accessors/Queries
///===========================================================================================
////===========================================================================================

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
FeatureFactory* FeatureFactory::FindFactoryByName( const std::string& name )
{
    return s_featureFactories.at( name );
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
std::vector< FeatureFactory* > FeatureFactory::FindAllFactoriesOfType( FeatureType type )
{
    std::vector< FeatureFactory* > factoriesOfType;

    for (FeatureFactories::iterator featureFactoryIter = s_featureFactories.begin(); featureFactoryIter != s_featureFactories.end(); ++featureFactoryIter)
    {
        FeatureFactory* factory = featureFactoryIter->second;
        FeatureType factoryType = factory->GetType();

        if (type == factoryType)
            factoriesOfType.push_back( factory );
    }

    return factoriesOfType;
}

///---------------------------------------------------------------------------------
///
///---------------------------------------------------------------------------------
Feature* FeatureFactory::SpawnFeature( const XMLNode& possibleSaveData )
{
    Feature* newFeature = new Feature( *m_templateFeature, m_renderer, m_parentClock, possibleSaveData );
    return newFeature;
}

////===========================================================================================
///===========================================================================================
// Mutators
///===========================================================================================
////===========================================================================================


////===========================================================================================
///===========================================================================================
// Update
///===========================================================================================
////===========================================================================================

////===========================================================================================
///===========================================================================================
// Render
///===========================================================================================
////===========================================================================================


////===========================================================================================
///===========================================================================================
// Private Functions
///===========================================================================================
////===========================================================================================

