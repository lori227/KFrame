#include "KFDataProxyPlugin.h"
#include "KFDataProxyModule.h"

namespace KFrame
{
	void KFDataProxyPlugin::Install()
	{
		_kf_plugin_manage->RegistModule< KFDataProxyPlugin, KFDataProxyInterface >( new KFDataProxyModule() );
	}

	void KFDataProxyPlugin::UnInstall()
	{
		_kf_plugin_manage->UnRegistModule< KFDataProxyPlugin, KFDataProxyInterface >();
	}

	void KFDataProxyPlugin::LoadModule()
	{
		_kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
		_kf_tcp_client = _kf_plugin_manage->FindModule< KFTcpClientInterface >();
		_kf_cluster_proxy = _kf_plugin_manage->FindModule< KFClusterProxyInterface >();
	}

}