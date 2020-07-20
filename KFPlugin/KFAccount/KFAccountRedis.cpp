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
        auto straccountkey = __DATABASE_KEY_3__( __STRING__( account ), account, channel );
        auto kfquery = redisdriver->HGetUInt64( straccountkey, __STRING__( accountid ) );
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

        redisdriver->WriteMulti();
        redisdriver->SAdd( __STRING__( accountlist ), accountid );
        redisdriver->HSet( straccountkey, __STRING__( accountid ), accountid );
        redisdriver->HMSet( __DATABASE_KEY_2__( __STRING__( accountid ), accountid ), accountdata );
        auto kfresult = redisdriver->WriteExec();
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

        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;

        auto& kfextend = channeldata[ __STRING__( extend ) ];
        StringMap values;
        __JSON_TO_MAP__( kfextend, values );
        redisdriver->HMSet( __DATABASE_KEY_2__( __STRING__( extend ), accountid ), values );
    }

    StringMap KFAccountRedis::QueryChannelData( uint64 accountid )
    {
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        auto kfresult = redisdriver->HGetAll( __DATABASE_KEY_2__( __STRING__( extend ), accountid ) );
        return kfresult->_value;
    }

    std::string KFAccountRedis::MakeAccountToken( const std::string& account, uint32 channel, uint64 accountid, uint32 expiretime )
    {
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        auto tokenkey = __FORMAT__( "{}:{}", __STRING__( token ), accountid );
        auto token = KFAccountLogic::MakeAccountToken( account, channel, accountid, expiretime );

        StringMap values;
        values[ __STRING__( token ) ] = token;
        values[ __STRING__( account ) ] = account;
        values[ __STRING__( channel ) ] = __TO_STRING__( channel );

        redisdriver->WriteMulti();
        redisdriver->HMSet( tokenkey, values );
        redisdriver->Expire( tokenkey, expiretime );
        redisdriver->WriteExec();
        return token;
    }

    StringMap KFAccountRedis::VerifyAccountToken( uint64 accountid, const std::string& token )
    {
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        auto kftokenresult = redisdriver->HGetAll( __DATABASE_KEY_2__( __STRING__( token ), accountid ) );
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

        redisdriver->WriteMulti();
        redisdriver->HMSet( tokenkey, values );
        redisdriver->Expire( tokenkey, expiretime );
        redisdriver->WriteExec();
    }

    StringMap KFAccountRedis::VerifyZoneToken( uint64 accountid, uint32 zoneid, const std::string& token )
    {
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        auto kftoken = redisdriver->HGetAll( __DATABASE_KEY_3__( __STRING__( token ), zoneid, accountid ) );
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
        StringMap values;
        values[ __STRING__( ip ) ] = ip;
        values[ __STRING__( zoneid ) ] = __TO_STRING__( zoneid );

        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        redisdriver->HMSet( __DATABASE_KEY_2__( __STRING__( accountid ), accountid ), values );
    }

    std::tuple<uint64, bool> KFAccountRedis::QueryCreatePlayer( uint64 accountid, uint32 zoneid )
    {
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;

        // 查询是否存在
        auto queryplayerid = redisdriver->HGetUInt64( __DATABASE_KEY_2__( __STRING__( accountid ), accountid ), __DATABASE_KEY_2__( __STRING__( zone ), zoneid ) );
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

        // 保存玩家id信息
        redisdriver->WriteMulti();
        redisdriver->HSet( __DATABASE_KEY_2__( __STRING__( player ), playerid ), __STRING__( accountid ), accountid );
        redisdriver->HSet( __DATABASE_KEY_2__( __STRING__( accountid ), accountid ), __DATABASE_KEY_2__( __STRING__( zone ), zoneid ), playerid );
        auto kfresult = redisdriver->WriteExec();
        if ( !kfresult->IsOk() )
        {
            return std::make_tuple( _invalid_int, false );
        }

        return std::make_tuple( playerid, true );
    }

    // 更新玩家在线信息
    bool KFAccountRedis::UpdateOnline( uint64 accountid, uint64 playerid, uint64 worldid, uint64 gameid )
    {
        StringMap values;
        values[ __STRING__( game ) ] = __TO_STRING__( gameid );
        values[ __STRING__( world ) ] = __TO_STRING__( worldid );
        values[ __STRING__( playerid ) ] = __TO_STRING__( playerid );

        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        auto kfresult = redisdriver->HMSet( __DATABASE_KEY_2__( __STRING__( accountid ), accountid ), values );
        return kfresult->IsOk();
    }

    void KFAccountRedis::SaveWeiXinAccessToken( const std::string& machinecode, const std::string& openid,
            const std::string& scope, const std::string& accesstoken, uint32 expirestime )
    {
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        auto strtokenkey = __DATABASE_KEY_2__( __STRING__( access_token ), machinecode );

        StringMap values;
        values[ __STRING__( scope ) ] = scope;
        values[ __STRING__( openid ) ] = openid;
        values[ __STRING__( token ) ] = accesstoken;

        redisdriver->WriteMulti();
        redisdriver->HMSet( strtokenkey, values );
        redisdriver->Expire( strtokenkey, expirestime - 200 );
        redisdriver->WriteExec();
    }

    StringMap KFAccountRedis::QueryWeiXinAccessToken( const std::string& machinecode )
    {
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        auto kfresult = redisdriver->HGetAll( __DATABASE_KEY_2__( __STRING__( access_token ), machinecode ) );
        return kfresult->_value;
    }

    void KFAccountRedis::SaveWeiXinRefreshToken( const std::string& machinecode, const std::string& refreshtoken )
    {
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        auto strtokenkey = __DATABASE_KEY_2__( __STRING__( refresh_token ), machinecode );

        redisdriver->WriteMulti();
        redisdriver->HSet( strtokenkey, __STRING__( token ), refreshtoken );
        redisdriver->Expire( strtokenkey, 2590000 );
        redisdriver->WriteExec();
    }

    std::string KFAccountRedis::QueryWeiXinRefreshToken( const std::string& machinecode )
    {
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        auto kfresult = redisdriver->HGet( __DATABASE_KEY_2__( __STRING__( refresh_token ), machinecode ), __STRING__( token ) );
        return kfresult->_value;
    }

    uint64 KFAccountRedis::CheckIpInBlackList( const std::string& ip )
    {
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        auto kfresult = redisdriver->HGetUInt64( __DATABASE_KEY_2__( __STRING__( ipblacklist ), ip ), __STRING__( endtime ) );
        return kfresult->_value;
    }

    bool KFAccountRedis::AddIpBlackList( const std::string& ip, uint64 time, const std::string& comment )
    {
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        auto stripkey = __DATABASE_KEY_2__( __STRING__( ipblacklist ), ip );

        StringMap values;
        values[ __STRING__( comment ) ] = comment;
        values[ __STRING__( starttime ) ] = __TO_STRING__( KFGlobal::Instance()->_real_time );
        values[ __STRING__( endtime ) ] = __TO_STRING__( KFGlobal::Instance()->_real_time + time );

        redisdriver->WriteMulti();
        redisdriver->HMSet( stripkey, values );
        redisdriver->Expire( stripkey, time );
        redisdriver->SAdd( __STRING__( ipblacklist ), ip );
        auto kfresult = redisdriver->WriteExec();
        return kfresult->IsOk();
    }

    bool KFAccountRedis::RemoveIpBlackList( const std::string& ip )
    {
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;

        redisdriver->WriteMulti();
        redisdriver->SRem( __STRING__( ipblacklist ), ip );
        redisdriver->Del( __DATABASE_KEY_2__( __STRING__( ipblacklist ), ip ) );
        auto kfresult = redisdriver->WriteExec();
        return kfresult->IsOk();
    }

    StringMapList KFAccountRedis::QueryIpBlackList()
    {
        StringMapList stringmaplist;
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        auto kflist = redisdriver->SMembers( __STRING__( ipblacklist ) );

        StringList removes;
        for ( auto& ip : kflist->_value )
        {
            auto kfdata = redisdriver->HGetAll( __DATABASE_KEY_2__( __STRING__( ipblacklist ), ip ) );
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
            redisdriver->SRem( __STRING__( ipblacklist ), removes );
        }

        return stringmaplist;
    }

    bool KFAccountRedis::CheckIpInWhiteList( const std::string& ip )
    {
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        auto kfresult = redisdriver->HGetUInt64( __DATABASE_KEY_2__( __STRING__( ipwhitelist ), ip ), __STRING__( endtime ) );
        return kfresult->_value > 0u;
    }

    bool KFAccountRedis::AddIpWhiteList( const std::string& ip, uint64 time, const std::string& comment )
    {
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        auto stripkey = __DATABASE_KEY_2__( __STRING__( ipwhitelist ), ip );

        StringMap values;
        values[ __STRING__( comment ) ] = comment;
        values[ __STRING__( starttime ) ] = __TO_STRING__( KFGlobal::Instance()->_real_time );
        values[ __STRING__( endtime ) ] = __TO_STRING__( KFGlobal::Instance()->_real_time + time );

        redisdriver->WriteMulti();
        redisdriver->HMSet( stripkey, values );
        redisdriver->Expire( stripkey, time );
        redisdriver->SAdd( __STRING__( ipwhitelist ), ip );
        auto kfresult = redisdriver->WriteExec();
        return kfresult->IsOk();
    }

    bool KFAccountRedis::RemoveIpWhiteList( const std::string& ip )
    {
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        redisdriver->WriteMulti();
        redisdriver->SRem( __STRING__( ipwhitelist ), ip );
        redisdriver->Del( __DATABASE_KEY_2__( __STRING__( ipwhitelist ), ip ) );
        auto kfresult = redisdriver->WriteExec();
        return kfresult->IsOk();
    }

    StringMapList KFAccountRedis::QueryIpWhiteList()
    {
        StringMapList stringmaplist;
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        auto kflist = redisdriver->SMembers( __STRING__( ipwhitelist ) );

        StringList removes;
        for ( auto& ip : kflist->_value )
        {
            auto kfdata = redisdriver->HGetAll( __DATABASE_KEY_2__( __STRING__( ipwhitelist ), ip ) );
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
            redisdriver->SRem( __STRING__( ipwhitelist ), removes );
        }

        return stringmaplist;
    }

    uint64 KFAccountRedis::CheckAccountInBlackList( uint64 accountid )
    {
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        auto kfresult = redisdriver->HGetUInt64( __DATABASE_KEY_2__(  __STRING__( accountblacklist ), accountid ), __STRING__( endtime ) );
        return kfresult->_value;
    }

    bool KFAccountRedis::AddAccountBlackList( uint64 accountid, const std::string& account, uint32 channel, uint64 time, const std::string& comment )
    {
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        auto straccountkey = __DATABASE_KEY_2__( __STRING__( accountblacklist ), accountid );

        StringMap values;
        values[ __STRING__( comment ) ] = comment;
        values[ __STRING__( account ) ] = account;
        values[ __STRING__( channel ) ] = __TO_STRING__( channel );
        values[ __STRING__( starttime ) ] = __TO_STRING__( KFGlobal::Instance()->_real_time );
        values[ __STRING__( endtime ) ] = __TO_STRING__( KFGlobal::Instance()->_real_time + time );

        redisdriver->WriteMulti();
        redisdriver->HMSet( straccountkey, values );
        redisdriver->Expire( straccountkey, time );
        redisdriver->SAdd( __STRING__( accountblacklist ), accountid );
        auto kfresult = redisdriver->WriteExec();
        return kfresult->IsOk();
    }

    bool KFAccountRedis::RemoveAccountBlackList( uint64 accountid )
    {
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;

        redisdriver->WriteMulti();
        redisdriver->SRem( __STRING__( accountblacklist ), accountid );
        redisdriver->Del(  __DATABASE_KEY_2__( __STRING__( accountblacklist ), accountid ) );
        auto kfresult = redisdriver->WriteExec();
        return kfresult->IsOk();
    }

    StringMapList KFAccountRedis::QueryAccountBlackList()
    {
        StringMapList stringmaplist;
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        auto kflist = redisdriver->SMembers( __STRING__( accountblacklist ) );

        StringList removes;
        for ( auto& accountid : kflist->_value )
        {
            auto kfdata = redisdriver->HGetAll( __DATABASE_KEY_2__( __STRING__( accountblacklist ), accountid ) );
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
            redisdriver->SRem( __STRING__( accountblacklist ), removes );
        }

        return stringmaplist;
    }

    bool KFAccountRedis::CheckAccountInWhiteList( uint64 accountid )
    {
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        auto kfresult = redisdriver->HGetUInt64( __DATABASE_KEY_2__( __STRING__( accountwhitelist ), accountid ), __STRING__( endtime ) );
        return kfresult->_value > 0u;
    }

    bool KFAccountRedis::AddAccountWhiteList( uint64 accountid, const std::string& account, uint32 channel, uint64 time, const std::string& comment )
    {
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        auto straccountkey = __DATABASE_KEY_2__( __STRING__( accountwhitelist ), accountid );

        StringMap values;
        values[ __STRING__( comment ) ] = comment;
        values[ __STRING__( account ) ] = account;
        values[ __STRING__( channel ) ] = __TO_STRING__( channel );
        values[ __STRING__( starttime ) ] = __TO_STRING__( KFGlobal::Instance()->_real_time );
        values[ __STRING__( endtime ) ] = __TO_STRING__( KFGlobal::Instance()->_real_time + time );

        redisdriver->WriteMulti();
        redisdriver->SAdd( __STRING__( accountwhitelist ), accountid );
        redisdriver->HMSet( straccountkey, values );
        redisdriver->Expire( __DATABASE_KEY_2__( __STRING__( accountwhitelist ), accountid ), time );
        auto kfresult = redisdriver->WriteExec();
        return kfresult->IsOk();
    }

    bool KFAccountRedis::RemoveAccountWhiteList( uint64 accountid )
    {
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;

        redisdriver->WriteMulti();
        redisdriver->SRem( __STRING__( accountwhitelist ), accountid );
        redisdriver->Del( __DATABASE_KEY_2__( __STRING__( accountwhitelist ), accountid ) );
        auto kfresult = redisdriver->WriteExec();
        return kfresult->IsOk();
    }

    StringMapList KFAccountRedis::QueryAccountWhiteList()
    {
        StringMapList stringmaplist;
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        auto kflist = redisdriver->SMembers( __STRING__( accountwhitelist ) );

        StringList removes;
        for ( auto& accountid : kflist->_value )
        {
            auto kfdata = redisdriver->HGetAll( __DATABASE_KEY_2__( __STRING__( accountwhitelist ), accountid ) );
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
            redisdriver->SRem( __STRING__( accountwhitelist ), removes );
        }

        return stringmaplist;
    }

}