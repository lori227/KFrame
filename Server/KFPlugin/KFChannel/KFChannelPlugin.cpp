#include "KFChannelPlugin.h"
#include "KFChannelModule.h"
#include "KFConfig/KFConfigInterface.h"
#include "KFRedis/KFRedisInterface.h"
#include "KFHttpClient/KFHttpClientInterface.h"
#include "KFHttpServer/KFHttpServerInterface.h"

//////////////////////////////////////////////////////////////////////////

namespace KFrame
{
	void KFChannelPlugin::Install()
	{
		_kf_plugin_manage->RegistModule< KFChannelPlugin, KFChannelInterface >( new KFChannelModule() );
	}
	
	void KFChannelPlugin::UnInstall()
	{
		_kf_plugin_manage->UnRegistModule< KFChannelPlugin, KFChannelInterface >();
	}
	
	void KFChannelPlugin::LoadModule()
	{
		_kf_config = _kf_plugin_manage->FindModule< KFConfigInterface >();
		_kf_redis = _kf_plugin_manage->FindModule< KFRedisInterface >();
		_kf_http_client = _kf_plugin_manage->FindModule< KFHttpClientInterface >();
		_kf_http_server = _kf_plugin_manage->FindModule< KFHttpServerInterface >();
	}
}