#include "KFRelationProxyPlugin.h"
#include "KFRelationProxyModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
	void KFRelationProxyPlugin::Install()
	{
		_kf_plugin_manage->RegistModule< KFRelationProxyPlugin, KFRelationProxyInterface >( new KFRelationProxyModule() );
	}

	void KFRelationProxyPlugin::UnInstall()
	{
		_kf_plugin_manage->UnRegistModule< KFRelationProxyPlugin, KFRelationProxyInterface >();
	}
	
	void KFRelationProxyPlugin::LoadModule()
	{
		_kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
		_kf_tcp_client = _kf_plugin_manage->FindModule< KFTcpClientInterface >();
		_kf_cluster_proxy = _kf_plugin_manage->FindModule< KFClusterProxyInterface >();
	}
}