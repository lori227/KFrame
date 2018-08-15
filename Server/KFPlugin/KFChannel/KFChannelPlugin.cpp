#include "KFChannelPlugin.h"
#include "KFChannelModule.h"

//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFChannelPlugin::Install()
    {
        __REGISTER_MODULE__( KFChannel );
    }

    void KFChannelPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFChannel );
    }

    void KFChannelPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_redis, KFRedisInterface );
        __FIND_MODULE__( _kf_http_client, KFHttpClientInterface );
        __FIND_MODULE__( _kf_http_server, KFHttpServerInterface );
    }
}