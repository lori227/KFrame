#include "KFLoginGatePlugin.h"
#include "KFLoginGateModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
	void KFLoginGatePlugin::Install()
	{
		_kf_plugin_manage->RegistModule< KFLoginGatePlugin, KFLoginGateInterface >( new KFLoginGateModule() );
	}

	void KFLoginGatePlugin::UnInstall()
	{
		_kf_plugin_manage->UnRegistModule< KFLoginGatePlugin, KFLoginGateInterface >();
	}
	
	void KFLoginGatePlugin::LoadModule()
	{
		_kf_zone = _kf_plugin_manage->FindModule< KFZoneInterface >();
		_kf_gate = _kf_plugin_manage->FindModule< KFGateInterface >();
		_kf_timer = _kf_plugin_manage->FindModule< KFTimerInterface >();
		_kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
		_kf_display = _kf_plugin_manage->FindModule< KFDisplayInterface >();
		_kf_connection = _kf_plugin_manage->FindModule< KFConnectionInterface >();
		_kf_http_client = _kf_plugin_manage->FindModule< KFHttpClientInterface >();
	}
}