#include "KFClothesPlugin.h"
#include "KFClothesModule.h"

//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
	void KFClothesPlugin::Install()
	{
		_kf_plugin_manage->RegistModule< KFClothesPlugin, KFClothesInterface >( new KFClothesModule() );
	}

	void KFClothesPlugin::UnInstall()
	{
		_kf_plugin_manage->UnRegistModule< KFClothesPlugin, KFClothesInterface >();
	}
	
	void KFClothesPlugin::LoadModule()
	{
		_kf_config = _kf_plugin_manage->FindModule< KFConfigInterface >();
		_kf_timer = _kf_plugin_manage->FindModule< KFTimerInterface >();
		_kf_kernel = _kf_plugin_manage->FindModule< KFKernelInterface >();
		_kf_player = _kf_plugin_manage->FindModule< KFPlayerInterface >();
		_kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
		_kf_mail = _kf_plugin_manage->FindModule< KFMailClientInterface >();
		_kf_display = _kf_plugin_manage->FindModule< KFDisplayInterface >();
	}
}