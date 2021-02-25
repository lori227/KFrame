#include "KFBasicDatabaseMongo.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
#define __BASIC_TABLE_NAME__ __STRING__( basic )
#define __BASIC_MONGO_DRIVER__ _kf_mongo->Create( __STRING__( basic ) )

    void KFBasicDatabaseMongo::UpdateBasicIntValue( uint64 player_id, uint64 server_id, const StringUInt64& values )
    {
        auto mongo_driver = __BASIC_MONGO_DRIVER__;

        KFDBValue dbvalue;
        for ( auto& iter : values )
        {
            dbvalue.AddValue( iter.first, iter.second );
        }
        mongo_driver->Insert( __BASIC_TABLE_NAME__, player_id, dbvalue );
    }

    void KFBasicDatabaseMongo::UpdateBasicStrValue( uint64 player_id, uint64 server_id, const StringMap& values )
    {
        auto mongo_driver = __BASIC_MONGO_DRIVER__;

        KFDBValue dbvalue;
        for ( auto& iter : values )
        {
            dbvalue.AddValue( iter.first, iter.second );
        }
        mongo_driver->Insert( __BASIC_TABLE_NAME__, player_id, dbvalue );
    }

    void KFBasicDatabaseMongo::ClearBasicServerId( uint64 server_id )
    {
        auto mongo_driver = __BASIC_MONGO_DRIVER__;

        KFMongoSelector selector_data;
        selector_data._document.AddExpression( __STRING__( serverid ), MongoKeyword::_eq, server_id );
        mongo_driver->Insert( __BASIC_TABLE_NAME__, __STRING__( serverid ), 0u, selector_data );
    }

    uint64 KFBasicDatabaseMongo::QueryBasicServerId( uint64 player_id )
    {
        return QueryBasicIntValue( player_id, __STRING__( serverid ) );
    }

    uint32 KFBasicDatabaseMongo::QueryBasicAttribute( uint64 player_id, StringMap& values )
    {
        auto mongo_driver = __BASIC_MONGO_DRIVER__;
        auto result = mongo_driver->QueryRecord( __BASIC_TABLE_NAME__, player_id );
        if ( !result->IsOk() )
        {
            return KFMsg::PublicDatabaseBusy;
        }

        __DBVALUE_TO_MAP__( result->_value, values );
        return KFMsg::Ok;
    }

    std::string KFBasicDatabaseMongo::FormatNameTable( uint32 zone_id )
    {
        return __FORMAT__( "{}:{}", __STRING__( playername ), zone_id );
    }

    uint32 KFBasicDatabaseMongo::SetPlayerName( uint32 zone_id, uint64 player_id, const std::string& old_name, const std::string& new_name )
    {
        auto mongo_driver = __BASIC_MONGO_DRIVER__;

        auto name_table = FormatNameTable( zone_id );
        auto query_player_id_result = mongo_driver->QueryUInt64( name_table, new_name, __STRING__( playerid ) );
        if ( !query_player_id_result->IsOk() )
        {
            return KFMsg::NameDatabaseBusy;
        }

        // 如果不存在, 设置新名字
        if ( query_player_id_result->_value == _invalid_int )
        {
            // 保存名字
            auto result = mongo_driver->Insert( name_table, new_name, __STRING__( playerid ), player_id );
            if ( !result )
            {
                return KFMsg::NameDatabaseBusy;
            }

            // 删除旧的名字关联
            if ( !old_name.empty() )
            {
                mongo_driver->Delete( name_table, old_name );
            }
        }
        else if ( query_player_id_result->_value != player_id )
        {
            // 存在, 并且不是设定者
            return KFMsg::NameAlreadyExist;
        }

        return KFMsg::NameSetOk;
    }

    uint64 KFBasicDatabaseMongo::QueryBasicPlayerId( const std::string& player_name, uint32 zone_id )
    {
        auto mongo_driver = __BASIC_MONGO_DRIVER__;

        auto name_table = FormatNameTable( zone_id );
        auto query_player_id_result = mongo_driver->QueryUInt64( name_table, player_name, __STRING__( playerid ) );
        return query_player_id_result->_value;
    }

    uint64 KFBasicDatabaseMongo::QueryBasicIntValue( uint64 player_id, const std::string& data_name )
    {
        auto mongo_driver = __BASIC_MONGO_DRIVER__;
        auto result = mongo_driver->QueryUInt64( __BASIC_TABLE_NAME__, player_id, data_name );
        return result->_value;
    }

    std::string KFBasicDatabaseMongo::QueryBasicStrValue( uint64 player_id, const std::string& data_name )
    {
        auto mongo_driver = __BASIC_MONGO_DRIVER__;
        auto result = mongo_driver->QueryString( __BASIC_TABLE_NAME__, player_id, data_name );
        return result->_value;
    }
}