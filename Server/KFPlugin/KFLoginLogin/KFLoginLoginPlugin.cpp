#include "KFLoginLoginPlugin.h"
#include "KFLoginLoginModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
	void KFLoginLoginPlugin::Install()
	{
		_kf_plugin_manage->RegistModule< KFLoginLoginPlugin, KFLoginLoginInterface >( new KFLoginLoginModule() );
	}

	void KFLoginLoginPlugin::UnInstall()
	{
		_kf_plugin_manage->UnRegistModule< KFLoginLoginPlugin, KFLoginLoginInterface >();
	}
	
	void KFLoginLoginPlugin::LoadModule()
	{
		_kf_zone = _kf_plugin_manage->FindModule< KFZoneInterface >();
		_kf_login = _kf_plugin_manage->FindModule< KFLoginInterface >();
		_kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
		_kf_connection = _kf_plugin_manage->FindModule< KFConnectionInterface >();
		_kf_tcp_server = _kf_plugin_manage->FindModule< KFTcpServerInterface >();
		_kf_http_client = _kf_plugin_manage->FindModule< KFHttpClientInterface >();
		_kf_data = _kf_plugin_manage->FindModule< KFDataClientInterface >();
	}
}