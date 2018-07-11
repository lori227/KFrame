#include "KFPlatformPlugin.h"
#include "KFPlatformModule.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFRedis/KFRedisInterface.h"
#include "KFHttpServer/KFHttpServerInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFPlatformPlugin::Install()
    {
        __REGISTER_MODULE__( KFPlatform );
    }

    void KFPlatformPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFPlatform );
    }

    void KFPlatformPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_redis, KFRedisInterface );
        __FIND_MODULE__( _kf_timer, KFTimerInterface );
        __FIND_MODULE__( _kf_http_client, KFHttpClientInterface );
        __FIND_MODULE__( _kf_http_server, KFHttpServerInterface );
    }
}