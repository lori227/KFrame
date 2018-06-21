#include "KFMySQLPlugin.h"
#include "KFMySQLModule.h"

//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
	void KFMySQLPlugin::Install()
	{
		_kf_plugin_manage->RegistModule< KFMySQLPlugin, KFMySQLInterface >( new KFMySQLModule() );
	}

	void KFMySQLPlugin::UnInstall()
	{
		_kf_plugin_manage->UnRegistModule< KFMySQLPlugin, KFMySQLInterface >();
	}

	void KFMySQLPlugin::LoadModule()
	{
		_kf_config = _kf_plugin_manage->FindModule< KFConfigInterface >();
	}
}