#include "KFClusterClientPlugin.hpp"
#include "KFClusterClientModule.hpp"
#include "KFConfig/KFConfigInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFClusterClientPlugin::Install()
    {
        __REGISTER_MODULE__( KFClusterClient );
    }

    void KFClusterClientPlugin::UnInstall()
    {
        __UN_MODULE__( KFClusterClient );
    }

    void KFClusterClientPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_timer, KFTimerInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_tcp_client, KFTcpClientInterface );
        __FIND_MODULE__( _kf_ip_address, KFIpAddressInterface );
    }

    void KFClusterClientPlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFRouteConfig );
    }
}