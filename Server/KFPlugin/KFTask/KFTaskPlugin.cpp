#include "KFTaskPlugin.h"
#include "KFTaskModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
	void KFTaskPlugin::Install()
	{
		_kf_plugin_manage->RegistModule< KFTaskPlugin, KFTaskInterface >( new KFTaskModule() );
	}

	void KFTaskPlugin::UnInstall()
	{
		_kf_plugin_manage->UnRegistModule< KFTaskPlugin, KFTaskInterface >();
	}

	void KFTaskPlugin::LoadModule()
	{
		_kf_config = _kf_plugin_manage->FindModule< KFConfigInterface >();
		_kf_kernel = _kf_plugin_manage->FindModule< KFKernelInterface >();
		_kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
		_kf_player = _kf_plugin_manage->FindModule< KFPlayerInterface >();
		_kf_display = _kf_plugin_manage->FindModule< KFDisplayInterface >();
	}

}