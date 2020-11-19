#include "KFIpAddressPlugin.hpp"
#include "KFIpAddressModule.hpp"
#include "KFLoader/KFLoaderInterface.h"

namespace KFrame
{
    void KFIpAddressPlugin::Install()
    {
        __REGISTER_MODULE__( KFIpAddress );
    }

    void KFIpAddressPlugin::UnInstall()
    {
        __UN_MODULE__( KFIpAddress );
    }

    void KFIpAddressPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_timer, KFTimerInterface );
        __FIND_MODULE__( _kf_loader, KFLoaderInterface );
        __FIND_MODULE__( _kf_http_client, KFHttpClientInterface );
    }

    void KFIpAddressPlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFIpConfig );
    }
}