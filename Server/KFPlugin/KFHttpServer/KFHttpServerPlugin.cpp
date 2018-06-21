#include "KFHttpServerPlugin.h"
#include "KFHttpServerModule.h"
#include "KFConfig/KFConfigInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
	void KFHttpServerPlugin::Install()
	{
		_kf_plugin_manage->RegistModule< KFHttpServerPlugin, KFHttpServerInterface >( new KFHttpServerModule() );
	}

	void KFHttpServerPlugin::UnInstall()
	{
		_kf_plugin_manage->UnRegistModule< KFHttpServerPlugin, KFHttpServerInterface >();
	}
	
	void KFHttpServerPlugin::LoadModule()
	{
		_kf_config = _kf_plugin_manage->FindModule< KFConfigInterface >();
	}
}