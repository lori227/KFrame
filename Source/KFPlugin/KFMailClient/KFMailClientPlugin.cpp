#include "KFMailClientPlugin.h"
#include "KFMailClientModule.h"

namespace KFrame
{
	void KFMailClientPlugin::Install()
	{
		_kf_plugin_manage->RegistModule< KFMailClientPlugin, KFMailClientInterface >( new KFMailClientModule() );
	}

	void KFMailClientPlugin::UnInstall()
	{
		_kf_plugin_manage->UnRegistModule< KFMailClientPlugin, KFMailClientInterface >();
	}

	void KFMailClientPlugin::LoadModule()
	{
		_kf_timer = _kf_plugin_manage->FindModule< KFTimerInterface >();
		_kf_config = _kf_plugin_manage->FindModule< KFConfigInterface >();
		_kf_player = _kf_plugin_manage->FindModule< KFPlayerInterface >();
		_kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
		_kf_kernel = _kf_plugin_manage->FindModule< KFKernelInterface >();
		_kf_cluster = _kf_plugin_manage->FindModule< KFClusterClientInterface >();
		_kf_route = _kf_plugin_manage->FindModule< KFRouteClientInterface >();
		_kf_option = _kf_plugin_manage->FindModule< KFOptionInterface >();
		_kf_display = _kf_plugin_manage->FindModule< KFDisplayInterface >();
	}

}