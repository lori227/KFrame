#include "KFLoginGamePlugin.h"
#include "KFLoginGameModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
	void KFLoginGamePlugin::Install()
	{
		_kf_plugin_manage->RegistModule< KFLoginGamePlugin, KFLoginGameInterface >( new KFLoginGameModule() );
	}

	void KFLoginGamePlugin::UnInstall()
	{
		_kf_plugin_manage->UnRegistModule< KFLoginGamePlugin, KFLoginGameInterface >();
	}
	
	void KFLoginGamePlugin::LoadModule()
	{
		_kf_zone = _kf_plugin_manage->FindModule< KFZoneInterface >();
		_kf_game = _kf_plugin_manage->FindModule< KFGameInterface >();
		_kf_timer = _kf_plugin_manage->FindModule< KFTimerInterface >();
		_kf_player = _kf_plugin_manage->FindModule< KFPlayerInterface >();
		_kf_kernel = _kf_plugin_manage->FindModule< KFKernelInterface >();
		_kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
		_kf_data = _kf_plugin_manage->FindModule< KFDataClientInterface >();
		_kf_tcp_server = _kf_plugin_manage->FindModule< KFTcpServerInterface >();
	}
}