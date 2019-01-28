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

        __REGISTER_DEPLOY_COMMAND_FUNCTION__( __KF_STRING__( shutdown ), &KFGateModule::OnDeployShutDownServer );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_LOGIN_REQ, &KFGateModule::HandleLoginReq );
        __REGISTER_MESSAGE__( KFMsg::MSG_LOGOUT_REQ, &KFGateModule::HandleLogoutReq );

        __REGISTER_MESSAGE__( KFMsg::S2S_LOGIN_TO_GATE_ACK, &KFGateModule::HandleLoginToGateAck );
        __REGISTER_MESSAGE__( KFMsg::S2S_ENTER_TO_GATE_ACK, &KFGateModule::HandleEnterToGateAck );

        __REGISTER_MESSAGE__( KFMsg::S2S_KICK_PLAYER_TO_GATE_REQ, &KFGateModule::HandleKickPlayerToGateReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_BROADCAST_TO_GATE, &KFGateModule::HandleBroadcastMessageReq );
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

        __UNREGISTER_DEPLOY_COMMAND_FUNCTION__( __KF_STRING__( shutdown ) );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::MSG_LOGIN_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_LOGOUT_REQ );

        __UNREGISTER_MESSAGE__( KFMsg::S2S_LOGIN_TO_GATE_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_ENTER_TO_GATE_ACK );

        __UNREGISTER_MESSAGE__( KFMsg::S2S_BROADCAST_TO_GATE );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_KICK_PLAYER_TO_GATE_REQ );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    __KF_DEPLOY_COMMAND_FUNCTION__( KFGateModule::OnDeployShutDownServer )
    {
        _is_login_close = true;
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    __KF_TIMER_FUNCTION__( KFGateModule::OnTimerUpdateOnlineToAuth )
    {
        auto kfglobal = KFGlobal::Instance();

        auto loginid = _kf_login_conhash.FindHashNode( kfglobal->_app_id._union._id );
        if ( loginid == _invalid_int )
        {
            return;
        }

        KFMsg::S2SUpdateZoneToLoginReq req;
        auto pbzone = req.mutable_zonedata();
        pbzone->set_ip( kfglobal->_interanet_ip );
        pbzone->set_port( kfglobal->_listen_port );
        pbzone->set_count( _kf_role_list.Size() );
        _kf_tcp_client->SendNetMessage( loginid, KFMsg::S2S_UPDATE_ZONE_TO_LOGIN_REQ, &req );
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
    void KFGateModule::SendLoginAckMessage( uint64 sessionid, uint32 result, uint64 bantime )
    {
        // 消息到这里的都是错误结果
        _kf_display->SendToClient( sessionid, result, bantime );

        // 2秒后主动断开游戏
        _kf_tcp_server->CloseNetHandle( sessionid, 2000, __FUNC_LINE__ );
    }

    __KF_MESSAGE_FUNCTION__( KFGateModule::HandleLoginReq )
    {
        auto sessionid = __ROUTE_SERVER_ID__;
        __PROTO_PARSE__( KFMsg::MsgLoginReq );

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
            return SendLoginAckMessage( sessionid, KFMsg::VersionNotCompatibility, 0 );
        }

        // 服务器正在关闭中
        if ( _is_login_close )
        {
            return SendLoginAckMessage( sessionid, KFMsg::LoginIsClose, 0 );
        }

        // 没有可用的login
        auto loginserverid = _kf_login_conhash.FindHashNode( accountid );
        if ( loginserverid == _invalid_int )
        {
            return SendLoginAckMessage( sessionid, KFMsg::LoginNoLoginServer, 0 );
        }

        // ip
        auto& ip = _kf_tcp_server->GetHandleIp( sessionid );

        // 发送到Login服务器验证
        KFMsg::S2SLoginToLoginReq req;
        req.set_ip( ip );
        req.set_token( token );
        req.set_accountid( accountid );
        req.set_sessionid( sessionid );
        auto ok = _kf_tcp_client->SendNetMessage( loginserverid, KFMsg::S2S_LOGIN_TO_LOGIN_REQ, &req );
        if ( !ok )
        {
            __LOG_ERROR__( "session[{}] accountid[{}] send login failed!", sessionid, accountid );

            // 发送错误
            SendLoginAckMessage( sessionid, KFMsg::LoginSystemBusy, 0 );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFGateModule::HandleLoginToGateAck )
    {
        __PROTO_PARSE__( KFMsg::S2SLoginToGateAck );
        __LOG_DEBUG__( "player[{}] login result[{}]!", kfmsg.accountid(), kfmsg.result() );

        SendLoginAckMessage( kfmsg.sessionid(), kfmsg.result(), kfmsg.bantime() );
    }

    __KF_MESSAGE_FUNCTION__( KFGateModule::HandleEnterToGateAck )
    {
        __PROTO_PARSE__( KFMsg::S2SEnterToGateAck );

        auto pblogin = &kfmsg.pblogin();
        __LOG_DEBUG__( "player[{}:{}] session[{}] enter game!", pblogin->accountid(), pblogin->playerid(), pblogin->sessionid() );

        // 绑定角色id
        if ( !_kf_tcp_server->BindObjectId( pblogin->sessionid(), pblogin->playerid() ) )
        {
            __LOG_ERROR__( "player[{}:{}] session[{}] failed!", pblogin->accountid(), pblogin->playerid(), pblogin->sessionid() );
            return SendLoginAckMessage( pblogin->sessionid(), KFMsg::LoginBindPlayerError, 0 );
        }

        // 创建角色
        auto kfrole = _kf_role_list.Create( pblogin->playerid() );
        kfrole->_game_id = __ROUTE_SERVER_ID__;
        kfrole->_role_id = pblogin->playerid();
        kfrole->_session_id = pblogin->sessionid();

        // 通知进入游戏
        KFMsg::MsgLoginAck ack;
        ack.set_servertime( kfmsg.servertime() );
        ack.mutable_playerdata()->CopyFrom( kfmsg.playerdata() );
        auto ok = kfrole->SendToClient( KFMsg::MSG_LOGIN_ACK, &ack );
        if ( !ok )
        {
            __LOG_ERROR__( "player[{}:{}] session[{}] enter failed!", pblogin->accountid(), pblogin->playerid(), pblogin->sessionid() );
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

        KFMsg::S2SDisconnectToGameReq req;
        req.set_playerid( handleid );
        kfrole->SendMessageToGame( KFMsg::S2S_DISCONNEC_TO_GAME_REQ, &req );

        // 删除玩家
        _kf_role_list.Remove( handleid );
    }

    __KF_MESSAGE_FUNCTION__( KFGateModule::HandleKickPlayerToGateReq )
    {
        __PROTO_PARSE__( KFMsg::S2SKickPlayerToGateReq );

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

    __KF_MESSAGE_FUNCTION__( KFGateModule::HandleLogoutReq )
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

        KFMsg::S2SLogoutToGameReq req;
        req.set_playerid( playerid );
        kfrole->SendMessageToGame( KFMsg::S2S_LOGOUT_TO_GAME_REQ, &req );

        // 删除玩家
        _kf_role_list.Remove( playerid );
    }

    //// 走马灯
    //__KF_COMMAND_FUNCTION__( OnCommandMarquee );

    //// 系统公告
    //__KF_COMMAND_FUNCTION__( OnCommandNotice );

    //__REGISTER_COMMAND_FUNCTION__( __KF_STRING__( notice ), &KFWorldModule::OnCommandNotice );
    //__REGISTER_COMMAND_FUNCTION__( __KF_STRING__( marquee ), &KFWorldModule::OnCommandMarquee );

    //__UNREGISTER_COMMAND_FUNCTION__( __KF_STRING__( notice ) );
    //__UNREGISTER_COMMAND_FUNCTION__( __KF_STRING__( marquee ) );
    //__KF_COMMAND_FUNCTION__( KFWorldModule::OnCommandMarquee )
    //{
    //    KFMsg::MsgTellMarquee tell;
    //    tell.set_content( param );
    //    BroadcastMessageToGame( KFMsg::MSG_TELL_MARQUEE, &tell );
    //}

    //__KF_COMMAND_FUNCTION__( KFWorldModule::OnCommandNotice )
    //{
    //    KFMsg::MsgTellSysNotcie tell;
    //    tell.set_content( param );
    //    BroadcastMessageToGame( KFMsg::MSG_TELL_SYS_NOTICE, &tell );
    //}
}
