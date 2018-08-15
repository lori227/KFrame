#include "KFMatchShardPlugin.h"
#include "KFMatchShardModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFMatchShardPlugin::Install()
    {
        __REGISTER_MODULE__( KFMatchShard );
    }

    void KFMatchShardPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFMatchShard );
    }

    void KFMatchShardPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_tcp_client, KFTcpClientInterface );
        __FIND_MODULE__( _kf_tcp_server, KFTcpServerInterface );
        __FIND_MODULE__( _kf_cluster, KFClusterClientInterface );
        __FIND_MODULE__( _kf_cluster_shard, KFClusterShardInterface );
    }
}