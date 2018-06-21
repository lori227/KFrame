#include "KFKernelPlugin.h"
#include "KFKernelModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
	void KFKernelPlugin::Install()
	{
		_kf_plugin_manage->RegistModule< KFKernelPlugin, KFKernelInterface >( new KFKernelModule() );
	}

	void KFKernelPlugin::UnInstall()
	{
		_kf_plugin_manage->UnRegistModule< KFKernelPlugin, KFKernelInterface >();
	}

	void KFKernelPlugin::LoadModule()
	{
		_kf_config = _kf_plugin_manage->FindModule<KFConfigInterface>();
		_kf_timer = _kf_plugin_manage->FindModule< KFTimerInterface >();
	}
}