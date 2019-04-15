#include "KFClusterShardPlugin.hpp"
#include "KFClusterShardModule.hpp"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFClusterShardPlugin::Install()
    {
        __REGISTER_MODULE__( KFClusterShard );
    }

    void KFClusterShardPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFClusterShard );
    }

    void KFClusterShardPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_message, KFMessageInterface );
        __FIND_MODULE__( _kf_tcp_client, KFTcpClientInterface );
        __FIND_MODULE__( _kf_tcp_server, KFTcpServerInterface );
    }
}