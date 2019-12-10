#include "KFPublicShardPlugin.hpp"
#include "KFPublicShardModule.hpp"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFPublicShardPlugin::Install()
    {
        __REGISTER_MODULE__( KFPublicShard );
    }

    void KFPublicShardPlugin::UnInstall()
    {
        __UN_MODULE__( KFPublicShard );
    }

    void KFPublicShardPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_redis, KFRedisInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_route, KFRouteClientInterface );
    }
}