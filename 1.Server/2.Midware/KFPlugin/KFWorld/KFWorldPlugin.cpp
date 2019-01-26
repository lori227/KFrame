#include "KFWorldPlugin.h"
#include "KFWorldModule.h"

namespace KFrame
{
    void KFWorldPlugin::Install()
    {
        __REGISTER_MODULE__( KFWorld );
    }

    void KFWorldPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFWorld );
    }

    void KFWorldPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_zone, KFZoneInterface );
        __FIND_MODULE__( _kf_timer, KFTimerInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_ip_address, KFIpAddressInterface );
        __FIND_MODULE__( _kf_tcp_server, KFTcpServerInterface );
        __FIND_MODULE__( _kf_tcp_client, KFTcpClientInterface );
        __FIND_MODULE__( _kf_http_server, KFHttpServerInterface );
        __FIND_MODULE__( _kf_http_client, KFHttpClientInterface );
        __FIND_MODULE__( _kf_command, KFDeployCommandInterface );
    }

}