#include "KFGroupClientPlugin.h"
#include "KFGroupClientModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
	void KFGroupClientPlugin::Install()
	{
		_kf_plugin_manage->RegistModule< KFGroupClientPlugin, KFGroupClientInterface >( new KFGroupClientModule() );
	}

	void KFGroupClientPlugin::UnInstall()
	{
		_kf_plugin_manage->UnRegistModule< KFGroupClientPlugin, KFGroupClientInterface >();
	}
	
	void KFGroupClientPlugin::LoadModule()
	{
		_kf_player = _kf_plugin_manage->FindModule< KFPlayerInterface >();
		_kf_config = _kf_plugin_manage->FindModule< KFConfigInterface >();
		_kf_kernel = _kf_plugin_manage->FindModule< KFKernelInterface >();
		_kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
		_kf_display = _kf_plugin_manage->FindModule< KFDisplayInterface >();
		_kf_route = _kf_plugin_manage->FindModule< KFRouteClientInterface >();
		_kf_match = _kf_plugin_manage->FindModule< KFMatchClientInterface >();
		_kf_cluster = _kf_plugin_manage->FindModule< KFClusterClientInterface >();
	}
}