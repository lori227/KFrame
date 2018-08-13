#include "KFBattleProxyPlugin.h"
#include "KFBattleProxyModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFBattleProxyPlugin::Install()
    {
        __REGISTER_MODULE__( KFBattleProxy );
    }

    void KFBattleProxyPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFBattleProxy );
    }

    void KFBattleProxyPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_tcp_server, KFTcpServerInterface );
        __FIND_MODULE__( _kf_tcp_client, KFTcpClientInterface );
        __FIND_MODULE__( _kf_cluster_proxy, KFClusterProxyInterface );
        __FIND_MODULE__( _kf_log_client, KFLogClientInterface );
    }
}