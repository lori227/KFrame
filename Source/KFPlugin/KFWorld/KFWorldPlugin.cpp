#include "KFWorldPlugin.h"
#include "KFWorldModule.h"

namespace KFrame
{
	void KFWorldPlugin::Install()
	{
		_kf_plugin_manage->RegistModule< KFWorldPlugin, KFWorldInterface >( new KFWorldModule() );
	}

	void KFWorldPlugin::UnInstall()
	{
		_kf_plugin_manage->UnRegistModule< KFWorldPlugin, KFWorldInterface >();
	}

	void KFWorldPlugin::LoadModule()
	{
		_kf_zone = _kf_plugin_manage->FindModule< KFZoneInterface >();
		_kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
		_kf_connection = _kf_plugin_manage->FindModule< KFConnectionInterface >();
		_kf_tcp_server = _kf_plugin_manage->FindModule< KFTcpServerInterface >();
		_kf_tcp_client = _kf_plugin_manage->FindModule< KFTcpClientInterface >();
		_kf_http_server = _kf_plugin_manage->FindModule<KFHttpServerInterface >();
		_kf_http_client = _kf_plugin_manage->FindModule< KFHttpClientInterface >();
	}

}