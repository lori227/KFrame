#include "KFGateModule.h"
#include "KFProtocol/KFProtocol.h"

namespace KFrame
{
    void KFGateModule::BeforeRun()
    {
        __REGISTER_CLIENT_LOST_FUNCTION__( &KFGateModule::OnClientLostLogin );
        __REGISTER_CLIENT_CONNECTION_FUNCTION__( &KFGateModule::OnClientConnectionLogin );
        __REGISTER_SERVER_LOST_FUNCTION__( &KFGateModule::OnPlayerDisconnection );
        __REGISTER_SERVER_TRANSMIT_FUNCTION__( &KFGateModule::SendMessageToGame );
        __REGISTER_CLIENT_TRANSMIT_FUNCTION__( &KFGateModule::SendToClient );
        __REGISTER_COMMAND_FUNCTION__( __KF_STRING__( marquee ), &KFGateModule::OnCommandMarquee );
        __REGISTER_COMMAND_FUNCTION__( __KF_STRING__( notice ), &KFGateModule::OnCommandNotice );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __REGISTER_MESSAGE__( KFMsg::MSG_LOGIN_VERIFY_REQ, &KFGateModule::HandleLoginVerifyReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_LOGIN_LOGIN_VERIFY_ACK, &KFGateModule::HandleLoginVerifyAck );
        __REGISTER_MESSAGE__( KFMsg::S2S_KICK_GATE_PLAYER_REQ, &KFGateModule::HandleKickGatePlayerReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_BROADCAST_TO_GATE, &KFGateModule::HandleBroadcastMessageReq );
        __REGISTER_MESSAGE__( KFMsg::S2S_LOGIN_GAME_ACK, &KFGateModule::HandleLoginGameAck );
        __REGISTER_MESSAGE__( KFMsg::MSG_LOGIN_OUT_REQ, &KFGateModule::HandleLoginOutReq );
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    void KFGateModule::BeforeShut()
    {
        __UNREGISTER_CLIENT_LOST_FUNCTION__();
        __UNREGISTER_CLIENT_CONNECTION_FUNCTION__();
        __UNREGISTER_SERVER_LOST_FUNCTION__();
        __UNREGISTER_SERVER_TRANSMIT_FUNCTION__();
        __UNREGISTER_CLIENT_TRANSMIT_FUNCTION__();
        __UNREGISTER_COMMAND_FUNCTION__( __KF_STRING__( marquee ) );
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        __UNREGISTER_MESSAGE__( KFMsg::MSG_LOGIN_VERIFY_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_KICK_GATE_PLAYER_REQ );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_BROADCAST_TO_GATE );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_LOGIN_LOGIN_VERIFY_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::S2S_LOGIN_GAME_ACK );
        __UNREGISTER_MESSAGE__( KFMsg::MSG_LOGIN_OUT_REQ );
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    KFRole* KFGateModule::CreateRole( uint64 roleid )
    {
        auto kfrole = _kf_role_list.Create( roleid );
        kfrole->_role_id = roleid;

        return kfrole;
    }

    KFRole* KFGateModule::FindRole( uint64 roleid )
    {
        return _kf_role_list.Find( roleid );
    }

    bool KFGateModule::RemoveRole( uint64 roleid )
    {
        return _kf_role_list.Remove( roleid );
    }

    uint32 KFGateModule::GetRoleCount()
    {
        return _kf_role_list.Size();
    }

    void KFGateModule::SendLoginFailedMessage( uint64 sessionid, uint32 result, uint64 bantime )
    {
        // 消息到这里的都是错误结果
        _kf_display->SendToClient( sessionid, result, bantime );

        // 2秒后主动断开游戏
        _kf_tcp_server->CloseNetHandle( sessionid, 2000, __FUNC_LINE__ );
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

    __KF_MESSAGE_FUNCTION__( KFGateModule::HandleLoginVerifyReq )
    {
        __PROTO_PARSE__( KFMsg::MsgLoginVerifyReq );

        // 连接id
        auto sessionid = __KF_HEAD_ID__( kfid );

        auto& token = kfmsg.token();
        auto accountid = kfmsg.accountid();

        __LOG_DEBUG__( "session[{}] accountid[{}:{}] login gate req!", sessionid, accountid );

        // 注册连接器
        auto ok = _kf_tcp_server->RegisteNetHandle( sessionid, sessionid, _invalid_int );
        if ( !ok )
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
        ok = _kf_tcp_client->SendNetMessage( loginserverid, KFMsg::S2S_LOGIN_LOGIN_VERIFY_REQ, &req );
        if ( ok )
        {
            __LOG_DEBUG__( "accountid[{}:{}] login verify!", accountid, ip );
        }
        else
        {
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

    __KF_SERVER_LOST_FUNCTION__( KFGateModule::OnPlayerDisconnection )
    {
        __LOG_DEBUG__( "client[{}] disconnection!", handleid );

        auto kfrole = FindRole( handleid );
        if ( kfrole == nullptr )
        {
            return;
        }

        KFMsg::S2SPlayerDisconnectionReq req;
        req.set_playerid( handleid );
        kfrole->SendMessageToGame( KFMsg::S2S_PLAYER_DISCONNECTION_REQ, &req );

        // 删除玩家
        RemoveRole( handleid );
    }

    //////////////////////////////////////////////////////////////////////////////////////////////
#define __KF_MAX_CLIENT_MSG_ID__ 10000
    __KF_TRANSMIT_FUNCTION__( KFGateModule::SendToClient )
    {
        auto playerid = __KF_DATA_ID__( kfid );
        if ( msgid == _invalid_int || msgid >= __KF_MAX_CLIENT_MSG_ID__ )
        {
            __LOG_ERROR__( "client[{}] msgid[{}] length[{}] error!", playerid, msgid, length );
            return false;
        }

        auto kfrole = FindRole( playerid );
        if ( kfrole != nullptr )
        {
            kfrole->SendToClient( msgid, data, length );
        }

        return true;
    }

    __KF_TRANSMIT_FUNCTION__( KFGateModule::SendMessageToGame )
    {
        auto playerid = __KF_HEAD_ID__( kfid );
        if ( msgid == _invalid_int || msgid >= __KF_MAX_CLIENT_MSG_ID__ )
        {
            __LOG_ERROR__( "client[{}] msgid[{}] length[{}] error!", playerid, msgid, length );
            return false;
        }

        auto kfrole = FindRole( playerid );
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
            auto kfproxy = iter.second;
            kfproxy->SendToClient( msgid, msgdata.data(), static_cast< uint32 >( msgdata.length() ) );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFGateModule::HandleKickGatePlayerReq )
    {
        __PROTO_PARSE__( KFMsg::S2SKickGatePlayerReq );

        auto playerid = kfmsg.playerid();
        auto kfrole = FindRole( playerid );
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

        // 删除内存
        RemoveRole( playerid );
    }

    __KF_MESSAGE_FUNCTION__( KFGateModule::HandleLoginGameAck )
    {
        __PROTO_PARSE__( KFMsg::S2SLoginGameAck );
        auto result = kfmsg.result();
        auto pblogin = &kfmsg.pblogin();

        __LOG_DEBUG__( "player[{}:{}] session[{}] enter game req!", pblogin->accountid(), pblogin->playerid(), pblogin->sessionid() );

        if ( result != KFMsg::Success )
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
        auto kfrole = CreateRole( pblogin->playerid() );
        kfrole->_game_id = __KF_HEAD_ID__( kfid );
        kfrole->_role_id = pblogin->playerid();
        kfrole->_session_id = pblogin->sessionid();

        // 通知进入游戏
        KFMsg::MsgEnterGame enter;
        enter.set_servertime( kfmsg.servertime() );
        enter.mutable_playerdata()->CopyFrom( kfmsg.playerdata() );
        auto ok = kfrole->SendToClient( KFMsg::MSG_LOGIN_ENTER_GAME, &enter );
        if ( ok )
        {
            __LOG_DEBUG__( "player[{}:{}] session[{}] enter game ok!", pblogin->accountid(), pblogin->playerid(), pblogin->sessionid() );
        }
        else
        {
            __LOG_ERROR__( "player[{}:{}] session[{}] enter game failed!", pblogin->accountid(), pblogin->playerid(), pblogin->sessionid() );
        }
    }

    __KF_MESSAGE_FUNCTION__( KFGateModule::HandleLoginOutReq )
    {
        auto playerid = __KF_HEAD_ID__( kfid );
        __LOG_DEBUG__( "player[{}] login out!", playerid );

        auto kfrole = FindRole( playerid );
        if ( kfrole == nullptr )
        {
            return;
        }

        // 断开链接
        _kf_tcp_server->CloseNetHandle( kfrole->_session_id, 1000, __FUNC_LINE__ );

        KFMsg::S2SLoginOutReq req;
        req.set_playerid( playerid );
        kfrole->SendMessageToGame( KFMsg::S2S_LOGIN_OUT_REQ, &req );

        // 删除玩家
        RemoveRole( playerid );
    }

    __KF_COMMAND_FUNCTION__( KFGateModule::OnCommandMarquee )
    {
        KFMsg::MsgTellMarquee tell;
        tell.set_content( param );
        _kf_tcp_server->SendNetMessage( KFMsg::MSG_TELL_MARQUEE, &tell );
    }

    __KF_COMMAND_FUNCTION__( KFGateModule::OnCommandNotice )
    {
        KFMsg::MsgTellSysNotcie tell;
        tell.set_content( param );
        _kf_tcp_server->SendNetMessage( KFMsg::MSG_TELL_SYS_NOTICE, &tell );
    }
}
