#include "KFRedisManage.h"
#include "KFRedisConfig.h"
#include "KFThread/KFThread.h"

namespace KFrame
{
	KFRedisManage::KFRedisManage()
	{
	}

	KFRedisManage::~KFRedisManage()
	{
	
	}

	void KFRedisManage::ShutDown()
	{
		{
			for ( auto& iter : _redis_execute._objects )
			{
				iter.second->ShutDown();
			}
		}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	KFRedisExecute* KFRedisManage::CreateExecute( uint32 id, const std::string& ip, uint32 port, const std::string& password )
	{
		auto kfredisexecute = FindRedisExecute( id );
		if ( kfredisexecute != nullptr )
		{
			return kfredisexecute;
		}

		kfredisexecute = __KF_CREATE_BATCH__( KFRedisExecute, 5 );
		auto result = kfredisexecute->Initialize( ip.c_str(), port, password.c_str() );
		if ( result != KFCommonEnum::OK )
		{
			// KFLogger::LogInit( KFLogger::Error, "redis connect[ id=%u ip=%s:%u ] failed!", id, ip.c_str(), port );
		}
		else
		{
			// KFLogger::LogInit( KFLogger::Info, "redis connect[ id=%u ip=%s:%u ] ok!", id, ip.c_str(), port );
		}

		InsertRedisExecute( id, kfredisexecute );
		return kfredisexecute;
	}

	KFRedisExecute* KFRedisManage::CreateExecute( uint32 id )
	{
		auto kfsetting = _kf_redis_config->FindRedisSetting( id );
		if ( kfsetting == nullptr )
		{
			return nullptr;
		}

		return CreateExecute( id, kfsetting->_ip.c_str(), kfsetting->_port, kfsetting->_password.c_str() );
	}

	KFRedisExecute* KFRedisManage::CreateExecute( const std::string& field, uint32 logicid )
	{
		auto kfsetting = _kf_redis_config->FindRedisSetting( field, logicid );
		if ( kfsetting == nullptr )
		{
			return nullptr;
		}

		return CreateExecute( kfsetting->_id, kfsetting->_ip.c_str(), kfsetting->_port, kfsetting->_password.c_str() );
	}

	KFRedisExecute* KFRedisManage::FindRedisExecute( uint32 id )
	{
		auto threadid = KFThread::GetThreadID();
		auto key = RedisKey( threadid, id );

		return _redis_execute.Find( key );
	}

	void KFRedisManage::InsertRedisExecute( uint32 id, KFRedisExecute* kfredisexecute )
	{
		auto threadid = KFThread::GetThreadID();
		auto key = RedisKey( threadid, id );
		_redis_execute.Insert( key, kfredisexecute );
	}
}