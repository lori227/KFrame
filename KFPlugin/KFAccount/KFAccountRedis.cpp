#include "KFAccountRedis.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
#define __ACCOUNT_REDIS_DRIVER__ _kf_redis->Create( __STRING__( account ) )

    StringMap KFAccountRedis::QueryCreateAccount( const std::string& account, uint32 channel )
    {
        StringMap accountdata;
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;

        // 先查询是否存在
        auto kfquery = redisdriver->QueryUInt64( "hget {}:{}:{} {}", __STRING__( account ), account, channel, __STRING__( accountid ) );
        if ( !kfquery->IsOk() )
        {
            __LOG_DEBUG__( "account[{}] channel[{}] query accountid failed", account, channel );
            return accountdata;
        }

        auto accountid = kfquery->_value;
        if ( accountid != _invalid_int )
        {
            auto queryaccountdata = redisdriver->QueryMap( "hgetall {}:{}", __STRING__( accountid ), accountid );
            if ( !queryaccountdata->IsOk() || !queryaccountdata->_value.empty() )
            {
                return queryaccountdata->_value;
            }
        }
        else
        {
            accountid = KFGlobal::Instance()->MTMakeUuid( __STRING__( account ) );
        }

        // 创建账号id
        accountdata[ __STRING__( account ) ] = account;
        accountdata[ __STRING__( channel ) ] = __TO_STRING__( channel );
        accountdata[ __STRING__( accountid ) ] = __TO_STRING__( accountid );

        redisdriver->Append( "hset {}:{}:{} {} {}", __STRING__( account ), account, channel, __STRING__( accountid ), accountid );
        redisdriver->Append( "sadd {} {}", __STRING__( accountlist ), accountid );
        redisdriver->Append( accountdata, "hmset {}:{}", __STRING__( accountid ), accountid );
        auto kfresult = redisdriver->Pipeline();
        if ( kfresult->IsOk() )
        {
            __LOG_DEBUG__( "create account successfully, account[{}] accountid[{}] channel[{}] ", account, accountid, channel );
        }
        else
        {
            // 失败清空数据
            accountdata.clear();
            __LOG_DEBUG__( "account[{}] channel[{}] save account failed", account, channel );
        }

        return accountdata;
    }

    void KFAccountRedis::SaveChannelData( uint64 accountid, KFJson& channeldata )
    {
        if ( !__JSON_HAS_MEMBER__( channeldata, __STRING__( extend ) ) )
        {
            return;
        }

        auto& kfextend = channeldata[ __STRING__( extend ) ];

        StringMap values;
        __JSON_TO_MAP__( kfextend, values );

        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        redisdriver->Update( values, "hmset {}:{}", __STRING__( extend ), accountid );
    }

    StringMap KFAccountRedis::QueryChannelData( uint64 accountid )
    {
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        auto kfresult = redisdriver->QueryMap( "hgetall {}:{}", __STRING__( extend ), accountid );
        return kfresult->_value;
    }

    std::string KFAccountRedis::MakeAccountToken( const std::string& account, uint32 channel, uint64 accountid, uint32 expiretime )
    {
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        auto token = KFAccountLogic::MakeAccountToken( account, channel, accountid, expiretime );

        auto tokenkey = __FORMAT__( "{}:{}", __STRING__( token ), accountid );
        redisdriver->Append( "hmset {} {} {} {} {} {} {}",
                             tokenkey,
                             __STRING__( token ), token,
                             __STRING__( account ), account,
                             __STRING__( channel ), channel );
        redisdriver->Append( "expire {} {}", tokenkey, expiretime );
        redisdriver->Pipeline();
        return token;
    }

    StringMap KFAccountRedis::VerifyAccountToken( uint64 accountid, const std::string& token )
    {
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        auto kftokenresult = redisdriver->QueryMap( "hgetall {}:{}", __STRING__( token ), accountid );
        if ( !kftokenresult->_value.empty() )
        {
            auto querytoken = kftokenresult->_value[ __STRING__( token ) ];
            if ( querytoken == token )
            {
                return kftokenresult->_value;
            }
        }

        static StringMap _result;
        return _result;
    }

    void KFAccountRedis::SaveZoneToken( const std::string& account, uint32 channel, uint64 accountid, uint32 zoneid, const std::string& token, uint32 expiretime )
    {
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        auto tokenkey = __FORMAT__( "{}:{}:{}", __STRING__( token ), zoneid, accountid );

        // 保存验证信息
        StringMap values;
        values[ __STRING__( token ) ] = token;
        values[ __STRING__( account ) ] = account;
        values[ __STRING__( channel ) ] = __TO_STRING__( channel );
        values[ __STRING__( accountid ) ] = __TO_STRING__( accountid );
        redisdriver->Append( values, "hmset {}", tokenkey );
        redisdriver->Append( "expire {} {}", tokenkey, expiretime );
        redisdriver->Pipeline();
    }

    StringMap KFAccountRedis::VerifyZoneToken( uint64 accountid, uint32 zoneid, const std::string& token )
    {
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        auto kftoken = redisdriver->QueryMap( "hgetall {}:{}:{}", __STRING__( token ), zoneid, accountid );
        if ( !kftoken->_value.empty() )
        {
            auto querytoken = kftoken->_value[ __STRING__( token ) ];
            if ( querytoken == token )
            {
                return kftoken->_value;
            }
        }

        static StringMap _result;
        return _result;
    }

    void KFAccountRedis::SaveLoginData( uint64 accountid, const std::string& ip, uint32 zoneid )
    {
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        redisdriver->Execute( "hmset {}:{} {} {} {} {}",
                              __STRING__( accountid ), accountid,
                              __STRING__( ip ), ip,
                              __STRING__( zoneid ), zoneid );
    }

    std::tuple<uint64, bool> KFAccountRedis::QueryCreatePlayer( uint64 accountid, uint32 zoneid )
    {
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;

        // 查询是否存在
        auto queryplayerid = redisdriver->QueryUInt64( "hget {}:{} {}:{}", __STRING__( accountid ), accountid, __STRING__( zone ), zoneid );
        if ( !queryplayerid->IsOk() )
        {
            return std::make_tuple( _invalid_int, false );
        }

        // 存在playerid, 直接返回
        if ( queryplayerid->_value != _invalid_int )
        {
            return std::make_tuple( queryplayerid->_value, false );
        }

        // 创建playerid
        auto playerid = KFGlobal::Instance()->MTMakeUuid( __STRING__( player ), zoneid );

        redisdriver->Execute( "hset {}:{} {} {}", __STRING__( player ), playerid, __STRING__( accountid ), accountid );
        auto voidresult = redisdriver->Execute( "hset {}:{} {}:{} {}", __STRING__( accountid ), accountid, __STRING__( zone ), zoneid, playerid );
        if ( !voidresult->IsOk() )
        {
            return std::make_tuple( _invalid_int, false );
        }

        return std::make_tuple( playerid, true );
    }

    // 更新玩家在线信息
    bool KFAccountRedis::UpdateOnline( uint64 accountid, uint64 playerid, uint64 worldid, uint64 gameid )
    {
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        auto kfresult = redisdriver->Execute( "hmset {}:{} {} {} {} {} {} {}",
                                              __STRING__( accountid ), accountid,
                                              __STRING__( playerid ), playerid,
                                              __STRING__( world ), worldid,
                                              __STRING__( game ), gameid );

        return kfresult->IsOk();
    }

    void KFAccountRedis::SaveWeiXinAccessToken( const std::string& machinecode, const std::string& openid,
            const std::string& scope, const std::string& accesstoken, uint32 expirestime )
    {
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        redisdriver->Append( "hmset {}:{} {} {} {} {} {} {}",
                             __STRING__( access_token ), machinecode, __STRING__( token ), accesstoken,
                             __STRING__( openid ), openid, __STRING__( scope ), scope );

        redisdriver->Append( "expire {}:{} {}", __STRING__( access_token ), machinecode, expirestime - 200 );
        redisdriver->Pipeline();
    }

    StringMap KFAccountRedis::QueryWeiXinAccessToken( const std::string& machinecode )
    {
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        auto kfresult = redisdriver->QueryMap( "hgetall {}:{}", __STRING__( access_token ), machinecode );
        return kfresult->_value;
    }

    void KFAccountRedis::SaveWeiXinRefreshToken( const std::string& machinecode, const std::string& refreshtoken )
    {
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        redisdriver->Append( "hset {}:{} {} {}", __STRING__( refresh_token ), machinecode, __STRING__( token ), refreshtoken );
        redisdriver->Append( "expire {}:{} {}", __STRING__( refresh_token ), machinecode, 2590000 );
        redisdriver->Pipeline();
    }

    std::string KFAccountRedis::QueryWeiXinRefreshToken( const std::string& machinecode )
    {
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        auto kfresult = redisdriver->QueryString( "hget {}:{} {}", __STRING__( refresh_token ), machinecode, __STRING__( token ) );
        return kfresult->_value;
    }

    uint64 KFAccountRedis::CheckIpInBlackList( const std::string& ip )
    {
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        auto kfresult = redisdriver->QueryUInt64( "hget {}:{} {}", __STRING__( ipblacklist ), ip, __STRING__( endtime ) );
        return kfresult->_value;
    }

    bool KFAccountRedis::AddIpBlackList( const std::string& ip, uint64 time, const std::string& comment )
    {
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;

        redisdriver->Append( "sadd {} {}", __STRING__( ipblacklist ), ip );
        redisdriver->Append( "hmset {}:{} {} {} {} {} {} {}",
                             __STRING__( ipblacklist ), ip,
                             __STRING__( starttime ), KFGlobal::Instance()->_real_time,
                             __STRING__( endtime ), KFGlobal::Instance()->_real_time + time,
                             __STRING__( comment ), comment );
        redisdriver->Append( "expire {}:{} {}", __STRING__( ipblacklist ), ip, time );
        auto kfresult = redisdriver->Pipeline();
        return kfresult->IsOk();
    }

    bool KFAccountRedis::RemoveIpBlackList( const std::string& ip )
    {
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        redisdriver->Append( "srem {} {}", __STRING__( ipblacklist ), ip );
        redisdriver->Append( "del {}:{}", __STRING__( ipblacklist ), ip );
        auto kfresult = redisdriver->Pipeline();
        return kfresult->IsOk();
    }

    StringMapList KFAccountRedis::QueryIpBlackList()
    {
        StringMapList stringmaplist;
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        auto kflist = redisdriver->QueryList( "smembers {}", __STRING__( ipblacklist ) );

        StringList removes;
        for ( auto& ip : kflist->_value )
        {
            auto kfdata = redisdriver->QueryMap( "hgetall {}:{}", __STRING__( ipblacklist ), ip );
            if ( !kfdata->IsOk() )
            {
                continue;
            }

            if ( kfdata->_value.empty() )
            {
                removes.push_back( ip );
                continue;
            }


            StringMap values;
            values = kfdata->_value;
            stringmaplist.emplace_back( values );
        }

        if ( !removes.empty() )
        {
            redisdriver->Update( removes, "srem {}", __STRING__( ipblacklist ) );
        }

        return stringmaplist;
    }

    bool KFAccountRedis::CheckIpInWhiteList( const std::string& ip )
    {
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        auto kfresult = redisdriver->QueryUInt64( "hget {}:{} {}", __STRING__( ipwhitelist ), ip, __STRING__( endtime ) );
        return kfresult->_value > 0u;
    }

    bool KFAccountRedis::AddIpWhiteList( const std::string& ip, uint64 time, const std::string& comment )
    {
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;

        redisdriver->Append( "sadd {} {}", __STRING__( ipwhitelist ), ip );
        redisdriver->Append( "hmset {}:{} {} {} {} {} {} {}",
                             __STRING__( ipwhitelist ), ip,
                             __STRING__( starttime ), KFGlobal::Instance()->_real_time,
                             __STRING__( endtime ), KFGlobal::Instance()->_real_time + time,
                             __STRING__( comment ), comment );
        redisdriver->Append( "expire {}:{} {}", __STRING__( ipwhitelist ), ip, time );
        auto kfresult = redisdriver->Pipeline();
        return kfresult->IsOk();
    }

    bool KFAccountRedis::RemoveIpWhiteList( const std::string& ip )
    {
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        redisdriver->Append( "srem {} {}", __STRING__( ipwhitelist ), ip );
        redisdriver->Append( "del {}:{}", __STRING__( ipwhitelist ), ip );
        auto kfresult = redisdriver->Pipeline();
        return kfresult->IsOk();
    }

    StringMapList KFAccountRedis::QueryIpWhiteList()
    {
        StringMapList stringmaplist;
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        auto kflist = redisdriver->QueryList( "smembers {}", __STRING__( ipwhitelist ) );

        StringList removes;
        for ( auto& ip : kflist->_value )
        {
            auto kfdata = redisdriver->QueryMap( "hgetall {}:{}", __STRING__( ipwhitelist ), ip );
            if ( !kfdata->IsOk() )
            {
                continue;
            }

            if ( kfdata->_value.empty() )
            {
                removes.push_back( ip );
                continue;
            }


            StringMap values;
            values = kfdata->_value;
            stringmaplist.emplace_back( values );
        }

        if ( !removes.empty() )
        {
            redisdriver->Update( removes, "srem {}", __STRING__( ipwhitelist ) );
        }

        return stringmaplist;
    }

    uint64 KFAccountRedis::CheckAccountInBlackList( uint64 accountid )
    {
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        auto kfresult = redisdriver->QueryUInt64( "hget {}:{} {}", __STRING__( accountblacklist ), accountid, __STRING__( endtime ) );
        return kfresult->_value;
    }

    bool KFAccountRedis::AddAccountBlackList( uint64 accountid, const std::string& account, uint32 channel, uint64 time, const std::string& comment )
    {
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;

        redisdriver->Append( "sadd {} {}", __STRING__( accountblacklist ), accountid );
        redisdriver->Append( "hmset {}:{} {} {} {} {} {} {} {} {} {} {}",
                             __STRING__( accountblacklist ), accountid,
                             __STRING__( starttime ), KFGlobal::Instance()->_real_time,
                             __STRING__( endtime ), KFGlobal::Instance()->_real_time + time,
                             __STRING__( account ), account,
                             __STRING__( channel ), channel,
                             __STRING__( comment ), comment );
        redisdriver->Append( "expire {}:{} {}", __STRING__( accountblacklist ), accountid, time );
        auto kfresult = redisdriver->Pipeline();
        return kfresult->IsOk();
    }

    bool KFAccountRedis::RemoveAccountBlackList( uint64 accountid )
    {
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        redisdriver->Append( "srem {} {}", __STRING__( accountblacklist ), accountid );
        redisdriver->Append( "del {}:{}", __STRING__( accountblacklist ), accountid );
        auto kfresult = redisdriver->Pipeline();
        return kfresult->IsOk();
    }

    StringMapList KFAccountRedis::QueryAccountBlackList()
    {
        StringMapList stringmaplist;
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        auto kflist = redisdriver->QueryList( "smembers {}", __STRING__( accountblacklist ) );

        StringList removes;
        for ( auto& accountid : kflist->_value )
        {
            auto kfdata = redisdriver->QueryMap( "hgetall {}:{}", __STRING__( accountblacklist ), accountid );
            if ( !kfdata->IsOk() )
            {
                continue;
            }

            if ( kfdata->_value.empty() )
            {
                removes.push_back( accountid );
                continue;
            }


            StringMap values;
            values = kfdata->_value;
            stringmaplist.emplace_back( values );
        }

        if ( !removes.empty() )
        {
            redisdriver->Update( removes, "srem {}", __STRING__( accountblacklist ) );
        }

        return stringmaplist;
    }

    bool KFAccountRedis::CheckAccountInWhiteList( uint64 accountid )
    {
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        auto kfresult = redisdriver->QueryUInt64( "hget {}:{} {}", __STRING__( accountwhitelist ), accountid, __STRING__( endtime ) );
        return kfresult->_value > 0u;
    }

    bool KFAccountRedis::AddAccountWhiteList( uint64 accountid, const std::string& account, uint32 channel, uint64 time, const std::string& comment )
    {
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;

        redisdriver->Append( "sadd {} {}", __STRING__( accountwhitelist ), accountid );
        redisdriver->Append( "hmset {}:{} {} {} {} {} {} {} {} {} {} {}",
                             __STRING__( accountwhitelist ), accountid,
                             __STRING__( starttime ), KFGlobal::Instance()->_real_time,
                             __STRING__( endtime ), KFGlobal::Instance()->_real_time + time,
                             __STRING__( account ), account,
                             __STRING__( channel ), channel,
                             __STRING__( comment ), comment );
        redisdriver->Append( "expire {}:{} {}", __STRING__( accountwhitelist ), accountid, time );
        auto kfresult = redisdriver->Pipeline();
        return kfresult->IsOk();
    }

    bool KFAccountRedis::RemoveAccountWhiteList( uint64 accountid )
    {
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        redisdriver->Append( "srem {} {}", __STRING__( accountwhitelist ), accountid );
        redisdriver->Append( "del {}:{}", __STRING__( accountwhitelist ), accountid );
        auto kfresult = redisdriver->Pipeline();
        return kfresult->IsOk();
    }

    StringMapList KFAccountRedis::QueryAccountWhiteList()
    {
        StringMapList stringmaplist;
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        auto kflist = redisdriver->QueryList( "smembers {}", __STRING__( accountwhitelist ) );

        StringList removes;
        for ( auto& accountid : kflist->_value )
        {
            auto kfdata = redisdriver->QueryMap( "hgetall {}:{}", __STRING__( accountwhitelist ), accountid );
            if ( !kfdata->IsOk() )
            {
                continue;
            }

            if ( kfdata->_value.empty() )
            {
                removes.push_back( accountid );
                continue;
            }


            StringMap values;
            values = kfdata->_value;
            stringmaplist.emplace_back( values );
        }

        if ( !removes.empty() )
        {
            redisdriver->Update( removes, "srem {}", __STRING__( accountwhitelist ) );
        }

        return stringmaplist;
    }

}