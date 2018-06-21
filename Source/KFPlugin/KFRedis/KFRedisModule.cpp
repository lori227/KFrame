#include "KFRedisModule.h"
#include "KFRedisConfig.h"
#include "KFRedisManage.h"

namespace KFrame
{
	KFRedisModule::KFRedisModule()
	{
	}

	KFRedisModule::~KFRedisModule()
	{
	}

	void KFRedisModule::InitModule()
	{
		/////////////////////////////////////////////////////////////////////////////////////
		_kf_config->AddConfig( _kf_redis_config, _kf_plugin->_plugin_name, _kf_plugin->_config, false );
	}

	void KFRedisModule::BeforeShut()
	{
		_kf_config->RemoveConfig( _kf_plugin->_plugin_name );
	}

	void KFRedisModule::ShutDown()
	{
		_kf_redis_manage->ShutDown();
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 创建RedisExecute
	KFRedisDriver* KFRedisModule::CreateExecute( uint32 id )
	{
		return _kf_redis_manage->CreateExecute( id );
	}

	KFRedisDriver* KFRedisModule::CreateExecute( const std::string& field, uint32 logicid /* = 0 */ )
	{
		return _kf_redis_manage->CreateExecute( field, logicid );
	}

	KFRedisDriver* KFRedisModule::CreateExecute( uint32 id, const std::string& ip, uint32 port, const std::string& password )
	{
		return _kf_redis_manage->CreateExecute( id, ip, port, password );
	}
}