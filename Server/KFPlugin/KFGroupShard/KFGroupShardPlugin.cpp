#include "KFGroupShardPlugin.h"
#include "KFGroupShardModule.h"

//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
	void KFGroupShardPlugin::Install()
	{
		_kf_plugin_manage->RegistModule< KFGroupShardPlugin, KFGroupShardInterface >( new KFGroupShardModule() );
	}

	void KFGroupShardPlugin::UnInstall()
	{
		_kf_plugin_manage->UnRegistModule< KFGroupShardPlugin, KFGroupShardInterface >();
	}
	
	void KFGroupShardPlugin::LoadModule()
	{
		_kf_config = _kf_plugin_manage->FindModule< KFConfigInterface >();
		_kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
		_kf_kernel = _kf_plugin_manage->FindModule< KFKernelInterface >();
		_kf_display = _kf_plugin_manage->FindModule< KFDisplayInterface >();
		_kf_tcp_server = _kf_plugin_manage->FindModule< KFTcpServerInterface >();
		_kf_cluster_shard = _kf_plugin_manage->FindModule< KFClusterShardInterface >();
	}
}