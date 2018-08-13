#include "KFMatchProxyPlugin.h"
#include "KFMatchProxyModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFMatchProxyPlugin::Install()
    {
        __REGISTER_MODULE__( KFMatchProxy );
    }

    void KFMatchProxyPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFMatchProxy );
    }

    void KFMatchProxyPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_tcp_client, KFTcpClientInterface );
        __FIND_MODULE__( _kf_cluster_proxy, KFClusterProxyInterface );
        __FIND_MODULE__( _kf_log_client, KFLogClientInterface );
    }
}