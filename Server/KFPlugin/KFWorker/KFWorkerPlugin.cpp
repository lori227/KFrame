#include "KFWorkerPlugin.h"
#include "KFWorkerModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFWorkerPlugin::Install()
    {
        __REGISTER_MODULE__( KFWorker );
        _kf_plugin_manage->RegistModule< KFWorkerPlugin, KFWorkerInterface >( new KFWorkerModule() );
    }

    void KFWorkerPlugin::UnInstall()
    {
        __UNREGISTER_MODULE__( KFWorker );
    }

    void KFWorkerPlugin::LoadModule()
    {
        __FIND_MODULE__( _kf_tcp_server, KFTcpServerInterface );
        __FIND_MODULE__( _kf_cluster_shard, KFClusterShardInterface );
    }
}