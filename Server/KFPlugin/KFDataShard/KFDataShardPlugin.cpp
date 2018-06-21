#include "KFDataShardPlugin.h"
#include "KFDataShardModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
	void KFDataShardPlugin::Install()
	{
		_kf_plugin_manage->RegistModule< KFDataShardPlugin, KFDataShardInterface >( new KFDataShardModule() );
	}

	void KFDataShardPlugin::UnInstall()
	{
		_kf_plugin_manage->UnRegistModule< KFDataShardPlugin, KFDataShardInterface >();
	}
	
	void KFDataShardPlugin::LoadModule()
	{
		_kf_timer = _kf_plugin_manage->FindModule< KFTimerInterface >();
		_kf_redis = _kf_plugin_manage->FindModule< KFRedisInterface >();
		_kf_worker = _kf_plugin_manage->FindModule< KFWorkerInterface >();
		_kf_config = _kf_plugin_manage->FindModule< KFConfigInterface >();
		_kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
		_kf_tcp_server = _kf_plugin_manage->FindModule< KFTcpServerInterface >();
		_kf_cluster_shard = _kf_plugin_manage->FindModule< KFClusterShardInterface >();
	}
}