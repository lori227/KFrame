#include "KFBattleClientPlugin.h"
#include "KFBattleClientModule.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
	void KFBattleClientPlugin::Install()
	{
		_kf_plugin_manage->RegistModule< KFBattleClientPlugin, KFBattleClientInterface >( new KFBattleClientModule() );
	}

	void KFBattleClientPlugin::UnInstall()
	{
		_kf_plugin_manage->UnRegistModule< KFBattleClientPlugin, KFBattleClientInterface >();
	}
	
	void KFBattleClientPlugin::LoadModule()
	{
		_kf_option = _kf_plugin_manage->FindModule< KFOptionInterface >();
		_kf_player = _kf_plugin_manage->FindModule< KFPlayerInterface >();
		_kf_kernel = _kf_plugin_manage->FindModule< KFKernelInterface >();
		_kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
		_kf_cluster = _kf_plugin_manage->FindModule< KFClusterClientInterface >();
		_kf_rank = _kf_plugin_manage->FindModule< KFRankClientInterface >();
	}
}