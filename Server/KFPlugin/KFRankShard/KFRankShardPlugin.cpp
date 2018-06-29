#include "KFRankShardPlugin.h"
#include "KFRankShardModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFRankShardPlugin::Install()
    {
        _kf_plugin_manage->RegistModule< KFRankShardPlugin, KFRankShardInterface >( new KFRankShardModule() );
    }

    void KFRankShardPlugin::UnInstall()
    {
        _kf_plugin_manage->UnRegistModule< KFRankShardPlugin, KFRankShardInterface >();
    }

    void KFRankShardPlugin::LoadModule()
    {
        _kf_redis = _kf_plugin_manage->FindModule< KFRedisInterface >();
        _kf_worker = _kf_plugin_manage->FindModule< KFWorkerInterface >();
        _kf_option = _kf_plugin_manage->FindModule< KFOptionInterface >();
        _kf_config = _kf_plugin_manage->FindModule< KFConfigInterface >();
        _kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
        _kf_schedule = _kf_plugin_manage->FindModule< KFScheduleInterface >();
        _kf_tcp_server = _kf_plugin_manage->FindModule< KFTcpServerInterface >();
        _kf_cluster_shard = _kf_plugin_manage->FindModule< KFClusterShardInterface >();
    }
}