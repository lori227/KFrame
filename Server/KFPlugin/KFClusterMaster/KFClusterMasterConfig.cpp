#include "KFClusterMasterConfig.h"

namespace KFrame
{
    const KFClusterSetting* KFClusterMasterConfig::FindClusterSetting( const std::string& name )
    {
        return _cluster_list.Find( name );
    }

    bool KFClusterMasterConfig::LoadConfig()
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