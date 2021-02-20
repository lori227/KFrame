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
        auto table = __FORMAT__( "{}:{}", __STRING__( account ), channel );
        auto kfquery = mongodriver->QueryUInt64( table, account, __STRING__( accountid ) );
        if ( kfquery->_value != _invalid_int )
        {
            auto kfaccountdata = mongodriver->QueryRecord( __STRING__( accountid ), kfquery->_value );
            __DBVALUE_TO_MAP__( kfaccountdata->_value, accountdata );
            return accountdata;
        }

        auto accountid = KFGlobal::Instance()->MTMakeUuid( __STRING__( account ) );
        if ( !mongodriver->Insert( table, account, __STRING__( accountid ), accountid ) )
        {
            __LOG_DEBUG__( "account[{}] channel[{}] save account failed", account, channel );
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

        auto mongodriver = __ACCOUNT_MONGO_DRIVER__;

        auto& kfextend = channeldata[ __STRING__( extend ) ];
        KFDBValue dbvalue;
        __JSON_TO_DBVALUE__( kfextend, dbvalue );
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
        auto mongodriver = __ACCOUNT_MONGO_DRIVER__;
        mongodriver->CreateIndex( __STRING__( token ), MongoKeyword::_expire );

        auto token = KFAccountLogic::MakeAccountToken( account, channel, accountid, expiretime );

        KFDBValue dbvalue;
        dbvalue.AddValue( __STRING__( token ), token );
        dbvalue.AddValue( __STRING__( account ), account );
        dbvalue.AddValue( __STRING__( channel ), channel );
        dbvalue.AddValue( MongoKeyword::_expire, expiretime );
        mongodriver->Insert( __STRING__( token ), accountid, dbvalue );
        return token;
    }

    StringMap KFAccountMongo::VerifyAccountToken( uint64 accountid, const std::string& token )
    {
        StringMap accountdata;

        auto mongodriver = __ACCOUNT_MONGO_DRIVER__;
        auto kftokenresult = mongodriver->QueryRecord( __STRING__( token ), accountid );
        if ( !kftokenresult->_value.IsEmpty() )
        {
            auto querytoken = kftokenresult->_value.FindStrValue( __STRING__( token ) );
            if ( querytoken == token )
            {
                __DBVALUE_TO_MAP__( kftokenresult->_value, accountdata );
            }
        }

        return accountdata;
    }

    void KFAccountMongo::SaveZoneToken( const std::string& account, uint32 channel, uint64 accountid, uint32 zone_id, const std::string& token, uint32 expiretime )
    {
        auto mongodriver = __ACCOUNT_MONGO_DRIVER__;

        auto table = __FORMAT__( "{}:{}", __STRING__( zonetoken ), zone_id );
        mongodriver->CreateIndex( table, MongoKeyword::_expire );

        KFDBValue dbvalue;
        dbvalue.AddValue( __STRING__( token ), token );
        dbvalue.AddValue( __STRING__( account ), account );
        dbvalue.AddValue( __STRING__( channel ), channel );
        dbvalue.AddValue( __STRING__( accountid ), accountid );
        dbvalue.AddValue( MongoKeyword::_expire, expiretime );
        mongodriver->Insert( table, accountid, dbvalue );
    }

    StringMap KFAccountMongo::VerifyZoneToken( uint64 accountid, uint32 zone_id, const std::string& token )
    {
        StringMap accountdata;
        auto mongodriver = __ACCOUNT_MONGO_DRIVER__;

        auto table = __FORMAT__( "{}:{}", __STRING__( zonetoken ), zone_id );
        auto kftokenresult = mongodriver->QueryRecord( table, accountid );
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

    void KFAccountMongo::SaveLoginData( uint64 accountid, const std::string& ip, uint32 zone_id )
    {
        KFDBValue dbvalue;
        dbvalue.AddValue( __STRING__( ip ), ip );
        dbvalue.AddValue( __STRING__( zone_id ), zone_id );

        auto mongodriver = __ACCOUNT_MONGO_DRIVER__;
        mongodriver->Insert( __STRING__( accountid ), accountid, dbvalue );
    }

    std::tuple<uint64, bool> KFAccountMongo::QueryCreatePlayer( uint64 accountid, uint32 zone_id )
    {
        auto mongodriver = __ACCOUNT_MONGO_DRIVER__;

        // 查询是否存在
        auto strkey = __FORMAT__( "{}:{}", __STRING__( zone ), zone_id );
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
        auto playerid = KFGlobal::Instance()->MTMakeUuid( __STRING__( player ), zone_id );
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

    void KFAccountMongo::SaveWeiXinAccessToken( const std::string& machinecode, const std::string& openid,
            const std::string& scope, const std::string& accesstoken, uint32 expirestime )
    {
        auto mongodriver = __ACCOUNT_MONGO_DRIVER__;
        mongodriver->CreateIndex( __STRING__( access_token ), MongoKeyword::_expire );

        KFDBValue dbvalue;
        dbvalue.AddValue( __STRING__( token ), accesstoken );
        dbvalue.AddValue( __STRING__( openid ), openid );
        dbvalue.AddValue( __STRING__( scope ), scope );
        dbvalue.AddValue( MongoKeyword::_expire, expirestime - 200 );
        mongodriver->Insert( __STRING__( access_token ), machinecode, dbvalue );
    }

    StringMap KFAccountMongo::QueryWeiXinAccessToken( const std::string& machinecode )
    {
        auto mongodriver = __ACCOUNT_MONGO_DRIVER__;
        auto kfresult = mongodriver->QueryRecord( __STRING__( access_token ), machinecode );

        StringMap tokendata;
        __DBVALUE_TO_MAP__( kfresult->_value, tokendata );
        return tokendata;
    }

    void KFAccountMongo::SaveWeiXinRefreshToken( const std::string& machinecode, const std::string& refreshtoken )
    {
        auto mongodriver = __ACCOUNT_MONGO_DRIVER__;
        mongodriver->CreateIndex( __STRING__( refresh_token ), MongoKeyword::_expire );

        KFDBValue dbvalue;
        dbvalue.AddValue( __STRING__( token ), refreshtoken );
        dbvalue.AddValue( MongoKeyword::_expire, 2590000 );
        mongodriver->Insert( __STRING__( refresh_token ), machinecode, dbvalue );
    }

    std::string KFAccountMongo::QueryWeiXinRefreshToken( const std::string& machinecode )
    {
        auto mongodriver = __ACCOUNT_MONGO_DRIVER__;
        auto kfresult = mongodriver->QueryString( __STRING__( refresh_token ), machinecode, __STRING__( token ) );
        return kfresult->_value;
    }

    uint64 KFAccountMongo::CheckIpInBlackList( const std::string& ip )
    {
        auto mongodriver = __ACCOUNT_MONGO_DRIVER__;
        auto kfresult = mongodriver->QueryUInt64( __STRING__( ipblacklist ), ip, __STRING__( endtime ) );
        return kfresult->_value;
    }

    bool KFAccountMongo::AddIpBlackList( const std::string& ip, uint64 time, const std::string& comment )
    {
        auto mongodriver = __ACCOUNT_MONGO_DRIVER__;
        mongodriver->CreateIndex( __STRING__( ipblacklist ), MongoKeyword::_expire );


        KFDBValue dbvalue;
        dbvalue.AddValue( __STRING__( starttime ), KFGlobal::Instance()->_real_time );
        dbvalue.AddValue( __STRING__( endtime ), KFGlobal::Instance()->_real_time + time );
        dbvalue.AddValue( __STRING__( ip ), ip );
        dbvalue.AddValue( __STRING__( comment ), comment );
        dbvalue.AddValue( MongoKeyword::_expire, time );
        return mongodriver->Insert( __STRING__( ipblacklist ), ip, dbvalue );
    }

    bool KFAccountMongo::RemoveIpBlackList( const std::string& ip )
    {
        auto mongodriver = __ACCOUNT_MONGO_DRIVER__;
        return mongodriver->Delete( __STRING__( ipblacklist ), ip );
    }

    StringMapList KFAccountMongo::QueryIpBlackList()
    {
        auto mongodriver = __ACCOUNT_MONGO_DRIVER__;
        auto kfresult = mongodriver->QueryListRecord( __STRING__( ipblacklist ) );

        StringMapList stringmaplist;
        for ( auto& dbvalue : kfresult->_value )
        {
            StringMap values;
            __DBVALUE_TO_MAP__( dbvalue, values );
            stringmaplist.emplace_back( values );
        }

        return stringmaplist;
    }

    bool KFAccountMongo::CheckIpInWhiteList( const std::string& ip )
    {
        auto mongodriver = __ACCOUNT_MONGO_DRIVER__;
        auto kfresult = mongodriver->QueryUInt64( __STRING__( ipwhitelist ), ip, __STRING__( endtime ) );
        return kfresult->_value > 0u;
    }

    bool KFAccountMongo::AddIpWhiteList( const std::string& ip, uint64 time, const std::string& comment )
    {
        auto mongodriver = __ACCOUNT_MONGO_DRIVER__;
        mongodriver->CreateIndex( __STRING__( ipwhitelist ), MongoKeyword::_expire );


        KFDBValue dbvalue;
        dbvalue.AddValue( __STRING__( starttime ), KFGlobal::Instance()->_real_time );
        dbvalue.AddValue( __STRING__( endtime ), KFGlobal::Instance()->_real_time + time );
        dbvalue.AddValue( __STRING__( ip ), ip );
        dbvalue.AddValue( __STRING__( comment ), comment );
        dbvalue.AddValue( MongoKeyword::_expire, time );
        return mongodriver->Insert( __STRING__( ipwhitelist ), ip, dbvalue );
    }

    bool KFAccountMongo::RemoveIpWhiteList( const std::string& ip )
    {
        auto mongodriver = __ACCOUNT_MONGO_DRIVER__;
        return mongodriver->Delete( __STRING__( ipwhitelist ), ip );
    }

    StringMapList KFAccountMongo::QueryIpWhiteList()
    {
        auto mongodriver = __ACCOUNT_MONGO_DRIVER__;
        auto kfresult = mongodriver->QueryListRecord( __STRING__( ipwhitelist ) );

        StringMapList stringmaplist;
        for ( auto& dbvalue : kfresult->_value )
        {
            StringMap values;
            __DBVALUE_TO_MAP__( dbvalue, values );
            stringmaplist.emplace_back( values );
        }

        return stringmaplist;
    }

    uint64 KFAccountMongo::CheckAccountInBlackList( uint64 accountid )
    {
        auto mongodriver = __ACCOUNT_MONGO_DRIVER__;
        auto kfresult = mongodriver->QueryUInt64( __STRING__( accountblacklist ), accountid, __STRING__( endtime ) );
        return kfresult->_value;
    }

    bool KFAccountMongo::AddAccountBlackList( uint64 accountid, const std::string& account, uint32 channel, uint64 time, const std::string& comment )
    {
        auto mongodriver = __ACCOUNT_MONGO_DRIVER__;
        mongodriver->CreateIndex( __STRING__( accountblacklist ), MongoKeyword::_expire );

        KFDBValue dbvalue;
        dbvalue.AddValue( __STRING__( starttime ), KFGlobal::Instance()->_real_time );
        dbvalue.AddValue( __STRING__( endtime ), KFGlobal::Instance()->_real_time + time );
        dbvalue.AddValue( __STRING__( account ), account );
        dbvalue.AddValue( __STRING__( channel ), channel );
        dbvalue.AddValue( __STRING__( accountid ), accountid );
        dbvalue.AddValue( __STRING__( comment ), comment );
        dbvalue.AddValue( MongoKeyword::_expire, time );
        return mongodriver->Insert( __STRING__( accountblacklist ), accountid, dbvalue );
    }

    bool KFAccountMongo::RemoveAccountBlackList( uint64 accountid )
    {
        auto mongodriver = __ACCOUNT_MONGO_DRIVER__;
        return mongodriver->Delete( __STRING__( accountblacklist ), accountid );
    }

    StringMapList KFAccountMongo::QueryAccountBlackList()
    {
        auto mongodriver = __ACCOUNT_MONGO_DRIVER__;
        auto kfresult = mongodriver->QueryListRecord( __STRING__( accountblacklist ) );

        StringMapList stringmaplist;
        for ( auto& dbvalue : kfresult->_value )
        {
            StringMap values;
            __DBVALUE_TO_MAP__( dbvalue, values );
            stringmaplist.emplace_back( values );
        }

        return stringmaplist;
    }

    bool KFAccountMongo::CheckAccountInWhiteList( uint64 accountid )
    {
        auto mongodriver = __ACCOUNT_MONGO_DRIVER__;
        auto kfresult = mongodriver->QueryUInt64( __STRING__( accountwhitelist ), accountid, __STRING__( endtime ) );
        return kfresult->_value > 0u;
    }

    bool KFAccountMongo::AddAccountWhiteList( uint64 accountid, const std::string& account, uint32 channel, uint64 time, const std::string& comment )
    {
        auto mongodriver = __ACCOUNT_MONGO_DRIVER__;
        mongodriver->CreateIndex( __STRING__( accountwhitelist ), MongoKeyword::_expire );

        KFDBValue dbvalue;
        dbvalue.AddValue( __STRING__( starttime ), KFGlobal::Instance()->_real_time );
        dbvalue.AddValue( __STRING__( endtime ), KFGlobal::Instance()->_real_time + time );
        dbvalue.AddValue( __STRING__( account ), account );
        dbvalue.AddValue( __STRING__( channel ), channel );
        dbvalue.AddValue( __STRING__( accountid ), accountid );
        dbvalue.AddValue( __STRING__( comment ), comment );
        dbvalue.AddValue( MongoKeyword::_expire, time );
        return mongodriver->Insert( __STRING__( accountwhitelist ), accountid, dbvalue );
    }

    bool KFAccountMongo::RemoveAccountWhiteList( uint64 accountid )
    {
        auto mongodriver = __ACCOUNT_MONGO_DRIVER__;
        return mongodriver->Delete( __STRING__( accountwhitelist ), accountid );
    }

    StringMapList KFAccountMongo::QueryAccountWhiteList()
    {
        auto mongodriver = __ACCOUNT_MONGO_DRIVER__;
        auto kfresult = mongodriver->QueryListRecord( __STRING__( accountwhitelist ) );

        StringMapList stringmaplist;
        for ( auto& dbvalue : kfresult->_value )
        {
            StringMap values;
            __DBVALUE_TO_MAP__( dbvalue, values );
            stringmaplist.emplace_back( values );
        }

        return stringmaplist;
    }
}