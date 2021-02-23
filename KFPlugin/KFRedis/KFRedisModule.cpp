#include "KFRedisModule.hpp"
#include "KFRedisConfig.hpp"

namespace KFrame
{
    void KFRedisModule::ShutDown()
    {
        KFLocker lock( _mt_mutex );
        for ( auto& iter : _redis_logic._objects )
        {
            iter.second->ShutDown();
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    const KFRedisConnectOption* KFRedisModule::FindRedisConnectOption( const std::string& module, uint32 logic_id )
    {
        auto setting = KFRedisConfig::Instance()->FindSetting( module );
        if ( setting == nullptr )
        {
            __LOG_ERROR__( "[{}:{}] can't find redis setting", module, logic_id );
            return nullptr;
        }

        for ( auto& connect_option : setting->_connect_option )
        {
            if ( logic_id >= connect_option._min_logic_id && logic_id <= connect_option._max_logic_id )
            {
                return &connect_option;
            }
        }

        return nullptr;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::shared_ptr<KFRedisDriver> KFRedisModule::Create( const std::string& module, uint32 logic_id /* = 0 */ )
    {
        auto redis_option = FindRedisConnectOption( module, logic_id );
        if ( redis_option == nullptr )
        {
            __LOG_ERROR__( "[{}:{}] can't find redis option", module, logic_id );
            return nullptr;
        }

        auto redis_logic = FindRedisLogic( redis_option->_runtime_id );
        if ( redis_logic == nullptr )
        {
            redis_logic = __MAKE_SHARED__( KFRedisLogic );
            redis_logic->Initialize( module, redis_option );
            InsertRedisLogic( redis_option->_runtime_id, redis_logic );
        }

        return redis_logic;
    }

    std::shared_ptr<KFRedisLogic> KFRedisModule::FindRedisLogic( uint32 logic_id )
    {
        auto thread_id = KFThread::GetID();
        auto key = RedisKey( thread_id, logic_id );

        KFLocker lock( _mt_mutex );
        return _redis_logic.Find( key );
    }

    void KFRedisModule::InsertRedisLogic( uint32 logic_id, std::shared_ptr<KFRedisLogic> redis_logic )
    {
        auto thread_id = KFThread::GetID();
        auto key = RedisKey( thread_id, logic_id );

        KFLocker lock( _mt_mutex );
        _redis_logic.Insert( key, redis_logic );
    }
}