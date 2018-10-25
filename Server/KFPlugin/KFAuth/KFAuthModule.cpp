#include "KFAuthModule.h"
#include "KFJson.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
#define __DIR_REDIS_DRIVER__ _kf_redis->CreateExecute( __KF_STRING__( dir ) )
#define __ACCOUNT_REDIS_DRIVER__ _kf_redis->CreateExecute( __KF_STRING__( account ) )

    void KFAuthModule::BeforeRun()
    {
        __REGISTER_HTTP_FUNCTION__( __KF_STRING__( auth ), false, &KFAuthModule::HandleAuthLogin );
        __REGISTER_HTTP_FUNCTION__( __KF_STRING__( verify ), false, &KFAuthModule::HandleVerifyToken );
        __REGISTER_HTTP_FUNCTION__( __KF_STRING__( activation ), false, &KFAuthModule::HandleAuthActivation );
        __REGISTER_HTTP_FUNCTION__( __KF_STRING__( onlinezone ), false, &KFAuthModule::HandleUpdateOnline );
        /////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFAuthModule::BeforeShut()
    {
        __UNREGISTER_HTTP_FUNCTION__( __KF_STRING__( auth ) );
        __UNREGISTER_HTTP_FUNCTION__( __KF_STRING__( verify ) );
        __UNREGISTER_HTTP_FUNCTION__( __KF_STRING__( activation ) );
        ///////////////////////////////////////////////////////////////////////////
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_HTTP_FUNCTION__( KFAuthModule::HandleAuthLogin )
    {
        // 判断ip黑名单
        //authjson[ __KF_STRING__( ip ) ] = ip;

        // 渠道验证
        auto authdata = _kf_channel->AuthChannelLogin( data );

        KFJson authjson( authdata );
        auto retcode = _kf_http_server->GetResponseCode( authjson );
        if ( retcode != KFMsg::Success )
        {
            return _kf_http_server->SendResponseCode( retcode );
        }

        auto account = authjson.GetString( __KF_STRING__( account ) );
        auto channel = authjson.GetUInt32( __KF_STRING__( channel ) );
        if ( account.empty() )
        {
            return _kf_http_server->SendResponseCode( KFMsg::AccountIsEmpty );
        }

        // 查询创建账号
        auto accountdata = QueryCreateAccount( account, channel );
        auto accountid = KFUtility::ToValue<uint32>( accountdata[ __KF_STRING__( accountid ) ] );
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
                KFJson response;
                response.SetValue( __KF_STRING__( accountid ), accountid );
                response.SetValue<uint32>( __KF_STRING__( retcode ), KFMsg::ActivationAccount );
                return _kf_http_server->SendResponse( response );
            }
        }

        // 判断是否已经在线, 在线需要踢人下线
        auto ok = KickAccountOffline( accountdata );
        if ( !ok )
        {
            return _kf_http_server->SendResponseCode( KFMsg::LoginAlreadyOnline );
        }

        // 保存渠道的数据
        // UpdateChannelData( accountid, channel, authjson );

        // 保存token
        auto token = CreateLoginToken( accountid, accountdata );

        // todo: 目前在认证服务器返回登录列表,
        // todo: 以后只返回目录服务器地址, 客户端去目录服务器请求
        // 获得dir服务器地址
        return QueryDirList( accountid, token, accountdata );
    }

    void KFAuthModule::UpdateChannelData( uint32 accountid, uint32 channel, KFJson& kfjson )
    {
        MapString values;
        values[ __KF_STRING__( name ) ] = kfjson.GetString( __KF_STRING__( name ) );
        values[ __KF_STRING__( sex ) ] = kfjson.GetString( __KF_STRING__( sex ) );
        values[ __KF_STRING__( icon ) ] = kfjson.GetString( __KF_STRING__( icon ) );

        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        redisdriver->Update( values, "hmset {}:{}", __KF_STRING__( extend ), accountid );
    }

    MapString KFAuthModule::QueryCreateAccount( const std::string& account, uint32 channel )
    {
        MapString accountdata;
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;

        // 先查询redis
        auto queryaccountid = redisdriver->QueryUInt32( "hget {}:{}:{} {}", __KF_STRING__( account ), account, channel, __KF_STRING__( accountid ) );
        if ( !queryaccountid->IsOk() )
        {
            return accountdata;
        }

        uint32 accountid = queryaccountid->_value;
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
            auto newid = redisdriver->QueryUInt64( "incr {}", __KF_STRING__( accountmake ) );
            if ( newid->_value == _invalid_int )
            {
                return accountdata;
            }

            accountid = static_cast< uint32 >( newid->_value ) + 500000;
        }

        // 创建账号id
        accountdata[ __KF_STRING__( accountflag ) ] = "0";
        accountdata[ __KF_STRING__( account ) ] = account;
        accountdata[ __KF_STRING__( channel ) ] = __TO_STRING__( channel );
        accountdata[ __KF_STRING__( accountid ) ] = __TO_STRING__( accountid );

        redisdriver->Append( "hset {}:{}:{} {} {}", __KF_STRING__( account ), account, channel, __KF_STRING__( accountid ), accountid );
        redisdriver->Append( "sadd {} {}", __KF_STRING__( accountlist ), accountid );
        redisdriver->Append( accountdata, "hmset {}:{}", __KF_STRING__( accountid ), accountid );
        auto kfresult = redisdriver->Pipeline();
        if ( !kfresult->IsOk() )
        {
            // 失败清空数据
            accountdata.clear();
        }

        return accountdata;
    }

    std::string KFAuthModule::CreateLoginToken( uint32 accountid, MapString& accountdata )
    {
        static auto _token_expire_time_option = _kf_option->FindOption( __KF_STRING__( tokenexpiretime ) );

        // 创建token
        auto md5temp = __FORMAT__( "{}-{}", accountid, KFGlobal::Instance()->_game_time );
        auto token = KFCrypto::Md5Encode( md5temp );

        auto channel = accountdata[ __KF_STRING__( channel ) ];
        auto account = accountdata[ __KF_STRING__( account ) ];
        auto tokenkey = __FORMAT__( "{}:{}", __KF_STRING__( login ), token );

        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        redisdriver->Append( "hmset {} {} {} {} {} {} {}", tokenkey,
                             __KF_STRING__( account ), account,
                             __KF_STRING__( accountid ), accountid,
                             __KF_STRING__( channel ), channel );
        redisdriver->Append( "expire {} {}", tokenkey, _token_expire_time_option->_str_value );
        redisdriver->Pipeline();

        return token;
    }

    bool KFAuthModule::KickAccountOffline( MapString& accountdata )
    {
        auto online = accountdata[ __KF_STRING__( online ) ];
        if ( online != "1" )
        {
            return true;
        }

        auto url = accountdata[ __KF_STRING__( zonehttp ) ];
        if ( url.empty() )
        {
            return true;
        }

        auto kickurl = url + __KF_STRING__( kickonline );

        KFJson kfjson;
        kfjson.SetValue( __KF_STRING__( playerid ), accountdata[ __KF_STRING__( playerid ) ] );
        auto kickresult = _kf_http_client->StartSTHttpClient( kickurl, kfjson );

        KFJson kfresponse( kickresult );
        auto retcode = _kf_http_server->GetResponseCode( kfresponse );
        return retcode == KFMsg::Success;
    }

    std::string KFAuthModule::QueryDirList( uint32 accountid, const std::string& token, MapString& accountdata )
    {
        // 获得dir服务器地址
        auto loginredis = __DIR_REDIS_DRIVER__;
        auto kfresult = loginredis->QueryString( "srandmember {}", __KF_STRING__( dirurl ) );
        if ( !kfresult->IsOk() || kfresult->_value.empty() )
        {
            return _kf_http_server->SendResponseCode( KFMsg::DirServerBusy );
        }

        std::string dirurl = kfresult->_value;
        auto zoneid = KFUtility::ToValue<uint32>( accountdata[ __KF_STRING__( zoneid ) ] );

        // 获取服务器列表
        KFJson dirrequest;
        dirrequest.SetValue( __KF_STRING__( zoneid ), zoneid );
        dirrequest.SetValue( __KF_STRING__( accountid ), accountid );
        auto urlresponse = _kf_http_client->StartSTHttpClient( dirurl, dirrequest );

        KFJson kfdirjson( urlresponse );
        auto dirretcode = _kf_http_server->GetResponseCode( kfdirjson );
        if ( dirretcode != KFMsg::Success )
        {
            // 失败删除
            loginredis->Execute( "srem {} {}", __KF_STRING__( dirurl ), dirurl );
            return _kf_http_server->SendResponseCode( dirretcode );
        }

        // 返回认证结果
        kfdirjson.SetValue( __KF_STRING__( token ), token );
        kfdirjson.SetValue( __KF_STRING__( accountid ), accountid );
        return _kf_http_server->SendResponse( kfdirjson );
    }

    __KF_HTTP_FUNCTION__( KFAuthModule::HandleAuthActivation )
    {
        KFJson request( data );
        auto accountid = request.GetUInt32( __KF_STRING__( accountid ) );
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
            // 保存激活码
            auto code = request.GetString( __KF_STRING__( code ) );

            // 判断激活码是否存在
            auto isexist = redisdriver->QueryUInt64( "sismember {} {}", __KF_STRING__( activationcode ), code );
            if ( !isexist->IsOk() )
            {
                return _kf_http_server->SendResponseCode( KFMsg::AuthDatabaseBusy );
            }

            if ( isexist->_value == _invalid_int )
            {
                return _kf_http_server->SendResponseCode( KFMsg::ActivationCodeError );
            }

            redisdriver->Append( "srem {} {}", __KF_STRING__( activationcode ), code );
            redisdriver->Append( "hset {}:{} {} {}", __KF_STRING__( accountid ), accountid, __KF_STRING__( activation ), code );
            redisdriver->Pipeline();
        }

        // 创建登录token
        auto token = CreateLoginToken( accountid, accountdata->_value );
        return QueryDirList( accountid, token, accountdata->_value );
    }

    __KF_HTTP_FUNCTION__( KFAuthModule::HandleVerifyToken )
    {
        // 判断ip黑名单

        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;

        KFJson request( data );
        auto zoneid = request.GetUInt32( __KF_STRING__( zoneid ) );
        auto logiczoneid = request.GetUInt32( __KF_STRING__( logiczoneid ) );
        auto accountid = request.GetUInt32( __KF_STRING__( accountid ) );
        if ( zoneid == _invalid_int || logiczoneid == _invalid_int )
        {
            return _kf_http_server->SendResponseCode( KFMsg::HttpDataError );
        }

        // 判断token是否正确
        auto token = request.GetString( __KF_STRING__( token ) );
        auto loginkey = __FORMAT__( "{}:{}", __KF_STRING__( login ), token );
        auto querytoken = redisdriver->QueryMap( "hgetall {}", loginkey );
        if ( !querytoken->IsOk() )
        {
            return _kf_http_server->SendResponseCode( KFMsg::AuthDatabaseBusy );
        }

        // 获得账号和渠道
        auto queryaccountid = KFUtility::ToValue< uint32 >( querytoken->_value[ __KF_STRING__( accountid ) ] );
        auto account = querytoken->_value[ __KF_STRING__( account ) ];
        auto channel = KFUtility::ToValue< uint32 >( querytoken->_value[ __KF_STRING__( channel ) ] );

        if ( queryaccountid == _invalid_int || channel == _invalid_int || queryaccountid != accountid )
        {
            return _kf_http_server->SendResponseCode( KFMsg::LoginTokenError );
        }

        auto playerid = QueryCreatePlayerId( channel, accountid, zoneid, logiczoneid );
        if ( playerid == _invalid_int )
        {
            return _kf_http_server->SendResponseCode( KFMsg::AuthDatabaseBusy );
        }

        // 返回结果
        KFJson response;
        response.SetValue( __KF_STRING__( token ), token );
        response.SetValue( __KF_STRING__( channel ), channel );
        response.SetValue( __KF_STRING__( playerid ), playerid );
        response.SetValue( __KF_STRING__( account ), account );
        response.SetValue( __KF_STRING__( accountid ), accountid );

        // 加载渠道数据
        //{
        //    KFJson kfchanneljson;
        //    auto kfresult = redisdriver->QueryMap( "hgetall {}:{}", __KF_STRING__( extend ), accountid );
        //    for ( auto& iter : kfresult->_value )
        //    {
        //        kfchanneljson.SetValue( iter.first, iter.second );
        //    }
        //    response.SetValue( __KF_STRING__( channeldata ), kfchanneljson );
        //}

        return _kf_http_server->SendResponse( response );
    }


    uint32 KFAuthModule::QueryCreatePlayerId( uint32 channel, uint32 accountid, uint32 zoneid, uint32 logiczoneid )
    {
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;

        // 查询是否存在
        auto uint32result = redisdriver->QueryUInt32( "hget {}:{} {}", __KF_STRING__( user ), accountid, logiczoneid );
        if ( !uint32result->IsOk() )
        {
            return _invalid_int;
        }

        // 存在playerid, 直接返回
        if ( uint32result->_value != _invalid_int )
        {
            return uint32result->_value;
        }

        // 创建playerid
        auto uint64result = redisdriver->QueryUInt64( "incr {}:{}", __KF_STRING__( playeridcreate ), zoneid );
        if ( !uint64result->IsOk() || uint64result->_value == _invalid_int )
        {
            return _invalid_int;
        }

        uint64 newuserid = uint64result->_value + 10000;
        auto playerid = KFUtility::CalcPlayerid( static_cast< uint32 >( newuserid ), zoneid );

        auto voidresult = redisdriver->Execute( "hset {}:{} {} {}", __KF_STRING__( user ), accountid, logiczoneid, playerid );
        if ( !voidresult->IsOk() )
        {
            return _invalid_int;
        }

        return playerid;
    }

    __KF_HTTP_FUNCTION__( KFAuthModule::HandleUpdateOnline )
    {
        KFJson request( data );

        auto accountid = request.GetString( __KF_STRING__( accountid ) );
        auto playerid = request.GetString( __KF_STRING__( playerid ) );
        auto online = request.GetString( __KF_STRING__( online ) );
        auto zoneid = request.GetString( __KF_STRING__( zoneid ) );

        MapString values;
        values[ __KF_STRING__( playerid ) ] = playerid;
        values[ __KF_STRING__( online ) ] = online;
        values[ __KF_STRING__( zoneid ) ] = zoneid;

        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        redisdriver->Update( values, "hmset {}:{}", __KF_STRING__( accountid ), accountid );
        return _kf_http_server->SendResponseCode( KFMsg::Success );
    }
}