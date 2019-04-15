#include "KFRankShardPlugin.hpp"
#include "KFRankShardModule.hpp"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFRankShardPlugin::Install()
    {
        __REGISTER_MODULE__( KFRankShard );
    }

    void KFRankShardPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFRankShard );
    }

    void KFRankShardPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_redis, KFRedisInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_tcp_client, KFTcpClientInterface );
        __FIND_MODULE__( _kf_route, KFRouteClientInterface );
    }
}