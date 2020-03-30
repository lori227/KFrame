#include "KFDirClientPlugin.hpp"
#include "KFDirClientModule.hpp"
#include "KFConfig/KFConfigInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFDirClientPlugin::Install()
    {
        __REGISTER_MODULE__( KFDirClient );
    }

    void KFDirClientPlugin::UnInstall()
    {
        __UN_MODULE__( KFDirClient );
    }

    void KFDirClientPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_ip_address, KFIpAddressInterface );
        __FIND_MODULE__( _kf_http_client, KFHttpClientInterface );
    }

    void KFDirClientPlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFZoneConfig );
    }
}