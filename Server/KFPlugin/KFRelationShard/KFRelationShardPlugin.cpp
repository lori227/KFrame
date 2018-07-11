#include "KFRelationShardPlugin.h"
#include "KFRelationShardModule.h"

//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFRelationShardPlugin::Install()
    {
        __REGISTER_MODULE__( KFRelationShard );
    }

    void KFRelationShardPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFRelationShard );
    }

    void KFRelationShardPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_redis, KFRedisInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_worker, KFWorkerInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_tcp_server, KFTcpServerInterface );
        __FIND_MODULE__( _kf_schedule, KFScheduleInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_cluster_shard, KFClusterShardInterface );
    }
}