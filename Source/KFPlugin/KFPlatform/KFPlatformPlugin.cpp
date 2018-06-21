#include "KFPlatformPlugin.h"
#include "KFPlatformModule.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFRedis/KFRedisInterface.h"
#include "KFHttpServer/KFHttpServerInterface.h"
//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
	void KFPlatformPlugin::Install()
	{
		_kf_plugin_manage->RegistModule< KFPlatformPlugin, KFPlatformInterface >( new KFPlatformModule() );
	}

	void KFPlatformPlugin::UnInstall()
	{
		_kf_plugin_manage->UnRegistModule< KFPlatformPlugin, KFPlatformInterface >();
	}
	
	void KFPlatformPlugin::LoadModule()
	{
		_kf_config = _kf_plugin_manage->FindModule< KFConfigInterface >();
		_kf_redis = _kf_plugin_manage->FindModule< KFRedisInterface >();
		_kf_timer = _kf_plugin_manage->FindModule< KFTimerInterface >();
		_kf_http_client = _kf_plugin_manage->FindModule< KFHttpClientInterface >();
		_kf_http_server = _kf_plugin_manage->FindModule< KFHttpServerInterface >();
	}
}