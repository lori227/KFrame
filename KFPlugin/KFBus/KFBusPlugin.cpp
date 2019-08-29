#include "KFBusPlugin.hpp"
#include "KFBusModule.hpp"
#include "KFConfig/KFConfigInterface.h"

namespace KFrame
{
    void KFBusPlugin::Install()
    {
        __REGISTER_MODULE__( KFBus );
    }

    void KFBusPlugin::UnInstall()
    {
        __UN_MODULE__( KFBus );
    }

    void KFBusPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_ip_address, KFIpAddressInterface );
        __FIND_MODULE__( _kf_tcp_client, KFTcpClientInterface );
    }

    void KFBusPlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFBusConfig );
    }
}