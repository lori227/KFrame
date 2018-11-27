#include "KFGatePlugin.h"
#include "KFGateModule.h"

namespace KFrame
{
    void KFGatePlugin::Install()
    {
        __REGISTER_MODULE__( KFGate );
    }

    void KFGatePlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFGate );
    }

    void KFGatePlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_timer, KFTimerInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_ip_address, KFIpAddressInterface );
        __FIND_MODULE__( _kf_http_client, KFHttpClientInterface );
        __FIND_MODULE__( _kf_tcp_server, KFTcpServerInterface );
        __FIND_MODULE__( _kf_tcp_client, KFTcpClientInterface );
        __FIND_MODULE__( _kf_command, KFDeployCommandInterface );
    }

}