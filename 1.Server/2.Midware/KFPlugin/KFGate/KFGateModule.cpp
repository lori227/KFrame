#include "KFGateModule.hpp"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFGateModule::BeforeRun()
    {
        __REGISTER_LOOP_TIMER__( 0, 10000, &KFGateModule::OnTimerUpdateOnlineToAuth );

        __REGISTER_CLIENT_LOST_FUNCTION__( &KFGateModule::OnClientLostLogin );
        __REGISTER_CLIENT_CONNECTION_FUNCTION__( &KFGateModule::OnClientConnectionLogin );
        __REGISTER_SERVER_LOST_FUNCTION__( &KFGateModule::OnPlayerDisconnection );
        __REGISTER_SERVER_TRANSMIT_FUNCTION__( &KFGateModule::SendMessageToGame );
        __REGISTER_CLIENT_TRANSMIT_FUNCTION__( &KFGateModule::SendToClient );

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_LOGIN_VERIFY_REQ, &KFGateModule::HandleLoginVerifyReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_LOGIN_OUT_REQ, &KFGateModule::HandleLoginOutReq );


        __REGISTER_MESSAGE__( KFMsg::S2S_LOGIN_LOGIN_VERIFY_ACK, &KFGateModule::HandleLoginVerifyAck );
        __REGISTER_MESSAGE__( KFMsg::S2S_KICK_GATE_PLAYER_REQ, &KFGateModule::HandleKickGatePlayerReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_BROADCAST_TO_GATE, &KFGateModule::HandleBroadcastMessageReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_LOGIN_GAME_ACK, &KFGateModule::HandleLoginGameAck );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFGateModule::BeforeShut()
    {
        __UNREGISTER_TIMER__();
        __UNREGISTER_CLIENT_LOST_FUNCTION__();
        __UNREGISTER_CLIENT_CONNECTION_FUNCTION__();
        __UNREGISTER_SERVER_LOST_FUNCTION__();
        __UNREGISTER_SERVER_TRANSMIT_FUNCTION__();
        __UNREGISTER_CLIENT_TRANSMIT_FUNCTION__();

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::MSG_LOGIN_VERIFY_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_KICK_GATE_PLAYER_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_BROADCAST_TO_GATE );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_LOGIN_LOGIN_VERIFY_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_LOGIN_GAME_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_LOGIN_OUT_REQ );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    __KF_TIMER_FUNCTION__( KFGateModule::OnTimerUpdateOnlineToAuth )
    {
        static auto _url = _kf_ip_address->GetAuthUrl() + __KF_STRING__( zoneupdate );

        // 更新gate的负载信息
        auto kfzone = _kf_zone->GetZone();
        auto kfglobal = KFGlobal::Instance();

        __JSON_DOCUMENT__( kfjson );
        __JSON_SET_VALUE__( kfjson, __KF_STRING__( id ), kfzone->_id );
        __JSON_SET_VALUE__( kfjson, __KF_STRING__( ip ), kfglobal->_interanet_ip );
        __JSON_SET_VALUE__( kfjson, __KF_STRING__( port ), kfglobal->_listen_port );
        __JSON_SET_VALUE__( kfjson, __KF_STRING__( count ), _kf_role_list.Size() );
        _kf_http_client->StartMTClient( _url, kfjson );
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_CLIENT_CONNECT_FUNCTION__( KFGateModule::OnClientConnectionLogin )
    {
        if ( servertype == __KF_STRING__( login ) )
        {
            _kf_login_conhash.AddHashNode( servertype, serverid, 100 );
        }
    }

    __KF_CLIENT_LOST_FUNCTION__( KFGateModule::OnClientLostLogin )
    {
        if ( servertype == __KF_STRING__( login ) )
        {
            _kf_login_conhash.RemoveHashNode( serverid );
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////
#define __KF_MAX_CLIENT_MSG_ID__ 10000
    __KF_TRANSMIT_MESSAGE_FUNCTION__( KFGateModule::SendToClient )
    {
        auto playerid = __ROUTE_RECV_ID__;
        if ( playerid == _invalid_int || msgid >= __KF_MAX_CLIENT_MSG_ID__ )
        {
            return false;
        }

        auto kfrole = _kf_role_list.Find( playerid );
        if ( kfrole != nullptr )
        {
            kfrole->SendToClient( msgid, data, length );
        }

        return true;
    }

    __KF_TRANSMIT_MESSAGE_FUNCTION__( KFGateModule::SendMessageToGame )
    {
        auto playerid = __ROUTE_SEND_ID__;
        if ( playerid == _invalid_int || msgid >= __KF_MAX_CLIENT_MSG_ID__ )
        {
            return false;
        }

        auto kfrole = _kf_role_list.Find( playerid );
        if ( kfrole != nullptr )
        {
            kfrole->SendMessageToGame( msgid, data, length );
        }

        return true;
    }

    __KF_MESSAGE_FUNCTION__( KFGateModule::HandleBroadcastMessageReq )
    {
        __PROTO_PARSE__( KFMsg::S2SBroadcastToGate );

        auto msgid = kfmsg.msgid();
        auto& msgdata = kfmsg.msgdata();

        for ( auto& iter : _kf_role_list._objects )
        {
            auto kfrole = iter.second;
            kfrole->SendToClient( msgid, msgdata.data(), static_cast< uint32 >( msgdata.length() ) );
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void KFGateModule::SendLoginFailedMessage( uint64 sessionid, uint32 result, uint64 bantime )
    {
        // 消息到这里的都是错误结果
        _kf_display->SendToClient( sessionid, result, bantime );

        // 2秒后主动断开游戏
        _kf_tcp_server->CloseNetHandle( sessionid, 2000, __FUNC_LINE__ );
    }

    __KF_MESSAGE_FUNCTION__( KFGateModule::HandleLoginVerifyReq )
    {
        auto sessionid = __ROUTE_SERVER_ID__;
        __PROTO_PARSE__( KFMsg::MsgLoginVerifyReq );

        auto& token = kfmsg.token();
        auto accountid = kfmsg.accountid();

        __LOG_DEBUG__( "session[{}] accountid[{}] login gate req!", sessionid, accountid );

        // 注册连接器
        if ( !_kf_tcp_server->RegisteNetHandle( sessionid, sessionid, _invalid_int ) )
        {
            return __LOG_ERROR__( "accountid[{}] register[{}] failed!", accountid, sessionid );
        }

        // 判断版本号
        auto compatibility = KFGlobal::Instance()->CheckVersionCompatibility( kfmsg.version() );
        if ( !compatibility )
        {
            return SendLoginFailedMessage( sessionid, KFMsg::VersionNotCompatibility, 0 );
        }

        // 没有可用的login
        auto loginserverid = _kf_login_conhash.FindHashNode( accountid );
        if ( loginserverid == _invalid_int )
        {
            return SendLoginFailedMessage( sessionid, KFMsg::LoginSystemBusy, 0 );
        }

        // ip
        auto& ip = _kf_tcp_server->GetHandleIp( sessionid );

        // 发送到Login服务器验证
        KFMsg::S2SLoginLoginVerifyReq req;
        req.set_ip( ip );
        req.set_token( token );
        req.set_sessionid( sessionid );
        req.set_accountid( accountid );
        auto ok = _kf_tcp_client->SendNetMessage( loginserverid, KFMsg::S2S_LOGIN_LOGIN_VERIFY_REQ, &req );
        if ( !ok )
        {
            __LOG_ERROR__( "session[{}] accountid[{}] send login failed!", sessionid, accountid );

            // 发送错误
            SendLoginFailedMessage( sessionid, KFMsg::LoginSystemBusy, 0 );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFGateModule::HandleLoginVerifyAck )
    {
        __PROTO_PARSE__( KFMsg::S2SLoginLoginVerifyAck );
        __LOG_DEBUG__( "player[{}] login verify result[{}]!", kfmsg.accountid(), kfmsg.result() );

        SendLoginFailedMessage( kfmsg.sessionid(), kfmsg.result(), kfmsg.bantime() );
    }

    __KF_MESSAGE_FUNCTION__( KFGateModule::HandleLoginGameAck )
    {
        __PROTO_PARSE__( KFMsg::S2SLoginGameAck );
        auto result = kfmsg.result();
        auto pblogin = &kfmsg.pblogin();

        __LOG_DEBUG__( "player[{}:{}] session[{}] enter game!", pblogin->accountid(), pblogin->playerid(), pblogin->sessionid() );

        if ( result != KFMsg::Ok )
        {
            __LOG_ERROR__( "player[{}:{}] login failed[{}]!", pblogin->accountid(), pblogin->playerid(), result );
            return SendLoginFailedMessage( pblogin->sessionid(), result, 0 );
        }

        // 绑定角色id
        if ( !_kf_tcp_server->BindObjectId( pblogin->sessionid(), pblogin->playerid() ) )
        {
            return __LOG_ERROR__( "player[{}:{}] session[{}] failed!", pblogin->accountid(), pblogin->playerid(), pblogin->sessionid() );
        }

        // 创建角色
        auto kfrole = _kf_role_list.Create( pblogin->playerid() );
        kfrole->_game_id = __ROUTE_SERVER_ID__;
        kfrole->_role_id = pblogin->playerid();
        kfrole->_session_id = pblogin->sessionid();

        // 通知进入游戏
        KFMsg::MsgEnterGame enter;
        enter.set_servertime( kfmsg.servertime() );
        enter.mutable_playerdata()->CopyFrom( kfmsg.playerdata() );
        auto ok = kfrole->SendToClient( KFMsg::MSG_LOGIN_ENTER_GAME, &enter );
        if ( !ok )
        {
            __LOG_ERROR__( "player[{}:{}] session[{}] enter game failed!", pblogin->accountid(), pblogin->playerid(), pblogin->sessionid() );
        }
    }

    __KF_SERVER_LOST_FUNCTION__( KFGateModule::OnPlayerDisconnection )
    {
        auto kfrole = _kf_role_list.Find( handleid );
        if ( kfrole == nullptr )
        {
            return;
        }

        __LOG_DEBUG__( "client[{}] disconnection!", handleid );

        KFMsg::S2SPlayerDisconnectionReq req;
        req.set_playerid( handleid );
        kfrole->SendMessageToGame( KFMsg::S2S_PLAYER_DISCONNECTION_REQ, &req );

        // 删除玩家
        _kf_role_list.Remove( handleid );
    }

    __KF_MESSAGE_FUNCTION__( KFGateModule::HandleKickGatePlayerReq )
    {
        __PROTO_PARSE__( KFMsg::S2SKickGatePlayerReq );

        auto playerid = kfmsg.playerid();
        auto kfrole = _kf_role_list.Find( playerid );
        if ( kfrole == nullptr )
        {
            return;
        }

        // 通知客户端你被踢了
        KFMsg::MsgTellBeKick kick;
        kick.set_type( kfmsg.type() );
        kfrole->SendToClient( KFMsg::MSG_TELL_BE_KICK, &kick );

        // 删除连接关系
        _kf_tcp_server->CloseNetHandle( kfrole->_session_id, 1000, __FUNC_LINE__ );

        // 删除玩家
        _kf_role_list.Remove( playerid );
    }

    __KF_MESSAGE_FUNCTION__( KFGateModule::HandleLoginOutReq )
    {
        auto playerid = __ROUTE_SEND_ID__;
        auto kfrole = _kf_role_list.Find( playerid );
        if ( kfrole == nullptr )
        {
            return;
        }

        __LOG_DEBUG__( "player[{}] login out!", playerid );

        // 断开链接
        _kf_tcp_server->CloseNetHandle( kfrole->_session_id, 1000, __FUNC_LINE__ );

        KFMsg::S2SLoginOutReq req;
        req.set_playerid( playerid );
        kfrole->SendMessageToGame( KFMsg::S2S_LOGIN_OUT_REQ, &req );

        // 删除玩家
        _kf_role_list.Remove( playerid );
    }

}
