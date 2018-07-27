#include "KFPlatformModule.h"
#include "KFJson.h"
#include "KFPlatformConfig.h"
#include "KFZoneManage.h"
#include "KFConvert/KFConvert.h"
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
        __REGISTER_RUN_FUNCTION__( &KFPlatformModule::Run );
        _kf_zone_manage->BeforeRun();

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
        redisdriver->VoidExecute( values, "hmset %s:%u", __KF_CHAR__( extend ), accountid );
    }

    std::string KFPlatformModule::SaveLoginToken( uint32 accountid, MapString& accountdata )
    {
        auto accountflag = KFUtility::ToValue<uint32>( accountdata[ __KF_STRING__( accountflag ) ] );

        // 创建token
        auto strtime = __TO_STRING__( KFGlobal::Instance()->_game_time );
        auto md5temp = KFUtility::Format( "%u%u%s", accountid, accountflag, strtime.c_str() );
        auto token = KFCrypto::Md5Encode( md5temp );

        // 保存登录token, 设置有效时间300秒
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;
        auto rediskey = KFUtility::Format( "%s:%s", __KF_CHAR__( login ), token.c_str() );
        redisdriver->AppendCommand( "hset %s %s %u", rediskey, __KF_CHAR__( accountid ), accountid );
        redisdriver->AppendCommand( "expire %s %u", rediskey, _kf_platform_config->_token_expire_time );
        redisdriver->PipelineExecute();

        // 返回内容
        KFJson response;
        response.SetValue( __KF_STRING__( token ), token );
        response.SetValue( __KF_STRING__( accountid ), accountid );
        response.SetValue( __KF_STRING__( accountflag ), accountflag );
        response.SetValue( __KF_STRING__( serverlisttype ), _kf_platform_config->_server_list_type );

        switch ( _kf_platform_config->_server_list_type )
        {
        case __SELECT_SERVER_DATA__:
        {
            auto zoneid = KFUtility::ToValue<uint32>( accountdata[ __KF_STRING__( zoneid ) ] );
            if ( zoneid == _invalid_int )
            {
                // 选择一个最小人数的分区
                VectorString zonevalues;
                redisdriver->VectorExecute( zonevalues, "zrevrange %s 0 0", __KF_CHAR__( zonebalance ) );
                if ( zonevalues.empty() )
                {
                    return _kf_http_server->SendResponseCode( KFMsg::LoginCanNotFindGate );
                }

                zoneid = KFUtility::ToValue< uint32 >( zonevalues.front() );
                if ( zoneid == _invalid_int )
                {
                    return _kf_http_server->SendResponseCode( KFMsg::LoginCanNotFindGate );
                }

                redisdriver->AppendCommand( "hset %s:%u %s %u",
                                            __KF_CHAR__( accountid ), accountid, __KF_CHAR__( zoneid ), zoneid );
                redisdriver->AppendCommand( "zincrby %s 1 %u", __KF_CHAR__( zonebalance ), 1 );
                redisdriver->PipelineExecute();
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

        MapString accountdata;
        auto ok = redisdriver->MapExecute( accountdata, "hgetall %s:%u", __KF_CHAR__( accountid ), accountid );
        if ( !ok || accountdata.empty() )
        {
            return _kf_http_server->SendResponseCode( KFMsg::PlatformBusy );
        }

        auto activation = accountdata[ __KF_STRING__( activation ) ];
        if ( activation.empty() )
        {
            // 保存激活码
            auto code = request.GetString( __KF_CHAR__( code ) );

            // 判断激活码是否存在

            uint64 isexist = 0;
            if ( !redisdriver->UInt64Execute( isexist, "sismember %s %s", __KF_CHAR__( activationcode ), code.c_str() ) )
            {
                return _kf_http_server->SendResponseCode( KFMsg::PlatformDatabaseBusy );
            }

            if ( isexist == 0 )
            {
                return _kf_http_server->SendResponseCode( KFMsg::ActivationCodeError );
            }

            redisdriver->AppendCommand( "srem %s %s", __KF_CHAR__( activationcode ), code.c_str() );
            redisdriver->AppendCommand( "hset %s:%u %s %s",
                                        __KF_CHAR__( accountid ), accountid, __KF_CHAR__( activation ), code.c_str() );
            redisdriver->PipelineExecute();
        }

        return SaveLoginToken( accountid, accountdata );
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
        std::string queryvalue;
        if ( !redisdriver->StringExecute( queryvalue, "hget %s:%s:%u %s", __KF_CHAR__( account ), account.c_str(), channel, __KF_CHAR__( accountid ) ) )
        {
            return accountdata;
        }

        if ( queryvalue != FUNCTION_EMPTY_STRING )
        {
            uint32 accountid = KFUtility::ToValue<uint32>( queryvalue );
            auto ok = redisdriver->MapExecute( accountdata, "hgetall %s:%u", __KF_CHAR__( accountid ), accountid );
            if ( !ok || !accountdata.empty() )
            {
                return accountdata;
            }
        }

        // 创建账号id
        uint64 newid = 0;
        redisdriver->UInt64Execute( newid, "incr %s", __KF_CHAR__( accountmake ) );
        if ( newid == 0 )
        {
            return accountdata;
        }

        auto accountid = static_cast< uint32 >( newid ) + 500000;

        accountdata[ __KF_STRING__( accountid ) ] = KFUtility::ToString( accountid );
        accountdata[ __KF_STRING__( accountflag ) ] = "0";
        accountdata[ __KF_STRING__( account ) ] = account;
        accountdata[ __KF_STRING__( channel ) ] = KFUtility::ToString( channel );

        redisdriver->AppendCommand( "hset %s:%s:%u %s %u", __KF_CHAR__( account ), account.c_str(), channel, __KF_CHAR__( accountid ), accountid );
        redisdriver->AppendCommand( "sadd %s %u", __KF_CHAR__( accountlist ), accountid );
        redisdriver->AppendCommand( accountdata, "hmset %s:%u", __KF_CHAR__( accountid ), accountid );
        redisdriver->PipelineExecute();
        return accountdata;
    }

    __KF_HTTP_FUNCTION__( KFPlatformModule::HandleVerifyToken )
    {
        KFJson request( data );
        auto token = request.GetString( __KF_STRING__( token ) );
        auto redisdriver = __ACCOUNT_REDIS_DRIVER__;

        // 判断token是否正确
        auto loginkey = KFUtility::Format( "%s:%s", __KF_CHAR__( login ), token.c_str() );

        std::string queryvalue = "";
        if ( !redisdriver->StringExecute( queryvalue, "hget %s %s", loginkey, __KF_CHAR__( accountid ) ) )
        {
            return _kf_http_server->SendResponseCode( KFMsg::PlatformDatabaseBusy );
        }

        // 删除token
        redisdriver->VoidExecute( "del %s", loginkey );

        // 获得accountid
        auto accountid = KFUtility::ToValue<uint32>( queryvalue );
        if ( accountid == 0 )
        {
            return _kf_http_server->SendResponseCode( KFMsg::LoginTokenError );
        }

        MapString accountdata;
        if ( !redisdriver->MapExecute( accountdata, "hgetall %s:%u", __KF_CHAR__( accountid ), accountid ) )
        {
            return _kf_http_server->SendResponseCode( KFMsg::PlatformDatabaseBusy );
        }

        // 设置账号
        KFJson response;
        response.SetValue( __KF_STRING__( token ), token );
        response.SetValue( __KF_STRING__( accountid ), accountid );
        response.SetValue( __KF_STRING__( channel ), accountdata[ __KF_STRING__( channel ) ] );
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
        redisdriver->VoidExecute( values, "hmset %s:%s", __KF_CHAR__( accountid ), accountid.c_str() );
        return _kf_http_server->SendResponseCode( KFMsg::Success );
    }
}
