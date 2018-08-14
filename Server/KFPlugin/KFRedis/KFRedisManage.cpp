#include "KFRedisManage.h"
#include "KFRedisConfig.h"

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
        KFLocker lock( _mt_mutex );
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
        auto result = kfredisexecute->Initialize( ip, port, password );
        if ( result != KFEnum::Ok )
        {
            __LOG_ERROR__( KFLogEnum::Sql, "redis connect[ id={} ip={}:{} ] failed!", id, ip, port );
        }
        else
        {
            __LOG_INFO__( KFLogEnum::Sql, "redis connect[ id={} ip={}:{} ] ok!", id, ip, port );
        }

        InsertRedisExecute( id, kfredisexecute );
        return kfredisexecute;
    }

    KFRedisExecute* KFRedisManage::CreateExecute( uint32 id )
    {
        auto kfsetting = _kf_redis_config->FindRedisSetting( id );
        if ( kfsetting == nullptr )
        {
            _kf_redis_config->LoadConfig();
            kfsetting = _kf_redis_config->FindRedisSetting( id );
            if ( kfsetting == nullptr )
            {
                return nullptr;
            }
        }

        return CreateExecute( id, kfsetting->_ip, kfsetting->_port, kfsetting->_password );
    }

    KFRedisExecute* KFRedisManage::CreateExecute( const std::string& field, uint32 logicid )
    {
        auto kfsetting = _kf_redis_config->FindRedisSetting( field, logicid );
        if ( kfsetting == nullptr )
        {
            _kf_redis_config->LoadConfig();
            kfsetting = _kf_redis_config->FindRedisSetting( field, logicid );
            if ( kfsetting == nullptr )
            {
                return nullptr;
            }
        }

        return CreateExecute( kfsetting->_id, kfsetting->_ip, kfsetting->_port, kfsetting->_password );
    }

    KFRedisExecute* KFRedisManage::FindRedisExecute( uint32 id )
    {
        auto threadid = KFThread::GetThreadID();
        auto key = RedisKey( threadid, id );

        KFLocker lock( _mt_mutex );
        return _redis_execute.Find( key );
    }

    void KFRedisManage::InsertRedisExecute( uint32 id, KFRedisExecute* kfredisexecute )
    {
        auto threadid = KFThread::GetThreadID();
        auto key = RedisKey( threadid, id );

        KFLocker lock( _mt_mutex );
        _redis_execute.Insert( key, kfredisexecute );
    }
}