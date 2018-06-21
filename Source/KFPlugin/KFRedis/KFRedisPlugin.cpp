#include "KFRedisPlugin.h"
#include "KFRedisModule.h"
#include "KFConfig/KFConfigInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
	void KFRedisPlugin::Install()
	{
		_kf_plugin_manage->RegistModule< KFRedisPlugin, KFRedisInterface >( new KFRedisModule() );
	}

	void KFRedisPlugin::UnInstall()
	{
		_kf_plugin_manage->UnRegistModule< KFRedisPlugin, KFRedisInterface >();
	}

	void KFRedisPlugin::LoadModule()
	{
		_kf_config = _kf_plugin_manage->FindModule< KFConfigInterface >();
	}
}