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
        __KF_ADD_CONFIG__( _kf_redis_config, false );
    }

    void KFRedisModule::ShutDown()
    {
        __KF_REMOVE_CONFIG__();
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