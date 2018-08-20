#include "KFPlatformModule.h"
#include "KFJson.h"
#include "KFPlatformConfig.h"
#include "KFZoneManage.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
#define __ACCOUNT_REDIS_DRIVER__ _kf_redis->CreateExecute( __KF_STRING__( account ) )

    KFPlatformModule::KFPlatformModule()
    {
    }

    KFPlatformModule::~KFPlatformModule()
    {
    }

    void KFPlatformModule::InitModule()
    {
        __KF_ADD_CONFIG__( _kf_platform_config, true );
    }

    void KFPlatformModule::BeforeRun()
    {
        _kf_zone_manage->BeforeRun();
        __REGISTER_RUN_FUNCTION__( &KFPlatformModule::Run );
        _server_list_type = _kf_platform_config->GetServerListType( KFGlobal::Instance()->_app_channel );

        __REGISTER_HTTP_FUNCTION__( __KF_STRING__( login ), false, &KFPlatformModule::HandleLoginToken );
        __REGISTER_HTTP_FUNCTION__( __KF_STRING__( token ), false, &KFPlatformModule::HandleVerifyToken );
        __REGISTER_HTTP_FUNCTION__( __KF_STRING__( updatezone ), false, &KFPlatformModule::HandleUpdateZone );
        __REGISTER_HTTP_FUNCTION__( __KF_STRING__( lostzone ), false, &KFPlatformModule::HandleLostZone );
        __REGISTER_HTTP_FUNCTION__( __KF_STRING__( status ), false, &KFPlatformModule::HandleUpdateZoneStatus );
        __REGISTER_HTTP_FUNCTION__( __KF_STRING__( zonelist ), false, &KFPlatformModule::HandleQueryZoneList );
        __REGISTER_HTTP_FUNCTION__( __KF_STRING__( zonestatus ), false, &KFPlatformModule::HandleQueryZoneStatus );
        __REGISTER_HTTP_FUNCTION__( __KF_STRING__( activation ), false, &KFPlatformModule::HandleActivationAccount );
        __REGISTER_HTTP_FUNCTION__( __KF_STRING__( zonehttp ), false, &KFPlatformModule::HandleZoneHttp );
        __REGISTER_HTTP_FUNCTION__( __KF_STRING__( onlinezone ), false, &KFPlatformModule::HandleUpdateOnline );
    }

    void KFPlatformModule::BeforeShut()
    {
        __KF_REMOVE_CONFIG__();
        __UNREGISTER_RUN_FUNCTION__();
        _kf_zone_manage->BeforeShut();

        __UNREGISTER_HTTP_FUNCTION__( __KF_STRING__( login ) );
        __UNREGISTER_HTTP_FUNCTION__( __KF_STRING__( token ) );
        __UNREGISTER_HTTP_FUNCTION__( __KF_STRING__( updatezone ) );
        __UNREGISTER_HTTP_FUNCTION__( __KF_STRING__( lostzone ) );
        __UNREGISTER_HTTP_FUNCTION__( __KF_STRING__( status ) );
        __UNREGISTER_HTTP_FUNCTION__( __KF_STRING__( zonelist ) );
        __UNREGISTER_HTTP_FUNCTION__( __KF_STRING__( zonestatus ) );
        __UNREGISTER_HTTP_FUNCTION__( __KF_STRING__( activation ) );
        __UNREGISTER_HTTP_FUNCTION__( __KF_STRING__( zonehttp ) );
        __UNREGISTER_HTTP_FUNCTION__( __KF_STRING__( onlinezone ) );
    }

    void KFPlatformModule::Run()
    {
        _kf_zone_manage->Run();
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////

    __KF_HTTP_FUNCTION__( KFPlatformModule::HandleLoginToken )
    {
        KFJson request( data );
        if ( !_kf_http_server->VerifySignature( request ) )
        {
            return _kf_http_server->SendResponseCode( KFMsg::SignError );
        }

        auto account = request.GetString( __KF_STRING__( account ) );
        auto channel = request.GetUInt32( __KF_STRING__( channel ) );
        if ( account.empty() )
        {
            return _kf_http_server->SendResponseCode( KFMsg::AccountIsEmpty );
        }

        // 查询创建账号
        auto accountdata = QueryCreateAccount( account, channel );
        auto accountid = KFUtility::ToValue<uint32>( accountdata[ __KF_STRING__( accountid ) ] );
        if ( accountid == 0 )
        {
            return _kf_http_server->SendResponseCode( KFMsg::PlatformBusy );
        }

        // 判断是否需要激活
        if ( _kf_platform_config->_is_open_activation )
        {
            auto activation = accountdata[ __KF_STRING__( activation ) ];
            if ( activation.empty() )
            {
                KFJson response;
                response.SetValue( __KF_STRING__( accountid ), accountid );
                response.SetValue< uint32 >( __KF_STRING__( retcode ), KFMsg::ActivationAccount );
                return _kf_http_server->SendResponse( response );
            }
        }

        // 保存额外数据
        UpdateExtendData( accountid, channel, request );

        // 保存token
        return SaveLoginToken( accountid, accountdata );
    }

    void KFPlatformModule::UpdateExtendData( uint32 accountid, uint32 channel, KFJson& kfjson )
    {
        MapString values;
        values[ __KF_STRING__( accountid ) ] = __TO_STRING__( accountid );
        values[ __KF_STRING__( channel ) ] = __TO_STRING__( channel );
        values[ __KF_STRING__( name ) ] = kfjson.GetString( __KF_STRING__( name ) );
        values[ __KF_STRING__( sex ) ] = kfjson.GetString( __KF_STRING__( sex ) );
        values[ __KF_STRING__( icon ) ] = kfjson.GetString( __KF_STRING__( icon ) );

        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        redisdriver->Update( values, "hmset {}:{}", __KF_STRING__( extend ), accountid );
    }

    std::string KFPlatformModule::SaveLoginToken( uint32 accountid, MapString& accountdata )
    {
        auto accountflag = KFUtility::ToValue<uint32>( accountdata[ __KF_STRING__( accountflag ) ] );

        // 创建token
        auto md5temp = __FORMAT__( "{}{}{}", accountid, accountflag, KFGlobal::Instance()->_game_time );
        auto token = KFCrypto::Md5Encode( md5temp );

        // 保存登录token, 设置有效时间300秒
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        auto rediskey = __FORMAT__( "{}:{}", __KF_STRING__( login ), token );
        redisdriver->Append( "hset {} {} {}", rediskey, __KF_STRING__( accountid ), accountid );
        redisdriver->Append( "expire {} {}", rediskey, _kf_platform_config->_token_expire_time );
        redisdriver->Pipeline();

        // 返回内容
        KFJson response;
        response.SetValue( __KF_STRING__( token ), token );
        response.SetValue( __KF_STRING__( accountid ), accountid );
        response.SetValue( __KF_STRING__( accountflag ), accountflag );
        response.SetValue( __KF_STRING__( serverlisttype ), _server_list_type );

        switch ( _server_list_type )
        {
        case __SELECT_SERVER_DATA__:
        {
            auto zoneid = KFUtility::ToValue<uint32>( accountdata[ __KF_STRING__( zoneid ) ] );
            if ( zoneid == _invalid_int )
            {
                // 选择一个最小人数的分区
                auto zonelist = redisdriver->QueryList( "zrevrange {} 0 0", __KF_STRING__( zonebalance ) );
                if ( zonelist->_value.empty() )
                {
                    return _kf_http_server->SendResponseCode( KFMsg::LoginCanNotFindGate );
                }

                zoneid = KFUtility::ToValue< uint32 >( zonelist->_value.front() );
                if ( zoneid == _invalid_int )
                {
                    return _kf_http_server->SendResponseCode( KFMsg::LoginCanNotFindGate );
                }

                redisdriver->Append( "hset {}:{} {} {}", __KF_STRING__( accountid ), accountid, __KF_STRING__( zoneid ), zoneid );
                redisdriver->Append( "zincrby {} 1 {}", __KF_STRING__( zonebalance ), 1 );
                redisdriver->Pipeline();
            }

            std::string ip = "";
            uint32 port = 0;

            _kf_zone_manage->RandZoneJson( zoneid, ip, port );
            if ( ip.empty() || port == 0 )
            {
                return _kf_http_server->SendResponseCode( KFMsg::LoginCanNotFindGate );
            }

            response.SetValue( __KF_STRING__( ip ), ip );
            response.SetValue( __KF_STRING__( port ), port );
        }
        break;
        default:
        {
            KFJson kfserverlistjson;
            _kf_zone_manage->GetZoneJson( kfserverlistjson );
            response.SetValue< Json::Value& >( __KF_STRING__( serverlist ), kfserverlistjson );
        }
        break;
        }

        // 判断是否已经在线
        auto online = accountdata[ __KF_STRING__( online ) ];
        if ( online == "1" )
        {
            auto zoneid = KFUtility::ToValue< uint32 >( accountdata[ __KF_STRING__( zoneid ) ] );
            auto url = _kf_zone_manage->GetZoneUrl( zoneid );
            if ( !url.empty() )
            {
                KFJson kfjson;
                kfjson.SetValue( __KF_STRING__( playerid ), accountdata[ __KF_STRING__( playerid ) ] );

                auto kickurl = url + __KF_STRING__( kickonline );
                _kf_http_client->StartMTHttpClient( kickurl, kfjson, false );
            }
        }

        return _kf_http_server->SendResponse( response );
    }

    __KF_HTTP_FUNCTION__( KFPlatformModule::HandleActivationAccount )
    {
        KFJson request( data );
        if ( !_kf_http_server->VerifySignature( request ) )
        {
            return _kf_http_server->SendResponseCode( KFMsg::SignError );
        }

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
            return _kf_http_server->SendResponseCode( KFMsg::PlatformBusy );
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
                return _kf_http_server->SendResponseCode( KFMsg::PlatformDatabaseBusy );
            }

            if ( isexist->_value == _invalid_int )
            {
                return _kf_http_server->SendResponseCode( KFMsg::ActivationCodeError );
            }

            redisdriver->Append( "srem {} {}", __KF_STRING__( activationcode ), code );
            redisdriver->Append( "hset {}:{} {} {}", __KF_STRING__( accountid ), accountid, __KF_STRING__( activation ), code );
            redisdriver->Pipeline();
        }

        return SaveLoginToken( accountid, accountdata->_value );
    }

    __KF_HTTP_FUNCTION__( KFPlatformModule::HandleQueryZoneList )
    {
        KFJson kfserverlistjson;
        _kf_zone_manage->GetZoneJson( kfserverlistjson );

        KFJson response;
        response.SetValue< Json::Value& >( __KF_STRING__( serverlist ), kfserverlistjson );
        return _kf_http_server->SendResponse( response );
    }

    MapString KFPlatformModule::QueryCreateAccount( const std::string& account, uint32 channel )
    {
        MapString accountdata;
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;

        // 先查询redis
        auto queryaccountid = redisdriver->QueryUInt32( "hget {}:{}:{} {}",
                              __KF_STRING__( account ), account, channel, __KF_STRING__( accountid ) );
        if ( !queryaccountid->IsOk() )
        {
            return accountdata;
        }

        uint32 accountid = queryaccountid->_value;
        if ( accountid != _invalid_int )
        {
            auto queryaccountdata = redisdriver->QueryMap( "hgetall {}:{}",
                                    __KF_STRING__( accountid ), accountid );
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

    __KF_HTTP_FUNCTION__( KFPlatformModule::HandleVerifyToken )
    {
        KFJson request( data );
        auto token = request.GetString( __KF_STRING__( token ) );
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;

        // 判断token是否正确
        auto loginkey = __FORMAT__( "{}:{}", __KF_STRING__( login ), token );
        auto queryaccountid = redisdriver->QueryUInt32( "hget {} {}", loginkey, __KF_STRING__( accountid ) );
        if ( !queryaccountid->IsOk() )
        {
            return _kf_http_server->SendResponseCode( KFMsg::PlatformDatabaseBusy );
        }

        auto accountid = queryaccountid->_value;
        if ( accountid == _invalid_int )
        {
            return _kf_http_server->SendResponseCode( KFMsg::LoginTokenError );
        }

        // 删除token
        redisdriver->Execute( "del {}", loginkey );

        auto accountdata = redisdriver->QueryMap( "hgetall {}:{}", __KF_STRING__( accountid ), accountid );
        if ( !accountdata->IsOk() )
        {
            return _kf_http_server->SendResponseCode( KFMsg::PlatformDatabaseBusy );
        }

        // 设置账号
        KFJson response;
        response.SetValue( __KF_STRING__( token ), token );
        response.SetValue( __KF_STRING__( accountid ), accountid );
        response.SetValue( __KF_STRING__( channel ), accountdata->_value[ __KF_STRING__( channel ) ] );
        return _kf_http_server->SendResponse( response );
    }

    __KF_HTTP_FUNCTION__( KFPlatformModule::HandleUpdateZone )
    {
        KFJson request( data );

        // 计算签名
        if ( !_kf_http_server->VerifySignature( request ) )
        {
            return _kf_http_server->SendResponseCode( KFMsg::SignError );
        }

        // 更新列表
        _kf_zone_manage->AddZoneData( request );
        return _kf_http_server->SendResponseCode( KFMsg::Success );
    }

    __KF_HTTP_FUNCTION__( KFPlatformModule::HandleZoneHttp )
    {
        KFJson request( data );

        // 计算签名
        if ( !_kf_http_server->VerifySignature( request ) )
        {
            return _kf_http_server->SendResponseCode( KFMsg::SignError );
        }

        auto zoneid = request.GetUInt32( __KF_STRING__( zoneid ) );
        auto url = request.GetString( __KF_STRING__( url ) );

        // 更新zone http
        _kf_zone_manage->UpdateZoneHttp( zoneid, url );
        return _kf_http_server->SendResponseCode( KFMsg::Success );
    }

    __KF_HTTP_FUNCTION__( KFPlatformModule::HandleLostZone )
    {
        KFJson request( data );

        // 计算签名
        if ( !_kf_http_server->VerifySignature( request ) )
        {
            return _kf_http_server->SendResponseCode( KFMsg::SignError );
        }

        auto zoneid = request.GetUInt32( __KF_STRING__( id ) );
        auto appid = request.GetUInt32( __KF_STRING__( appid ) );
        _kf_zone_manage->RemoveZoneAddress( zoneid, appid );

        return _kf_http_server->SendResponseCode( KFMsg::Success );
    }

    __KF_HTTP_FUNCTION__( KFPlatformModule::HandleUpdateZoneStatus )
    {
        KFJson request( data );

        auto zoneid = request.GetUInt32( __KF_STRING__( id ) );
        auto zonestatus = request.GetUInt32( __KF_STRING__( status ) );
        auto describe = request.GetString( __KF_STRING__( describe ) );

        // 计算签名
        if ( !_kf_http_server->VerifySignature( request ) )
        {
            return _kf_http_server->SendResponseCode( KFMsg::SignError );
        }

        // 更新列表
        _kf_zone_manage->UpdateZoneStatus( zoneid, zonestatus, describe );
        return _kf_http_server->SendResponseCode( KFMsg::Success );
    }

    __KF_HTTP_FUNCTION__( KFPlatformModule::HandleQueryZoneStatus )
    {
        KFJson request( data );

        auto zoneid = request.GetUInt32( __KF_STRING__( id ) );

        uint32 status = KFZoneStatusEnum::Shutoff;
        std::string describe = "";
        _kf_zone_manage->QueryZoneStatus( zoneid, status, describe );

        KFJson response;
        response.SetValue( __KF_STRING__( id ), zoneid );
        response.SetValue( __KF_STRING__( status ), status );
        response.SetValue( __KF_STRING__( describe ), describe );
        return _kf_http_server->SendResponse( response );
    }

    __KF_HTTP_FUNCTION__( KFPlatformModule::HandleUpdateOnline )
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
