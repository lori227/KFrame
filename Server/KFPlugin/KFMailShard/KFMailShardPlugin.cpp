#include "KFMailShardPlugin.h"
#include "KFMailShardModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFMailShardPlugin::Install ()
    {
        __REGISTER_MODULE__( KFMailShard );
    }

    void KFMailShardPlugin::UnInstall ()
    {
        __UNREGISTER_MODULE__( KFMailShard );
    }

    void KFMailShardPlugin::LoadModule ()
    {
        __FIND_MODULE__( _kf_redis, KFRedisInterface );
        __FIND_MODULE__( _kf_option, KFOptionInterface );
        __FIND_MODULE__( _kf_worker, KFWorkerInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_schedule, KFScheduleInterface );
        __FIND_MODULE__( _kf_cluster_shard, KFClusterShardInterface );
    }
}