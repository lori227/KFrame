﻿#include "KFGuildShardPlugin.h"
#include "KFGuildShardModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFGuildShardPlugin::Install()
    {
        __REGISTER_MODULE__( KFGuildShard );
    }

    void KFGuildShardPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFGuildShard );
    }

    void KFGuildShardPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_timer, KFTimerInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_redis, KFRedisInterface );
        __FIND_MODULE__( _kf_worker, KFWorkerInterface );
        __FIND_MODULE__( _kf_option, KFOptionInterface );
        __FIND_MODULE__( _kf_kernel, KFKernelInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_schedule, KFScheduleInterface );
        __FIND_MODULE__( _kf_tcp_server, KFTcpServerInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_cluster_shard, KFClusterShardInterface );
    }
}