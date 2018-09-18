#include "KFRedisModule.h"
#include "KFRedisConfig.h"

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
        __KF_ADD_CONFIG__( _kf_redis_config, false );
    }

    void KFRedisModule::ShutDown()
    {
        __KF_REMOVE_CONFIG__();
        KFLocker lock( _mt_mutex );
        for ( auto& iter : _redis_execute._objects )
        {
            iter.second->ShutDown();
        }
    }

    void KFRedisModule::AfterRun()
    {
        {
            KFLocker lock( _mt_mutex );
            for ( auto& iter : _redis_execute._objects )
            {
                iter.second->Run();
            }
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // 创建RedisExecute
    KFRedisDriver* KFRedisModule::CreateExecute( uint32 id )
    {
        auto kfsetting = _kf_redis_config->FindRedisSetting( id );
        if ( kfsetting == nullptr )
        {
            return nullptr;
        }

        return CreateExecute( id, kfsetting->_ip, kfsetting->_port, kfsetting->_password );
    }

    KFRedisDriver* KFRedisModule::CreateExecute( const std::string& field, uint32 logicid /* = 0 */ )
    {
        auto kfsetting = _kf_redis_config->FindRedisSetting( field, logicid );
        if ( kfsetting == nullptr )
        {
            return nullptr;
        }

        return CreateExecute( kfsetting->_id, kfsetting->_ip, kfsetting->_port, kfsetting->_password );
    }

    KFRedisDriver* KFRedisModule::CreateExecute( uint32 id, const std::string& ip, uint32 port, const std::string& password )
    {
        auto kfredisexecute = FindRedisExecute( id );
        if ( kfredisexecute != nullptr )
        {
            return kfredisexecute;
        }

        kfredisexecute = __KF_CREATE_BATCH__( KFRedisExecute, 5 );
        auto result = kfredisexecute->Initialize( ip, port, password );
        if ( result != KFEnum::Ok )
        {
            __LOG_ERROR__( "redis connect[ id={} ip={}:{} ] failed!", id, ip, port );
        }
        else
        {
            __LOG_INFO__( "redis connect[ id={} ip={}:{} ] ok!", id, ip, port );
        }

        InsertRedisExecute( id, kfredisexecute );
        return kfredisexecute;
    }

    KFRedisExecute* KFRedisModule::FindRedisExecute( uint32 id )
    {
        auto threadid = KFThread::GetThreadID();
        auto key = RedisKey( threadid, id );

        KFLocker lock( _mt_mutex );
        return _redis_execute.Find( key );
    }

    void KFRedisModule::InsertRedisExecute( uint32 id, KFRedisExecute* kfredisexecute )
    {
        auto threadid = KFThread::GetThreadID();
        auto key = RedisKey( threadid, id );

        KFLocker lock( _mt_mutex );
        _redis_execute.Insert( key, kfredisexecute );
    }
}