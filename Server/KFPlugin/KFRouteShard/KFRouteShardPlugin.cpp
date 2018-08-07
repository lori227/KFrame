#include "KFRouteShardPlugin.h"
#include "KFRouteShardModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFRouteShardPlugin::Install()
    {
        __REGISTER_MODULE__( KFRouteShard );
    }

    void KFRouteShardPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFRouteShard );
    }

    void KFRouteShardPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_tcp_server, KFTcpServerInterface );
        __FIND_MODULE__( _kf_tcp_client, KFTcpClientInterface );
        __FIND_MODULE__( _kf_log_client, KFLogClientInterface );
    }
}