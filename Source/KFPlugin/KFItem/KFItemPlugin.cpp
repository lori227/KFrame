#include "KFItemPlugin.h"
#include "KFItemModule.h"

//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
	void KFItemPlugin::Install()
	{
		_kf_plugin_manage->RegistModule< KFItemPlugin, KFItemInterface >( new KFItemModule() );
	}

	void KFItemPlugin::UnInstall()
	{
		_kf_plugin_manage->UnRegistModule< KFItemPlugin, KFItemInterface >();
	}
	
	void KFItemPlugin::LoadModule()
	{
		_kf_lua = _kf_plugin_manage->FindModule< KFLuaInterface >();
		_kf_config = _kf_plugin_manage->FindModule< KFConfigInterface >();
		_kf_timer = _kf_plugin_manage->FindModule< KFTimerInterface >();
		_kf_kernel = _kf_plugin_manage->FindModule< KFKernelInterface >();
		_kf_player = _kf_plugin_manage->FindModule< KFPlayerInterface >();
		_kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
		_kf_display = _kf_plugin_manage->FindModule< KFDisplayInterface >();
	}
}