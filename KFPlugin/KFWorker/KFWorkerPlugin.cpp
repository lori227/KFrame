#include "KFWorkerPlugin.hpp"
#include "KFWorkerModule.hpp"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFWorkerPlugin::Install()
    {
        __REGISTER_MODULE__( KFWorker );
    }

    void KFWorkerPlugin::UnInstall()
    {
        __UN_MODULE__( KFWorker );
    }

    void KFWorkerPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_tcp_server, KFTcpServerInterface );
        __FIND_MODULE__( _kf_cluster_shard, KFClusterShardInterface );
    }
}