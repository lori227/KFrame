#include "KFPlayerPlugin.h"
#include "KFPlayerModule.h"

namespace KFrame
{
	void KFPlayerPlugin::Install()
	{
		_kf_plugin_manage->RegistModule< KFPlayerPlugin, KFPlayerInterface >( new KFPlayerModule() );
	}

	void KFPlayerPlugin::UnInstall()
	{
		_kf_plugin_manage->UnRegistModule< KFPlayerPlugin, KFPlayerInterface >();
	}

	void KFPlayerPlugin::LoadModule()
	{
		_kf_zone = _kf_plugin_manage->FindModule< KFZoneInterface >();
		_kf_game = _kf_plugin_manage->FindModule< KFGameInterface >();
		_kf_timer = _kf_plugin_manage->FindModule< KFTimerInterface >();
		_kf_kernel = _kf_plugin_manage->FindModule< KFKernelInterface >();
		_kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
		_kf_player = _kf_plugin_manage->FindModule< KFPlayerInterface >();
		_kf_data = _kf_plugin_manage->FindModule< KFDataClientInterface >();
		_kf_display = _kf_plugin_manage->FindModule< KFDisplayInterface >();
		_kf_route = _kf_plugin_manage->FindModule< KFRouteClientInterface >();
		_kf_public = _kf_plugin_manage->FindModule< KFPublicClientInterface >();
		_kf_tcp_client = _kf_plugin_manage->FindModule< KFTcpClientInterface >();
	}

}