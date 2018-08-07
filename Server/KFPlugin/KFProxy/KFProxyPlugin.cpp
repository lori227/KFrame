#include "KFProxyPlugin.h"
#include "KFProxyModule.h"

namespace KFrame
{
    void KFProxyPlugin::Install()
    {
        __REGISTER_MODULE__( KFProxy );
    }

    void KFProxyPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFProxy );
    }

    void KFProxyPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_gate, KFGateInterface );
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_tcp_client, KFTcpClientInterface );
        __FIND_MODULE__( _kf_tcp_server, KFTcpServerInterface );
        __FIND_MODULE__( _kf_log_client, KFLogClientInterface );
    }

}