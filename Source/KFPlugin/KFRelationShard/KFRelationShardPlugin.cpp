#include "KFRelationShardPlugin.h"
#include "KFRelationShardModule.h"

//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
	void KFRelationShardPlugin::Install()
	{
		_kf_plugin_manage->RegistModule< KFRelationShardPlugin, KFRelationShardInterface >( new KFRelationShardModule() );
	}

	void KFRelationShardPlugin::UnInstall()
	{
		_kf_plugin_manage->UnRegistModule< KFRelationShardPlugin, KFRelationShardInterface >();
	}
	
	void KFRelationShardPlugin::LoadModule()
	{
		_kf_redis = _kf_plugin_manage->FindModule< KFRedisInterface >();
		_kf_config = _kf_plugin_manage->FindModule< KFConfigInterface >();
		_kf_worker = _kf_plugin_manage->FindModule< KFWorkerInterface >();
		_kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
		_kf_tcp_server = _kf_plugin_manage->FindModule< KFTcpServerInterface >();
		_kf_cluster_shard = _kf_plugin_manage->FindModule< KFClusterShardInterface >();
		_kf_schedule = _kf_plugin_manage->FindModule< KFScheduleInterface >();
		_kf_display = _kf_plugin_manage->FindModule< KFDisplayInterface >();
	}
}