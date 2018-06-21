#include "KFMatchProxyPlugin.h"
#include "KFMatchProxyModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
	void KFMatchProxyPlugin::Install()
	{
		_kf_plugin_manage->RegistModule< KFMatchProxyPlugin, KFMatchProxyInterface >( new KFMatchProxyModule() );
	}

	void KFMatchProxyPlugin::UnInstall()
	{
		_kf_plugin_manage->UnRegistModule< KFMatchProxyPlugin, KFMatchProxyInterface >();
	}
	
	void KFMatchProxyPlugin::LoadModule()
	{
		_kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
		_kf_tcp_client = _kf_plugin_manage->FindModule< KFTcpClientInterface >();
		_kf_cluster_proxy = _kf_plugin_manage->FindModule< KFClusterProxyInterface >();
	}
}