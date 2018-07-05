#include "KFCompoundPlugin.h"
#include "KFCompoundModule.h"

//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
	void KFCompoundPlugin::Install()
	{
		_kf_plugin_manage->RegistModule< KFCompoundPlugin, KFCompoundInterface >( new KFCompoundModule() );
	}

	void KFCompoundPlugin::UnInstall()
	{
		_kf_plugin_manage->UnRegistModule< KFCompoundPlugin, KFCompoundInterface >();
	}
	
	void KFCompoundPlugin::LoadModule()
	{
		_kf_config = _kf_plugin_manage->FindModule< KFConfigInterface >();
		_kf_player = _kf_plugin_manage->FindModule< KFPlayerInterface >();
		_kf_display = _kf_plugin_manage->FindModule< KFDisplayInterface >();
		_kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
	}
}