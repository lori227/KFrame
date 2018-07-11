#include "KFDataShardPlugin.h"
#include "KFDataShardModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFDataShardPlugin::Install()
    {
        __REGISTER_MODULE__( KFDataShard );
    }

    void KFDataShardPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFDataShard );
    }

    void KFDataShardPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_timer, KFTimerInterface );
        __FIND_MODULE__( _kf_redis, KFRedisInterface );
        __FIND_MODULE__( _kf_worker, KFWorkerInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_tcp_server, KFTcpServerInterface );
        __FIND_MODULE__( _kf_cluster_shard, KFClusterShardInterface );
    }
}