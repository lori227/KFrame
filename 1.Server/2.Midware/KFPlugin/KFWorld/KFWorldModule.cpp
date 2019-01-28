#include "KFWorldModule.h"

namespace KFrame
{
#define __ZONE_TIMER_ID__ 1

    void KFWorldModule::BeforeRun()
    {
        __REGISTER_LOOP_TIMER__( __ZONE_TIMER_ID__, 5000, &KFWorldModule::OnTimerZoneRegister );
        __REGISTER_SERVER_LOST_FUNCTION__( &KFWorldModule::OnServerLostGame );
        __REGISTER_SERVER_DISCOVER_FUNCTION__( &KFWorldModule::OnServerDisCoverGame );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::S2S_UPDATE_ZONE_TO_WORLD_REQ, &KFWorldModule::HandleUpdateZoneToWorldReq );

        __REGISTER_MESSAGE__( KFMsg::S2S_LOGIN_WORLD_VERIFY_REQ, &KFWorldModule::HandleLoginWorldVerifyReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_GAME_SYNC_ONLINE_REQ, &KFWorldModule::HandleGameSyncOnlineReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_TRANSMIT_TO_PLAYER, &KFWorldModule::HandleTransmitToPlayerReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_TRANSMIT_TO_SERVER, &KFWorldModule::HandleTransmitToServerReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_BROADCAST_TO_WORLD, &KFWorldModule::HandleBroadcastMessageReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_PLAYER_ENTER_WORLD_REQ, &KFWorldModule::HandlePlayerEnterWorldReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_PLAYER_LEAVE_WORLD_REQ, &KFWorldModule::HandlePlayerLeaveWorldReq );

        __REGISTER_COMMAND_FUNCTION__( __KF_STRING__( notice ), &KFWorldModule::OnCommandNotice );
        __REGISTER_COMMAND_FUNCTION__( __KF_STRING__( marquee ), &KFWorldModule::OnCommandMarquee );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_HTTP_FUNCTION__( __KF_STRING__( kickonline ), true, &KFWorldModule::HandleHttpKickOnline );
    }

    void KFWorldModule::BeforeShut()
    {
        __UNREGISTER_TIMER__();
        __UNREGISTER_SERVER_LOST_FUNCTION__();
        __UNREGISTER_SERVER_DISCOVER_FUNCTION__();
        __UNREGISTER_COMMAND_FUNCTION__( __KF_STRING__( notice ) );
        __UNREGISTER_COMMAND_FUNCTION__( __KF_STRING__( marquee ) );

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::S2S_UPDATE_ZONE_TO_WORLD_REQ );

        __UNREGISTER_MESSAGE__( KFMsg::S2S_GAME_SYNC_ONLINE_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_TRANSMIT_TO_PLAYER );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_TRANSMIT_TO_SERVER );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_BROADCAST_TO_WORLD );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_PLAYER_ENTER_WORLD_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_PLAYER_LEAVE_WORLD_REQ );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_HTTP_FUNCTION__( __KF_STRING__( kickonline ) );
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool KFWorldModule::SendToOnline( uint64 playerid, uint32 msgid, ::google::protobuf::Message* message )
    {
        auto kfonline = _kf_online_list.Find( playerid );
        if ( kfonline == nullptr )
        {
            return false;
        }

        kfonline->SendToOnline( msgid, message );
        return true;
    }

    bool KFWorldModule::SendToGame( uint64 gameid, uint32 msgid, ::google::protobuf::Message* message )
    {
        return _kf_tcp_server->SendNetMessage( gameid, msgid, message );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_SERVER_DISCOVER_FUNCTION__( KFWorldModule::OnServerDisCoverGame )
    {
        if ( handletype == __KF_STRING__( game ) )
        {
            _kf_game_conhash.AddHashNode( __KF_STRING__( game ), handleid, 100 );
        }
    }

    __KF_SERVER_LOST_FUNCTION__( KFWorldModule::OnServerLostGame )
    {
        if ( handletype == __KF_STRING__( game ) )
        {
            _kf_game_conhash.RemoveHashNode( handleid );
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    __KF_TIMER_FUNCTION__( KFWorldModule::OnTimerZoneRegister )
    {
        auto zone = _kf_zone->GetZone();
        auto _url = _kf_ip_address->GetAuthUrl() + __KF_STRING__( zoneregister );

        // 注册小区信息
        __JSON_DOCUMENT__( kfjson );
        __JSON_SET_VALUE__( kfjson, __KF_STRING__( id ), zone->_id );
        __JSON_SET_VALUE__( kfjson, __KF_STRING__( name ), zone->_name );
        __JSON_SET_VALUE__( kfjson, __KF_STRING__( url ), _kf_http_server->GetHttpUrl() );

        auto recvdata = _kf_http_client->STGet( _url, kfjson );
        __JSON_PARSE_STRING__( kfresult, recvdata );
        auto retcode = _kf_http_client->GetCode( kfresult );
        if ( retcode == KFMsg::Ok )
        {
            __UNREGISTER_OBJECT_TIMER__( __ZONE_TIMER_ID__ );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFWorldModule::HandleUpdateZoneToWorldReq )
    {
        __PROTO_PARSE__( KFMsg::S2SUpdateZoneToWorldReq );

        auto pbdata = kfmsg.zonedata();
        if ( !_zone_data.ip().empty() && _zone_data.count() < pbdata.count() )
        {
            return;
        }

        // 保存小区数据
        _zone_data.CopyFrom( pbdata );

        // 更新给auth服务器
        auto kfzone = _kf_zone->GetZone();
        static auto _url = _kf_ip_address->GetAuthUrl() + __KF_STRING__( zoneupdate );

        __JSON_DOCUMENT__( kfjson );
        __JSON_SET_VALUE__( kfjson, __KF_STRING__( id ), kfzone->_id );
        __JSON_SET_VALUE__( kfjson, __KF_STRING__( ip ), _zone_data.ip() );
        __JSON_SET_VALUE__( kfjson, __KF_STRING__( port ), _zone_data.port() );
        _kf_http_client->MTGet< KFWorldModule >( _url, kfjson );
    }

    //////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFWorldModule::HandleLoginWorldVerifyReq )
    {
        auto loginid = __ROUTE_SERVER_ID__;
        __PROTO_PARSE__( KFMsg::S2SLoginWorldVerifyReq );

        auto pblogin = &kfmsg.pblogin();
        __LOG_DEBUG__( "player[{}:{}:{}] login world req!", pblogin->account(), pblogin->accountid(), pblogin->playerid() );

        // 踢掉已经在线的玩家, 只有踢下线以后才能登陆
        KickOnline( pblogin->playerid(), __FUNC_LINE__ );

        // 选择Game服务器
        auto gameid = _kf_game_conhash.FindHashNode( pblogin->playerid() );
        if ( gameid == _invalid_int )
        {
            return SendVerifyFailedToLogin( KFMsg::CanNotFindGame, loginid, pblogin->gateid(), pblogin->accountid(), pblogin->sessionid() );
        }

        // 通知游戏服务器 保存验证信息
        KFMsg::S2SLoginTellTokenToGameReq req;
        req.mutable_pblogin()->CopyFrom( *pblogin );
        auto ok = _kf_tcp_server->SendNetMessage( gameid, KFMsg::S2S_LOGIN_TELL_TOKEN_TO_GAME_REQ, &req );
        if ( !ok )
        {
            __LOG_ERROR__( "player[{}:{}:{}] login game failed!", pblogin->account(), pblogin->accountid(), pblogin->playerid() );
        }
    }

    void KFWorldModule::SendVerifyFailedToLogin( uint32 result, uint64 loginid, uint64 gateid, uint64 accountid, uint64 sessionid )
    {
        KFMsg::S2SLoginFailedToLoginAck ack;
        ack.set_result( result );
        ack.set_gateid( gateid );
        ack.set_accountid( accountid );
        ack.set_sessionid( sessionid );
        auto ok = _kf_tcp_server->SendNetMessage( loginid, KFMsg::S2S_LOGIN_FAILED_TO_LOGIN_ACK, &ack );
        if ( !ok )
        {
            __LOG_ERROR__( "player[{}] world verify result[{}] failed!", accountid, result );
        }
    }
    //////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFWorldModule::HandleGameSyncOnlineReq )
    {
        __PROTO_PARSE__( KFMsg::S2SGameSyncOnlineReq );

        auto serverid = __ROUTE_SERVER_ID__;
        for ( auto i = 0; i < kfmsg.playerid_size(); ++i )
        {
            auto playerid = kfmsg.playerid( i );

            auto kfonline = _kf_online_list.Create( playerid );

            kfonline->_game_id = serverid;
            kfonline->_player_id = playerid;
        }
    }

    __KF_MESSAGE_FUNCTION__( KFWorldModule::HandlePlayerEnterWorldReq )
    {
        __PROTO_PARSE__( KFMsg::S2SPlayerEnterWorldReq );

        auto kfonline = _kf_online_list.Create( kfmsg.playerid() );
        kfonline->_player_id = kfmsg.playerid();
        kfonline->_account_id = kfmsg.accountid();
        kfonline->_game_id = __ROUTE_SERVER_ID__;

        // 更新到认证服务器
        UpdateOnlineToAuth( kfmsg.accountid(), kfmsg.playerid(), true );
    }

    __KF_MESSAGE_FUNCTION__( KFWorldModule::HandlePlayerLeaveWorldReq )
    {
        __PROTO_PARSE__( KFMsg::S2SPlayerLeaveWorldReq );

        _kf_online_list.Remove( kfmsg.playerid() );

        // 更新到认证服务器
        UpdateOnlineToAuth( kfmsg.accountid(), kfmsg.playerid(), false );
    }

    void KFWorldModule::UpdateOnlineToAuth( uint64 accountid, uint64 playerid, bool online )
    {
        static auto _update_url = _kf_ip_address->GetAuthUrl() + __KF_STRING__( onlinedata );

        // 在线信息
        __JSON_DOCUMENT__( sendjson );
        __JSON_SET_VALUE__( sendjson, __KF_STRING__( accountid ), accountid );

        if ( online )
        {
            __JSON_SET_VALUE__( sendjson, __KF_STRING__( online ), 1 );
            __JSON_SET_VALUE__( sendjson, __KF_STRING__( playerid ), playerid );
            __JSON_SET_VALUE__( sendjson, __KF_STRING__( zoneid ), _kf_zone->GetZone()->_id );
        }
        else
        {
            __JSON_SET_VALUE__( sendjson, __KF_STRING__( online ), 0 );
        }

        _kf_http_client->StartMTClient( _update_url, sendjson );

        // 记录在线玩家数量
        __LOG_DEBUG__( "online player count=[{}]", _kf_online_list.Size() );
    }

    bool KFWorldModule::KickOnline( uint64 playerid, const char* function, uint32 line )
    {
        auto kfonline = _kf_online_list.Find( playerid );
        if ( kfonline == nullptr )
        {
            return false;
        }

        __LOG_DEBUG__( "kick player[{}]!", playerid );

        // 发送踢出消息
        KFMsg::S2SKickGamePlayerReq req;
        req.set_playerid( playerid );
        req.set_type( KFMsg::KickEnum::LoginBeKick );
        auto ok = _kf_tcp_server->SendNetMessage( kfonline->_game_id, KFMsg::S2S_KICK_GAME_PLAYER_REQ, &req );
        if ( ok )
        {
            // 发送成功, 先删除在线信息, 避免gameserver掉线以后,登录不正常的问题
            _kf_online_list.Remove( playerid );
        }
        return ok;
    }

    __KF_HTTP_FUNCTION__( KFWorldModule::HandleHttpKickOnline )
    {
        __JSON_PARSE_STRING__( request, data );

        auto playerid = __JSON_GET_UINT64__( request, __KF_STRING__( playerid ) );
        KickOnline( playerid, __FUNC_LINE__ );

        return _invalid_str;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////
    __KF_MESSAGE_FUNCTION__( KFWorldModule::HandleTransmitToPlayerReq )
    {
        __PROTO_PARSE__( KFMsg::S2STransmitToPlayer );

        auto kfonline = _kf_online_list.Find( kfmsg.playerid() );
        if ( kfonline == nullptr )
        {
            return;
        }

        auto& msgdata = kfmsg.msgdata();
        kfonline->SendToOnline( kfmsg.msgid(), msgdata.data(), static_cast< uint32 >( msgdata.size() ) );
    }

    __KF_MESSAGE_FUNCTION__( KFWorldModule::HandleTransmitToServerReq )
    {
        __PROTO_PARSE__( KFMsg::S2STransmitToServer );

        auto& msgdata = kfmsg.msgdata();
        _kf_tcp_server->SendMessageToType( __KF_STRING__( game ), kfmsg.msgid(), msgdata.data(), static_cast< uint32 >( msgdata.size() ) );
    }

    void KFWorldModule::BroadcastMessageToGame( uint32 msgid, google::protobuf::Message* message )
    {
        auto strdata = message->SerializeAsString();
        BroadcastMessageToGame( msgid, strdata.data(), static_cast< uint32 >( strdata.length() ) );
    }

    void KFWorldModule::BroadcastMessageToGame( uint32 msgid, const char* data, uint32 length )
    {
        // 随机选择一个服务器
        auto gameid = _kf_game_conhash.FindHashNode( msgid, true );

        KFMsg::S2SBroadcastToGame req;
        req.set_msgid( msgid );
        req.set_msgdata( data, length );
        _kf_tcp_server->SendNetMessage( gameid, KFMsg::S2S_BROADCAST_TO_GAME, &req );
    }

    __KF_MESSAGE_FUNCTION__( KFWorldModule::HandleBroadcastMessageReq )
    {
        __PROTO_PARSE__( KFMsg::S2SBroadcastToWord );

        BroadcastMessageToGame( kfmsg.msgid(), kfmsg.msgdata().data(), static_cast< uint32 >( kfmsg.msgdata().length() ) );
    }

    __KF_COMMAND_FUNCTION__( KFWorldModule::OnCommandMarquee )
    {
        KFMsg::MsgTellMarquee tell;
        tell.set_content( param );
        BroadcastMessageToGame( KFMsg::MSG_TELL_MARQUEE, &tell );
    }

    __KF_COMMAND_FUNCTION__( KFWorldModule::OnCommandNotice )
    {
        KFMsg::MsgTellSysNotcie tell;
        tell.set_content( param );
        BroadcastMessageToGame( KFMsg::MSG_TELL_SYS_NOTICE, &tell );
    }
}
