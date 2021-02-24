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

        KFMongoSelector kfselector;
        kfselector._document.AddExpression( __STRING__( server_id ), MongoKeyword::_eq, server_id );
        mongo_driver->Insert( __BASIC_TABLE_NAME__, __STRING__( server_id ), 0u, kfselector );
    }

    uint64 KFBasicDatabaseMongo::QueryBasicServerId( uint64 player_id )
    {
        return QueryBasicIntValue( player_id, __STRING__( server_id ) );
    }

    uint32 KFBasicDatabaseMongo::QueryBasicAttribute( uint64 player_id, StringMap& values )
    {
        auto mongo_driver = __BASIC_MONGO_DRIVER__;
        auto kfresult = mongo_driver->QueryRecord( __BASIC_TABLE_NAME__, player_id );
        if ( !kfresult->IsOk() )
        {
            return KFMsg::PublicDatabaseBusy;
        }

        __DBVALUE_TO_MAP__( kfresult->_value, values );
        return KFMsg::Ok;
    }

    std::string KFBasicDatabaseMongo::FormatNameTable( uint32 zone_id )
    {
        return __FORMAT__( "{}:{}", __STRING__( playername ), zone_id );
    }

    uint32 KFBasicDatabaseMongo::SetPlayerName( uint32 zone_id, uint64 player_id, const std::string& oldname, const std::string& newname )
    {
        auto mongo_driver = __BASIC_MONGO_DRIVER__;

        auto nametable = FormatNameTable( zone_id );
        auto kfplayerid = mongo_driver->QueryUInt64( nametable, newname, __STRING__( playerid ) );
        if ( !kfplayerid->IsOk() )
        {
            return KFMsg::NameDatabaseBusy;
        }

        // 如果不存在, 设置新名字
        if ( kfplayerid->_value == _invalid_int )
        {
            // 保存名字
            auto result = mongo_driver->Insert( nametable, newname, __STRING__( playerid ), player_id );
            if ( !result )
            {
                return KFMsg::NameDatabaseBusy;
            }

            // 删除旧的名字关联
            if ( !oldname.empty() )
            {
                mongo_driver->Delete( nametable, oldname );
            }
        }
        else if ( kfplayerid->_value != player_id )
        {
            // 存在, 并且不是设定者
            return KFMsg::NameAlreadyExist;
        }

        return KFMsg::NameSetOk;
    }

    uint64 KFBasicDatabaseMongo::QueryBasicPlayerid( const std::string& playername, uint32 zone_id )
    {
        auto mongo_driver = __BASIC_MONGO_DRIVER__;

        auto nametable = FormatNameTable( zone_id );
        auto kfplayerid = mongo_driver->QueryUInt64( nametable, playername, __STRING__( playerid ) );
        return kfplayerid->_value;
    }

    uint64 KFBasicDatabaseMongo::QueryBasicIntValue( uint64 player_id, const std::string& data_name )
    {
        auto mongo_driver = __BASIC_MONGO_DRIVER__;
        auto kfresult = mongo_driver->QueryUInt64( __BASIC_TABLE_NAME__, player_id, data_name );
        return kfresult->_value;
    }

    std::string KFBasicDatabaseMongo::QueryBasicStrValue( uint64 player_id, const std::string& data_name )
    {
        auto mongo_driver = __BASIC_MONGO_DRIVER__;
        auto kfresult = mongo_driver->QueryString( __BASIC_TABLE_NAME__, player_id, data_name );
        return kfresult->_value;
    }
}