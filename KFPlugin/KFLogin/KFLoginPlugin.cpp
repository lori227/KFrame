#include "KFLoginPlugin.hpp"
#include "KFLoginModule.hpp"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    void KFLoginPlugin::Install()
    {
        __REGISTER_MODULE__( KFLogin );
    }

    void KFLoginPlugin::UnInstall()
    {
        __UN_MODULE__( KFLogin );
    }

    void KFLoginPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_tcp_server, KFTcpServerInterface );
        __FIND_MODULE__( _kf_tcp_client, KFTcpClientInterface );
        __FIND_MODULE__( _kf_ip_address, KFIpAddressInterface );
        __FIND_MODULE__( _kf_http_client, KFHttpClientInterface );
    }

    void KFLoginPlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFZoneConfig );
    }
}

