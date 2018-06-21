#include "KFGatePlugin.h"
#include "KFGateModule.h"

namespace KFrame
{
	void KFGatePlugin::Install()
	{
		_kf_plugin_manage->RegistModule< KFGatePlugin, KFGateInterface >( new KFGateModule() );
	}

	void KFGatePlugin::UnInstall()
	{
		_kf_plugin_manage->UnRegistModule< KFGatePlugin, KFGateInterface >();
	}

	void KFGatePlugin::LoadModule()
	{
		_kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
		_kf_tcp_client = _kf_plugin_manage->FindModule< KFTcpClientInterface >();
		_kf_tcp_server = _kf_plugin_manage->FindModule< KFTcpServerInterface >();
	}

}