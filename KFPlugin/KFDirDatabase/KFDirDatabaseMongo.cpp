#include "KFDirDatabaseMongo.hpp"

namespace KFrame
{
#define __DIR_MONGO_DRIVER__ _kf_mongo->Create( __STRING__( dir ) )

    bool KFDirDatabaseMongo::ZoneRegister( uint32 zone_id, KFJson& zone_data )
    {
        // 先保存小区基本信息
        auto mongo_driver = __DIR_MONGO_DRIVER__;
        mongo_driver->CreateIndex( __STRING__( gate ), MongoKeyword::_expire );

        KFDBValue db_value;
        __JSON_TO_DBVALUE__( zone_data, db_value );
        return mongo_driver->Insert( __STRING__( zone ), zone_id, db_value );
    }

    bool KFDirDatabaseMongo::ZoneUpdate( uint64 app_id, uint32 zone_id, uint32 count, const std::string& ip, uint32 port, uint32 expire_time )
    {
        auto mongo_driver = __DIR_MONGO_DRIVER__;

        KFDBValue db_value;
        db_value.AddValue( __STRING__( ip ), ip );
        db_value.AddValue( __STRING__( port ), port );
        db_value.AddValue( __STRING__( appid ), app_id );
        db_value.AddValue( MongoKeyword::_expire, expire_time );
        return mongo_driver->Insert( __STRING__( gate ), zone_id, db_value );
    }

    uint32 KFDirDatabaseMongo::QueryZoneStatus( uint32 zone_id )
    {
        auto mongo_driver = __DIR_MONGO_DRIVER__;
        auto result = mongo_driver->QueryUInt64( __STRING__( zone ), zone_id, __STRING__( status ) );
        return result->_value;
    }

    bool KFDirDatabaseMongo::UpdateZoneStatus( uint32 zone_id, uint32 status )
    {
        auto mongo_driver = __DIR_MONGO_DRIVER__;
        if ( zone_id != 0u )
        {
            return mongo_driver->Insert( __STRING__( zone ), zone_id, __STRING__( status ), status );
        }

        return mongo_driver->Insert( __STRING__( zone ), __STRING__( status ), status );
    }

    StringMapList KFDirDatabaseMongo::QueryZoneList( const std::string& flag )
    {
        StringMapList zone_list;
        auto mongo_driver = __DIR_MONGO_DRIVER__;
        auto result = mongo_driver->QueryListRecord( __STRING__( zone ) );
        for ( auto& db_value : result->_value )
        {
            auto zone_id = db_value.FindValue( __STRING__( zoneid ) );
            auto zone_data = QueryZoneData( zone_id );
            if ( !zone_data.empty() )
            {
                if ( flag.empty() || flag == zone_data[ __STRING__( flag ) ] )
                {
                    zone_list.emplace_back( zone_data );
                }
            }
        }

        return zone_list;
    }

    StringMap KFDirDatabaseMongo::QueryZoneIp( uint32 zone_id )
    {
        StringMap values;
        auto mongo_driver = __DIR_MONGO_DRIVER__;

        auto login_zone_id = mongo_driver->QueryUInt64( __STRING__( zone ), zone_id, __STRING__( loginzoneid ) );
        if ( login_zone_id->_value == 0u )
        {
            return values;
        }

        auto ip_data = mongo_driver->QueryRecord( __STRING__( gate ), login_zone_id->_value );
        __DBVALUE_TO_MAP__( ip_data->_value, values );
        return values;
    }

    StringMap KFDirDatabaseMongo::QueryZoneData( uint32 zone_id )
    {
        StringMap zone_data;
        auto mongo_driver = __DIR_MONGO_DRIVER__;

        auto query_zone_data = mongo_driver->QueryRecord( __STRING__( zone ), zone_id );
        if ( query_zone_data->_value.IsEmpty() )
        {
            return zone_data;
        }

        auto login_zone_id = query_zone_data->_value.FindValue( __STRING__( loginzoneid ) );
        auto gate_data = mongo_driver->QueryRecord( __STRING__( gate ), login_zone_id );
        if ( gate_data->_value.IsEmpty() )
        {
            return zone_data;
        }

        __DBVALUE_TO_MAP__( query_zone_data->_value, zone_data );
        __DBVALUE_TO_MAP__( gate_data->_value, zone_data );
        return zone_data;
    }

    bool KFDirDatabaseMongo::ZoneBalance( uint32 zone_id, uint32 count )
    {
        auto mongo_driver = __DIR_MONGO_DRIVER__;
        return mongo_driver->Operate( __STRING__( zone ), zone_id, __STRING__( count ), KFEnum::Add, count );
    }

    bool KFDirDatabaseMongo::SetZoneRecommend( const std::string& flag, uint32 zone_id, bool is_recommend )
    {
        auto mongo_driver = __DIR_MONGO_DRIVER__;

        auto ok = false;
        if ( is_recommend )
        {
            ok = mongo_driver->Push( __STRING__( zonerecommendlist ), flag, __STRING__( zoneid ), zone_id );
        }
        else
        {
            ok = mongo_driver->Pull( __STRING__( zonerecommendlist ), flag, __STRING__( zoneid ), zone_id );
        }

        return ok;
    }

    uint32 KFDirDatabaseMongo::GetZoneRecommend( const std::string& flag )
    {
        auto zone_id = 0u;
        auto mongo_driver = __DIR_MONGO_DRIVER__;
        auto result = mongo_driver->QueryListUInt64( __STRING__( zonerecommendlist ), flag, __STRING__( zoneid ) );
        if ( result->_value.empty() )
        {
            auto size = ( uint32 )result->_value.size();
            auto index = rand() % size;
            auto iter = result->_value.begin();
            std::advance( iter, index );
            zone_id = *iter;
        }

        return zone_id;
    }

    uint32 KFDirDatabaseMongo::BalanceAllocZone( const std::string& flag )
    {
        auto zone_id = GetZoneRecommend( flag );
        if ( zone_id != _invalid_int )
        {
            return zone_id;
        }
        // 找一个人数最少的区
        auto mongo_driver = __DIR_MONGO_DRIVER__;

        // 数量升序显示
        KFMongoSelector selector_data;
        selector_data.AddReturn( __STRING__( zoneid ), MongoKeyword::_asc );
        selector_data.AddReturn( __STRING__( count ), MongoKeyword::_asc );
        auto result = mongo_driver->QueryListRecord( __STRING__( zone ), selector_data );
        for ( auto& db_value : result->_value )
        {
            auto zone_id = db_value.FindValue( __STRING__( zoneid ) );
            auto result = mongo_driver->QueryRecord( __STRING__( gate ), zone_id );
            if ( !result->_value.IsEmpty() )
            {
                return zone_id;
            }
        }

        return 1u;
    }

    StringMap KFDirDatabaseMongo::AllocPlayerZone( const std::string& flag, uint32 zone_id )
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

    bool KFDirDatabaseMongo::SetWorldUrl( uint64 world_id, const std::string& url )
    {
        auto mongo_driver = __DIR_MONGO_DRIVER__;
        return mongo_driver->Insert( __STRING__( worldurl ), world_id, __STRING__( url ), url );
    }

    std::string KFDirDatabaseMongo::GetWorldUrl( uint64 world_id )
    {
        auto mongo_driver = __DIR_MONGO_DRIVER__;
        auto result = mongo_driver->QueryString( __STRING__( worldurl ), world_id, __STRING__( url ) );
        return result->_value;
    }

    // 更新masterip
    bool KFDirDatabaseMongo::UpdateMasterIp( const std::string& app_name, uint64 appid, uint32 zone_id, const std::string& ip, uint32 port, uint32 expire_time )
    {
        auto mongo_driver = __DIR_MONGO_DRIVER__;
        mongo_driver->CreateIndex( __STRING__( master ), MongoKeyword::_expire );

        KFDBValue db_value;
        db_value.AddValue( __STRING__( appname ), app_name );
        db_value.AddValue( __STRING__( appid ), appid );
        db_value.AddValue( __STRING__( zoneid ), zone_id );
        db_value.AddValue( __STRING__( ip ), ip );
        db_value.AddValue( __STRING__( port ), port );
        db_value.AddValue( MongoKeyword::_expire, expire_time );
        return mongo_driver->Insert( __STRING__( master ), appid, db_value );
    }

    StringMap KFDirDatabaseMongo::QueryMasterIp( const std::string& app_name, uint32 zone_id )
    {
        StringMap master_data;
        auto mongo_driver = __DIR_MONGO_DRIVER__;

        KFMongoSelector selector_data;
        selector_data.AddLimitReturn( MongoKeyword::_id );
        selector_data._document.AddExpression( __STRING__( appname ), MongoKeyword::_eq, app_name );
        selector_data._document.AddExpression( __STRING__( zoneid ), MongoKeyword::_eq, zone_id );
        auto result = mongo_driver->QueryListRecord( __STRING__( master ), selector_data );
        if ( !result->_value.empty() )
        {
            auto size = ( uint32 )result->_value.size();
            auto index = rand() % size;
            auto iter = result->_value.begin();
            std::advance( iter, index );

            auto& db_value = *iter;
            __DBVALUE_TO_MAP__( db_value, master_data );
        }

        return master_data;
    }

    StringMapList KFDirDatabaseMongo::QueryMasterList( const std::string& app_name, uint32 zone_id )
    {
        StringMapList master_data_list;
        auto mongo_driver = __DIR_MONGO_DRIVER__;

        KFMongoSelector selector_data;
        selector_data.AddLimitReturn( MongoKeyword::_id );
        selector_data._document.AddExpression( __STRING__( appname ), MongoKeyword::_eq, app_name );
        selector_data._document.AddExpression( __STRING__( zoneid ), MongoKeyword::_eq, zone_id );
        auto result = mongo_driver->QueryListRecord( __STRING__( master ), selector_data );
        for ( auto& db_value : result->_value )
        {
            StringMap master_data;
            __DBVALUE_TO_MAP__( db_value, master_data );
            master_data_list.emplace_back( master_data );
        }

        return master_data_list;
    }
}