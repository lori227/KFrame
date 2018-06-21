#include "KFClusterShardPlugin.h"
#include "KFClusterShardModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
	void KFClusterShardPlugin::Install()
	{
		_kf_plugin_manage->RegistModule< KFClusterShardPlugin, KFClusterShardInterface >( new KFClusterShardModule() );
	}

	void KFClusterShardPlugin::UnInstall()
	{
		_kf_plugin_manage->UnRegistModule< KFClusterShardPlugin, KFClusterShardInterface >();
	}
	
	void KFClusterShardPlugin::LoadModule()
	{
		_kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
		_kf_tcp_client = _kf_plugin_manage->FindModule< KFTcpClientInterface >();
		_kf_tcp_server = _kf_plugin_manage->FindModule< KFTcpServerInterface >();
	}
}