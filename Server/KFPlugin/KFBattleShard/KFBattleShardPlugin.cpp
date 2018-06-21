#include "KFBattleShardPlugin.h"
#include "KFBattleShardModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
	void KFBattleShardPlugin::Install()
	{
		_kf_plugin_manage->RegistModule< KFBattleShardPlugin, KFBattleShardInterface >( new KFBattleShardModule() );
	}

	void KFBattleShardPlugin::UnInstall()
	{
		_kf_plugin_manage->UnRegistModule< KFBattleShardPlugin, KFBattleShardInterface >();
	}
	
	void KFBattleShardPlugin::LoadModule()
	{
		_kf_config = _kf_plugin_manage->FindModule< KFConfigInterface >();
		_kf_redis = _kf_plugin_manage->FindModule< KFRedisInterface >();
		_kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
		_kf_tcp_server = _kf_plugin_manage->FindModule< KFTcpServerInterface >();
		_kf_cluster_shard = _kf_plugin_manage->FindModule< KFClusterShardInterface >();
		_kf_cluster = _kf_plugin_manage->FindModule< KFClusterClientInterface >();
	}
}