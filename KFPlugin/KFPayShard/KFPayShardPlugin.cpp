#include "KFPayShardPlugin.hpp"
#include "KFPayShardModule.hpp"
#include "KFConfig/KFConfigInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFPayShardPlugin::Install()
    {
        __REGISTER_MODULE__( KFPayShard );
    }

    void KFPayShardPlugin::UnInstall()
    {
        __UN_MODULE__( KFPayShard );
    }

    void KFPayShardPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_redis, KFRedisInterface );
        __FIND_MODULE__( _kf_mongo, KFMongoInterface );
        __FIND_MODULE__( _kf_channel, KFChannelInterface );
        __FIND_MODULE__( _kf_http_server, KFHttpServerInterface );
    }

    void KFPayShardPlugin::AddConfig()
    {
    }
}