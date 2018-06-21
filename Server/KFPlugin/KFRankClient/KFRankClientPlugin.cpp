#include "KFRankClientPlugin.h"
#include "KFRankClientModule.h"

namespace KFrame
{
	void KFRankClientPlugin::Install()
	{
		_kf_plugin_manage->RegistModule< KFRankClientPlugin, KFRankClientInterface >( new KFRankClientModule() );
	}

	void KFRankClientPlugin::UnInstall()
	{
		_kf_plugin_manage->UnRegistModule< KFRankClientPlugin, KFRankClientInterface >();
	}

	void KFRankClientPlugin::LoadModule()
	{
		_kf_config = _kf_plugin_manage->FindModule< KFConfigInterface >();
		_kf_player = _kf_plugin_manage->FindModule< KFPlayerInterface >();
		_kf_kernel = _kf_plugin_manage->FindModule< KFKernelInterface >();
		_kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
		_kf_cluster = _kf_plugin_manage->FindModule< KFClusterClientInterface >();
	}

}