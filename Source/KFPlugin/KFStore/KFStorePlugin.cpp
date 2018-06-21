#include "KFStorePlugin.h"
#include "KFStoreModule.h"

//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
	void KFStorePlugin::Install()
	{
		_kf_plugin_manage->RegistModule< KFStorePlugin, KFStoreInterface >( new KFStoreModule() );
	}

	void KFStorePlugin::UnInstall()
	{
		_kf_plugin_manage->UnRegistModule< KFStorePlugin, KFStoreInterface >();
	}

	void KFStorePlugin::LoadModule()
	{
		_kf_config = _kf_plugin_manage->FindModule< KFConfigInterface >();
		_kf_kernel = _kf_plugin_manage->FindModule< KFKernelInterface >();
		_kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
		_kf_player = _kf_plugin_manage->FindModule< KFPlayerInterface >();
		_kf_mail = _kf_plugin_manage->FindModule< KFMailClientInterface >();
		_kf_option = _kf_plugin_manage->FindModule< KFOptionInterface >();
		_kf_display = _kf_plugin_manage->FindModule< KFDisplayInterface >();
	}
}