#include "KFGiftPlugin.h"
#include "KFGiftModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
	void KFGiftPlugin::Install()
	{
		_kf_plugin_manage->RegistModule< KFGiftPlugin, KFGiftInterface >( new KFGiftModule() );
	}

	void KFGiftPlugin::UnInstall()
	{
		_kf_plugin_manage->UnRegistModule< KFGiftPlugin, KFGiftInterface >();
	}

	void KFGiftPlugin::LoadModule()
	{
		_kf_zone = _kf_plugin_manage->FindModule< KFZoneInterface >();
		_kf_kernel = _kf_plugin_manage->FindModule< KFKernelInterface >();
		_kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
		_kf_player = _kf_plugin_manage->FindModule< KFPlayerInterface >();
		_kf_http_client = _kf_plugin_manage->FindModule< KFHttpClientInterface >();
		_kf_connection = _kf_plugin_manage->FindModule< KFConnectionInterface >();
		_kf_display = _kf_plugin_manage->FindModule< KFDisplayInterface >();
	}

}