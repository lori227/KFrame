#include "KFBattleShardPlugin.h"
#include "KFBattleShardModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFBattleShardPlugin::Install()
    {
        __REGISTER_MODULE__( KFBattleShard );
    }

    void KFBattleShardPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFBattleShard );
    }

    void KFBattleShardPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_redis, KFRedisInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_tcp_server, KFTcpServerInterface );
        __FIND_MODULE__( _kf_cluster_shard, KFClusterShardInterface );
    }
}