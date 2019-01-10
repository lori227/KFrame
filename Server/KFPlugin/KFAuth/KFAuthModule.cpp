#include "KFAuthModule.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
#define __ZONE_REDIS_DRIVER__ _kf_redis->Create( __KF_STRING__( zone ) )
#define __ACCOUNT_REDIS_DRIVER__ _kf_redis->Create( __KF_STRING__( account ) )

#define __PUBLIC_REDIS_DRIVER__ _kf_redis->Create( __KF_STRING__( public ) )

    void KFAuthModule::BeforeRun()
    {
        __REGISTER_HTTP_FUNCTION__( __KF_STRING__( zoneregister ), false, &KFAuthModule::HandleZoneRegister );
        __REGISTER_HTTP_FUNCTION__( __KF_STRING__( zoneupdate ), false, &KFAuthModule::HandleZoneUpdate );
        __REGISTER_HTTP_FUNCTION__( __KF_STRING__( zonelist ), false, &KFAuthModule::HandleQueryZoneList );
        __REGISTER_HTTP_FUNCTION__( __KF_STRING__( zoneip ), false, &KFAuthModule::HandleQueryZoneIp );

        __REGISTER_HTTP_FUNCTION__( __KF_STRING__( auth ), false, &KFAuthModule::HandleAuthLogin );
        __REGISTER_HTTP_FUNCTION__( __KF_STRING__( verify ), false, &KFAuthModule::HandleVerifyToken );
        __REGISTER_HTTP_FUNCTION__( __KF_STRING__( activation ), false, &KFAuthModule::HandleAuthActivation );
        __REGISTER_HTTP_FUNCTION__( __KF_STRING__( onlinedata ), false, &KFAuthModule::HandleOnlineData );

        __REGISTER_HTTP_FUNCTION__( __KF_STRING__( ban ), false, &KFAuthModule::HandleBanLogin );
        __REGISTER_HTTP_FUNCTION__( __KF_STRING__( unban ), false, &KFAuthModule::HandleUnBanLogin );
        __REGISTER_HTTP_FUNCTION__( __KF_STRING__( queryban ), false, &KFAuthModule::HandleQueryBanLogin );
        __REGISTER_HTTP_FUNCTION__( __KF_STRING__( queryaccount ), false, &KFAuthModule::HandleQueryAccountData );
        /////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFAuthModule::BeforeShut()
    {
        __UNREGISTER_HTTP_FUNCTION__( __KF_STRING__( zoneregister ) );
        __UNREGISTER_HTTP_FUNCTION__( __KF_STRING__( zoneupdate ) );
        __UNREGISTER_HTTP_FUNCTION__( __KF_STRING__( zonelist ) );
        __UNREGISTER_HTTP_FUNCTION__( __KF_STRING__( zoneip ) );

        __UNREGISTER_HTTP_FUNCTION__( __KF_STRING__( auth ) );
        __UNREGISTER_HTTP_FUNCTION__( __KF_STRING__( verify ) );
        __UNREGISTER_HTTP_FUNCTION__( __KF_STRING__( activation ) );
        __UNREGISTER_HTTP_FUNCTION__( __KF_STRING__( onlinedata ) );

        __UNREGISTER_HTTP_FUNCTION__( __KF_STRING__( ban ) );
        __UNREGISTER_HTTP_FUNCTION__( __KF_STRING__( unban ) );
        __UNREGISTER_HTTP_FUNCTION__( __KF_STRING__( queryban ) );
        __UNREGISTER_HTTP_FUNCTION__( __KF_STRING__( queryaccount ) );
        ///////////////////////////////////////////////////////////////////////////
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_HTTP_FUNCTION__( KFAuthModule::HandleZoneRegister )
    {
        __JSON_PARSE_STRING__( request, data );

        auto id = __JSON_GET_UINT32__( request, __KF_STRING__( id ) );
        auto name = __JSON_GET_STRING__( request, __KF_STRING__( name ) );
        auto url = __JSON_GET_UINT32__( request, __KF_STRING__( url ) );

        // 先保存小区基本信息
        auto redisdriver = __ZONE_REDIS_DRIVER__;
        redisdriver->Append( "zincrby {} 0 {}", __KF_STRING__( zonelist ), id );
        redisdriver->Append( "hmset {}:{} {} {} {} {}", __KF_STRING__( zone ), id, __KF_STRING__( id ), id, __KF_STRING__( name ), name );
        redisdriver->Append( "set {}:{} {}", __KF_STRING__( zoneurl ), id, url );
        auto kfresult = redisdriver->Pipeline();
        if ( !kfresult->IsOk() )
        {
            return _kf_http_server->SendResponseCode( KFMsg::Error );
        }

        return _kf_http_server->SendResponseCode( KFMsg::Ok );
    }

    __KF_HTTP_FUNCTION__( KFAuthModule::HandleQueryZoneList )
    {
        auto redisdriver = __ZONE_REDIS_DRIVER__;
        auto kflist = redisdriver->QueryList( "zrange {} 0 -1", __KF_STRING__( zonelist ) );
        if ( !kflist->IsOk() )
        {
            return _kf_http_server->SendResponseCode( KFMsg::AuthDatabaseBusy );
        }

        __JSON_DOCUMENT__( response );
        __JSON_ARRAY__( kfarray );
        for ( auto& strid : kflist->_value )
        {
            auto kfmap = redisdriver->QueryMap( "hgetall {}:{}", __KF_STRING__( zone ), strid );
            if ( kfmap->_value.empty() )
            {
                __JSON_OBJECT__( kfzone );
                __JSON_FROM_MAP__( kfzone, kfmap->_value );
                __JSON_ADD_VALUE__( kfarray, kfzone );
            }
        }
        __JSON_SET_VALUE__( response, __KF_STRING__( zonelist ), kfarray );

        return _kf_http_server->SendResponse( response );
    }

    __KF_HTTP_FUNCTION__( KFAuthModule::HandleZoneUpdate )
    {
        __JSON_PARSE_STRING__( request, data );

        auto id = __JSON_GET_UINT32__( request, __KF_STRING__( id ) );
        auto count = __JSON_GET_UINT32__( request, __KF_STRING__( count ) );

        auto redisdriver = __ZONE_REDIS_DRIVER__;

        // 先读取数量
        auto kfcount = redisdriver->QueryUInt64( "hget {}:{} {}", __KF_STRING__( zoneip ), id, __KF_STRING__( count ) );
        if ( !kfcount->IsOk() )
        {
            return _kf_http_server->SendResponseCode( KFMsg::AuthDatabaseBusy );
        }

        // 更新比较小的
        if ( kfcount->_value == _invalid_int || kfcount->_value > count )
        {
            MapString values;
            __JSON_TO_MAP__( request, values );
            redisdriver->Update( values, "hmset {}:{}", __KF_STRING__( zoneip ), id );
        }

        redisdriver->Execute( "expire {}:{} {}", __KF_STRING__( zoneip ), id, 30 );
        return _kf_http_client->SendResponseCode( KFMsg::Ok );
    }

    __KF_HTTP_FUNCTION__( KFAuthModule::HandleQueryZoneIp )
    {
        __JSON_PARSE_STRING__( request, data );

        auto id = __JSON_GET_UINT32__( request, __KF_STRING__( id ) );

        auto redisdriver = __ZONE_REDIS_DRIVER__;

        auto kfmap = redisdriver->QueryMap( "hgetall {}:{}", __KF_STRING__( zoneip ), id );
        if ( !kfmap->IsOk() )
        {
            return _kf_http_server->SendResponseCode( KFMsg::AuthDatabaseBusy );
        }

        __JSON_DOCUMENT__( response );
        __JSON_SET_VALUE__( response, __KF_STRING__( id ), id );
        __JSON_SET_VALUE__( response, __KF_STRING__( ip ), kfmap->_value[ __KF_STRING__( ip ) ] );
        __JSON_SET_VALUE__( response, __KF_STRING__( port ), KFUtility::ToValue( kfmap->_value[ __KF_STRING__( port ) ] ) );

        return _kf_http_server->SendResponse( response );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_HTTP_FUNCTION__( KFAuthModule::HandleAuthLogin )
    {
        // 渠道验证
        auto authdata = _kf_channel->AuthChannelLogin( data );

        __JSON_PARSE_STRING__( authjson, authdata );
        auto retcode = _kf_http_server->GetResponseCode( authjson );
        if ( retcode != KFMsg::Ok )
        {
            return _kf_http_server->SendResponseCode( retcode );
        }

        auto account = __JSON_GET_STRING__( authjson, __KF_STRING__( account ) );
        auto channel = __JSON_GET_UINT32__( authjson, __KF_STRING__( channel ) );
        if ( account.empty() )
        {
            return _kf_http_server->SendResponseCode( KFMsg::AccountIsEmpty );
        }

        __LOG_DEBUG__( "account[{}] channel[{}] auth login!", account, channel );

        // 查询创建账号
        auto accountdata = QueryCreateAccount( account, channel );
        auto accountid = KFUtility::ToValue( accountdata[ __KF_STRING__( accountid ) ] );
        if ( accountid == 0 )
        {
            return _kf_http_server->SendResponseCode( KFMsg::AuthServerBusy );
        }

        // 判断是否需要激活
        static auto _open_activation_option = _kf_option->FindOption( __KF_STRING__( openactivation ) );
        if ( _open_activation_option->_uint32_value == 1 )
        {
            auto activation = accountdata[ __KF_STRING__( activation ) ];
            if ( activation.empty() )
            {
                __JSON_DOCUMENT__( response );
                __JSON_SET_VALUE__( response, __KF_STRING__( accountid ), accountid );
                __JSON_SET_VALUE__( response, __KF_STRING__( retcode ), KFMsg::ActivationAccount );
                return _kf_http_server->SendResponse( response );
            }
        }

        // 判断是否已经在线, 在线需要踢人下线
        KickAccountOffline( accountdata );

        // 保存渠道的数据
        UpdateChannelData( accountid, channel, authjson );

        // 保存token
        auto token = CreateLoginToken( accountid, accountdata );

        // 查询小区信息, 返回给客户端
        return QueryZoneData( accountid, token, accountdata );
    }

    void KFAuthModule::UpdateChannelData( uint64 accountid, uint32 channel, KFJson& kfjson )
    {
        if ( !__JSON_HAS_MEMBER__( kfjson, __KF_STRING__( extend ) ) )
        {
            return;
        }

        auto& kfextend = kfjson[ __KF_STRING__( extend ) ];

        MapString values;
        __JSON_TO_MAP__( kfextend, values );

        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        redisdriver->Update( values, "hmset {}:{}", __KF_STRING__( extend ), accountid );
    }

    MapString KFAuthModule::QueryCreateAccount( const std::string& account, uint32 channel )
    {
        MapString accountdata;
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;

        // 先查询redis
        auto kfquery = redisdriver->QueryUInt64( "hget {}:{}:{} {}", __KF_STRING__( account ), account, channel, __KF_STRING__( accountid ) );
        if ( !kfquery->IsOk() )
        {
            __LOG_DEBUG__( "account[{}] channel[{}] query accountid failed!", account, channel );
            return accountdata;
        }

        auto accountid = kfquery->_value;
        if ( accountid != _invalid_int )
        {
            auto queryaccountdata = redisdriver->QueryMap( "hgetall {}:{}", __KF_STRING__( accountid ), accountid );
            if ( !queryaccountdata->IsOk() || !queryaccountdata->_value.empty() )
            {
                return queryaccountdata->_value;
            }
        }
        else
        {
            auto newid = redisdriver->Execute( "incr {}", __KF_STRING__( accountmake ) );
            if ( newid->_value == _invalid_int )
            {
                __LOG_DEBUG__( "account[{}] channel[{}] incr newid failed!", account, channel );
                return accountdata;
            }

            accountid = newid->_value + 500000u;
        }

        // 创建账号id
        accountdata[ __KF_STRING__( account ) ] = account;
        accountdata[ __KF_STRING__( channel ) ] = __TO_STRING__( channel );
        accountdata[ __KF_STRING__( accountid ) ] = __TO_STRING__( accountid );

        redisdriver->Append( "hset {}:{}:{} {} {}", __KF_STRING__( account ), account, channel, __KF_STRING__( accountid ), accountid );
        redisdriver->Append( "sadd {} {}", __KF_STRING__( accountlist ), accountid );
        redisdriver->Append( accountdata, "hmset {}:{}", __KF_STRING__( accountid ), accountid );
        auto kfresult = redisdriver->Pipeline();
        if ( kfresult->IsOk() )
        {
            __LOG_DEBUG__( "create account successfully, account[{}] accountid[{}] channel[{}] ", account, accountid, channel );
        }
        else
        {
            // 失败清空数据
            accountdata.clear();
            __LOG_DEBUG__( "account[{}] channel[{}] save account failed!", account, channel );
        }

        return accountdata;
    }

    std::string KFAuthModule::CreateLoginToken( uint64 accountid, MapString& accountdata )
    {
        static auto _token_expire_time_option = _kf_option->FindOption( __KF_STRING__( tokenexpiretime ) );

        // 创建token
        auto md5temp = __FORMAT__( "{}-{}", accountid, KFGlobal::Instance()->_game_time );
        auto token = KFCrypto::Md5Encode( md5temp );

        auto channel = accountdata[ __KF_STRING__( channel ) ];
        auto account = accountdata[ __KF_STRING__( account ) ];
        auto tokenkey = __FORMAT__( "{}:{}", __KF_STRING__( login ), token );

        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        redisdriver->Append( "hmset {} {} {} {} {} {} {}", tokenkey, __KF_STRING__( account ), account, __KF_STRING__( accountid ), accountid, __KF_STRING__( channel ), channel );
        redisdriver->Append( "expire {} {}", tokenkey, _token_expire_time_option->_str_value );
        redisdriver->Pipeline();

        return token;
    }

    bool KFAuthModule::KickAccountOffline( MapString& accountdata )
    {
        auto online = accountdata[ __KF_STRING__( online ) ];
        if ( online != "1" )
        {
            return false;
        }

        auto redisdriver = __ZONE_REDIS_DRIVER__;
        auto strzoneid = accountdata[ __KF_STRING__( zoneid ) ];
        auto kfquery = redisdriver->QueryString( "get {}:{}", __KF_STRING__( zoneurl ), strzoneid );
        if ( !kfquery->IsOk() || kfquery->_value.empty() )
        {
            return false;
        }

        auto kickurl = kfquery->_value + __KF_STRING__( kickonline );

        __JSON_DOCUMENT__( kfkickjson );
        __JSON_SET_VALUE__( kfkickjson, __KF_STRING__( playerid ), accountdata[ __KF_STRING__( playerid ) ] );
        _kf_http_client->StartMTHttpClient( kickurl, kfkickjson );

        return true;
    }

    uint32 KFAuthModule::BalanceAllocZoneId()
    {
        // 选择一个最小人数的分区
        auto redisdriver = __ZONE_REDIS_DRIVER__;
        auto zonelist = redisdriver->QueryList( "zrange {} 0 -1", __KF_STRING__( zonelist ) );
        if ( zonelist->_value.empty() )
        {
            return _invalid_int;
        }

        for ( auto& strid : zonelist->_value )
        {
            auto queryip = redisdriver->QueryString( "hget {}:{} {}", __KF_STRING__( zoneip ), strid, __KF_STRING__( ip ) );
            if ( !queryip->_value.empty() )
            {
                return KFUtility::ToValue< uint32 >( strid );
            }
        }

        return _invalid_int;
    }

    std::string KFAuthModule::QueryZoneData( uint64 accountid, const std::string& token, MapString& accountdata )
    {
        // 上次登录的小区id
        auto zoneid = KFUtility::ToValue< uint32 >( accountdata[ __KF_STRING__( zoneid ) ] );
        if ( zoneid == _invalid_int )
        {
            // 没有登录, 说明是个新号, 找到负载最小的一个小区信息
            zoneid = BalanceAllocZoneId();
            if ( zoneid == _invalid_int )
            {
                return _kf_http_server->SendResponseCode( KFMsg::ZoneServerBusy );
            }
        }

        // 查询小区信息
        auto redisdriver = __ZONE_REDIS_DRIVER__;

        auto kfzonemap = redisdriver->QueryMap( "hgetall {}:{}", __KF_STRING__( zone ), zoneid );
        if ( !kfzonemap->IsOk() )
        {
            return _kf_http_server->SendResponseCode( KFMsg::ZoneDatabaseBusy );
        }

        __JSON_DOCUMENT__( response );
        __JSON_SET_VALUE__( response, __KF_STRING__( accountid ), accountid );
        __JSON_SET_VALUE__( response, __KF_STRING__( token ), token );

        __JSON_OBJECT__( kfzone );
        __JSON_FROM_MAP__( kfzone, kfzonemap->_value );

        auto kfzoneipmap = redisdriver->QueryMap( "hgetall {}:{}", __KF_STRING__( zoneip ), zoneid );
        if ( !kfzoneipmap->_value.empty() )
        {
            __JSON_SET_VALUE__( kfzone, __KF_STRING__( ip ), kfzoneipmap->_value[ __KF_STRING__( ip ) ] );
            __JSON_SET_VALUE__( kfzone, __KF_STRING__( port ), KFUtility::ToValue( kfzoneipmap->_value[ __KF_STRING__( port ) ] ) );
        }

        return _kf_http_server->SendResponse( response );
    }

    __KF_HTTP_FUNCTION__( KFAuthModule::HandleAuthActivation )
    {
        __JSON_PARSE_STRING__( request, data );
        auto accountid = __JSON_GET_UINT64__( request, __KF_STRING__( accountid ) );
        if ( accountid == _invalid_int )
        {
            return _kf_http_server->SendResponseCode( KFMsg::AccountIsEmpty );
        }

        // 查询创建账号
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        auto accountdata = redisdriver->QueryMap( "hgetall {}:{}", __KF_STRING__( accountid ), accountid );
        if ( !accountdata->IsOk() || accountdata->_value.empty() )
        {
            return _kf_http_server->SendResponseCode( KFMsg::AuthDatabaseBusy );
        }

        auto activation = accountdata->_value[ __KF_STRING__( activation ) ];
        if ( activation.empty() )
        {
            return _kf_http_server->SendResponseCode( KFMsg::InvalidActivationCode );
        }

        // 保存激活码
        auto code = __JSON_GET_STRING__( request, __KF_STRING__( code ) );

        //// 判断激活码是否存在
        //auto isexist = redisdriver->QueryUInt64( "sismember {} {}", __KF_STRING__( activationcode ), code );
        //if ( !isexist->IsOk() )
        //{
        //    return _kf_http_server->SendResponseCode( KFMsg::AuthDatabaseBusy );
        //}

        //if ( isexist->_value == _invalid_int )
        //{
        //    return _kf_http_server->SendResponseCode( KFMsg::ActivationCodeError );
        //}

        //redisdriver->Append( "srem {} {}", __KF_STRING__( activationcode ), code );

        return VerifyActivationCode( accountdata->_value, accountid, code );
    }

    std::string KFAuthModule::VerifyActivationCode( MapString& accountdata, uint64 accountid, const std::string& activationcode )
    {
        auto apiurl = _kf_platform->MakePlatformUrl( __KF_STRING__( verifyactivationcode ) );

        auto account = accountdata[ __KF_STRING__( account ) ];

        __JSON_DOCUMENT__( postjson );
        __JSON_SET_VALUE__( postjson, __KF_STRING__( accountid ), accountid );
        __JSON_SET_VALUE__( postjson, __KF_STRING__( activationCode ), activationcode );
        __JSON_SET_VALUE__( postjson, __KF_STRING__( openid ), account );
        __JSON_SET_VALUE__( postjson, __KF_STRING__( channel ), KFGlobal::Instance()->_app_id._union._app_data._channel_id );

        //去平台通过激活码激活
        auto result = _kf_http_client->StartSTHttpClient( apiurl, postjson );
        __JSON_PARSE_STRING__( resp, result );

        if ( !__JSON_HAS_MEMBER__( resp, __KF_STRING__( code ) ) )
        {
            __LOG_ERROR__( "Activate code failed, account={} accountid={} info={}", account, accountid, result );
            _kf_http_server->SendResponseCode( KFMsg::ActivationCodeError );
            return _invalid_str;
        }

        auto code = __JSON_GET_UINT32__( resp, __KF_STRING__( code ) );
        if ( code != 0 )
        {
            __LOG_ERROR__( "Activate code failed, account={} accountid={} info={}", account, accountid, result );
            _kf_http_server->SendResponseCode( KFMsg::ActivationCodeError );
            return _invalid_str;
        }

        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        redisdriver->Execute( "hset {}:{} {} {}", __KF_STRING__( accountid ), accountid, __KF_STRING__( activation ), activationcode );

        // 创建登录token
        auto token = CreateLoginToken( accountid, accountdata );
        return QueryZoneData( accountid, token, accountdata );
    }

    __KF_HTTP_FUNCTION__( KFAuthModule::HandleVerifyToken )
    {
        __JSON_PARSE_STRING__( request, data );

        auto loginip = __JSON_GET_STRING__( request, __KF_STRING__( ip ) );
        auto zoneid = __JSON_GET_UINT32__( request, __KF_STRING__( zoneid ) );
        auto accountid = __JSON_GET_UINT64__( request, __KF_STRING__( accountid ) );
        auto logiczoneid = __JSON_GET_UINT32__( request, __KF_STRING__( logiczoneid ) );
        if ( zoneid == _invalid_int || logiczoneid == _invalid_int )
        {
            return _kf_http_server->SendResponseCode( KFMsg::HttpDataError );
        }

        // 判断token是否正确
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        auto token = __JSON_GET_STRING__( request, __KF_STRING__( token ) );
        auto loginkey = __FORMAT__( "{}:{}", __KF_STRING__( login ), token );
        auto querytoken = redisdriver->QueryMap( "hgetall {}", loginkey );
        if ( !querytoken->IsOk() )
        {
            return _kf_http_server->SendResponseCode( KFMsg::AuthDatabaseBusy );
        }

        // 获得账号和渠道
        auto account = querytoken->_value[ __KF_STRING__( account ) ];
        auto channel = KFUtility::ToValue< uint32 >( querytoken->_value[ __KF_STRING__( channel ) ] );
        auto queryaccountid = KFUtility::ToValue( querytoken->_value[ __KF_STRING__( accountid ) ] );
        if ( queryaccountid == _invalid_int || channel == _invalid_int || queryaccountid != accountid )
        {
            return _kf_http_server->SendResponseCode( KFMsg::LoginTokenError );
        }

        auto playerid = QueryCreatePlayerId( channel, accountid, zoneid, logiczoneid );
        if ( playerid == _invalid_int )
        {
            return _kf_http_server->SendResponseCode( KFMsg::AuthDatabaseBusy );
        }

        // 判断黑名单
        auto bantime = CheckLoginBlackList( loginip, accountid, playerid );
        if ( bantime != _invalid_int )
        {
            __JSON_DOCUMENT__( response );
            __JSON_SET_VALUE__( response, __KF_STRING__( bantime ), bantime );
            return _kf_http_server->SendResponse( response, KFMsg::BanForbidLogin );
        }

        // 保存玩家登陆ip
        redisdriver->Execute( "hset {}:{} {} {}", __KF_STRING__( accountid ), queryaccountid, __KF_STRING__( ip ), loginip );

        // 返回结果
        __JSON_DOCUMENT__( response );
        __JSON_SET_VALUE__( response, __KF_STRING__( token ), token );
        __JSON_SET_VALUE__( response, __KF_STRING__( channel ), channel );
        __JSON_SET_VALUE__( response, __KF_STRING__( playerid ), playerid );
        __JSON_SET_VALUE__( response, __KF_STRING__( account ), account );
        __JSON_SET_VALUE__( response, __KF_STRING__( accountid ), accountid );

        // 加载渠道数据
        auto kfresult = redisdriver->QueryMap( "hgetall {}:{}", __KF_STRING__( extend ), accountid );
        if ( !kfresult->_value.empty() )
        {
            KFJson kfchanneljson;
            __JSON_FROM_MAP__( kfchanneljson, kfresult->_value );
            __JSON_SET_VALUE__( response, __KF_STRING__( channeldata ), kfchanneljson );
        }

        return _kf_http_server->SendResponse( response );
    }

    uint64 KFAuthModule::QueryCreatePlayerId( uint32 channel, uint64 accountid, uint32 zoneid, uint32 logiczoneid )
    {
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;

        // 查询是否存在
        auto queryplayerid = redisdriver->QueryString( "hget {}:{} {}", __KF_STRING__( user ), accountid, logiczoneid );
        if ( !queryplayerid->IsOk() )
        {
            return _invalid_int;
        }

        // 存在playerid, 直接返回
        if ( !queryplayerid->_value.empty() )
        {
            return KFUtility::ToValue( queryplayerid->_value );
        }

        // 创建playerid
        auto uint64result = redisdriver->Execute( "incr {}:{}", __KF_STRING__( playeridcreate ), zoneid );
        if ( !uint64result->IsOk() || uint64result->_value == _invalid_int )
        {
            return _invalid_int;
        }

        auto newuserid = uint64result->_value + 10000u;
        auto playerid = KFUtility::CalcPlayerid( newuserid, zoneid );

        redisdriver->Execute( "hset {}:{} {} {}", __KF_STRING__( player ), playerid, __KF_STRING__( accountid ), accountid );
        auto voidresult = redisdriver->Execute( "hset {}:{} {} {}", __KF_STRING__( user ), accountid, logiczoneid, playerid );
        if ( !voidresult->IsOk() )
        {
            return _invalid_int;
        }

        // 小区玩家数量更新
        {
            auto redisdriver = __ZONE_REDIS_DRIVER__;
            redisdriver->Append( "zincrby {} 1 {}", __KF_STRING__( zonelist ), zoneid );
        }

        return playerid;
    }

    __KF_HTTP_FUNCTION__( KFAuthModule::HandleOnlineData )
    {
        __JSON_PARSE_STRING__( request, data );

        auto accountid = __JSON_GET_STRING__( request, __KF_STRING__( accountid ) );

        MapString values;
        __JSON_TO_MAP__( request, values );

        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        redisdriver->Update( values, "hmset {}:{}", __KF_STRING__( accountid ), accountid );

        return _kf_http_server->SendResponseCode( KFMsg::Ok );
    }

    uint64 KFAuthModule::CheckLoginBlackList( const std::string& ip, uint64 accountid, uint64 playerid )
    {
        // 数据保存在自己的数据库, 没有后台也可以直接操作数据库
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;

        // ip
        auto kfquery = redisdriver->QueryString( "hget {} {}", __KF_STRING__( banip ), ip );
        if ( kfquery->IsOk() && !kfquery->_value.empty() )
        {
            auto bantime = KFUtility::ToValue< uint64 >( kfquery->_value );
            if ( bantime > KFGlobal::Instance()->_real_time )
            {
                return bantime;
            }

            redisdriver->Execute( "hdel {} {}", __KF_STRING__( banip ), ip );
        }

        // accountid
        kfquery = redisdriver->QueryString( "hget {} {}", __KF_STRING__( banaccountid ), accountid );
        if ( kfquery->IsOk() && !kfquery->_value.empty() )
        {
            auto bantime = KFUtility::ToValue< uint64 >( kfquery->_value );
            if ( bantime > KFGlobal::Instance()->_real_time )
            {
                return bantime;
            }

            redisdriver->Execute( "hdel {} {}", __KF_STRING__( banaccountid ), accountid );
        }

        // playerid
        kfquery = redisdriver->QueryString( "hget {} {}", __KF_STRING__( banplayerid ), playerid );
        if ( kfquery->IsOk() && !kfquery->_value.empty() )
        {
            auto bantime = KFUtility::ToValue< uint64 >( kfquery->_value );
            if ( bantime > KFGlobal::Instance()->_real_time )
            {
                return bantime;
            }

            redisdriver->Execute( "hdel {} {}", __KF_STRING__( banplayerid ), playerid );
        }

        return _invalid_int;
    }

    __KF_HTTP_FUNCTION__( KFAuthModule::HandleBanLogin )
    {
        __JSON_PARSE_STRING__( request, data );
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;

        auto bantime = __JSON_GET_UINT64__( request, __KF_STRING__( bantime ) );

        uint64 kickaccountid = _invalid_int;

        // 账号
        if ( __JSON_HAS_MEMBER__( request, __KF_STRING__( accountid ) ) )
        {
            auto accountid = __JSON_GET_UINT64__( request, __KF_STRING__( accountid ) );
            redisdriver->Execute( "hset {} {} {}", __KF_STRING__( banaccountid ), accountid, bantime );

            kickaccountid = accountid;
        }

        // 角色
        if ( __JSON_HAS_MEMBER__( request, __KF_STRING__( playerid ) ) )
        {
            auto playerid = __JSON_GET_UINT64__( request, __KF_STRING__( playerid ) );
            redisdriver->Execute( "hset {} {} {}", __KF_STRING__( banplayerid ), playerid, bantime );

            if ( kickaccountid != _invalid_int )
            {
                auto kfquery = redisdriver->QueryString( "hget {}:{} {}", __KF_STRING__( player ), playerid, __KF_STRING__( accountid ) );
                kickaccountid = KFUtility::ToValue< uint64 >( kfquery->_value );
            }
        }

        // ip
        if ( __JSON_HAS_MEMBER__( request, __KF_STRING__( ip ) ) )
        {
            auto banip = __JSON_GET_STRING__( request, __KF_STRING__( ip ) );
            redisdriver->Execute( "hset {} {} {}", __KF_STRING__( banip ), banip, bantime );
        }

        // 踢人
        if ( kickaccountid != _invalid_int )
        {
            auto accountdata = redisdriver->QueryMap( "hgetall {}:{}", __KF_STRING__( accountid ), kickaccountid );
            KickAccountOffline( accountdata->_value );
        }

        return _kf_http_server->SendResponseCode( KFMsg::Ok );
    }

    __KF_HTTP_FUNCTION__( KFAuthModule::HandleUnBanLogin )
    {
        __JSON_PARSE_STRING__( request, data );
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;

        // 账号
        if ( __JSON_HAS_MEMBER__( request, __KF_STRING__( accountid ) ) )
        {
            auto accountid = __JSON_GET_UINT64__( request, __KF_STRING__( accountid ) );
            redisdriver->Execute( "hset {} {}", __KF_STRING__( banaccountid ), accountid );
        }

        // 角色
        if ( __JSON_HAS_MEMBER__( request, __KF_STRING__( playerid ) ) )
        {
            auto playerid = __JSON_GET_UINT64__( request, __KF_STRING__( playerid ) );
            redisdriver->Execute( "hdel {} {}", __KF_STRING__( banplayerid ), playerid );
        }

        // ip
        if ( __JSON_HAS_MEMBER__( request, __KF_STRING__( ip ) ) )
        {
            auto banip = __JSON_GET_STRING__( request, __KF_STRING__( ip ) );
            redisdriver->Execute( "hset {} {}", __KF_STRING__( banip ), banip );
        }

        return _kf_http_server->SendResponseCode( KFMsg::Ok );
    }

    __KF_HTTP_FUNCTION__( KFAuthModule::HandleQueryBanLogin )
    {
        __JSON_DOCUMENT__( response );
        __JSON_PARSE_STRING__( request, data );
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;

        // 角色
        if ( __JSON_HAS_MEMBER__( request, __KF_STRING__( playerid ) ) )
        {
            auto playerid = __JSON_GET_UINT64__( request, __KF_STRING__( playerid ) );
            auto kfquery = redisdriver->QueryString( "hget {} {}", __KF_STRING__( banplayerid ), playerid );
            __JSON_SET_VALUE__( response, __KF_STRING__( banplayerid ), kfquery->_value );
        }

        // 账号
        if ( __JSON_HAS_MEMBER__( request, __KF_STRING__( accountid ) ) )
        {
            auto accountid = __JSON_GET_UINT64__( request, __KF_STRING__( accountid ) );
            auto kfquery = redisdriver->QueryString( "hget {} {}", __KF_STRING__( banaccountid ), accountid );
            __JSON_SET_VALUE__( response, __KF_STRING__( banaccountid ), kfquery->_value );
        }

        // ip
        if ( __JSON_HAS_MEMBER__( request, __KF_STRING__( ip ) ) )
        {
            auto banip = __JSON_GET_STRING__( request, __KF_STRING__( ip ) );
            auto kfquery = redisdriver->QueryString( "hget {} {}", __KF_STRING__( banip ), banip );
            __JSON_SET_VALUE__( response, __KF_STRING__( banip ), kfquery->_value );
        }

        return _kf_http_server->SendResponse( response );
    }

    __KF_HTTP_FUNCTION__( KFAuthModule::HandleQueryAccountData )
    {
        __JSON_PARSE_STRING__( request, data );
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;

        auto accountid = __JSON_GET_STRING__( request, __KF_STRING__( accountid ) );
        if ( accountid.empty() )
        {
            auto playerid = __JSON_GET_STRING__( request, __KF_STRING__( playerid ) );
            if ( playerid.empty() )
            {
                auto playername = __JSON_GET_STRING__( request, __KF_STRING__( name ) );

                auto publicdriver = __PUBLIC_REDIS_DRIVER__;
                auto kfquery = publicdriver->QueryString( "get {}:{}", __KF_STRING__( name ), playername );
                playerid = kfquery->_value;
            }

            auto kfquery = redisdriver->QueryString( "hget {}:{} {}", __KF_STRING__( player ), playerid, __KF_STRING__( accountid ) );
            accountid = kfquery->_value;
        }

        auto kfquery = redisdriver->QueryMap( "hgetall {}:{}", __KF_STRING__( accountid ), accountid );

        __JSON_DOCUMENT__( response );
        __JSON_FROM_MAP__( response, kfquery->_value );
        return _kf_http_server->SendResponse( response );
    }
}