#include "KFWorkerPlugin.h"
#include "KFWorkerModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFWorkerPlugin::Install()
    {
        _kf_plugin_manage->RegistModule< KFWorkerPlugin, KFWorkerInterface >( new KFWorkerModule() );
    }

    void KFWorkerPlugin::UnInstall()
    {
        _kf_plugin_manage->UnRegistModule< KFWorkerPlugin, KFWorkerInterface >();
    }

    void KFWorkerPlugin::LoadModule()
    {
        _kf_redis = _kf_plugin_manage->FindModule< KFRedisInterface >();
        _kf_tcp_server = _kf_plugin_manage->FindModule< KFTcpServerInterface >();
        _kf_cluster_shard = _kf_plugin_manage->FindModule< KFClusterShardInterface >();
    }
}