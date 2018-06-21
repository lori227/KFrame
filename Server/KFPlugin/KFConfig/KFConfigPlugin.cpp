#include "KFConfigPlugin.h"
#include "KFConfigModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
	void KFConfigPlugin::Install()
	{
		_kf_plugin_manage->RegistModule< KFConfigPlugin, KFConfigInterface >( new KFConfigModule() );
	}

	void KFConfigPlugin::UnInstall()
	{
		_kf_plugin_manage->UnRegistModule< KFConfigPlugin, KFConfigInterface >();
	}

	void KFConfigPlugin::LoadModule()
	{

	}
}