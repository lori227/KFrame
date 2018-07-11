#include "KFRankProxyPlugin.h"
#include "KFRankProxyModule.h"

namespace KFrame
{
    void KFRankProxyPlugin::Install()
    {
        __REGISTER_MODULE__( KFRankProxy );
    }

    void KFRankProxyPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFRankProxy );
    }

    void KFRankProxyPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_tcp_client, KFTcpClientInterface );
        __FIND_MODULE__( _kf_cluster_proxy, KFClusterProxyInterface );
    }

}