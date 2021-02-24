#include "KFAccountMongo.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
#define __ACCOUNT_MONGO_DRIVER__ _kf_mongo->Create( __STRING__( account ) )

    StringMap KFAccountMongo::QueryCreateAccount( const std::string& account, uint32 channel )
    {
        StringMap account_data;
        auto mongo_driver = __ACCOUNT_MONGO_DRIVER__;

        // 先查询是否存在
        auto table = __FORMAT__( "{}:{}", __STRING__( account ), channel );
        auto query_account_id = mongo_driver->QueryUInt64( table, account, __STRING__( accountid ) );
        if ( query_account_id->_value != _invalid_int )
        {
            auto query_account_data = mongo_driver->QueryRecord( __STRING__( accountid ), query_account_id->_value );
            __DBVALUE_TO_MAP__( query_account_data->_value, account_data );
            return account_data;
        }

        auto account_id = KFGlobal::Instance()->MTMakeUuid( __STRING__( account ) );
        if ( !mongo_driver->Insert( table, account, __STRING__( accountid ), account_id ) )
        {
            __LOG_DEBUG__( "account[{}] channel[{}] save account failed", account, channel );
            return account_data;
        }

        // 创建账号id
        KFDBValue db_value;
        db_value.AddValue( __STRING__( account ), account );
        db_value.AddValue( __STRING__( channel ), channel );
        db_value.AddValue( __STRING__( accountid ), account_id );
        if ( !mongo_driver->Insert( __STRING__( accountid ), account_id, db_value ) )
        {
            __LOG_DEBUG__( "account[{}] channel[{}] save account_id failed", account, channel );
            return account_data;
        }

        __DBVALUE_TO_MAP__( db_value, account_data );
        __LOG_DEBUG__( "create account successfully, account=[{}] account_id=[{}] channel=[{}] ", account, account_id, channel );
        return account_data;
    }

    void KFAccountMongo::SaveChannelData( uint64 account_id, KFJson& channel_data )
    {
        if ( !__JSON_HAS_MEMBER__( channel_data, __STRING__( extend ) ) )
        {
            return;
        }

        auto mongo_driver = __ACCOUNT_MONGO_DRIVER__;

        auto& extend_data = channel_data[ __STRING__( extend ) ];
        KFDBValue db_value;
        __JSON_TO_DBVALUE__( extend_data, db_value );
        mongo_driver->Insert( __STRING__( extend ), account_id, db_value );
    }

    StringMap KFAccountMongo::QueryChannelData( uint64 account_id )
    {
        auto mongo_driver = __ACCOUNT_MONGO_DRIVER__;
        auto result = mongo_driver->QueryRecord( __STRING__( extend ), account_id );

        StringMap channel_data;
        __DBVALUE_TO_MAP__( result->_value, channel_data );
        return channel_data;
    }

    std::string KFAccountMongo::MakeAccountToken( const std::string& account, uint32 channel, uint64 account_id, uint32 expire_time )
    {
        auto mongo_driver = __ACCOUNT_MONGO_DRIVER__;
        mongo_driver->CreateIndex( __STRING__( token ), MongoKeyword::_expire );

        auto token = KFAccountLogic::MakeAccountToken( account, channel, account_id, expire_time );

        KFDBValue db_value;
        db_value.AddValue( __STRING__( token ), token );
        db_value.AddValue( __STRING__( account ), account );
        db_value.AddValue( __STRING__( channel ), channel );
        db_value.AddValue( MongoKeyword::_expire, expire_time );
        mongo_driver->Insert( __STRING__( token ), account_id, db_value );
        return token;
    }

    StringMap KFAccountMongo::VerifyAccountToken( uint64 account_id, const std::string& token )
    {
        StringMap account_data;

        auto mongo_driver = __ACCOUNT_MONGO_DRIVER__;
        auto result = mongo_driver->QueryRecord( __STRING__( token ), account_id );
        if ( !result->_value.IsEmpty() )
        {
            auto query_token = result->_value.FindStrValue( __STRING__( token ) );
            if ( query_token == token )
            {
                __DBVALUE_TO_MAP__( result->_value, account_data );
            }
        }

        return account_data;
    }

    void KFAccountMongo::SaveZoneToken( const std::string& account, uint32 channel, uint64 account_id, uint32 zone_id, const std::string& token, uint32 expire_time )
    {
        auto mongo_driver = __ACCOUNT_MONGO_DRIVER__;

        auto table = __FORMAT__( "{}:{}", __STRING__( zonetoken ), zone_id );
        mongo_driver->CreateIndex( table, MongoKeyword::_expire );

        KFDBValue db_value;
        db_value.AddValue( __STRING__( token ), token );
        db_value.AddValue( __STRING__( account ), account );
        db_value.AddValue( __STRING__( channel ), channel );
        db_value.AddValue( __STRING__( accountid ), account_id );
        db_value.AddValue( MongoKeyword::_expire, expire_time );
        mongo_driver->Insert( table, account_id, db_value );
    }

    StringMap KFAccountMongo::VerifyZoneToken( uint64 account_id, uint32 zone_id, const std::string& token )
    {
        StringMap account_data;
        auto mongo_driver = __ACCOUNT_MONGO_DRIVER__;

        auto table = __FORMAT__( "{}:{}", __STRING__( zonetoken ), zone_id );
        auto result = mongo_driver->QueryRecord( table, account_id );
        if ( result->IsOk() )
        {
            auto query_token = result->_value.FindStrValue( __STRING__( token ) );
            if ( query_token == token )
            {
                __DBVALUE_TO_MAP__( result->_value, account_data );
            }
        }

        return account_data;
    }

    void KFAccountMongo::SaveLoginData( uint64 account_id, const std::string& ip, uint32 zone_id )
    {
        KFDBValue db_value;
        db_value.AddValue( __STRING__( ip ), ip );
        db_value.AddValue( __STRING__( zoneid ), zone_id );

        auto mongo_driver = __ACCOUNT_MONGO_DRIVER__;
        mongo_driver->Insert( __STRING__( accountid ), account_id, db_value );
    }

    std::tuple<uint64, bool> KFAccountMongo::QueryCreatePlayer( uint64 account_id, uint32 zone_id )
    {
        auto mongo_driver = __ACCOUNT_MONGO_DRIVER__;

        // 查询是否存在
        auto zone_key = __FORMAT__( "{}:{}", __STRING__( zone ), zone_id );
        auto query_player_id = mongo_driver->QueryUInt64( __STRING__( accountid ), account_id, zone_key );
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
        mongo_driver->Insert( __STRING__( player ), player_id, __STRING__( accountid ), account_id );
        if ( !mongo_driver->Insert( __STRING__( accountid ), account_id, zone_key, player_id ) )
        {
            return std::make_tuple( _invalid_int, false );
        }

        return std::make_tuple( player_id, true );
    }

    bool KFAccountMongo::UpdateOnline( uint64 account_id, uint64 player_id, uint64 world_id, uint64 game_id )
    {
        KFDBValue db_value;
        db_value.AddValue( __STRING__( playerid ), player_id );
        db_value.AddValue( __STRING__( world ), world_id );
        db_value.AddValue( __STRING__( game ), game_id );

        auto mongo_driver = __ACCOUNT_MONGO_DRIVER__;
        return mongo_driver->Insert( __STRING__( accountid ), account_id, db_value );
    }

    void KFAccountMongo::SaveWeiXinAccessToken( const std::string& machine_code, const std::string& open_id,
            const std::string& scope, const std::string& access_token, uint32 expire_time )
    {
        auto mongo_driver = __ACCOUNT_MONGO_DRIVER__;
        mongo_driver->CreateIndex( __STRING__( access_token ), MongoKeyword::_expire );

        KFDBValue db_value;
        db_value.AddValue( __STRING__( token ), access_token );
        db_value.AddValue( __STRING__( openid ), open_id );
        db_value.AddValue( __STRING__( scope ), scope );
        db_value.AddValue( MongoKeyword::_expire, expire_time - 200 );
        mongo_driver->Insert( __STRING__( access_token ), machine_code, db_value );
    }

    StringMap KFAccountMongo::QueryWeiXinAccessToken( const std::string& machine_code )
    {
        auto mongo_driver = __ACCOUNT_MONGO_DRIVER__;
        auto result = mongo_driver->QueryRecord( __STRING__( access_token ), machine_code );

        StringMap token_data;
        __DBVALUE_TO_MAP__( result->_value, token_data );
        return token_data;
    }

    void KFAccountMongo::SaveWeiXinRefreshToken( const std::string& machine_code, const std::string& refresh_token )
    {
        auto mongo_driver = __ACCOUNT_MONGO_DRIVER__;
        mongo_driver->CreateIndex( __STRING__( refresh_token ), MongoKeyword::_expire );

        KFDBValue db_value;
        db_value.AddValue( __STRING__( token ), refresh_token );
        db_value.AddValue( MongoKeyword::_expire, 2590000 );
        mongo_driver->Insert( __STRING__( refresh_token ), machine_code, db_value );
    }

    std::string KFAccountMongo::QueryWeiXinRefreshToken( const std::string& machine_code )
    {
        auto mongo_driver = __ACCOUNT_MONGO_DRIVER__;
        auto result = mongo_driver->QueryString( __STRING__( refresh_token ), machine_code, __STRING__( token ) );
        return result->_value;
    }

    uint64 KFAccountMongo::CheckIpInBlackList( const std::string& ip )
    {
        auto mongo_driver = __ACCOUNT_MONGO_DRIVER__;
        auto result = mongo_driver->QueryUInt64( __STRING__( ipblacklist ), ip, __STRING__( endtime ) );
        return result->_value;
    }

    bool KFAccountMongo::AddIpBlackList( const std::string& ip, uint64 time, const std::string& comment )
    {
        auto mongo_driver = __ACCOUNT_MONGO_DRIVER__;
        mongo_driver->CreateIndex( __STRING__( ipblacklist ), MongoKeyword::_expire );

        KFDBValue db_value;
        db_value.AddValue( __STRING__( starttime ), KFGlobal::Instance()->_real_time );
        db_value.AddValue( __STRING__( endtime ), KFGlobal::Instance()->_real_time + time );
        db_value.AddValue( __STRING__( ip ), ip );
        db_value.AddValue( __STRING__( comment ), comment );
        db_value.AddValue( MongoKeyword::_expire, time );
        return mongo_driver->Insert( __STRING__( ipblacklist ), ip, db_value );
    }

    bool KFAccountMongo::RemoveIpBlackList( const std::string& ip )
    {
        auto mongo_driver = __ACCOUNT_MONGO_DRIVER__;
        return mongo_driver->Delete( __STRING__( ipblacklist ), ip );
    }

    StringMapList KFAccountMongo::QueryIpBlackList()
    {
        auto mongo_driver = __ACCOUNT_MONGO_DRIVER__;
        auto result = mongo_driver->QueryListRecord( __STRING__( ipblacklist ) );

        StringMapList ip_black_list;
        for ( auto& db_value : result->_value )
        {
            StringMap values;
            __DBVALUE_TO_MAP__( db_value, values );
            ip_black_list.emplace_back( values );
        }

        return ip_black_list;
    }

    bool KFAccountMongo::CheckIpInWhiteList( const std::string& ip )
    {
        auto mongo_driver = __ACCOUNT_MONGO_DRIVER__;
        auto result = mongo_driver->QueryUInt64( __STRING__( ipwhitelist ), ip, __STRING__( endtime ) );
        return result->_value > 0u;
    }

    bool KFAccountMongo::AddIpWhiteList( const std::string& ip, uint64 time, const std::string& comment )
    {
        auto mongo_driver = __ACCOUNT_MONGO_DRIVER__;
        mongo_driver->CreateIndex( __STRING__( ipwhitelist ), MongoKeyword::_expire );

        KFDBValue db_value;
        db_value.AddValue( __STRING__( starttime ), KFGlobal::Instance()->_real_time );
        db_value.AddValue( __STRING__( endtime ), KFGlobal::Instance()->_real_time + time );
        db_value.AddValue( __STRING__( ip ), ip );
        db_value.AddValue( __STRING__( comment ), comment );
        db_value.AddValue( MongoKeyword::_expire, time );
        return mongo_driver->Insert( __STRING__( ipwhitelist ), ip, db_value );
    }

    bool KFAccountMongo::RemoveIpWhiteList( const std::string& ip )
    {
        auto mongo_driver = __ACCOUNT_MONGO_DRIVER__;
        return mongo_driver->Delete( __STRING__( ipwhitelist ), ip );
    }

    StringMapList KFAccountMongo::QueryIpWhiteList()
    {
        auto mongo_driver = __ACCOUNT_MONGO_DRIVER__;
        auto result = mongo_driver->QueryListRecord( __STRING__( ipwhitelist ) );

        StringMapList ip_white_list;
        for ( auto& db_value : result->_value )
        {
            StringMap values;
            __DBVALUE_TO_MAP__( db_value, values );
            ip_white_list.emplace_back( values );
        }

        return ip_white_list;
    }

    uint64 KFAccountMongo::CheckAccountInBlackList( uint64 account_id )
    {
        auto mongo_driver = __ACCOUNT_MONGO_DRIVER__;
        auto result = mongo_driver->QueryUInt64( __STRING__( accountblacklist ), account_id, __STRING__( endtime ) );
        return result->_value;
    }

    bool KFAccountMongo::AddAccountBlackList( uint64 account_id, const std::string& account, uint32 channel, uint64 time, const std::string& comment )
    {
        auto mongo_driver = __ACCOUNT_MONGO_DRIVER__;
        mongo_driver->CreateIndex( __STRING__( accountblacklist ), MongoKeyword::_expire );

        KFDBValue db_value;
        db_value.AddValue( __STRING__( starttime ), KFGlobal::Instance()->_real_time );
        db_value.AddValue( __STRING__( endtime ), KFGlobal::Instance()->_real_time + time );
        db_value.AddValue( __STRING__( account ), account );
        db_value.AddValue( __STRING__( channel ), channel );
        db_value.AddValue( __STRING__( accountid ), account_id );
        db_value.AddValue( __STRING__( comment ), comment );
        db_value.AddValue( MongoKeyword::_expire, time );
        return mongo_driver->Insert( __STRING__( accountblacklist ), account_id, db_value );
    }

    bool KFAccountMongo::RemoveAccountBlackList( uint64 account_id )
    {
        auto mongo_driver = __ACCOUNT_MONGO_DRIVER__;
        return mongo_driver->Delete( __STRING__( accountblacklist ), account_id );
    }

    StringMapList KFAccountMongo::QueryAccountBlackList()
    {
        auto mongo_driver = __ACCOUNT_MONGO_DRIVER__;
        auto result = mongo_driver->QueryListRecord( __STRING__( accountblacklist ) );

        StringMapList account_black_list;
        for ( auto& db_value : result->_value )
        {
            StringMap values;
            __DBVALUE_TO_MAP__( db_value, values );
            account_black_list.emplace_back( values );
        }

        return account_black_list;
    }

    bool KFAccountMongo::CheckAccountInWhiteList( uint64 account_id )
    {
        auto mongo_driver = __ACCOUNT_MONGO_DRIVER__;
        auto result = mongo_driver->QueryUInt64( __STRING__( accountwhitelist ), account_id, __STRING__( endtime ) );
        return result->_value > 0u;
    }

    bool KFAccountMongo::AddAccountWhiteList( uint64 account_id, const std::string& account, uint32 channel, uint64 time, const std::string& comment )
    {
        auto mongo_driver = __ACCOUNT_MONGO_DRIVER__;
        mongo_driver->CreateIndex( __STRING__( accountwhitelist ), MongoKeyword::_expire );

        KFDBValue db_value;
        db_value.AddValue( __STRING__( starttime ), KFGlobal::Instance()->_real_time );
        db_value.AddValue( __STRING__( endtime ), KFGlobal::Instance()->_real_time + time );
        db_value.AddValue( __STRING__( account ), account );
        db_value.AddValue( __STRING__( channel ), channel );
        db_value.AddValue( __STRING__( accountid ), account_id );
        db_value.AddValue( __STRING__( comment ), comment );
        db_value.AddValue( MongoKeyword::_expire, time );
        return mongo_driver->Insert( __STRING__( accountwhitelist ), account_id, db_value );
    }

    bool KFAccountMongo::RemoveAccountWhiteList( uint64 account_id )
    {
        auto mongo_driver = __ACCOUNT_MONGO_DRIVER__;
        return mongo_driver->Delete( __STRING__( accountwhitelist ), account_id );
    }

    StringMapList KFAccountMongo::QueryAccountWhiteList()
    {
        auto mongo_driver = __ACCOUNT_MONGO_DRIVER__;
        auto result = mongo_driver->QueryListRecord( __STRING__( accountwhitelist ) );

        StringMapList account_white_list;
        for ( auto& db_value : result->_value )
        {
            StringMap values;
            __DBVALUE_TO_MAP__( db_value, values );
            account_white_list.emplace_back( values );
        }

        return account_white_list;
    }
}