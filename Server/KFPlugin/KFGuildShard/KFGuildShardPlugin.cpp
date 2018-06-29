#include "KFGuildShardPlugin.h"
#include "KFGuildShardModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFGuildShardPlugin::Install()
    {
        _kf_plugin_manage->RegistModule< KFGuildShardPlugin, KFGuildShardInterface >( new KFGuildShardModule() );
    }

    void KFGuildShardPlugin::UnInstall()
    {
        _kf_plugin_manage->UnRegistModule< KFGuildShardPlugin, KFGuildShardInterface >();
    }

    void KFGuildShardPlugin::LoadModule()
    {
        _kf_redis = _kf_plugin_manage->FindModule< KFRedisInterface >();
        _kf_worker = _kf_plugin_manage->FindModule< KFWorkerInterface >();
        _kf_option = _kf_plugin_manage->FindModule< KFOptionInterface >();
        _kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
        _kf_schedule = _kf_plugin_manage->FindModule< KFScheduleInterface >();
        _kf_cluster_shard = _kf_plugin_manage->FindModule< KFClusterShardInterface >();
    }
}