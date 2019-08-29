#include "KFChannelPlugin.hpp"
#include "KFChannelModule.hpp"
#include "KFConfig/KFConfigInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFChannelPlugin::Install()
    {
        __REGISTER_MODULE__( KFChannel );
    }

    void KFChannelPlugin::UnInstall()
    {
        __UN_MODULE__( KFChannel );
    }

    void KFChannelPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_redis, KFRedisInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_http_client, KFHttpClientInterface );
        __FIND_MODULE__( _kf_http_server, KFHttpServerInterface );
    }

    void KFChannelPlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFChannelConfig );
    }

}