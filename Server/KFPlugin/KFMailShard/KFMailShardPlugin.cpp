#include "KFMailShardPlugin.h"
#include "KFMailShardModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFMailShardPlugin::Install ()
    {
        _kf_plugin_manage->RegistModule< KFMailShardPlugin, KFMailShardInterface > ( new KFMailShardModule () );
    }

    void KFMailShardPlugin::UnInstall ()
    {
        _kf_plugin_manage->UnRegistModule< KFMailShardPlugin, KFMailShardInterface > ();
    }

    void KFMailShardPlugin::LoadModule ()
    {
        _kf_redis = _kf_plugin_manage->FindModule< KFRedisInterface > ();
        _kf_option = _kf_plugin_manage->FindModule< KFOptionInterface >();
        _kf_worker = _kf_plugin_manage->FindModule< KFWorkerInterface >();
        _kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
        _kf_schedule = _kf_plugin_manage->FindModule< KFScheduleInterface >();
        _kf_cluster_shard = _kf_plugin_manage->FindModule< KFClusterShardInterface >();

    }
}