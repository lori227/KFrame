#include "KFRouteProxyPlugin.h"
#include "KFRouteProxyModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFRouteProxyPlugin::Install()
    {
        __REGISTER_MODULE__( KFRouteProxy );
    }

    void KFRouteProxyPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFRouteProxy );
    }

    void KFRouteProxyPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_tcp_server, KFTcpServerInterface );
        __FIND_MODULE__( _kf_tcp_client, KFTcpClientInterface );
    }
}