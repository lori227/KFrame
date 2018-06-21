#include "KFGroupProxyPlugin.h"
#include "KFGroupProxyModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
	void KFGroupProxyPlugin::Install()
	{
		_kf_plugin_manage->RegistModule< KFGroupProxyPlugin, KFGroupProxyInterface >( new KFGroupProxyModule() );
	}

	void KFGroupProxyPlugin::UnInstall()
	{
		_kf_plugin_manage->UnRegistModule< KFGroupProxyPlugin, KFGroupProxyInterface >();
	}
	
	void KFGroupProxyPlugin::LoadModule()
	{
		_kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
		_kf_cluster_proxy = _kf_plugin_manage->FindModule< KFClusterProxyInterface >();

	}
}