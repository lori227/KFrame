#include "KFClusterMasterConfig.h"

namespace KFrame
{
    const KFClusterSetting* KFClusterMasterConfig::FindClusterSetting( const std::string& name )
    {
        return _cluster_setting_list.Find( name );
    }

    bool KFClusterMasterConfig::LoadConfig()
    {
        //////////////////////////////////////////////////////////////////
        KFXml kfxml( _file );
        auto config = kfxml.RootNode();
        auto clusters = config.FindNode( "Clusters" );
        auto cluster = clusters.FindNode( "Cluster" );
        while ( cluster.IsValid() )
        {
            auto name = cluster.GetString( "Name" );
            auto kfsetting = _cluster_setting_list.Create( name );

            kfsetting->_name = name;
            kfsetting->_type = cluster.GetString( "Type" );
            kfsetting->_key = cluster.GetString( "Key" );

            cluster.NextNode();
        }
        //////////////////////////////////////////////////////////////////

        return true;
    }
}