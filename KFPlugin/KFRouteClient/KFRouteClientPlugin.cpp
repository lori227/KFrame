#include "KFRouteClientPlugin.hpp"
#include "KFRouteClientModule.hpp"
#include "KFConfig/KFConfigInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFRouteClientPlugin::Install()
    {
        __REGISTER_MODULE__( KFRouteClient );
    }

    void KFRouteClientPlugin::UnInstall()
    {
        __UN_MODULE__( KFRouteClient );
    }

    void KFRouteClientPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_timer, KFTimerInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_ip_address, KFIpAddressInterface );
        __FIND_MODULE__( _kf_tcp_client, KFTcpClientInterface );
        __FIND_MODULE__( _kf_cluster_client, KFClusterClientInterface );
    }

    void KFRouteClientPlugin::AddConfig()
    {
        __KF_ADD_CONFIG__( KFRouteConfig );
    }
}