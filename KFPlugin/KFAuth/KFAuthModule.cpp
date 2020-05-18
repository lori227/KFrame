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
    }

    void KFAuthModule::BeforeShut()
    {
        // auth
        __UN_HTTP__( __STRING__( auth ) );
        __UN_HTTP__( __STRING__( verify ) );
        __UN_HTTP__( __STRING__( online ) );
        ///////////////////////////////////////////////////////////////////////////
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_HTTP_FUNCTION__( KFAuthModule::HandleAuthLogin )
    {
        // 验证白名单


        __JSON_PARSE_STRING__( request, data );

        // 渠道验证
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
        auto token = __JSON_GET_STRING__( request, __STRING__( token ) );
        auto accountid = __JSON_GET_UINT64__( request, __STRING__( accountid ) );

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
        auto zoneid = __JSON_GET_UINT32__( request, __STRING__( zoneid ) );
        auto isnew = std::get<1>( playerdata );
        if ( isnew )
        {
            _kf_dir_attribute->ZoneBalance( zoneid, 1u );
        }

        // 保存登录ip
        auto loginip = __JSON_GET_STRING__( request, __STRING__( ip ) );
        _kf_account->SaveLoginData( accountid, ip, zoneid );

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
}