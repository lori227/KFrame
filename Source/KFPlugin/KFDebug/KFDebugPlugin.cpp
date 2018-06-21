#include "KFDebugPlugin.h"
#include "KFDebugModule.h"

//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
	void KFDebugPlugin::Install()
	{
		_kf_plugin_manage->RegistModule< KFDebugPlugin, KFDebugInterface >( new KFDebugModule() );
	}

	void KFDebugPlugin::UnInstall()
	{
		_kf_plugin_manage->UnRegistModule< KFDebugPlugin, KFDebugInterface >();
	}
	
	void KFDebugPlugin::LoadModule()
	{
		_kf_lua = _kf_plugin_manage->FindModule< KFLuaInterface >();
		_kf_option = _kf_plugin_manage->FindModule< KFOptionInterface >();
		_kf_player = _kf_plugin_manage->FindModule< KFPlayerInterface >();
		_kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
	}
}