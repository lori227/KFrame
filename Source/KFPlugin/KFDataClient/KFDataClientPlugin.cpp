#include "KFDataClientPlugin.h"
#include "KFDataClientModule.h"

namespace KFrame
{
	void KFDataClientPlugin::Install()
	{
		_kf_plugin_manage->RegistModule< KFDataClientPlugin, KFDataClientInterface >( new KFDataClientModule() );
	}

	void KFDataClientPlugin::UnInstall()
	{
		_kf_plugin_manage->UnRegistModule< KFDataClientPlugin, KFDataClientInterface >();
	}

	void KFDataClientPlugin::LoadModule()
	{
		_kf_message = _kf_plugin_manage->FindModule< KFMessageInterface >();
		_kf_cluster = _kf_plugin_manage->FindModule< KFClusterClientInterface >();
	}

}