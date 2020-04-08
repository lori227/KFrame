#include "KFAccountMongo.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
#define __ACCOUNT_MONGO_DRIVER__ _kf_mongo->Create( __STRING__( account ) )

    StringMap KFAccountMongo::QueryCreateAccount( const std::string& account, uint32 channel )
    {
        StringMap accountdata;
        auto mongodriver = __ACCOUNT_MONGO_DRIVER__;

        // 先查询是否存在
        auto strkey = __FORMAT__( "{}:{}", account, channel );
        auto kfquery = mongodriver->QueryUInt64( __STRING__( account ), strkey, __STRING__( accountid ) );
        auto accountid = kfquery->_value;
        if ( accountid != _invalid_int )
        {
            auto kfaccountdata = mongodriver->QueryRecord( __STRING__( accountid ), accountid );
            __DBVALUE_TO_MAP__( kfaccountdata->_value, accountdata );

            return accountdata;
        }
        else
        {
            accountid = KFGlobal::Instance()->MTMakeUuid( __STRING__( account ) );
        }

        if ( !mongodriver->Insert( __STRING__( account ), strkey, __STRING__( accountid ), accountid ) )
        {
            __LOG_DEBUG__( "account[{}] channel[{}] save account failed", account, channel );
            return accountdata;
        }

        if ( !mongodriver->Insert( __STRING__( accountlist ), 0u, __STRING__( accountid ), accountid ) )
        {
            __LOG_DEBUG__( "account[{}] channel[{}] save accountlist failed", account, channel );
            return accountdata;
        }

        // 创建账号id
        KFDBValue dbvalue;
        dbvalue.AddValue( __STRING__( account ), account );
        dbvalue.AddValue( __STRING__( channel ), channel );
        dbvalue.AddValue( __STRING__( accountid ), accountid );

        if ( !mongodriver->Insert( __STRING__( accountid ), accountid, dbvalue ) )
        {
            __LOG_DEBUG__( "account[{}] channel[{}] save accountid failed", account, channel );
            return accountdata;
        }

        __DBVALUE_TO_MAP__( dbvalue, accountdata );
        __LOG_DEBUG__( "create account successfully, account[{}] accountid[{}] channel[{}] ", account, accountid, channel );
        return accountdata;
    }

    void KFAccountMongo::SaveChannelData( uint64 accountid, KFJson& channeldata )
    {
        if ( !__JSON_HAS_MEMBER__( channeldata, __STRING__( extend ) ) )
        {
            return;
        }

        auto& kfextend = channeldata[ __STRING__( extend ) ];

        KFDBValue dbvalue;
        __JSON_TO_DBVALUE__( kfextend, dbvalue );

        auto mongodriver = __ACCOUNT_MONGO_DRIVER__;
        mongodriver->Insert( __STRING__( extend ), accountid, dbvalue );
    }

    StringMap KFAccountMongo::QueryChannelData( uint64 accountid )
    {
        auto mongodriver = __ACCOUNT_MONGO_DRIVER__;
        auto kfresult = mongodriver->QueryRecord( __STRING__( extend ), accountid );

        StringMap channeldata;
        __DBVALUE_TO_MAP__( kfresult->_value, channeldata );

        return channeldata;
    }

    std::string KFAccountMongo::MakeAccountToken( const std::string& account, uint32 channel, uint64 accountid, uint32 expiretime )
    {
        auto token = KFAccountLogic::MakeAccountToken( account, channel, accountid, expiretime );

        KFDBValue dbvalue;
        dbvalue.AddValue( __STRING__( token ), token );
        dbvalue.AddValue( __STRING__( account ), account );
        dbvalue.AddValue( __STRING__( channel ), channel );
        dbvalue.AddValue( MongoKeyword::_expire, expiretime );

        auto mongodriver = __ACCOUNT_MONGO_DRIVER__;
        mongodriver->Insert( __STRING__( token ), accountid, dbvalue );

        return token;
    }

    StringMap KFAccountMongo::VerifyAccountToken( uint64 accountid, const std::string& token )
    {
        StringMap accountdata;

        auto mongodriver = __ACCOUNT_MONGO_DRIVER__;
        auto kftokenresult = mongodriver->QueryRecord( __STRING__( token ), accountid );
        if ( kftokenresult->IsOk() )
        {
            auto querytoken = kftokenresult->_value.FindStrValue( __STRING__( token ) );
            if ( querytoken == token )
            {
                __DBVALUE_TO_MAP__( kftokenresult->_value, accountdata );
            }
        }

        return accountdata;
    }

    void KFAccountMongo::SaveZoneToken( const std::string& account, uint32 channel, uint64 accountid, uint32 zoneid, const std::string& token, uint32 expiretime )
    {
        KFDBValue dbvalue;
        dbvalue.AddValue( __STRING__( token ), token );
        dbvalue.AddValue( __STRING__( account ), account );
        dbvalue.AddValue( __STRING__( channel ), channel );
        dbvalue.AddValue( __STRING__( accountid ), accountid );
        dbvalue.AddValue( MongoKeyword::_expire, expiretime );

        auto mongodriver = __ACCOUNT_MONGO_DRIVER__;
        auto tokenkey = __FORMAT__( "{}:{}", zoneid, accountid );
        mongodriver->Insert( __STRING__( zonetoken ), tokenkey, dbvalue );
    }

    StringMap KFAccountMongo::VerifyZoneToken( uint64 accountid, uint32 zoneid, const std::string& token )
    {
        StringMap accountdata;

        auto mongodriver = __ACCOUNT_MONGO_DRIVER__;
        auto tokenkey = __FORMAT__( "{}:{}", zoneid, accountid );
        auto kftokenresult = mongodriver->QueryRecord( __STRING__( zonetoken ), tokenkey );
        if ( kftokenresult->IsOk() )
        {
            auto querytoken = kftokenresult->_value.FindStrValue( __STRING__( token ) );
            if ( querytoken == token )
            {
                __DBVALUE_TO_MAP__( kftokenresult->_value, accountdata );
            }
        }

        return accountdata;
    }

    void KFAccountMongo::SaveLoginData( uint64 accountid, const std::string& ip, uint32 zoneid )
    {
        KFDBValue dbvalue;
        dbvalue.AddValue( __STRING__( ip ), ip );
        dbvalue.AddValue( __STRING__( zoneid ), zoneid );

        auto mongodriver = __ACCOUNT_MONGO_DRIVER__;
        mongodriver->Insert( __STRING__( accountid ), accountid, dbvalue );
    }

    std::tuple<uint64, bool> KFAccountMongo::QueryCreatePlayer( uint64 accountid, uint32 zoneid )
    {
        auto mongodriver = __ACCOUNT_MONGO_DRIVER__;

        // 查询是否存在
        auto strkey = __FORMAT__( "{}:{}", __STRING__( zone ), zoneid );
        auto queryplayerid = mongodriver->QueryUInt64( __STRING__( accountid ), accountid, strkey );
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
        mongodriver->Insert( __STRING__( player ), playerid, __STRING__( accountid ), accountid );

        if ( !mongodriver->Insert( __STRING__( accountid ), accountid, strkey, playerid ) )
        {
            return std::make_tuple( _invalid_int, false );
        }

        return std::make_tuple( playerid, true );
    }

    bool KFAccountMongo::UpdateOnline( uint64 accountid, uint64 playerid, uint64 worldid, uint64 gameid )
    {
        KFDBValue dbvalue;
        dbvalue.AddValue( __STRING__( playerid ), playerid );
        dbvalue.AddValue( __STRING__( world ), worldid );
        dbvalue.AddValue( __STRING__( game ), gameid );

        auto mongodriver = __ACCOUNT_MONGO_DRIVER__;
        return mongodriver->Insert( __STRING__( accountid ), accountid, dbvalue );
    }
}