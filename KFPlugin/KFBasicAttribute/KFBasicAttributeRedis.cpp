#include "KFBasicAttributeRedis.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
#define __BASIC_REDIS_DRIVER__ _kf_redis->Create( __STRING__( basic ) )

    void KFBasicAttributeRedis::UpdateBasicIntValue( uint64 playerid, uint64 serverid, const KeyValue& values )
    {
        StringMap invalues;
        for ( auto& iter : values )
        {
            invalues[ iter.first ] = __TO_STRING__( iter.second );
        }

        UpdateBasicStrValue( playerid, serverid, invalues );
    }

    void KFBasicAttributeRedis::UpdateBasicStrValue( uint64 playerid, uint64 serverid, const StringMap& values )
    {
        auto redisdriver = __BASIC_REDIS_DRIVER__;
        redisdriver->Update( values, "hmset {}:{}", __STRING__( basic ), playerid );

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
        case KFMsg::FrameOnlineStatus:
            redisdriver->Execute( "sadd {}:{} {}", __STRING__( onlinelist ), serverid, playerid );
            break;
        case KFMsg::FrameOfflineStatus:
            redisdriver->Execute( "srem {}:{} {}", __STRING__( onlinelist ), serverid, playerid );
            break;
        default:
            break;
        }
    }

    void KFBasicAttributeRedis::ClearBasicServerId( uint64 serverid )
    {
        auto redisdriver = __BASIC_REDIS_DRIVER__;
        auto kfplayerlist = redisdriver->QueryList( "smembers {}:{}", __STRING__( onlinelist ), serverid );
        if ( kfplayerlist->_value.empty() )
        {
            return;
        }

        for ( auto& id : kfplayerlist->_value )
        {
            auto queryserverid = redisdriver->QueryUInt64( "hget {}:{} {}", __STRING__( basic ), id, __STRING__( serverid ) );
            if ( queryserverid->_value == serverid )
            {
                redisdriver->Execute( "hmset {}:{} {} {} {} {}", __STRING__( basic ), id,
                                      __STRING__( serverid ), 0,
                                      __STRING__( status ), KFMsg::FrameOfflineStatus );
            }

        }

        redisdriver->Execute( "del {}:{}", __STRING__( onlinelist ), serverid );
    }

    uint64 KFBasicAttributeRedis::QueryBasicServerId( uint64 playerid )
    {
        auto redisdriver = __BASIC_REDIS_DRIVER__;
        auto kfserverid = redisdriver->QueryUInt64( "hget {}:{} {}", __STRING__( basic ), playerid, __STRING__( serverid ) );
        return kfserverid->_value;
    }

    uint32 KFBasicAttributeRedis::QueryBasicAttribute( uint64 playerid, StringMap& values )
    {
        auto redisdriver = __BASIC_REDIS_DRIVER__;
        auto kfquery = redisdriver->QueryMap( "hgetall {}:{}", __STRING__( basic ), playerid );
        if ( !kfquery->IsOk() )
        {
            return KFMsg::PublicDatabaseBusy;
        }

        values.swap( kfquery->_value );
        return KFMsg::Ok;
    }

    std::string KFBasicAttributeRedis::FormatNameKey( const std::string& name, uint32 zoneid )
    {
        return __FORMAT__( "{}:{}:{}:{}", __STRING__( player ), __STRING__( name ), zoneid, name );
    }

    uint32 KFBasicAttributeRedis::SetPlayerName( uint32 zoneid, uint64 playerid, const std::string& oldname, const std::string& newname )
    {
        auto redisdriver = __BASIC_REDIS_DRIVER__;

        auto newnamekey = FormatNameKey( newname, zoneid );
        auto kfplayerid = redisdriver->QueryUInt64( "get {}", newnamekey );
        if ( !kfplayerid->IsOk() )
        {
            return KFMsg::NameDatabaseBusy;
        }

        // 如果不存在, 设置新名字
        if ( kfplayerid->_value == _invalid_int )
        {
            // 保存名字
            auto kfresult = redisdriver->Execute( "set {} {}", newnamekey, playerid );
            if ( !kfresult->IsOk() )
            {
                return KFMsg::NameDatabaseBusy;
            }

            // 删除旧的名字关联
            if ( !oldname.empty() )
            {
                auto oldnamekey = FormatNameKey( oldname, zoneid );
                redisdriver->Execute( "del {}", oldnamekey );
            }
        }
        else if ( kfplayerid->_value != playerid )
        {
            // 存在, 并且不是设定者
            return KFMsg::NameAlreadyExist;
        }

        return KFMsg::NameSetOk;
    }

    uint64 KFBasicAttributeRedis::QueryBasicPlayerid( const std::string& playername, uint32 zoneid )
    {
        auto redisdriver = __BASIC_REDIS_DRIVER__;

        auto namekey = FormatNameKey( playername, zoneid );
        auto kfplayerid = redisdriver->QueryUInt64( "get {}", namekey );
        return kfplayerid->_value;
    }
}