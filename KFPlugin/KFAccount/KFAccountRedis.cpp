#include "KFAccountRedis.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
#define __ACCOUNT_REDIS_DRIVER__ _kf_redis->Create( __STRING__( account ) )

    StringMap KFAccountRedis::QueryCreateAccount( const std::string& account, uint32 channel )
    {
        StringMap account_data;
        auto redis_driver = __ACCOUNT_REDIS_DRIVER__;

        // 先查询是否存在
        auto account_key = __DATABASE_KEY_3__( __STRING__( account ), account, channel );
        auto query_account_id = redis_driver->HGetUInt64( account_key, __STRING__( accountid ) );
        if ( !query_account_id->IsOk() )
        {
            __LOG_DEBUG__( "account[{}] channel[{}] query account_id failed", account, channel );
            return account_data;
        }

        auto account_id = query_account_id->_value;
        if ( account_id != _invalid_int )
        {
            auto query_account_data = redis_driver->QueryMap( "hgetall {}:{}", __STRING__( accountid ), account_id );
            if ( !query_account_data->IsOk() || !query_account_data->_value.empty() )
            {
                return query_account_data->_value;
            }
        }
        else
        {
            account_id = KFGlobal::Instance()->MTMakeUuid( __STRING__( account ) );
        }

        // 创建账号id
        account_data[ __STRING__( account ) ] = account;
        account_data[ __STRING__( channel ) ] = __TO_STRING__( channel );
        account_data[ __STRING__( accountid ) ] = __TO_STRING__( account_id );

        redis_driver->WriteMulti();
        redis_driver->SAdd( __STRING__( accountlist ), account_id );
        redis_driver->HSet( account_key, __STRING__( accountid ), account_id );
        redis_driver->HMSet( __DATABASE_KEY_2__( __STRING__( accountid ), account_id ), account_data );
        auto result = redis_driver->WriteExec();
        if ( result->IsOk() )
        {
            __LOG_DEBUG__( "create account successfully, account=[{}] account_id=[{}] channel=[{}] ", account, account_id, channel );
        }
        else
        {
            // 失败清空数据
            account_data.clear();
            __LOG_DEBUG__( "account[{}] channel[{}] save account failed", account, channel );
        }

        return account_data;
    }

    void KFAccountRedis::SaveChannelData( uint64 account_id, KFJson& channel_data )
    {
        if ( !__JSON_HAS_MEMBER__( channel_data, __STRING__( extend ) ) )
        {
            return;
        }

        auto& extend_data = channel_data[ __STRING__( extend ) ];
        StringMap values;
        __JSON_TO_MAP__( extend_data, values );

        auto redis_driver = __ACCOUNT_REDIS_DRIVER__;
        redis_driver->HMSet( __DATABASE_KEY_2__( __STRING__( extend ), account_id ), values );
    }

    StringMap KFAccountRedis::QueryChannelData( uint64 account_id )
    {
        auto redis_driver = __ACCOUNT_REDIS_DRIVER__;
        auto result = redis_driver->HGetAll( __DATABASE_KEY_2__( __STRING__( extend ), account_id ) );
        return result->_value;
    }

    std::string KFAccountRedis::MakeAccountToken( const std::string& account, uint32 channel, uint64 account_id, uint32 expire_time )
    {
        auto redis_driver = __ACCOUNT_REDIS_DRIVER__;
        auto token_key = __FORMAT__( "{}:{}", __STRING__( token ), account_id );
        auto token = KFAccountLogic::MakeAccountToken( account, channel, account_id, expire_time );

        StringMap values;
        values[ __STRING__( token ) ] = token;
        values[ __STRING__( account ) ] = account;
        values[ __STRING__( channel ) ] = __TO_STRING__( channel );

        redis_driver->WriteMulti();
        redis_driver->HMSet( token_key, values );
        redis_driver->Expire( token_key, expire_time );
        redis_driver->WriteExec();
        return token;
    }

    StringMap KFAccountRedis::VerifyAccountToken( uint64 account_id, const std::string& token )
    {
        auto redis_driver = __ACCOUNT_REDIS_DRIVER__;
        auto token_result = redis_driver->HGetAll( __DATABASE_KEY_2__( __STRING__( token ), account_id ) );
        if ( !token_result->_value.empty() )
        {
            auto query_token = token_result->_value[ __STRING__( token ) ];
            if ( query_token == token )
            {
                return token_result->_value;
            }
        }

        static StringMap _result;
        return _result;
    }

    void KFAccountRedis::SaveZoneToken( const std::string& account, uint32 channel, uint64 account_id, uint32 zone_id, const std::string& token, uint32 expire_time )
    {
        auto redis_driver = __ACCOUNT_REDIS_DRIVER__;
        auto token_key = __FORMAT__( "{}:{}:{}", __STRING__( token ), zone_id, account_id );

        // 保存验证信息
        StringMap values;
        values[ __STRING__( token ) ] = token;
        values[ __STRING__( account ) ] = account;
        values[ __STRING__( channel ) ] = __TO_STRING__( channel );
        values[ __STRING__( accountid ) ] = __TO_STRING__( account_id );

        redis_driver->WriteMulti();
        redis_driver->HMSet( token_key, values );
        redis_driver->Expire( token_key, expire_time );
        redis_driver->WriteExec();
    }

    StringMap KFAccountRedis::VerifyZoneToken( uint64 account_id, uint32 zone_id, const std::string& token )
    {
        auto redis_driver = __ACCOUNT_REDIS_DRIVER__;
        auto token_result = redis_driver->HGetAll( __DATABASE_KEY_3__( __STRING__( token ), zone_id, account_id ) );
        if ( !token_result->_value.empty() )
        {
            auto query_token = token_result->_value[ __STRING__( token ) ];
            if ( query_token == token )
            {
                return token_result->_value;
            }
        }

        static StringMap _result;
        return _result;
    }

    void KFAccountRedis::SaveLoginData( uint64 account_id, const std::string& ip, uint32 zone_id )
    {
        StringMap values;
        values[ __STRING__( ip ) ] = ip;
        values[ __STRING__( zoneid ) ] = __TO_STRING__( zone_id );

        auto redis_driver = __ACCOUNT_REDIS_DRIVER__;
        redis_driver->HMSet( __DATABASE_KEY_2__( __STRING__( accountid ), account_id ), values );
    }

    std::tuple<uint64, bool> KFAccountRedis::QueryCreatePlayer( uint64 account_id, uint32 zone_id )
    {
        auto redis_driver = __ACCOUNT_REDIS_DRIVER__;

        // 查询是否存在
        auto query_player_id = redis_driver->HGetUInt64( __DATABASE_KEY_2__( __STRING__( accountid ), account_id ), __DATABASE_KEY_2__( __STRING__( zone ), zone_id ) );
        if ( !query_player_id->IsOk() )
        {
            return std::make_tuple( _invalid_int, false );
        }

        // 存在playerid, 直接返回
        if ( query_player_id->_value != _invalid_int )
        {
            return std::make_tuple( query_player_id->_value, false );
        }

        // 创建playerid
        auto player_id = KFGlobal::Instance()->MTMakeUuid( __STRING__( player ), zone_id );

        // 保存玩家id信息
        redis_driver->WriteMulti();
        redis_driver->HSet( __DATABASE_KEY_2__( __STRING__( player ), player_id ), __STRING__( accountid ), account_id );
        redis_driver->HSet( __DATABASE_KEY_2__( __STRING__( accountid ), account_id ), __DATABASE_KEY_2__( __STRING__( zone ), zone_id ), player_id );
        auto result = redis_driver->WriteExec();
        if ( !result->IsOk() )
        {
            return std::make_tuple( _invalid_int, false );
        }

        return std::make_tuple( player_id, true );
    }

    // 更新玩家在线信息
    bool KFAccountRedis::UpdateOnline( uint64 account_id, uint64 player_id, uint64 world_id, uint64 game_id )
    {
        StringMap values;
        values[ __STRING__( game ) ] = __TO_STRING__( game_id );
        values[ __STRING__( world ) ] = __TO_STRING__( world_id );
        values[ __STRING__( playerid ) ] = __TO_STRING__( player_id );

        auto redis_driver = __ACCOUNT_REDIS_DRIVER__;
        auto result = redis_driver->HMSet( __DATABASE_KEY_2__( __STRING__( accountid ), account_id ), values );
        return result->IsOk();
    }

    void KFAccountRedis::SaveWeiXinAccessToken( const std::string& machine_code, const std::string& openid,
            const std::string& scope, const std::string& access_token, uint32 expire_time )
    {
        auto redis_driver = __ACCOUNT_REDIS_DRIVER__;
        auto token_key = __DATABASE_KEY_2__( __STRING__( access_token ), machine_code );

        StringMap values;
        values[ __STRING__( scope ) ] = scope;
        values[ __STRING__( openid ) ] = openid;
        values[ __STRING__( token ) ] = access_token;

        redis_driver->WriteMulti();
        redis_driver->HMSet( token_key, values );
        redis_driver->Expire( token_key, expire_time - 200 );
        redis_driver->WriteExec();
    }

    StringMap KFAccountRedis::QueryWeiXinAccessToken( const std::string& machine_code )
    {
        auto redis_driver = __ACCOUNT_REDIS_DRIVER__;
        auto result = redis_driver->HGetAll( __DATABASE_KEY_2__( __STRING__( access_token ), machine_code ) );
        return result->_value;
    }

    void KFAccountRedis::SaveWeiXinRefreshToken( const std::string& machine_code, const std::string& refresh_token )
    {
        auto redis_driver = __ACCOUNT_REDIS_DRIVER__;
        auto token_key = __DATABASE_KEY_2__( __STRING__( refresh_token ), machine_code );

        redis_driver->WriteMulti();
        redis_driver->HSet( token_key, __STRING__( token ), refresh_token );
        redis_driver->Expire( token_key, 2590000 );
        redis_driver->WriteExec();
    }

    std::string KFAccountRedis::QueryWeiXinRefreshToken( const std::string& machine_code )
    {
        auto redis_driver = __ACCOUNT_REDIS_DRIVER__;
        auto result = redis_driver->HGet( __DATABASE_KEY_2__( __STRING__( refresh_token ), machine_code ), __STRING__( token ) );
        return result->_value;
    }

    uint64 KFAccountRedis::CheckIpInBlackList( const std::string& ip )
    {
        auto redis_driver = __ACCOUNT_REDIS_DRIVER__;
        auto result = redis_driver->HGetUInt64( __DATABASE_KEY_2__( __STRING__( ipblacklist ), ip ), __STRING__( endtime ) );
        return result->_value;
    }

    bool KFAccountRedis::AddIpBlackList( const std::string& ip, uint64 time, const std::string& comment )
    {
        auto redis_driver = __ACCOUNT_REDIS_DRIVER__;
        auto ip_key = __DATABASE_KEY_2__( __STRING__( ipblacklist ), ip );

        StringMap values;
        values[ __STRING__( comment ) ] = comment;
        values[ __STRING__( starttime ) ] = __TO_STRING__( KFGlobal::Instance()->_real_time );
        values[ __STRING__( endtime ) ] = __TO_STRING__( KFGlobal::Instance()->_real_time + time );

        redis_driver->WriteMulti();
        redis_driver->HMSet( ip_key, values );
        redis_driver->Expire( ip_key, time );
        redis_driver->SAdd( __STRING__( ipblacklist ), ip );
        auto result = redis_driver->WriteExec();
        return result->IsOk();
    }

    bool KFAccountRedis::RemoveIpBlackList( const std::string& ip )
    {
        auto redis_driver = __ACCOUNT_REDIS_DRIVER__;

        redis_driver->WriteMulti();
        redis_driver->SRem( __STRING__( ipblacklist ), ip );
        redis_driver->Del( __DATABASE_KEY_2__( __STRING__( ipblacklist ), ip ) );
        auto result = redis_driver->WriteExec();
        return result->IsOk();
    }

    StringMapList KFAccountRedis::QueryIpBlackList()
    {
        StringMapList ip_black_list;
        auto redis_driver = __ACCOUNT_REDIS_DRIVER__;
        auto list_result = redis_driver->SMembers( __STRING__( ipblacklist ) );

        StringList removes;
        for ( auto& ip : list_result->_value )
        {
            auto ip_data = redis_driver->HGetAll( __DATABASE_KEY_2__( __STRING__( ipblacklist ), ip ) );
            if ( !ip_data->IsOk() )
            {
                continue;
            }

            if ( ip_data->_value.empty() )
            {
                removes.push_back( ip );
                continue;
            }


            StringMap values;
            values = ip_data->_value;
            ip_black_list.emplace_back( values );
        }

        if ( !removes.empty() )
        {
            redis_driver->SRem( __STRING__( ipblacklist ), removes );
        }

        return ip_black_list;
    }

    bool KFAccountRedis::CheckIpInWhiteList( const std::string& ip )
    {
        auto redis_driver = __ACCOUNT_REDIS_DRIVER__;
        auto result = redis_driver->HGetUInt64( __DATABASE_KEY_2__( __STRING__( ipwhitelist ), ip ), __STRING__( endtime ) );
        return result->_value > 0u;
    }

    bool KFAccountRedis::AddIpWhiteList( const std::string& ip, uint64 time, const std::string& comment )
    {
        auto redis_driver = __ACCOUNT_REDIS_DRIVER__;
        auto ip_key = __DATABASE_KEY_2__( __STRING__( ipwhitelist ), ip );

        StringMap values;
        values[ __STRING__( comment ) ] = comment;
        values[ __STRING__( starttime ) ] = __TO_STRING__( KFGlobal::Instance()->_real_time );
        values[ __STRING__( endtime ) ] = __TO_STRING__( KFGlobal::Instance()->_real_time + time );

        redis_driver->WriteMulti();
        redis_driver->HMSet( ip_key, values );
        redis_driver->Expire( ip_key, time );
        redis_driver->SAdd( __STRING__( ipwhitelist ), ip );
        auto result = redis_driver->WriteExec();
        return result->IsOk();
    }

    bool KFAccountRedis::RemoveIpWhiteList( const std::string& ip )
    {
        auto redis_driver = __ACCOUNT_REDIS_DRIVER__;
        redis_driver->WriteMulti();
        redis_driver->SRem( __STRING__( ipwhitelist ), ip );
        redis_driver->Del( __DATABASE_KEY_2__( __STRING__( ipwhitelist ), ip ) );
        auto result = redis_driver->WriteExec();
        return result->IsOk();
    }

    StringMapList KFAccountRedis::QueryIpWhiteList()
    {
        StringMapList ip_white_list;
        auto redis_driver = __ACCOUNT_REDIS_DRIVER__;
        auto list_result = redis_driver->SMembers( __STRING__( ipwhitelist ) );

        StringList removes;
        for ( auto& ip : list_result->_value )
        {
            auto ip_data = redis_driver->HGetAll( __DATABASE_KEY_2__( __STRING__( ipwhitelist ), ip ) );
            if ( !ip_data->IsOk() )
            {
                continue;
            }

            if ( ip_data->_value.empty() )
            {
                removes.push_back( ip );
                continue;
            }

            StringMap values;
            values = ip_data->_value;
            ip_white_list.emplace_back( values );
        }

        if ( !removes.empty() )
        {
            redis_driver->SRem( __STRING__( ipwhitelist ), removes );
        }

        return ip_white_list;
    }

    uint64 KFAccountRedis::CheckAccountInBlackList( uint64 account_id )
    {
        auto redis_driver = __ACCOUNT_REDIS_DRIVER__;
        auto result = redis_driver->HGetUInt64( __DATABASE_KEY_2__(  __STRING__( accountblacklist ), account_id ), __STRING__( endtime ) );
        return result->_value;
    }

    bool KFAccountRedis::AddAccountBlackList( uint64 account_id, const std::string& account, uint32 channel, uint64 time, const std::string& comment )
    {
        auto redis_driver = __ACCOUNT_REDIS_DRIVER__;
        auto account_key = __DATABASE_KEY_2__( __STRING__( accountblacklist ), account_id );

        StringMap values;
        values[ __STRING__( comment ) ] = comment;
        values[ __STRING__( account ) ] = account;
        values[ __STRING__( channel ) ] = __TO_STRING__( channel );
        values[ __STRING__( starttime ) ] = __TO_STRING__( KFGlobal::Instance()->_real_time );
        values[ __STRING__( endtime ) ] = __TO_STRING__( KFGlobal::Instance()->_real_time + time );

        redis_driver->WriteMulti();
        redis_driver->HMSet( account_key, values );
        redis_driver->Expire( account_key, time );
        redis_driver->SAdd( __STRING__( accountblacklist ), account_id );
        auto result = redis_driver->WriteExec();
        return result->IsOk();
    }

    bool KFAccountRedis::RemoveAccountBlackList( uint64 account_id )
    {
        auto redis_driver = __ACCOUNT_REDIS_DRIVER__;

        redis_driver->WriteMulti();
        redis_driver->SRem( __STRING__( accountblacklist ), account_id );
        redis_driver->Del(  __DATABASE_KEY_2__( __STRING__( accountblacklist ), account_id ) );
        auto result = redis_driver->WriteExec();
        return result->IsOk();
    }

    StringMapList KFAccountRedis::QueryAccountBlackList()
    {
        StringMapList account_black_list;
        auto redis_driver = __ACCOUNT_REDIS_DRIVER__;
        auto list_result = redis_driver->SMembers( __STRING__( accountblacklist ) );

        StringList removes;
        for ( auto& account_id : list_result->_value )
        {
            auto account_data = redis_driver->HGetAll( __DATABASE_KEY_2__( __STRING__( accountblacklist ), account_id ) );
            if ( !account_data->IsOk() )
            {
                continue;
            }

            if ( account_data->_value.empty() )
            {
                removes.push_back( account_id );
                continue;
            }

            StringMap values;
            values = account_data->_value;
            account_black_list.emplace_back( values );
        }

        if ( !removes.empty() )
        {
            redis_driver->SRem( __STRING__( accountblacklist ), removes );
        }

        return account_black_list;
    }

    bool KFAccountRedis::CheckAccountInWhiteList( uint64 account_id )
    {
        auto redis_driver = __ACCOUNT_REDIS_DRIVER__;
        auto result = redis_driver->HGetUInt64( __DATABASE_KEY_2__( __STRING__( accountwhitelist ), account_id ), __STRING__( endtime ) );
        return result->_value > 0u;
    }

    bool KFAccountRedis::AddAccountWhiteList( uint64 account_id, const std::string& account, uint32 channel, uint64 time, const std::string& comment )
    {
        auto redis_driver = __ACCOUNT_REDIS_DRIVER__;
        auto account_key = __DATABASE_KEY_2__( __STRING__( accountwhitelist ), account_id );

        StringMap values;
        values[ __STRING__( comment ) ] = comment;
        values[ __STRING__( account ) ] = account;
        values[ __STRING__( channel ) ] = __TO_STRING__( channel );
        values[ __STRING__( starttime ) ] = __TO_STRING__( KFGlobal::Instance()->_real_time );
        values[ __STRING__( endtime ) ] = __TO_STRING__( KFGlobal::Instance()->_real_time + time );

        redis_driver->WriteMulti();
        redis_driver->SAdd( __STRING__( accountwhitelist ), account_id );
        redis_driver->HMSet( account_key, values );
        redis_driver->Expire( __DATABASE_KEY_2__( __STRING__( accountwhitelist ), account_id ), time );
        auto result = redis_driver->WriteExec();
        return result->IsOk();
    }

    bool KFAccountRedis::RemoveAccountWhiteList( uint64 account_id )
    {
        auto redis_driver = __ACCOUNT_REDIS_DRIVER__;

        redis_driver->WriteMulti();
        redis_driver->SRem( __STRING__( accountwhitelist ), account_id );
        redis_driver->Del( __DATABASE_KEY_2__( __STRING__( accountwhitelist ), account_id ) );
        auto result = redis_driver->WriteExec();
        return result->IsOk();
    }

    StringMapList KFAccountRedis::QueryAccountWhiteList()
    {
        StringMapList account_white_list;
        auto redis_driver = __ACCOUNT_REDIS_DRIVER__;
        auto list_result = redis_driver->SMembers( __STRING__( accountwhitelist ) );

        StringList removes;
        for ( auto& account_id : list_result->_value )
        {
            auto account_data = redis_driver->HGetAll( __DATABASE_KEY_2__( __STRING__( accountwhitelist ), account_id ) );
            if ( !account_data->IsOk() )
            {
                continue;
            }

            if ( account_data->_value.empty() )
            {
                removes.push_back( account_id );
                continue;
            }

            StringMap values;
            values = account_data->_value;
            account_white_list.emplace_back( values );
        }

        if ( !removes.empty() )
        {
            redis_driver->SRem( __STRING__( accountwhitelist ), removes );
        }

        return account_white_list;
    }

}