#include "KFLoginProxyPlugin.h"
#include "KFLoginProxyModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
	void KFLoginProxyPlugin::Install()
	{
		_kf_plugin_manage->RegistModule< KFLoginProxyPlugin, KFLoginProxyInterface >( new KFLoginProxyModule() );
	}

	void KFLoginProxyPlugin::UnInstall()
	{
		_kf_plugin_manage->UnRegistModule< KFLoginProxyPlugin, KFLoginProxyInterface >();
	}
	
	void KFLoginProxyPlugin::LoadModule()
	{
		_kf_gate = _kf_plugin_manage->FindModule< KFGateInterface >();
		_kf_proxy = _kf_plugin_manage->FindModule< KFProxyInterface >();
		_kf_timer = _kf_plugin_manage->FindModule< KFTimerInterface >();
		_kf_config = _kf_plugin_manage->FindModule< KFConfigInterface >();
		_kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
		_kf_tcp_client = _kf_plugin_manage->FindModule< KFTcpClientInterface >();
		_kf_tcp_server = _kf_plugin_manage->FindModule< KFTcpServerInterface >();
		_kf_display = _kf_plugin_manage->FindModule< KFDisplayInterface >();
	}
}