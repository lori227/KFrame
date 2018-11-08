#include "KFRedisModule.h"
#include "KFRedisConfig.h"

namespace KFrame
{
    void KFRedisModule::InitModule()
    {
        __KF_ADD_CONFIG__( _kf_redis_config, false );
    }

    void KFRedisModule::ShutDown()
    {
        __KF_REMOVE_CONFIG__( _kf_redis_config );

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
    KFRedisDriver* KFRedisModule::CreateExecute( const std::string& module, uint32 logicid /* = 0 */ )
    {
        auto kfsetting = _kf_redis_config->FindRedisSetting( module, logicid );
        if ( kfsetting == nullptr )
        {
            return nullptr;
        }

        KFLocker lock( _mt_mutex );
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

        InsertRedisExecute( id, kfredisexecute );
        return kfredisexecute;
    }

    KFRedisExecute* KFRedisModule::FindRedisExecute( uint32 id )
    {
        auto threadid = KFThread::GetThreadID();
        auto key = RedisKey( threadid, id );
        return _redis_execute.Find( key );
    }

    void KFRedisModule::InsertRedisExecute( uint32 id, KFRedisExecute* kfredisexecute )
    {
        auto threadid = KFThread::GetThreadID();
        auto key = RedisKey( threadid, id );
        _redis_execute.Insert( key, kfredisexecute );
    }
}