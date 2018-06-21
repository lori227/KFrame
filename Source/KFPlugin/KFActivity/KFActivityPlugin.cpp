#include "KFActivityPlugin.h"
#include "KFActivityModule.h"

//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
	void KFActivityPlugin::Install()
	{
		_kf_plugin_manage->RegistModule< KFActivityPlugin, KFActivityInterface >( new KFActivityModule() );
	}

	void KFActivityPlugin::UnInstall()
	{
		_kf_plugin_manage->UnRegistModule< KFActivityPlugin, KFActivityInterface >();
	}

	void KFActivityPlugin::LoadModule()
	{
		_kf_config = _kf_plugin_manage->FindModule< KFConfigInterface >();
		_kf_kernel = _kf_plugin_manage->FindModule< KFKernelInterface >();
		_kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
		_kf_player = _kf_plugin_manage->FindModule< KFPlayerInterface >();
		_kf_display = _kf_plugin_manage->FindModule< KFDisplayInterface >();
	}
}