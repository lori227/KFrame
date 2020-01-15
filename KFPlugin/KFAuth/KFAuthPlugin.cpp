#include "KFAuthPlugin.hpp"
#include "KFAuthModule.hpp"
#include "KFConfig/KFConfigInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFAuthPlugin::Install()
    {
        __REGISTER_MODULE__( KFAuth );
    }

    void KFAuthPlugin::UnInstall()
    {
        __UN_MODULE__( KFAuth );
    }

    void KFAuthPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_redis, KFRedisInterface );
        __FIND_MODULE__( _kf_channel, KFChannelInterface );
        __FIND_MODULE__( _kf_ip_address, KFIpAddressInterface );
        __FIND_MODULE__( _kf_http_client, KFHttpClientInterface );
        __FIND_MODULE__( _kf_http_server, KFHttpServerInterface );
    }

    void KFAuthPlugin::AddConfig()
    {
    }
}