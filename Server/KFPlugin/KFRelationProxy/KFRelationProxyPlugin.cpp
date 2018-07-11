#include "KFRelationProxyPlugin.h"
#include "KFRelationProxyModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFRelationProxyPlugin::Install()
    {
        __REGISTER_MODULE__( KFRelationProxy );
    }

    void KFRelationProxyPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFRelationProxy );
    }

    void KFRelationProxyPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_tcp_client, KFTcpClientInterface );
        __FIND_MODULE__( _kf_cluster_proxy, KFClusterProxyInterface );
    }
}