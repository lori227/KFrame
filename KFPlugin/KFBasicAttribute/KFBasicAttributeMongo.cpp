#include "KFBasicAttributeMongo.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
#define __BASIC_TABLE_NAME__ __STRING__( basic )
#define __BASIC_MONGO_DRIVER__ _kf_mongo->Create( __STRING__( basic ) )

    void KFBasicAttributeMongo::UpdateBasicIntValue( uint64 playerid, uint64 serverid, const KeyValue& values )
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
        auto mongodriver = __BASIC_MONGO_DRIVER__;
        auto kfresult = mongodriver->QueryUInt64( __BASIC_TABLE_NAME__, playerid, __STRING__( serverid ) );
        return kfresult->_value;
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

    std::string KFBasicAttributeMongo::FormatNameKey( const std::string& name, uint32 zoneid )
    {
        return __FORMAT__( "{}:{}", zoneid, name );
    }

    uint32 KFBasicAttributeMongo::SetPlayerName( uint32 zoneid, uint64 playerid, const std::string& oldname, const std::string& newname )
    {
        auto mongodriver = __BASIC_MONGO_DRIVER__;

        auto newnamekey = FormatNameKey( newname, zoneid );
        auto kfplayerid = mongodriver->QueryUInt64( __STRING__( playername ), newnamekey, __STRING__( playerid ) );
        if ( !kfplayerid->IsOk() )
        {
            return KFMsg::NameDatabaseBusy;
        }

        // 如果不存在, 设置新名字
        if ( kfplayerid->_value == _invalid_int )
        {
            // 保存名字
            auto result = mongodriver->Insert( __STRING__( playername ), newnamekey, __STRING__( playerid ), playerid );
            if ( !result )
            {
                return KFMsg::NameDatabaseBusy;
            }

            // 删除旧的名字关联
            if ( !oldname.empty() )
            {
                auto oldnamekey = FormatNameKey( oldname, zoneid );
                mongodriver->Delete( __STRING__( name ), oldnamekey );
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

        auto namekey = FormatNameKey( playername, zoneid );
        auto kfplayerid = mongodriver->QueryUInt64( __STRING__( playername ), namekey, __STRING__( playerid ) );
        return kfplayerid->_value;
    }
}