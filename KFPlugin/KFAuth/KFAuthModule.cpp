#include "KFAuthModule.hpp"
#include "KFProtocol/KFProtocol.h"
namespace KFrame
{
    void KFAuthModule::BeforeRun()
    {
        /////////////////////////////////////////////////////////////////////////////////////////
        // auth
        __REGISTER_HTTP__( __STRING__( auth ), false, &KFAuthModule::HandleAuthLogin );
        __REGISTER_HTTP__( __STRING__( verify ), false, &KFAuthModule::HandleVerifyToken );
        __REGISTER_HTTP__( __STRING__( online ), false, &KFAuthModule::HandleAccountOnline );
        /////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_HTTP__( __STRING__( addipblacklist ), false, &KFAuthModule::HandleAddIpBlackList );
        __REGISTER_HTTP__( __STRING__( removeipblacklist ), false, &KFAuthModule::HandleRemoveIpBlackList );
        __REGISTER_HTTP__( __STRING__( queryipblacklist ), false, &KFAuthModule::HandleQueryIpBlackList );
        __REGISTER_HTTP__( __STRING__( addipwhitelist ), false, &KFAuthModule::HandleAddIpWhiteList );
        __REGISTER_HTTP__( __STRING__( removeipwhitelist ), false, &KFAuthModule::HandleRemoveIpWhiteList );
        __REGISTER_HTTP__( __STRING__( queryipwhitelist ), false, &KFAuthModule::HandleQueryIpWhiteList );
        __REGISTER_HTTP__( __STRING__( addaccountblacklist ), false, &KFAuthModule::HandleAddAccountBlackList );
        __REGISTER_HTTP__( __STRING__( removeaccountblacklist ), false, &KFAuthModule::HandleRemoveAccountBlackList );
        __REGISTER_HTTP__( __STRING__( queryaccountblacklist ), false, &KFAuthModule::HandleQueryAccountBlackList );
        __REGISTER_HTTP__( __STRING__( addaccountwhitelist ), false, &KFAuthModule::HandleAddAccountWhiteList );
        __REGISTER_HTTP__( __STRING__( removeaccountwhitelist ), false, &KFAuthModule::HandleRemoveAccountWhiteList );
        __REGISTER_HTTP__( __STRING__( queryaccountwhitelist ), false, &KFAuthModule::HandleQueryAccountWhiteList );

        /////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFAuthModule::BeforeShut()
    {
        // auth
        __UN_HTTP__( __STRING__( auth ) );
        __UN_HTTP__( __STRING__( verify ) );
        __UN_HTTP__( __STRING__( online ) );
        ///////////////////////////////////////////////////////////////////////////
        __UN_HTTP__( __STRING__( addipblacklist ) );
        __UN_HTTP__( __STRING__( removeipblacklist ) );
        __UN_HTTP__( __STRING__( queryipblacklist ) );
        __UN_HTTP__( __STRING__( addipwhitelist ) );
        __UN_HTTP__( __STRING__( removeipwhitelist ) );
        __UN_HTTP__( __STRING__( queryipwhitelist ) );
        __UN_HTTP__( __STRING__( addaccountblacklist ) );
        __UN_HTTP__( __STRING__( removeaccountblacklist ) );
        __UN_HTTP__( __STRING__( queryaccountblacklist ) );
        __UN_HTTP__( __STRING__( addaccountwhitelist ) );
        __UN_HTTP__( __STRING__( removeaccountwhitelist ) );
        __UN_HTTP__( __STRING__( queryaccountwhitelist ) );
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_HTTP_FUNCTION__( KFAuthModule::HandleAuthLogin )
    {
        // 验证ip黑名单
        {
            auto time = _kf_account->CheckIpInBlackList( ip );
            if ( time > KFGlobal::Instance()->_real_time )
            {
                __JSON_OBJECT_DOCUMENT__( response );
                __JSON_SET_VALUE__( response, __STRING__( time ), time );
                return _kf_http_server->SendResponse( response, KFMsg::IpInBlackList );
            }
        }

        // 渠道验证
        __JSON_PARSE_STRING__( request, data );
        auto authdata = _kf_channel->AuthLogin( request );

        __JSON_PARSE_STRING__( authjson, authdata );
        auto retcode = _kf_http_server->GetCode( authjson );
        if ( retcode != KFMsg::Ok )
        {
            return _kf_http_server->SendCode( retcode );
        }

        auto account = __JSON_GET_STRING__( authjson, __STRING__( account ) );
        auto channel = __JSON_GET_UINT32__( authjson, __STRING__( channel ) );
        if ( account.empty() )
        {
            return _kf_http_server->SendCode( KFMsg::AccountIsEmpty );
        }

        // 查询创建账号
        auto accountdata = _kf_account->QueryCreateAccount( account, channel );
        auto accountid = __TO_UINT64__( accountdata[ __STRING__( accountid ) ] );
        if ( accountid == 0 )
        {
            return _kf_http_server->SendCode( KFMsg::AuthServerBusy );
        }

        // 验证账号黑名单
        {
            auto time = _kf_account->CheckAccountInBlackList( accountid );
            if ( time > KFGlobal::Instance()->_real_time )
            {
                __JSON_OBJECT_DOCUMENT__( response );
                __JSON_SET_VALUE__( response, __STRING__( time ), time );
                return _kf_http_server->SendResponse( response, KFMsg::AccountInBlackList );
            }
        }

        // 踢掉在线玩家
        KickAccount( KFMsg::KickByLogin, accountid, accountdata );

        // 保存渠道的数据
        _kf_account->SaveChannelData( accountid, authjson );

        // 保存token
        auto token = _kf_account->MakeAccountToken( account, channel, accountid );

        // 查询小区信息, 返回给客户端
        auto zoneflag = __JSON_GET_STRING__( request, __STRING__( flag ) );
        auto zoneid = __TO_UINT32__( accountdata[ __STRING__( zoneid ) ] );
        auto zonedata = _kf_dir_attribute->AllocPlayerZone( zoneflag, zoneid );

        __JSON_OBJECT_DOCUMENT__( response );
        __JSON_SET_VALUE__( response, __STRING__( token ), token );
        __JSON_SET_VALUE__( response, __STRING__( accountid ), accountid );

        __JSON_OBJECT__( kfzone );
        __JSON_SET_VALUE__( kfzone, __STRING__( name ), zonedata[ __STRING__( name ) ] );
        __JSON_SET_VALUE__( kfzone, __STRING__( ip ), zonedata[ __STRING__( ip ) ] );
        __JSON_SET_VALUE__( kfzone, __STRING__( port ), __TO_UINT32__( zonedata[ __STRING__( port ) ] ) );
        __JSON_SET_VALUE__( kfzone, __STRING__( zoneid ), __TO_UINT32__( zonedata[ __STRING__( zoneid ) ] ) );
        __JSON_SET_VALUE__( response, __STRING__( zone ), kfzone );
        return _kf_http_server->SendResponse( response );
    }

    __KF_HTTP_FUNCTION__( KFAuthModule::HandleVerifyToken )
    {
        __JSON_PARSE_STRING__( request, data );

        auto loginip = __JSON_GET_STRING__( request, __STRING__( ip ) );
        auto token = __JSON_GET_STRING__( request, __STRING__( token ) );
        auto zoneid = __JSON_GET_UINT32__( request, __STRING__( zoneid ) );
        auto accountid = __JSON_GET_UINT64__( request, __STRING__( accountid ) );

        // 查询小区服务器状态
        auto status = _kf_dir_attribute->QueryZoneStatus( zoneid );
        if ( status == KFMsg::ServerStopStatus )
        {
            return _kf_http_server->SendCode( KFMsg::LoginServerStopStatus );
        }

        // 验证账号,ip白名单
        if ( status == KFMsg::ServerTestStatus )
        {
            if ( !_kf_account->CheckAccountInWhiteList( accountid ) )
            {
                if ( !_kf_account->CheckIpInWhiteList( loginip ) )
                {
                    return _kf_http_server->SendCode( KFMsg::LoginNotInWhiteList );
                }
            }
        }
        /////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////
        auto accountdata = _kf_account->VerifyAccountToken( accountid, token );
        if ( accountdata.empty() )
        {
            return _kf_http_server->SendCode( KFMsg::LoginTokenError );
        }

        // 查询玩家角色
        auto datazoneid = __JSON_GET_UINT32__( request, __STRING__( datazoneid ) );
        auto playerdata = _kf_account->QueryCreatePlayer( accountid, datazoneid );
        auto playerid = std::get<0>( playerdata );
        if ( playerid == _invalid_int )
        {
            return _kf_http_server->SendCode( KFMsg::LoginCreatePlayerError );
        }

        // 新创建玩家, 增加改小区的注册人数
        auto isnew = std::get<1>( playerdata );
        if ( isnew )
        {
            _kf_dir_attribute->ZoneBalance( zoneid, 1u );
        }

        // 保存登录ip
        _kf_account->SaveLoginData( accountid, loginip, zoneid );

        // 获得账号和渠道 小区信息
        auto account = accountdata[ __STRING__( account ) ];
        auto channel = __TO_UINT32__( accountdata[ __STRING__( channel ) ] );

        // 返回结果
        __JSON_OBJECT_DOCUMENT__( response );
        __JSON_SET_VALUE__( response, __STRING__( token ), token );
        __JSON_SET_VALUE__( response, __STRING__( channel ), channel );
        __JSON_SET_VALUE__( response, __STRING__( account ), account );
        __JSON_SET_VALUE__( response, __STRING__( accountid ), accountid );
        __JSON_SET_VALUE__( response, __STRING__( playerid ), playerid );
        __JSON_SET_VALUE__( response, __STRING__( ip ), loginip );

        // 渠道数据
        auto channeldata = _kf_account->QueryChannelData( accountid );
        if ( !channeldata.empty() )
        {
            KFJson kfchanneljson;
            __JSON_FROM_MAP__( kfchanneljson, channeldata );
            __JSON_SET_VALUE__( response, __STRING__( channeldata ), kfchanneljson );
        }

        return _kf_http_server->SendResponse( response );
    }

    __KF_HTTP_FUNCTION__( KFAuthModule::HandleAccountOnline )
    {
        __JSON_PARSE_STRING__( request, data );
        auto accountid = __JSON_GET_UINT64__( request, __STRING__( accountid ) );
        auto playerid = __JSON_GET_UINT64__( request, __STRING__( playerid ) );
        auto worldid = __JSON_GET_UINT64__( request, __STRING__( world ) );
        auto gameid = __JSON_GET_UINT64__( request, __STRING__( game ) );

        auto ok = _kf_account->UpdateOnline( accountid, playerid, worldid, gameid );
        return _kf_http_server->SendCode( ok ? KFMsg::Ok : KFMsg::Error );
    }

    // 踢掉在线玩家
    void KFAuthModule::KickAccount( uint32 type, uint64 accountid, StringMap& accountdata )
    {
        auto worldid = __TO_UINT64__( accountdata[ __STRING__( world ) ] );
        auto gameid = __TO_UINT64__( accountdata[ __STRING__( game ) ] );
        auto playerid = __TO_UINT64__( accountdata[ __STRING__( playerid ) ] );
        if ( worldid == _invalid_int || gameid == _invalid_int || playerid == _invalid_int )
        {
            return;
        }

        auto worldurl = _kf_dir_attribute->GetWorldUrl( worldid );
        if ( worldurl.empty() )
        {
            return;
        }

        auto url = worldurl + __STRING__( kickonline );

        __JSON_OBJECT_DOCUMENT__( kfjson );
        __JSON_SET_VALUE__( kfjson, __STRING__( type ), type );
        __JSON_SET_VALUE__( kfjson, __STRING__( game ), gameid );
        __JSON_SET_VALUE__( kfjson, __STRING__( playerid ), playerid );
        _kf_http_client->MTGet<KFAuthModule>( url, kfjson );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////
    __KF_HTTP_FUNCTION__( KFAuthModule::HandleAddIpBlackList )
    {
        __JSON_PARSE_STRING__( request, data );

        // 验证请求ip

        auto blackip = __JSON_GET_STRING__( request, __STRING__( ip ) );
        auto time = __JSON_GET_UINT64__( request, __STRING__( time ) );
        auto comment = __JSON_GET_STRING__( request, __STRING__( comment ) );
        auto ok = _kf_account->AddIpBlackList( blackip, time, comment );
        return _kf_http_server->SendCode( ok ? KFMsg::Ok : KFMsg::Error );
    }

    __KF_HTTP_FUNCTION__( KFAuthModule::HandleRemoveIpBlackList )
    {
        __JSON_PARSE_STRING__( request, data );

        // 验证请求ip

        auto blackip = __JSON_GET_STRING__( request, __STRING__( ip ) );
        auto ok = _kf_account->RemoveIpBlackList( blackip );
        return _kf_http_server->SendCode( ok ? KFMsg::Ok : KFMsg::Error );
    }

    __KF_HTTP_FUNCTION__( KFAuthModule::HandleQueryIpBlackList )
    {
        __JSON_PARSE_STRING__( request, data );

        // 验证请求ip

        auto querylist = _kf_account->QueryIpBlackList();

        __JSON_OBJECT_DOCUMENT__( response );
        __JSON_ARRAY__( retlist );
        for ( auto& values : querylist )
        {
            __JSON_OBJECT__( data );
            __JSON_FROM_MAP__( data, values );
            __JSON_ADD_VALUE__( retlist, data );
        }
        __JSON_SET_VALUE__( response, __STRING__( retlist ), retlist );

        return _kf_http_server->SendResponse( response );
    }

    __KF_HTTP_FUNCTION__( KFAuthModule::HandleAddIpWhiteList )
    {
        __JSON_PARSE_STRING__( request, data );

        // 验证请求ip

        auto blackip = __JSON_GET_STRING__( request, __STRING__( ip ) );
        auto time = __JSON_GET_UINT64__( request, __STRING__( time ) );
        auto comment = __JSON_GET_STRING__( request, __STRING__( comment ) );
        auto ok = _kf_account->AddIpWhiteList( blackip, time, comment );
        return _kf_http_server->SendCode( ok ? KFMsg::Ok : KFMsg::Error );
    }

    __KF_HTTP_FUNCTION__( KFAuthModule::HandleRemoveIpWhiteList )
    {
        __JSON_PARSE_STRING__( request, data );

        // 验证请求ip

        auto blackip = __JSON_GET_STRING__( request, __STRING__( ip ) );
        auto ok = _kf_account->RemoveIpWhiteList( blackip );
        return _kf_http_server->SendCode( ok ? KFMsg::Ok : KFMsg::Error );
    }

    __KF_HTTP_FUNCTION__( KFAuthModule::HandleQueryIpWhiteList )
    {
        __JSON_PARSE_STRING__( request, data );

        // 验证请求ip

        auto querylist = _kf_account->QueryIpWhiteList();

        __JSON_OBJECT_DOCUMENT__( response );
        __JSON_ARRAY__( retlist );
        for ( auto& values : querylist )
        {
            __JSON_OBJECT__( data );
            __JSON_FROM_MAP__( data, values );
            __JSON_ADD_VALUE__( retlist, data );
        }
        __JSON_SET_VALUE__( response, __STRING__( retlist ), retlist );

        return _kf_http_server->SendResponse( response );
    }

    __KF_HTTP_FUNCTION__( KFAuthModule::HandleAddAccountBlackList )
    {
        __JSON_PARSE_STRING__( request, data );

        // 验证请求ip

        auto accountid = __JSON_GET_UINT64__( request, __STRING__( accountid ) );
        auto account = __JSON_GET_STRING__( request, __STRING__( account ) );
        auto channel = __JSON_GET_UINT32__( request, __STRING__( channel ) );
        auto time = __JSON_GET_UINT64__( request, __STRING__( time ) );
        auto comment = __JSON_GET_STRING__( request, __STRING__( comment ) );
        auto ok = _kf_account->AddAccountBlackList( accountid, account, channel, time, comment );
        return _kf_http_server->SendCode( ok ? KFMsg::Ok : KFMsg::Error );
    }

    __KF_HTTP_FUNCTION__( KFAuthModule::HandleRemoveAccountBlackList )
    {
        __JSON_PARSE_STRING__( request, data );

        // 验证请求ip

        auto accountid = __JSON_GET_UINT64__( request, __STRING__( accountid ) );
        auto ok = _kf_account->RemoveAccountBlackList( accountid );
        return _kf_http_server->SendCode( ok ? KFMsg::Ok : KFMsg::Error );
    }


    __KF_HTTP_FUNCTION__( KFAuthModule::HandleQueryAccountBlackList )
    {
        __JSON_PARSE_STRING__( request, data );

        // 验证请求ip

        auto querylist = _kf_account->QueryAccountBlackList();

        __JSON_OBJECT_DOCUMENT__( response );
        __JSON_ARRAY__( retlist );
        for ( auto& values : querylist )
        {
            __JSON_OBJECT__( data );
            __JSON_FROM_MAP__( data, values );
            __JSON_ADD_VALUE__( retlist, data );
        }
        __JSON_SET_VALUE__( response, __STRING__( retlist ), retlist );

        return _kf_http_server->SendResponse( response );
    }

    __KF_HTTP_FUNCTION__( KFAuthModule::HandleAddAccountWhiteList )
    {
        __JSON_PARSE_STRING__( request, data );

        // 验证请求ip

        auto accountid = __JSON_GET_UINT64__( request, __STRING__( accountid ) );
        auto account = __JSON_GET_STRING__( request, __STRING__( account ) );
        auto channel = __JSON_GET_UINT32__( request, __STRING__( channel ) );
        auto time = __JSON_GET_UINT64__( request, __STRING__( time ) );
        auto comment = __JSON_GET_STRING__( request, __STRING__( comment ) );
        auto ok = _kf_account->AddAccountWhiteList( accountid, account, channel, time, comment );
        return _kf_http_server->SendCode( ok ? KFMsg::Ok : KFMsg::Error );
    }

    __KF_HTTP_FUNCTION__( KFAuthModule::HandleRemoveAccountWhiteList )
    {
        __JSON_PARSE_STRING__( request, data );

        // 验证请求ip

        auto accountid = __JSON_GET_UINT64__( request, __STRING__( accountid ) );
        auto ok = _kf_account->RemoveAccountWhiteList( accountid );
        return _kf_http_server->SendCode( ok ? KFMsg::Ok : KFMsg::Error );
    }

    __KF_HTTP_FUNCTION__( KFAuthModule::HandleQueryAccountWhiteList )
    {
        __JSON_PARSE_STRING__( request, data );

        // 验证请求ip

        auto querylist = _kf_account->QueryAccountWhiteList();

        __JSON_OBJECT_DOCUMENT__( response );
        __JSON_ARRAY__( retlist );
        for ( auto& values : querylist )
        {
            __JSON_OBJECT__( data );
            __JSON_FROM_MAP__( data, values );
            __JSON_ADD_VALUE__( retlist, data );
        }
        __JSON_SET_VALUE__( response, __STRING__( retlist ), retlist );

        return _kf_http_server->SendResponse( response );
    }
}