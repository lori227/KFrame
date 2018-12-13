#include "KFPublicShardPlugin.h"
#include "KFPublicShardModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFPublicShardPlugin::Install()
    {
        __REGISTER_MODULE__( KFPublicShard );
    }

    void KFPublicShardPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFPublicShard );
    }

    void KFPublicShardPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_redis, KFRedisInterface );
        __FIND_MODULE__( _kf_worker, KFWorkerInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_cluster_shard, KFClusterShardInterface );
    }
}