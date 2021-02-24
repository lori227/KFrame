#include "KFDirDatabaseRedis.hpp"

namespace KFrame
{
#define __DIR_REDIS_DRIVER__ _kf_redis->Create( __STRING__( dir ) )

    bool KFDirDatabaseRedis::ZoneRegister( uint32 zone_id, KFJson& zone_data )
    {
        auto redis_driver = __DIR_REDIS_DRIVER__;

        StringMap values;
        __JSON_TO_MAP__( zone_data, values );

        redis_driver->WriteMulti();
        redis_driver->ZIncrby( __STRING__( zonelist ), zone_id, 0 );
        redis_driver->HMSet( __DATABASE_KEY_2__(  __STRING__( zone ), zone_id ), values );
        auto result = redis_driver->WriteExec();
        return result->IsOk();
    }

    bool KFDirDatabaseRedis::ZoneUpdate( uint64 app_id, uint32 zone_id, uint32 count, const std::string& ip, uint32 port, uint32 expire_time )
    {
        // 判断最小负载
        auto redis_driver = __DIR_REDIS_DRIVER__;
        auto gate_key_name = __DATABASE_KEY_2__( __STRING__( gate ), zone_id );
        auto query_result = redis_driver->HGetAll( gate_key_name );
        if ( !query_result->_value.empty() )
        {
            auto last_app_id = __TO_UINT64__( query_result->_value[ __STRING__( appid ) ] );
            auto last_count = __TO_UINT32__( query_result->_value[ __STRING__( count ) ] );
            if ( last_app_id != app_id && count > last_count )
            {
                return true;
            }
        }

        StringMap values;
        values[ __STRING__( ip ) ] = ip;
        values[ __STRING__( port ) ] = __TO_STRING__( port );
        values[ __STRING__( count ) ] = __TO_STRING__( count );
        values[ __STRING__( appid ) ] = __TO_STRING__( app_id );

        redis_driver->WriteMulti();
        redis_driver->HMSet( gate_key_name, values );
        redis_driver->Expire( gate_key_name, expire_time );
        auto result = redis_driver->WriteExec();
        return result->IsOk();
    }

    uint32 KFDirDatabaseRedis::QueryZoneStatus( uint32 zone_id )
    {
        auto redis_driver = __DIR_REDIS_DRIVER__;
        auto result = redis_driver->HGetUInt64( __DATABASE_KEY_2__( __STRING__( zone ), zone_id ), __STRING__( status ) );
        return result->_value;
    }

    bool KFDirDatabaseRedis::UpdateZoneStatus( uint32 zone_id, uint32 status )
    {
        auto redis_driver = __DIR_REDIS_DRIVER__;
        if ( zone_id != 0u )
        {
            auto result = redis_driver->HSet( __DATABASE_KEY_2__( __STRING__( zone ), zone_id ), __STRING__( status ), status );
            return result->IsOk();
        }

        auto zone_list = redis_driver->ZRange( __STRING__( zonelist ), 0, -1 );
        if ( zone_list->_value.empty() )
        {
            return false;
        }

        redis_driver->WriteMulti();
        for ( auto& zone_pair : zone_list->_value )
        {
            redis_driver->HSet( __DATABASE_KEY_2__( __STRING__( zone ), zone_pair.first ), __STRING__( status ), status );
        }
        auto result = redis_driver->WriteExec();
        return result->IsOk();
    }

    StringMapList KFDirDatabaseRedis::QueryZoneList( const std::string& flag )
    {
        StringMapList zone_data_list;
        auto redis_driver = __DIR_REDIS_DRIVER__;
        auto query_list = redis_driver->ZRange( __STRING__( zonelist ), 0, -1 );
        for ( auto& zone_pair : query_list->_value )
        {
            auto zone_id = __TO_UINT32__( zone_pair.first );
            auto zone_data = QueryZoneData( zone_id );
            if ( !zone_data.empty() )
            {
                if ( flag.empty() || flag == zone_data[ __STRING__( flag ) ] )
                {
                    zone_data_list.emplace_back( zone_data );
                }
            }
        }

        return zone_data_list;
    }

    StringMap KFDirDatabaseRedis::QueryZoneIp( uint32 zone_id )
    {
        StringMap zone_data;
        auto redis_driver = __DIR_REDIS_DRIVER__;

        auto query_login_id = redis_driver->HGetUInt64( __DATABASE_KEY_2__( __STRING__( zone ), zone_id ), __STRING__( loginzoneid ) );
        if ( query_login_id->_value == 0u )
        {
            return zone_data;
        }

        auto result = redis_driver->HGetAll( __DATABASE_KEY_2__( __STRING__( gate ), query_login_id->_value ) );
        zone_data.swap( result->_value );
        return zone_data;
    }

    StringMap KFDirDatabaseRedis::QueryZoneData( uint32 zone_id )
    {
        StringMap zone_data;
        auto redis_driver = __DIR_REDIS_DRIVER__;
        auto query_zone_data = redis_driver->HGetAll( __DATABASE_KEY_2__( __STRING__( zone ), zone_id ) );
        if ( query_zone_data->_value.empty() )
        {
            return zone_data;
        }

        auto login_zone_id = query_zone_data->_value[ __STRING__( loginzoneid ) ];
        auto query_gate_data = redis_driver->HGetAll( __DATABASE_KEY_2__( __STRING__( gate ), login_zone_id ) );
        if ( query_gate_data->_value.empty() )
        {
            return zone_data;
        }

        zone_data = query_gate_data->_value;
        for ( auto& iter : query_zone_data->_value )
        {
            zone_data[ iter.first ] = iter.second;
        }
        return zone_data;
    }

    bool KFDirDatabaseRedis::ZoneBalance( uint32 zone_id, uint32 count )
    {
        auto redis_driver = __DIR_REDIS_DRIVER__;
        auto result = redis_driver->ZIncrby( __STRING__( zonelist ), zone_id, count );
        return result->IsOk();
    }

    bool KFDirDatabaseRedis::SetZoneRecommend( const std::string& flag, uint32 zone_id, bool is_recommend )
    {
        auto redis_driver = __DIR_REDIS_DRIVER__;

        auto ok = false;
        if ( is_recommend )
        {
            auto result = redis_driver->SAdd(  __DATABASE_KEY_2__( __STRING__( zonerecommendlist ), flag ), zone_id );
            ok = result->IsOk();
        }
        else
        {
            auto result = redis_driver->SRem( __DATABASE_KEY_2__( __STRING__( zonerecommendlist ), flag ), zone_id );
            ok = result->IsOk();
        }

        return ok;
    }

    uint32 KFDirDatabaseRedis::GetZoneRecommend( const std::string& flag )
    {
        auto redis_driver = __DIR_REDIS_DRIVER__;
        auto result = redis_driver->SRandMember( __DATABASE_KEY_2__( __STRING__( zonerecommendlist ), flag ) );
        return KFUtility::ToValue<uint32>( result->_value );
    }

    uint32 KFDirDatabaseRedis::BalanceAllocZone( const std::string& flag )
    {
        // todo:如果有推荐的服务器, 直接返回
        auto zone_id = GetZoneRecommend( flag );
        if ( zone_id != _invalid_int )
        {
            return zone_id;
        }

        // 选择一个最小人数的分区
        auto redis_driver = __DIR_REDIS_DRIVER__;
        auto query_zone_list = redis_driver->ZRange( __STRING__( zonelist ), 0, -1 );
        if ( query_zone_list->_value.empty() )
        {
            return _invalid_int;
        }

        for ( auto& zone_pair : query_zone_list->_value )
        {
            auto login_zone_id = redis_driver->HGetUInt64( __DATABASE_KEY_2__( __STRING__( zone ), zone_pair.first ), __STRING__( loginzoneid ) );
            if ( login_zone_id->_value == 0u )
            {
                continue;
            }

            auto query_ip = redis_driver->HGet( __DATABASE_KEY_2__( __STRING__( gate ), login_zone_id->_value ), __STRING__( ip ) );
            if ( !query_ip->_value.empty() )
            {
                return __TO_UINT32__( zone_pair.first );
            }
        }

        // 找不到 直接返回1
        return 1u;
    }

    StringMap KFDirDatabaseRedis::AllocPlayerZone( const std::string& flag, uint32 zone_id )
    {
        StringMap zone_data;
        if ( zone_id == _invalid_int )
        {
            zone_id = BalanceAllocZone( flag );
            if ( zone_id == _invalid_int )
            {
                return zone_data;
            }
        }

        return QueryZoneData( zone_id );
    }

    bool KFDirDatabaseRedis::SetWorldUrl( uint64 world_id, const std::string& url )
    {
        auto redis_driver = __DIR_REDIS_DRIVER__;
        auto result = redis_driver->Set( __DATABASE_KEY_2__( __STRING__( worldurl ), world_id ), url );
        return result->IsOk();
    }

    std::string KFDirDatabaseRedis::GetWorldUrl( uint64 world_id )
    {
        auto redis_driver = __DIR_REDIS_DRIVER__;
        auto result = redis_driver->Get( __DATABASE_KEY_2__( __STRING__( worldurl ), world_id ) );
        return result->_value;
    }

    // 更新masterip
    bool KFDirDatabaseRedis::UpdateMasterIp( const std::string& app_name, uint64 app_id, uint32 zone_id, const std::string& ip, uint32 port, uint32 expire_time )
    {
        auto redis_driver = __DIR_REDIS_DRIVER__;
        auto master_key_name = __DATABASE_KEY_2__( __STRING__( master ), app_id );

        StringMap values;
        values[ __STRING__( ip ) ] = ip;
        values[ __STRING__( appid ) ] = __TO_STRING__( app_id );
        values[ __STRING__( port ) ] = __TO_STRING__( port );

        redis_driver->WriteMulti();
        redis_driver->HMSet( master_key_name, values );
        redis_driver->Expire( master_key_name, expire_time );
        redis_driver->SAdd( __DATABASE_KEY_3__( __STRING__( master ), app_name, zone_id ), app_id );
        auto result = redis_driver->WriteExec();
        return result->IsOk();
    }

    StringMap KFDirDatabaseRedis::QueryMasterIp( const std::string& app_name, uint32 zone_id )
    {
        StringMap zone_data;
        auto redis_driver = __DIR_REDIS_DRIVER__;
        auto master_key_name = __DATABASE_KEY_3__( __STRING__( master ), app_name, zone_id );

        auto query_app_id = redis_driver->SRandMember( master_key_name );
        if ( !query_app_id->_value.empty() )
        {
            auto master_data = redis_driver->HGetAll( __DATABASE_KEY_2__( __STRING__( master ), query_app_id->_value ) );
            if ( !master_data->_value.empty() )
            {
                zone_data.swap( master_data->_value );
            }
            else
            {
                if ( master_data->IsOk() )
                {
                    // 执行正常, 说明master数据过期了
                    redis_driver->SRem( master_key_name, query_app_id->_value );
                }
            }
        }

        return zone_data;
    }

    StringMapList KFDirDatabaseRedis::QueryMasterList( const std::string& app_name, uint32 zone_id )
    {
        StringMapList master_data_list;
        auto redis_driver = __DIR_REDIS_DRIVER__;
        auto master_key_name = __DATABASE_KEY_3__( __STRING__( master ), app_name, zone_id );

        auto query_app_id_list = redis_driver->SMembers( master_key_name );
        for ( auto& app_id : query_app_id_list->_value )
        {
            auto query_master_data = redis_driver->HGetAll( __DATABASE_KEY_2__( __STRING__( master ), app_id ) );
            if ( !query_master_data->_value.empty() )
            {
                StringMap master_data;
                master_data.swap( query_master_data->_value );
                master_data_list.emplace_back( master_data );
            }
            else
            {
                if ( query_master_data->IsOk() )
                {
                    // 执行正常, 说明master数据过期了
                    redis_driver->SRem( master_key_name, app_id );
                }
            }
        }

        return master_data_list;
    }
}