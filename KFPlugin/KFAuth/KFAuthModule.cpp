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
        auto auth_data = _kf_channel->AuthLogin( request );

        __JSON_PARSE_STRING__( authjson, auth_data );
        auto ret_code = _kf_http_server->GetCode( authjson );
        if ( ret_code != KFMsg::Ok )
        {
            return _kf_http_server->SendCode( ret_code );
        }

        auto account = __JSON_GET_STRING__( authjson, __STRING__( account ) );
        auto channel = __JSON_GET_UINT32__( authjson, __STRING__( channel ) );
        if ( account.empty() )
        {
            return _kf_http_server->SendCode( KFMsg::AccountIsEmpty );
        }

        // 查询创建账号
        auto account_data = _kf_account->QueryCreateAccount( account, channel );
        auto account_id = __TO_UINT64__( account_data[ __STRING__( accountid ) ] );
        if ( account_id == 0 )
        {
            return _kf_http_server->SendCode( KFMsg::AuthServerBusy );
        }

        // 验证账号黑名单
        {
            auto time = _kf_account->CheckAccountInBlackList( account_id );
            if ( time > KFGlobal::Instance()->_real_time )
            {
                __JSON_OBJECT_DOCUMENT__( response );
                __JSON_SET_VALUE__( response, __STRING__( time ), time );
                return _kf_http_server->SendResponse( response, KFMsg::AccountInBlackList );
            }
        }

        // 踢掉在线玩家
        KickAccount( KFMsg::KickByLogin, account_id, account_data );

        // 保存渠道的数据
        _kf_account->SaveChannelData( account_id, authjson );

        // 保存token
        auto token = _kf_account->MakeAccountToken( account, channel, account_id );

        // 查询小区信息, 返回给客户端
        auto zone_flag = __JSON_GET_STRING__( request, __STRING__( flag ) );
        auto zone_id = __TO_UINT32__( account_data[ __STRING__( zoneid ) ] );
        auto zone_data = _kf_dir_database->AllocPlayerZone( zone_flag, zone_id );

        __JSON_OBJECT_DOCUMENT__( response );
        __JSON_SET_VALUE__( response, __STRING__( token ), token );
        __JSON_SET_VALUE__( response, __STRING__( accountid ), account_id );

        __JSON_OBJECT__( kfzone );
        __JSON_SET_VALUE__( kfzone, __STRING__( name ), zone_data[ __STRING__( name ) ] );
        __JSON_SET_VALUE__( kfzone, __STRING__( ip ), zone_data[ __STRING__( ip ) ] );
        __JSON_SET_VALUE__( kfzone, __STRING__( port ), __TO_UINT32__( zone_data[ __STRING__( port ) ] ) );
        __JSON_SET_VALUE__( kfzone, __STRING__( zoneid ), __TO_UINT32__( zone_data[ __STRING__( zoneid ) ] ) );
        __JSON_SET_VALUE__( response, __STRING__( zone ), kfzone );
        return _kf_http_server->SendResponse( response );
    }

    __KF_HTTP_FUNCTION__( KFAuthModule::HandleVerifyToken )
    {
        __JSON_PARSE_STRING__( request, data );

        auto login_ip = __JSON_GET_STRING__( request, __STRING__( ip ) );
        auto token = __JSON_GET_STRING__( request, __STRING__( token ) );
        auto zone_id = __JSON_GET_UINT32__( request, __STRING__( zoneid ) );
        auto account_id = __JSON_GET_UINT64__( request, __STRING__( accountid ) );

        // 查询小区服务器状态
        auto status = _kf_dir_database->QueryZoneStatus( zone_id );
        if ( status == KFMsg::ServerStopStatus )
        {
            return _kf_http_server->SendCode( KFMsg::LoginServerStopStatus );
        }

        // 验证账号,ip白名单
        if ( status == KFMsg::ServerTestStatus )
        {
            if ( !_kf_account->CheckAccountInWhiteList( account_id ) )
            {
                if ( !_kf_account->CheckIpInWhiteList( login_ip ) )
                {
                    return _kf_http_server->SendCode( KFMsg::LoginNotInWhiteList );
                }
            }
        }
        /////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////
        auto account_data = _kf_account->VerifyAccountToken( account_id, token );
        if ( account_data.empty() )
        {
            return _kf_http_server->SendCode( KFMsg::LoginTokenError );
        }

        // 查询玩家角色
        auto data_zone_id = __JSON_GET_UINT32__( request, __STRING__( datazoneid ) );
        auto player_data = _kf_account->QueryCreatePlayer( account_id, data_zone_id );
        auto player_id = std::get<0>( player_data );
        if ( player_id == _invalid_int )
        {
            return _kf_http_server->SendCode( KFMsg::LoginCreatePlayerError );
        }

        // 新创建玩家, 增加改小区的注册人数
        auto is_new_player = std::get<1>( player_data );
        if ( is_new_player )
        {
            _kf_dir_database->ZoneBalance( zone_id, 1u );
        }

        // 保存登录ip
        _kf_account->SaveLoginData( account_id, login_ip, zone_id );

        // 获得账号和渠道 小区信息
        auto account = account_data[ __STRING__( account ) ];
        auto channel = __TO_UINT32__( account_data[ __STRING__( channel ) ] );

        // 返回结果
        __JSON_OBJECT_DOCUMENT__( response );
        __JSON_SET_VALUE__( response, __STRING__( ip ), login_ip );
        __JSON_SET_VALUE__( response, __STRING__( token ), token );
        __JSON_SET_VALUE__( response, __STRING__( channel ), channel );
        __JSON_SET_VALUE__( response, __STRING__( account ), account );
        __JSON_SET_VALUE__( response, __STRING__( playerid ), player_id );
        __JSON_SET_VALUE__( response, __STRING__( accountid ), account_id );

        // 渠道数据
        auto channel_data = _kf_account->QueryChannelData( account_id );
        if ( !channel_data.empty() )
        {
            KFJson channel_json;
            __JSON_FROM_MAP__( channel_json, channel_data );
            __JSON_SET_VALUE__( response, __STRING__( channeldata ), channel_json );
        }

        return _kf_http_server->SendResponse( response );
    }

    __KF_HTTP_FUNCTION__( KFAuthModule::HandleAccountOnline )
    {
        __JSON_PARSE_STRING__( request, data );
        auto account_id = __JSON_GET_UINT64__( request, __STRING__( accountid ) );
        auto player_id = __JSON_GET_UINT64__( request, __STRING__( playerid ) );
        auto world_id = __JSON_GET_UINT64__( request, __STRING__( world ) );
        auto game_id = __JSON_GET_UINT64__( request, __STRING__( game ) );

        auto ok = _kf_account->UpdateOnline( account_id, player_id, world_id, game_id );
        return _kf_http_server->SendCode( ok ? KFMsg::Ok : KFMsg::Error );
    }

    // 踢掉在线玩家
    void KFAuthModule::KickAccount( uint32 type, uint64 account_id, StringMap& account_data )
    {
        auto world_id = __TO_UINT64__( account_data[ __STRING__( world ) ] );
        auto game_id = __TO_UINT64__( account_data[ __STRING__( game ) ] );
        auto player_id = __TO_UINT64__( account_data[ __STRING__( playerid ) ] );
        if ( world_id == _invalid_int || game_id == _invalid_int || player_id == _invalid_int )
        {
            return;
        }

        auto world_url = _kf_dir_database->GetWorldUrl( world_id );
        if ( world_url.empty() )
        {
            return;
        }

        auto url = world_url + __STRING__( kickonline );

        __JSON_OBJECT_DOCUMENT__( kfjson );
        __JSON_SET_VALUE__( kfjson, __STRING__( type ), type );
        __JSON_SET_VALUE__( kfjson, __STRING__( game ), game_id );
        __JSON_SET_VALUE__( kfjson, __STRING__( playerid ), player_id );
        _kf_http_client->MTGet<KFAuthModule>( url, kfjson );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////
    __KF_HTTP_FUNCTION__( KFAuthModule::HandleAddIpBlackList )
    {
        __JSON_PARSE_STRING__( request, data );

        // 验证请求ip

        auto black_ip = __JSON_GET_STRING__( request, __STRING__( ip ) );
        auto time = __JSON_GET_UINT64__( request, __STRING__( time ) );
        auto comment = __JSON_GET_STRING__( request, __STRING__( comment ) );
        auto ok = _kf_account->AddIpBlackList( black_ip, time, comment );
        return _kf_http_server->SendCode( ok ? KFMsg::Ok : KFMsg::Error );
    }

    __KF_HTTP_FUNCTION__( KFAuthModule::HandleRemoveIpBlackList )
    {
        __JSON_PARSE_STRING__( request, data );

        // 验证请求ip

        auto black_ip = __JSON_GET_STRING__( request, __STRING__( ip ) );
        auto ok = _kf_account->RemoveIpBlackList( black_ip );
        return _kf_http_server->SendCode( ok ? KFMsg::Ok : KFMsg::Error );
    }

    __KF_HTTP_FUNCTION__( KFAuthModule::HandleQueryIpBlackList )
    {
        __JSON_PARSE_STRING__( request, data );

        // 验证请求ip
        auto query_list = _kf_account->QueryIpBlackList();

        __JSON_OBJECT_DOCUMENT__( response );
        __JSON_ARRAY__( retlist );
        for ( auto& values : query_list )
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

        auto black_ip = __JSON_GET_STRING__( request, __STRING__( ip ) );
        auto time = __JSON_GET_UINT64__( request, __STRING__( time ) );
        auto comment = __JSON_GET_STRING__( request, __STRING__( comment ) );
        auto ok = _kf_account->AddIpWhiteList( black_ip, time, comment );
        return _kf_http_server->SendCode( ok ? KFMsg::Ok : KFMsg::Error );
    }

    __KF_HTTP_FUNCTION__( KFAuthModule::HandleRemoveIpWhiteList )
    {
        __JSON_PARSE_STRING__( request, data );

        // 验证请求ip

        auto black_ip = __JSON_GET_STRING__( request, __STRING__( ip ) );
        auto ok = _kf_account->RemoveIpWhiteList( black_ip );
        return _kf_http_server->SendCode( ok ? KFMsg::Ok : KFMsg::Error );
    }

    __KF_HTTP_FUNCTION__( KFAuthModule::HandleQueryIpWhiteList )
    {
        __JSON_PARSE_STRING__( request, data );

        // 验证请求ip

        auto query_list = _kf_account->QueryIpWhiteList();

        __JSON_OBJECT_DOCUMENT__( response );
        __JSON_ARRAY__( retlist );
        for ( auto& values : query_list )
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

        auto account_id = __JSON_GET_UINT64__( request, __STRING__( accountid ) );
        auto account = __JSON_GET_STRING__( request, __STRING__( account ) );
        auto channel = __JSON_GET_UINT32__( request, __STRING__( channel ) );
        auto time = __JSON_GET_UINT64__( request, __STRING__( time ) );
        auto comment = __JSON_GET_STRING__( request, __STRING__( comment ) );
        auto ok = _kf_account->AddAccountBlackList( account_id, account, channel, time, comment );
        return _kf_http_server->SendCode( ok ? KFMsg::Ok : KFMsg::Error );
    }

    __KF_HTTP_FUNCTION__( KFAuthModule::HandleRemoveAccountBlackList )
    {
        __JSON_PARSE_STRING__( request, data );

        // 验证请求ip

        auto account_id = __JSON_GET_UINT64__( request, __STRING__( accountid ) );
        auto ok = _kf_account->RemoveAccountBlackList( account_id );
        return _kf_http_server->SendCode( ok ? KFMsg::Ok : KFMsg::Error );
    }


    __KF_HTTP_FUNCTION__( KFAuthModule::HandleQueryAccountBlackList )
    {
        __JSON_PARSE_STRING__( request, data );

        // 验证请求ip

        auto query_list = _kf_account->QueryAccountBlackList();

        __JSON_OBJECT_DOCUMENT__( response );
        __JSON_ARRAY__( retlist );
        for ( auto& values : query_list )
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

        auto account_id = __JSON_GET_UINT64__( request, __STRING__( accountid ) );
        auto account = __JSON_GET_STRING__( request, __STRING__( account ) );
        auto channel = __JSON_GET_UINT32__( request, __STRING__( channel ) );
        auto time = __JSON_GET_UINT64__( request, __STRING__( time ) );
        auto comment = __JSON_GET_STRING__( request, __STRING__( comment ) );
        auto ok = _kf_account->AddAccountWhiteList( account_id, account, channel, time, comment );
        return _kf_http_server->SendCode( ok ? KFMsg::Ok : KFMsg::Error );
    }

    __KF_HTTP_FUNCTION__( KFAuthModule::HandleRemoveAccountWhiteList )
    {
        __JSON_PARSE_STRING__( request, data );

        // 验证请求ip

        auto account_id = __JSON_GET_UINT64__( request, __STRING__( accountid ) );
        auto ok = _kf_account->RemoveAccountWhiteList( account_id );
        return _kf_http_server->SendCode( ok ? KFMsg::Ok : KFMsg::Error );
    }

    __KF_HTTP_FUNCTION__( KFAuthModule::HandleQueryAccountWhiteList )
    {
        __JSON_PARSE_STRING__( request, data );

        // 验证请求ip

        auto query_list = _kf_account->QueryAccountWhiteList();

        __JSON_OBJECT_DOCUMENT__( response );
        __JSON_ARRAY__( retlist );
        for ( auto& values : query_list )
        {
            __JSON_OBJECT__( data );
            __JSON_FROM_MAP__( data, values );
            __JSON_ADD_VALUE__( retlist, data );
        }
        __JSON_SET_VALUE__( response, __STRING__( retlist ), retlist );

        return _kf_http_server->SendResponse( response );
    }
}