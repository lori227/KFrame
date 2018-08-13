#include "KFMailProxyPlugin.h"
#include "KFMailProxyModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFMailProxyPlugin::Install()
    {
        __REGISTER_MODULE__( KFMailProxy );
    }

    void KFMailProxyPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFMailProxy );
    }

    void KFMailProxyPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_tcp_client, KFTcpClientInterface );
        __FIND_MODULE__( _kf_cluster_proxy, KFClusterProxyInterface );
        __FIND_MODULE__( _kf_log_client, KFLogClientInterface );
    }
}