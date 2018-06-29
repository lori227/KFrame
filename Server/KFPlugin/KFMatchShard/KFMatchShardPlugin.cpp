#include "KFMatchShardPlugin.h"
#include "KFMatchShardModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
    void KFMatchShardPlugin::Install()
    {
        _kf_plugin_manage->RegistModule< KFMatchShardPlugin, KFMatchShardInterface >( new KFMatchShardModule() );
    }

    void KFMatchShardPlugin::UnInstall()
    {
        _kf_plugin_manage->UnRegistModule< KFMatchShardPlugin, KFMatchShardInterface >();
    }

    void KFMatchShardPlugin::LoadModule()
    {
        _kf_config = _kf_plugin_manage->FindModule< KFConfigInterface >();
        _kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
        _kf_tcp_server = _kf_plugin_manage->FindModule< KFTcpServerInterface >();
        _kf_cluster_shard = _kf_plugin_manage->FindModule< KFClusterShardInterface >();
        _kf_cluster = _kf_plugin_manage->FindModule< KFClusterClientInterface >();
    }
}