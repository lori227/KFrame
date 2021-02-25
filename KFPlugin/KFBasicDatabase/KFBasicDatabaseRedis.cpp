#include "KFBasicDatabaseRedis.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
#define __BASIC_REDIS_DRIVER__ _kf_redis->Create( __STRING__( basic ) )

    void KFBasicDatabaseRedis::UpdateBasicIntValue( uint64 player_id, uint64 server_id, const StringUInt64& values )
    {
        StringMap update_values;
        for ( auto& iter : values )
        {
            update_values[ iter.first ] = __TO_STRING__( iter.second );
        }

        UpdateBasicStrValue( player_id, server_id, update_values );
    }

    void KFBasicDatabaseRedis::UpdateBasicStrValue( uint64 player_id, uint64 server_id, const StringMap& values )
    {
        auto redis_driver = __BASIC_REDIS_DRIVER__;
        redis_driver->HMSet( __DATABASE_KEY_2__( __STRING__( basic ), player_id ), values );

        static auto _get_status_value = []( const StringMap & values )
        {
            auto iter = values.find( __STRING__( status ) );
            if ( iter == values.end() )
            {
                return 0u;
            }

            return __TO_UINT32__( iter->second );
        };

        auto status = _get_status_value( values );
        switch ( status )
        {
        case KFMsg::OnlineStatus:
            redis_driver->SAdd( __DATABASE_KEY_2__( __STRING__( onlinelist ), server_id ), player_id );
            break;
        case KFMsg::OfflineStatus:
            redis_driver->SRem( __DATABASE_KEY_2__( __STRING__( onlinelist ), server_id ), player_id );
            break;
        default:
            break;
        }
    }

    void KFBasicDatabaseRedis::ClearBasicServerId( uint64 server_id )
    {
        auto redis_driver = __BASIC_REDIS_DRIVER__;
        auto player_list = redis_driver->SMembers( __DATABASE_KEY_2__( __STRING__( onlinelist ), server_id ) );
        if ( player_list->_value.empty() )
        {
            return;
        }

        StringMap values;
        values[ __STRING__( serverid ) ] = "0";
        values[ __STRING__( status ) ] = __TO_STRING__( KFMsg::OfflineStatus );

        for ( auto& id : player_list->_value )
        {
            auto key_name = __DATABASE_KEY_2__( __STRING__( basic ), id );
            auto query_server_id_result = redis_driver->HGetUInt64( key_name, __STRING__( serverid ) );
            if ( query_server_id_result->_value == server_id )
            {
                redis_driver->HMSet( key_name, values );
            }
        }

        redis_driver->Del( __DATABASE_KEY_2__( __STRING__( onlinelist ), server_id ) );
    }

    uint64 KFBasicDatabaseRedis::QueryBasicServerId( uint64 player_id )
    {
        return QueryBasicIntValue( player_id, __STRING__( serverid ) );
    }

    uint32 KFBasicDatabaseRedis::QueryBasicAttribute( uint64 player_id, StringMap& values )
    {
        auto redis_driver = __BASIC_REDIS_DRIVER__;
        auto basic_result = redis_driver->HGetAll( __DATABASE_KEY_2__( __STRING__( basic ), player_id ) );
        if ( !basic_result->IsOk() )
        {
            return KFMsg::PublicDatabaseBusy;
        }

        if ( basic_result->_value.empty() )
        {
            return KFMsg::NameCanNotFind;
        }

        values.swap( basic_result->_value );
        return KFMsg::Ok;
    }

    std::string KFBasicDatabaseRedis::FormatNameKey( const std::string& name, uint32 zone_id )
    {
        return __DATABASE_KEY_4__( __STRING__( player ), __STRING__( name ), zone_id, name );
    }

    uint32 KFBasicDatabaseRedis::SetPlayerName( uint32 zone_id, uint64 player_id, const std::string& old_name, const std::string& new_name )
    {
        auto redis_driver = __BASIC_REDIS_DRIVER__;

        auto new_name_key = FormatNameKey( new_name, zone_id );
        auto query_player_id_result = redis_driver->GetUInt64( new_name_key );
        if ( !query_player_id_result->IsOk() )
        {
            return KFMsg::NameDatabaseBusy;
        }

        // 如果不存在, 设置新名字
        if ( query_player_id_result->_value == _invalid_int )
        {
            // 保存名字
            auto result = redis_driver->Set( new_name_key, player_id );
            if ( !result->IsOk() )
            {
                return KFMsg::NameDatabaseBusy;
            }

            // 删除旧的名字关联
            if ( !old_name.empty() )
            {
                auto old_name_key = FormatNameKey( old_name, zone_id );
                redis_driver->Del( old_name_key );
            }
        }
        else if ( query_player_id_result->_value != player_id )
        {
            // 存在, 并且不是设定者
            return KFMsg::NameAlreadyExist;
        }

        return KFMsg::NameSetOk;
    }

    uint64 KFBasicDatabaseRedis::QueryBasicPlayerId( const std::string& player_name, uint32 zone_id )
    {
        auto redis_driver = __BASIC_REDIS_DRIVER__;

        auto name_key = FormatNameKey( player_name, zone_id );
        auto query_player_id_result = redis_driver->GetUInt64( name_key );
        return query_player_id_result->_value;
    }

    uint64 KFBasicDatabaseRedis::QueryBasicIntValue( uint64 player_id, const std::string& data_name )
    {
        auto redis_driver = __BASIC_REDIS_DRIVER__;
        auto result = redis_driver->HGetUInt64( __DATABASE_KEY_2__( __STRING__( basic ), player_id ), data_name );
        return result->_value;
    }

    std::string KFBasicDatabaseRedis::QueryBasicStrValue( uint64 player_id, const std::string& data_name )
    {
        auto redis_driver = __BASIC_REDIS_DRIVER__;
        auto result = redis_driver->HGet( __DATABASE_KEY_2__( __STRING__( basic ), player_id ), data_name );
        return result->_value;
    }
}