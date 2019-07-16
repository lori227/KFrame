#include "KFRouteShardPlugin.hpp"
#include "KFRouteShardModule.hpp"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFRouteShardPlugin::Install()
    {
        __REGISTER_MODULE__( KFRouteShard );
    }

    void KFRouteShardPlugin::UnInstall()
    {
        __UN_MODULE__( KFRouteShard );
    }

    void KFRouteShardPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_cluster_shard, KFClusterShardInterface );
    }
}