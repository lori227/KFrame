#include "KFRedisModule.hpp"
#include "KFRedisConfig.hpp"

namespace KFrame
{
    void KFRedisModule::ShutDown()
    {
        {
            KFLocker lock( _mt_mutex );
            for ( auto& iter : _redis_logic._objects )
            {
                iter.second->ShutDown();
            }
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    const KFRedisConnectOption* KFRedisModule::FindRedisConnectOption( const std::string& module, uint32 logicid )
    {
        auto setting = KFRedisConfig::Instance()->FindSetting( module );
        if ( setting == nullptr )
        {
            __LOG_ERROR__( "[{}:{}] can't find redis setting", module, logicid );
            return nullptr;
        }

        for ( auto& connectoption : setting->_connect_option )
        {
            if ( logicid >= connectoption._min_logic_id && logicid <= connectoption._max_logic_id )
            {
                return &connectoption;
            }
        }

        return nullptr;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    KFRedisDriver* KFRedisModule::Create( const std::string& module, uint32 logicid /* = 0 */ )
    {
        auto kfredisoption = FindRedisConnectOption( module, logicid );
        if ( kfredisoption == nullptr )
        {
            __LOG_ERROR__( "[{}:{}] can't find redis option", module, logicid );
            return nullptr;
        }

        auto kfredislogic = FindRedisLogic( kfredisoption->_runtime_id );
        if ( kfredislogic != nullptr )
        {
            return kfredislogic;
        }

        kfredislogic = __KF_NEW__( KFRedisLogic );
        kfredislogic->Initialize( module, kfredisoption );

        InsertRedisLogic( kfredisoption->_runtime_id, kfredislogic );
        return kfredislogic;
    }

    KFRedisLogic* KFRedisModule::FindRedisLogic( uint32 id )
    {
        auto threadid = KFThread::GetThreadID();
        auto key = RedisKey( threadid, id );

        KFLocker lock( _mt_mutex );
        return _redis_logic.Find( key );
    }

    void KFRedisModule::InsertRedisLogic( uint32 id, KFRedisLogic* kfredislogic )
    {
        auto threadid = KFThread::GetThreadID();
        auto key = RedisKey( threadid, id );

        KFLocker lock( _mt_mutex );
        _redis_logic.Insert( key, kfredislogic );
    }
}