#include "KFBasicAttributeMongo.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
#define __BASIC_TABLE_NAME__ __STRING__( basic )
#define __BASIC_MONGO_DRIVER__ _kf_mongo->Create( __STRING__( basic ) )

    void KFBasicAttributeMongo::UpdateBasicIntValue( uint64 playerid, uint64 serverid, const StringUInt64& values )
    {
        auto mongodriver = __BASIC_MONGO_DRIVER__;

        KFDBValue dbvalue;
        for ( auto& iter : values )
        {
            dbvalue.AddValue( iter.first, iter.second );
        }
        mongodriver->Insert( __BASIC_TABLE_NAME__, playerid, dbvalue );
    }

    void KFBasicAttributeMongo::UpdateBasicStrValue( uint64 playerid, uint64 serverid, const StringMap& values )
    {
        auto mongodriver = __BASIC_MONGO_DRIVER__;

        KFDBValue dbvalue;
        for ( auto& iter : values )
        {
            dbvalue.AddValue( iter.first, iter.second );
        }
        mongodriver->Insert( __BASIC_TABLE_NAME__, playerid, dbvalue );
    }

    void KFBasicAttributeMongo::ClearBasicServerId( uint64 serverid )
    {
        auto mongodriver = __BASIC_MONGO_DRIVER__;

        KFMongoSelector kfselector;
        kfselector._document.AddExpression( __STRING__( serverid ), MongoKeyword::_eq, serverid );
        mongodriver->Insert( __BASIC_TABLE_NAME__, __STRING__( serverid ), 0u, kfselector );
    }

    uint64 KFBasicAttributeMongo::QueryBasicServerId( uint64 playerid )
    {
        return QueryBasicIntValue( playerid, __STRING__( serverid ) );
    }

    uint32 KFBasicAttributeMongo::QueryBasicAttribute( uint64 playerid, StringMap& values )
    {
        auto mongodriver = __BASIC_MONGO_DRIVER__;
        auto kfresult = mongodriver->QueryRecord( __BASIC_TABLE_NAME__, playerid );
        if ( !kfresult->IsOk() )
        {
            return KFMsg::PublicDatabaseBusy;
        }

        __DBVALUE_TO_MAP__( kfresult->_value, values );
        return KFMsg::Ok;
    }

    std::string KFBasicAttributeMongo::FormatNameTable( uint32 zoneid )
    {
        return __FORMAT__( "{}:{}", __STRING__( playername ), zoneid );
    }

    uint32 KFBasicAttributeMongo::SetPlayerName( uint32 zoneid, uint64 playerid, const std::string& oldname, const std::string& newname )
    {
        auto mongodriver = __BASIC_MONGO_DRIVER__;

        auto nametable = FormatNameTable( zoneid );
        auto kfplayerid = mongodriver->QueryUInt64( nametable, newname, __STRING__( playerid ) );
        if ( !kfplayerid->IsOk() )
        {
            return KFMsg::NameDatabaseBusy;
        }

        // 如果不存在, 设置新名字
        if ( kfplayerid->_value == _invalid_int )
        {
            // 保存名字
            auto result = mongodriver->Insert( nametable, newname, __STRING__( playerid ), playerid );
            if ( !result )
            {
                return KFMsg::NameDatabaseBusy;
            }

            // 删除旧的名字关联
            if ( !oldname.empty() )
            {
                mongodriver->Delete( nametable, oldname );
            }
        }
        else if ( kfplayerid->_value != playerid )
        {
            // 存在, 并且不是设定者
            return KFMsg::NameAlreadyExist;
        }

        return KFMsg::NameSetOk;
    }

    uint64 KFBasicAttributeMongo::QueryBasicPlayerid( const std::string& playername, uint32 zoneid )
    {
        auto mongodriver = __BASIC_MONGO_DRIVER__;

        auto nametable = FormatNameTable( zoneid );
        auto kfplayerid = mongodriver->QueryUInt64( nametable, playername, __STRING__( playerid ) );
        return kfplayerid->_value;
    }

    uint64 KFBasicAttributeMongo::QueryBasicIntValue( uint64 playerid, const std::string& dataname )
    {
        auto mongodriver = __BASIC_MONGO_DRIVER__;
        auto kfresult = mongodriver->QueryUInt64( __BASIC_TABLE_NAME__, playerid, dataname );
        return kfresult->_value;
    }

    std::string KFBasicAttributeMongo::QueryBasicStrValue( uint64 playerid, const std::string& dataname )
    {
        auto mongodriver = __BASIC_MONGO_DRIVER__;
        auto kfresult = mongodriver->QueryString( __BASIC_TABLE_NAME__, playerid, dataname );
        return kfresult->_value;
    }
}