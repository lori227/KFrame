#include "KFLoginProxyPlugin.h"
#include "KFLoginProxyModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFLoginProxyPlugin::Install()
    {
        __REGISTER_MODULE__( KFLoginProxy );
    }

    void KFLoginProxyPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFLoginProxy );
    }

    void KFLoginProxyPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_gate, KFGateInterface );
        __FIND_MODULE__( _kf_proxy, KFProxyInterface );
        __FIND_MODULE__( _kf_timer, KFTimerInterface );
        __FIND_MODULE__( _kf_config, KFConfigInterface );
        __FIND_MODULE__( _kf_display, KFDisplayInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_tcp_client, KFTcpClientInterface );
        __FIND_MODULE__( _kf_tcp_server, KFTcpServerInterface );
        __FIND_MODULE__( _kf_log_client, KFLogClientInterface );
    }
}