#include "KFMatchClientPlugin.h"
#include "KFMatchClientModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
	void KFMatchClientPlugin::Install()
	{
		_kf_plugin_manage->RegistModule< KFMatchClientPlugin, KFMatchClientInterface >( new KFMatchClientModule() );
	}

	void KFMatchClientPlugin::UnInstall()
	{
		_kf_plugin_manage->UnRegistModule< KFMatchClientPlugin, KFMatchClientInterface >();
	}
	
	void KFMatchClientPlugin::LoadModule()
	{
		_kf_zone = _kf_plugin_manage->FindModule< KFZoneInterface >();
		_kf_player = _kf_plugin_manage->FindModule< KFPlayerInterface >();
		_kf_config = _kf_plugin_manage->FindModule< KFConfigInterface >();
		_kf_kernel = _kf_plugin_manage->FindModule< KFKernelInterface >();
		_kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
		_kf_display = _kf_plugin_manage->FindModule< KFDisplayInterface >();
		_kf_cluster = _kf_plugin_manage->FindModule< KFClusterClientInterface >();
	}
}