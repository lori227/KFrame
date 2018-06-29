#include "KFClusterClientConfig.h"

namespace KFrame
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    KFClusterClientConfig::KFClusterClientConfig()
    {

    }

    KFClusterClientConfig::~KFClusterClientConfig()
    {

    }

    bool KFClusterClientConfig::LoadConfig( const char* file )
    {
        try
        {
            KFXml kfxml( file );
            auto config = kfxml.RootNode();

            //////////////////////////////////////////////////////////////////
            auto clusters = config.FindNode( "ClusterClients" );
            auto cluster = clusters.FindNode( "ClusterClient" );
            while ( cluster.IsValid() )
            {
                KFConnection setting;

                setting._name = cluster.GetString( "Cluster" );
                _cluster_client_setting.push_back( setting );

                cluster.NextNode();
            }

            //////////////////////////////////////////////////////////////////
        }
        catch ( ... )
        {
            return false;
        }

        return true;
    }
}