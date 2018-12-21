#include "KFClusterClientConfig.h"

namespace KFrame
{
    const KFClusterSetting* KFClusterClientConfig::FindClusterSetting( const std::string& name )
    {
        return _cluster_list.Find( name );
    }

    bool KFClusterClientConfig::LoadConfig()
    {
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( _file );
        auto config = kfxml.RootNode();

        auto clusters = config.FindNode( "Clusters" );
        auto xmlnode = clusters.FindNode( "Cluster" );
        while ( xmlnode.IsValid() )
        {
            auto name = xmlnode.GetString( "Name" );
            auto kfsetting = _cluster_list.Create( name );
            kfsetting->_name = name;
            kfsetting->_key = xmlnode.GetString( "Key" );

            xmlnode.NextNode();
        }
        //////////////////////////////////////////////////////////////////
        return true;
    }
}