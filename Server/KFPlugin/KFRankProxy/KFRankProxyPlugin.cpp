#include "KFRankProxyPlugin.h"
#include "KFRankProxyModule.h"

namespace KFrame
{
	void KFRankProxyPlugin::Install()
	{
		_kf_plugin_manage->RegistModule< KFRankProxyPlugin, KFRankProxyInterface >( new KFRankProxyModule() );
	}

	void KFRankProxyPlugin::UnInstall()
	{
		_kf_plugin_manage->UnRegistModule< KFRankProxyPlugin, KFRankProxyInterface >();
	}

	void KFRankProxyPlugin::LoadModule()
	{

	}

}