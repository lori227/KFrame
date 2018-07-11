#include "KFDataProxyPlugin.h"
#include "KFDataProxyModule.h"

namespace KFrame
{
    void KFDataProxyPlugin::Install()
    {
        __REGISTER_MODULE__( KFDataProxy );
    }

    void KFDataProxyPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFDataProxy );
    }

    void KFDataProxyPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_tcp_client, KFTcpClientInterface );
        __FIND_MODULE__( _kf_cluster_proxy, KFClusterProxyInterface );
    }

}