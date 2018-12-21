#include "KFClusterClientPlugin.h"
#include "KFClusterClientModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFClusterClientPlugin::Install()
    {
        __REGISTER_MODULE__( KFClusterClient );
    }

    void KFClusterClientPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFClusterClient );
    }

    void KFClusterClientPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_bus, KFBusInterface );
        __FIND_MODULE__( _kf_timer, KFTimerInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_option, KFOptionInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_tcp_client, KFTcpClientInterface );
        __FIND_MODULE__( _kf_ip_address, KFIpAddressInterface );
    }
}