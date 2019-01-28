#include "KFIpAddressPlugin.hpp"
#include "KFIpAddressModule.hpp"

namespace KFrame
{
    void KFIpAddressPlugin::Install()
    {
        __REGISTER_MODULE__( KFIpAddress );
    }

    void KFIpAddressPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFIpAddress );
    }

    void KFIpAddressPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_http_client, KFHttpClientInterface );
    }

}