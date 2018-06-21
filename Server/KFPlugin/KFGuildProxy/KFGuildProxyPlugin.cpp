#include "KFGuildProxyPlugin.h"
#include "KFGuildProxyModule.h"

namespace KFrame
{
	void KFGuildProxyPlugin::Install()
	{
		_kf_plugin_manage->RegistModule< KFGuildProxyPlugin, KFGuildProxyInterface >( new KFGuildProxyModule() );
	}

	void KFGuildProxyPlugin::UnInstall()
	{
		_kf_plugin_manage->UnRegistModule< KFGuildProxyPlugin, KFGuildProxyInterface >();
	}

	void KFGuildProxyPlugin::LoadModule()
	{

	}

}