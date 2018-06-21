#include "KFAchievePlugin.h"
#include "KFAchieveModule.h"

//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
	void KFAchievePlugin::Install()
	{
		_kf_plugin_manage->RegistModule< KFAchievePlugin, KFAchieveInterface >( new KFAchieveModule() );
	}

	void KFAchievePlugin::UnInstall()
	{
		_kf_plugin_manage->UnRegistModule< KFAchievePlugin, KFAchieveInterface >();
	}	

	void KFAchievePlugin::LoadModule()
	{
		_kf_config = _kf_plugin_manage->FindModule< KFConfigInterface >();
		_kf_kernel = _kf_plugin_manage->FindModule< KFKernelInterface >();
		_kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
		_kf_player = _kf_plugin_manage->FindModule< KFPlayerInterface >();
		_kf_display = _kf_plugin_manage->FindModule< KFDisplayInterface >();
	}
}