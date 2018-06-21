#include "KFPublicProxyPlugin.h"
#include "KFPublicProxyModule.h"

namespace KFrame
{
	void KFPublicProxyPlugin::Install()
	{
		_kf_plugin_manage->RegistModule< KFPublicProxyPlugin, KFPublicProxyInterface >( new KFPublicProxyModule() );
	}

	void KFPublicProxyPlugin::UnInstall()
	{
		_kf_plugin_manage->UnRegistModule< KFPublicProxyPlugin, KFPublicProxyInterface >();
	}

	void KFPublicProxyPlugin::LoadModule()
	{

	}

}